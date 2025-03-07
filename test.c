#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "filter.h"

#define DATA_SIZE 100
#define PEAK_PERIOD 5

int main() {
    boxcar_t boxcar;
    ema_t ema;
    gaussian_t gaussian;
    median_t median;

    float input[DATA_SIZE];
    float boxcar_output[DATA_SIZE];
    float ema_output[DATA_SIZE];
    float gaussian_output[DATA_SIZE];
    float median_output[DATA_SIZE];



    // Seed the random number generator
    srand(time(NULL));

    // Generate input data with some noise
    for (int i = 0; i < DATA_SIZE; i++) {
        input[i] = 10 + ((i % PEAK_PERIOD == 0) ? (rand() % 20) : 0); // Base value with noise
    }

    // Initialize filters
    boxcar_init(&boxcar, WINDOW_SIZE);
    ema_init(&ema, 0.1f);
    gaussian_init(&gaussian, WINDOW_SIZE, 1.0f);
    median_init(&median, WINDOW_SIZE);

    // Apply filters
    for (int i = 0; i < DATA_SIZE; i++) {
        boxcar_filter(&boxcar, input[i], &boxcar_output[i]);
        ema_filter(&ema, input[i], &ema_output[i]);
        gaussian_filter(&gaussian, input[i], &gaussian_output[i]);
        median_filter(&median, input[i], &median_output[i]);
        printf("Input: %.2f, Boxcar Output: %.2f, EMA Output: %.2f, Gaussian Output: %.2f, Median Output: %.2f\n", input[i], boxcar_output[i], ema_output[i], gaussian_output[i], median_output[i]);
    }

    #if MEM_TYPE == 1
        boxcar_free(&boxcar);
        gaussian_free(&gaussian);
        median_free(&median);
    #endif

    return 0;
}
