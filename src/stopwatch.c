#include "stopwatch.h"

Stopwatch* stopwatch_create() {
    Stopwatch* stopwatch = (Stopwatch*)malloc(sizeof(Stopwatch));
    stopwatch->counts = 0;
    stopwatch->total_time = 0.0f;
    return stopwatch;
}

void stopwatch_free(Stopwatch* stopwatch) {
    if (stopwatch) {
        free(stopwatch);
    }
}

void stopwatch_start(Stopwatch* stopwatch) {
    gettimeofday(&stopwatch->start, NULL);
}

void stopwatch_end(Stopwatch* stopwatch) {
    stopwatch->counts++;
    gettimeofday(&stopwatch->end, NULL);
    stopwatch->total_time += (stopwatch->end.tv_usec - stopwatch->start.tv_usec) / 1000000.0f + (stopwatch->end.tv_sec - stopwatch->start.tv_sec);
}

float stopwatch_total_seconds(Stopwatch* stopwatch) {
    return stopwatch->total_time;
}

float stopwatch_average_seconds(Stopwatch* stopwatch) {
    return stopwatch->total_time / stopwatch->counts;
}