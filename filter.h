#ifndef _FILTER_H
#define _FILTER_H

#include <stdint.h>
#include <stdlib.h>

#define BOXCAR
#define EMA
#define GAUSSIAN
#define MEDIAN

#define MEM_TYPE 1 // 0 for static, 1 for dynamic
#define WINDOW_SIZE 20

#ifdef BOXCAR
    typedef struct {
        uint8_t size;
        uint8_t index;

        #if MEM_TYPE == 0
            int16_t data[WINDOW_SIZE];
        #else
            float *data;
        #endif
        float sum;        
    } boxcar_t;

    void boxcar_init(boxcar_t *filter, uint8_t window_size);
    void boxcar_filter(boxcar_t *filter, float input, float *output);

    #if MEM_TYPE == 1
        void boxcar_free(boxcar_t *filter);
    #endif
#endif

#ifdef EMA
    typedef struct {
        float alpha;
        float prev_output;
        uint8_t initialized;
    } ema_t;

    void ema_init(ema_t *filter, float alpha);
    void ema_filter(ema_t *filter, float input, float *output);
#endif

#ifdef GAUSSIAN
    typedef struct {
        uint8_t window_size;
        uint8_t index;
        #if MEM_TYPE == 0
            float kernel[WINDOW_SIZE];
            float data[WINDOW_SIZE];
        #else
            float *kernel;
            float *data;
        #endif
        uint8_t initialized;
    } gaussian_t;

    void gaussian_kernel(float *kernel, uint8_t window_size, float sigma);
    void gaussian_init(gaussian_t *filter, uint8_t window_size, float sigma);
    void gaussian_filter(gaussian_t *filter, float input, float *output);
    void gaussian_free(gaussian_t *filter);

#endif

#ifdef MEDIAN
    typedef struct {
        uint8_t window_size;
        uint8_t index;
        #if MEM_TYPE == 0
            float data[WINDOW_SIZE];
        #else
            float *data;
        #endif
        uint8_t initialized;
    } median_t;

    void median_init(median_t *filter, uint8_t window_size);
    void median_filter(median_t *filter, float input, float *output);
    void median_free(median_t *filter);
#endif

#endif /* _FILTER_H */