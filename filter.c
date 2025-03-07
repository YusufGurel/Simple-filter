#include "filter.h"
#include <string.h>
#include <math.h>

void boxcar_init(boxcar_t *filter, uint8_t window_size) {
    filter->size = window_size;
    filter->sum = 0;
    filter->index = 0;
    #if MEM_TYPE == 0
        memset(filter->data, 0, window_size * sizeof(float));
    #else
        filter->data = (float *)malloc(window_size * sizeof(float));
        memset(filter->data, 0, window_size * sizeof(float));
    #endif
}

void boxcar_filter(boxcar_t *filter, float input, float *output) {
    filter->sum = filter->sum - filter->data[filter->index] + input;
    filter->data[filter->index] = input;
    filter->index = (filter->index + 1) % filter->size;

    *output = filter->sum / filter->size;
}

void boxcar_free(boxcar_t *filter) {
    free(filter->data);
}

#ifdef EMA
void ema_init(ema_t *filter, float alpha) {
    filter->alpha = alpha;
    filter->prev_output = 0.0f;
    filter->initialized = 0;
}

void ema_filter(ema_t *filter, float input, float *output) {
    if (!filter->initialized) {
        filter->prev_output = input;
        filter->initialized = 1;
    } else {
        filter->prev_output = filter->alpha * input + (1 - filter->alpha) * filter->prev_output;
    }
    *output = filter->prev_output;
}
#endif

#ifdef GAUSSIAN
void calculate_gaussian_kernel(float *kernel, uint8_t window_size, float sigma) {
    int half_size = window_size / 2;
    float sum = 0.0;
    for (int i = 0; i < window_size; i++) {
        int x = i - half_size;
        kernel[i] = expf(-(x * x) / (2 * sigma * sigma));
        sum += kernel[i];
    }

    // Normalize the kernel
    for (int i = 0; i < window_size; i++) {
        kernel[i] /= sum;
    }
}

void gaussian_init(gaussian_t *filter, uint8_t window_size, float sigma) {
    filter->window_size = window_size;
    filter->index = 0;
    filter->initialized = 0;
    #if MEM_TYPE == 0
        calculate_gaussian_kernel(filter->kernel, window_size, sigma);
        memset(filter->data, 0, window_size * sizeof(float));
    #else
        filter->kernel = (float *)malloc(window_size * sizeof(float));
        filter->data = (float *)malloc(window_size * sizeof(float));
        calculate_gaussian_kernel(filter->kernel, window_size, sigma);
        memset(filter->data, 0, window_size * sizeof(float));
    #endif
}

void gaussian_filter(gaussian_t *filter, float input, float *output) {
    filter->data[filter->index] = input;
    if (filter->index == filter->window_size - 1) {
        filter->initialized = 1;
    }

    float result = 0.0;
    if (filter->initialized) {
        for (int i = 0; i < filter->window_size; i++) {
            result += filter->kernel[i] * filter->data[(filter->index + i) % filter->window_size];
        }
    } else {
        for (int i = 0; i <= filter->index; i++) {
            result += filter->kernel[i] * filter->data[i];
        }
    }
    *output = result;

    filter->index = (filter->index + 1) % filter->window_size;
}

void gaussian_free(gaussian_t *filter) {
    #if MEM_TYPE == 1
        free(filter->kernel);
        free(filter->data);
    #endif
}
#endif

#ifdef MEDIAN
int compare(const void *a, const void *b) {
    return (*(float *)a - *(float *)b);
}

void median_init(median_t *filter, uint8_t window_size) {
    filter->window_size = window_size;
    filter->index = 0;
    filter->initialized = 0;
    #if MEM_TYPE == 0
        memset(filter->data, 0, window_size * sizeof(float));
    #else
        filter->data = (float *)malloc(window_size * sizeof(float));
        memset(filter->data, 0, window_size * sizeof(float));
    #endif
}

void median_filter(median_t *filter, float input, float *output) {
    filter->data[filter->index] = input;
    if (filter->index == filter->window_size - 1) {
        filter->initialized = 1;
    }

    float temp[WINDOW_SIZE];
    memcpy(temp, filter->data, filter->window_size * sizeof(float));
    qsort(temp, filter->window_size, sizeof(float), compare);

    if (filter->initialized) {
        *output = temp[filter->window_size / 2];
    } else {
        *output = temp[filter->index / 2];
    }

    filter->index = (filter->index + 1) % filter->window_size;
}

void median_free(median_t *filter) {
    #if MEM_TYPE == 1
        free(filter->data);
    #endif
}
#endif
