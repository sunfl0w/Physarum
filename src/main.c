#include "deps/glad/glad.h"
#include "gl_error_handeling.h"
#include "shader.h"
#include "shader_program.h"
#include "stopwatch.h"
#include "text_renderer.h"
#include "texture.h"

#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

static const int INDIVIDUALS_PER_INVOCATION = 8;
static const int SIM_LOCAL_COMPUTE_SIZE = 32;
static const int INDIVIDUALS_PER_WORKGROUP = INDIVIDUALS_PER_INVOCATION * SIM_LOCAL_COMPUTE_SIZE;
static const int MAX_INDIVIDUALS = 2097152;
static const int DIFF_LOCAL_COMPUTE_SIZE = 10;

int width = 640;
int height = 480;
int individuals = INDIVIDUALS_PER_WORKGROUP;
int simulation_speed = 50;
int sample_distance = 20;
int turn_rate = 30;

float blur_weight = 0.01;
float decay_rate = 0.1;

int show_fps_counter = 0;

int main(int argc, char* argv[]) {
    // Parse options
    int option;
    while ((option = getopt(argc, argv, "W:H:N:S:D:T:B:R:F")) != -1) {
        switch (option) {
        case 'W':
            width = strtol(optarg, NULL, 10);
            break;
        case 'H':
            height = strtol(optarg, NULL, 10);
            break;
        case 'N':
            individuals = strtol(optarg, NULL, 10);
            break;
        case 'S':
            simulation_speed = strtol(optarg, NULL, 10);
            break;
        case 'D':
            sample_distance = strtol(optarg, NULL, 10);
            break;
        case 'T':
            turn_rate = strtol(optarg, NULL, 10);
            break;
        case 'B':
            blur_weight = strtof(optarg, NULL);
            break;
        case 'R':
            decay_rate = strtof(optarg, NULL);
            break;
        case 'F':
            show_fps_counter = 1;
            break;
        default:
            printf("Unknown argument\n");
        }
    }

    if (individuals % INDIVIDUALS_PER_WORKGROUP != 0) {
        printf("Number of simulated individuals must be multiple of %d\n", INDIVIDUALS_PER_WORKGROUP);
        exit(1);
    }

    if (individuals > MAX_INDIVIDUALS) {
        printf("Number of simulated individuals must not exceed %d\n", MAX_INDIVIDUALS);
        exit(1);
    }

    if (width % DIFF_LOCAL_COMPUTE_SIZE != 0 || height % DIFF_LOCAL_COMPUTE_SIZE != 0) {
        printf("Width and height must be multiples of %d\n", DIFF_LOCAL_COMPUTE_SIZE);
        exit(1);
    }

    printf("Initializing simulation with the following parameters:\n");
    printf("SIM_WIDTH: %d, SIM_HEIGHT: %d\n", width, height);
    printf("INDIVIDUALS: %d\n", individuals);
    printf("SIMULATION_SPEED: %d\n", simulation_speed);
    printf("INDIVIDUAL_SAMPLE_DISTANCE: %d\n", sample_distance);
    printf("INDIVIDUAL_TURN_RATE: %d\n", turn_rate);
    printf("BLUR_WEIGHT: %.3f\n", blur_weight);
    printf("DECAY_RATE: %.3f\n", decay_rate);
    printf("\n");

    // Init SDL with OpenGL context
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window* window = SDL_CreateWindow("Physarum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(window);

    // VSYNC OFF
    SDL_GL_SetSwapInterval(0);

    if (!gladLoadGL()) {
        printf("Unable to load OpenGL\n");
        return -1;
    }

#ifdef ENABLE_OPENGL_DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_msg_callback, 0);
#endif
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Fill buffer with individual data
    float* sim_data = (float*)malloc(sizeof(float) * 4 * individuals);

    for (uint32_t i = 0; i < individuals; i++) {
        float angle = rand() / (float)RAND_MAX * 6.28319f;

        sim_data[i * 4] = sin(angle) * rand() / (float)RAND_MAX * width / 4.0f + width / 2.0f;
        sim_data[i * 4 + 1] = cos(angle) * rand() / (float)RAND_MAX * height / 4.0f + height / 2.0f;
        sim_data[i * 4 + 2] = angle + 3.1415f;
        if (sim_data[i * 4 + 2] > 6.28319f) {
            sim_data[i * 4 + 2] -= 6.28319f;
        }
    }

    // Image VertexShader
    Shader* tex_vs = shader_create();
    shader_load_from_file(tex_vs, "resources/tex.vs", GL_VERTEX_SHADER);

    // Image FragmentShader
    Shader* tex_fs = shader_create();
    shader_load_from_file(tex_fs, "resources/tex.fs", GL_FRAGMENT_SHADER);

    // Simulation ComputeShader
    Shader* sim_cs = shader_create();
    shader_load_from_file(sim_cs, "resources/sim.comp", GL_COMPUTE_SHADER);

    // Diffuse ComputeShader
    Shader* diffuse_cs = shader_create();
    shader_load_from_file(diffuse_cs, "resources/diffuse.comp", GL_COMPUTE_SHADER);

    // Texture ShaderProgram
    ShaderProgram* tex_sp = shader_program_create();
    shader_program_link(tex_sp, tex_vs, tex_fs);

    // Simulation ShaderProgram
    ShaderProgram* sim_sp = shader_program_create();
    shader_program_link_compute(sim_sp, sim_cs);

    // Diffuse ShaderProgram
    ShaderProgram* diffuse_sp = shader_program_create();
    shader_program_link_compute(diffuse_sp, diffuse_cs);

    // Define buffers and textures
    float vertices[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f};
    uint32_t indices[] = {0, 1, 3, 1, 2, 3};
    uint32_t tex_vao, tex_vbo, tex_ebo, tex, sim_cs_input_buffer, sim_cs_delta_buffer;

    // Delta
    float delta = 0.0f;

    glGenVertexArrays(1, &tex_vao);
    glGenBuffers(1, &tex_vbo);
    glGenBuffers(1, &tex_ebo);

    glBindVertexArray(tex_vao);
    glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tex_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenBuffers(1, &sim_cs_input_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sim_cs_input_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 4 * individuals, sim_data, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, sim_cs_input_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &sim_cs_delta_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sim_cs_delta_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &delta, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, sim_cs_delta_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    Texture* font_texture = texture_create();
    texture_load_from_file(font_texture, "resources/MyAsciiFont_BBG.png");

    TextRenderer* text_renderer = text_renderer_create();

    float identity[] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

    // Set shader program uniforms
    shader_program_use(sim_sp);
    shader_program_set_uniform_int(sim_sp, "simulation_width", width);
    shader_program_set_uniform_int(sim_sp, "simulation_height", height);
    shader_program_set_uniform_int(sim_sp, "simulation_speed", simulation_speed);
    shader_program_set_uniform_int(sim_sp, "sample_distance", sample_distance);
    shader_program_set_uniform_int(sim_sp, "turn_rate", turn_rate);

    shader_program_use(diffuse_sp);
    shader_program_set_uniform_int(diffuse_sp, "image_width", width);
    shader_program_set_uniform_int(diffuse_sp, "image_height", height);
    shader_program_set_uniform_float(diffuse_sp, "blur_weight", blur_weight);
    shader_program_set_uniform_float(diffuse_sp, "decay_rate", decay_rate);

    Stopwatch* cs_stopwatch = stopwatch_create();
    Stopwatch* display_stopwatch = stopwatch_create();

    struct timeval last, current;
    gettimeofday(&current, NULL);
    srand(time(NULL));

    float fps_display_delta = 0.0f;
    float current_fps = 0.0f;
    int frames_last_interval = 0;

    // Update loop
    int32_t running = 1;
    while (running) {
        last = current;
        gettimeofday(&current, NULL);
        delta = (current.tv_usec - last.tv_usec) / 1000000.0f + (current.tv_sec - last.tv_sec);

        frames_last_interval++;
        fps_display_delta += delta;
        if (fps_display_delta >= 1.0f) {
            fps_display_delta = 0.0f;
            current_fps = frames_last_interval;
            frames_last_interval = 0;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        stopwatch_start(cs_stopwatch);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, sim_cs_delta_buffer);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float), &delta);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        shader_program_use(sim_sp);
        glDispatchCompute(individuals / INDIVIDUALS_PER_WORKGROUP, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        shader_program_use(diffuse_sp);
        glDispatchCompute(width / DIFF_LOCAL_COMPUTE_SIZE, height / DIFF_LOCAL_COMPUTE_SIZE, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        stopwatch_end(cs_stopwatch);
        stopwatch_start(display_stopwatch);

        shader_program_use(tex_sp);
        shader_program_set_uniform_int(tex_sp, "tex", 0);
        shader_program_set_uniform_mat4(tex_sp, "m", identity);

        glBindVertexArray(tex_vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        if (show_fps_counter) {
            char fps_text_buffer[32];
            sprintf(fps_text_buffer, "%.1f FPS", current_fps);
            text_renderer_draw_text(text_renderer, fps_text_buffer, font_texture, tex_sp, -20, 19, 0.05f);
        }

        stopwatch_end(display_stopwatch);

        SDL_GL_SwapWindow(window);
    }

    printf("Average time compute shader: %f ms\n", stopwatch_average_seconds(cs_stopwatch) * 1000.0f);
    printf("Average time display: %f ms\n", stopwatch_average_seconds(display_stopwatch) * 1000.0f);

    free(sim_data);
    return 0;
}