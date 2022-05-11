#include "deps/glad/glad.h"
#include "gl_error_handeling.h"
#include "shader.h"
#include "shader_program.h"

#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define SCREEN_WIDTH 1800
#define SCREEN_HEIGHT 1200

#define SIMULATION_WIDTH 1800
#define SIMULATION_HEIGHT 1200
#define NUM_INDIVIDUALS 1000000

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window* window = SDL_CreateWindow("Physarum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    // VSYNC
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

    float* sim_data = (float*)malloc(sizeof(float) * 4 * NUM_INDIVIDUALS);
    float delta = 0.0f;
    struct timeval last, current;
    gettimeofday(&current, NULL);
    srand(time(NULL));

    for (uint32_t i = 0; i < NUM_INDIVIDUALS; i++) {
        float angle = rand() / (float)RAND_MAX * 6.28319f;

        sim_data[i * 4] = sin(angle) * rand() / (float)RAND_MAX * 300 + SIMULATION_WIDTH / 2.0f;
        sim_data[i * 4 + 1] = cos(angle) * rand() / (float)RAND_MAX * 300 + SIMULATION_HEIGHT / 2.0f;
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

    float vertices[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f};

    uint32_t indices[] = {0, 1, 3, 1, 2, 3};

    uint32_t tex_vao, tex_vbo, tex_ebo, tex, sim_cs_input_buffer, sim_cs_delta_buffer;

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SIMULATION_WIDTH, SIMULATION_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenBuffers(1, &sim_cs_input_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sim_cs_input_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 4 * NUM_INDIVIDUALS, sim_data, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, sim_cs_input_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &sim_cs_delta_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, sim_cs_delta_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), &delta, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, sim_cs_delta_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    int32_t running = 1;
    while (running) {
        last = current;
        gettimeofday(&current, NULL);
        delta = (current.tv_usec - last.tv_usec) / 1000000.0f + (current.tv_sec - last.tv_sec);

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

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, sim_cs_delta_buffer);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float), &delta);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        shader_program_use(sim_sp);
        glDispatchCompute(NUM_INDIVIDUALS / 100, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        shader_program_use(diffuse_sp);
        glDispatchCompute(SIMULATION_WIDTH, SIMULATION_HEIGHT, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        shader_program_use(tex_sp);
        glUniform1i(glGetUniformLocation(tex_sp, "tex"), 0);

        glBindVertexArray(tex_vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        SDL_GL_SwapWindow(window);
    }

    free(sim_data);
    return 0;
}