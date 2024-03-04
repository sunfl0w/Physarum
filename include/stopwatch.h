#include <sys/time.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    struct timeval start;
    struct timeval end;
    uint32_t counts;
    float total_time;
} Stopwatch;

Stopwatch* stopwatch_create();

void stopwatch_free(Stopwatch* stopwatch);

void stopwatch_start(Stopwatch* stopwatch);

void stopwatch_end(Stopwatch* stopwatch);

float stopwatch_total_seconds(Stopwatch* stopwatch);

float stopwatch_average_seconds(Stopwatch* stopwatch);