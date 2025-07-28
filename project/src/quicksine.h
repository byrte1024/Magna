#pragma once

// quicksine.h

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "util.h"
#include "memoryBlock.h"

#include <math.h>

#define PI 3.14159265358979323846
#define QSINE_SAMPLES (1 << 10)
#define TWO_PI (2.0 * PI)
#define HALF_PI (0.5 * PI)
#define THREE_HALVES_PI (1.5 * PI)

//lookup tables
extern double qsine_segmant_db[QSINE_SAMPLES];

void m_quicksine_init();

static inline double m_sin(double x) {
    // Normalize to [0, 2PI)
    x = fmod(x, TWO_PI);
    if (x < 0) x += TWO_PI;

    double theta = 0.0;
    bool flipSign = false;

    if (x < HALF_PI) {
        theta = x;
    }
    else if (x < PI) {
        theta = PI - x;
    }
    else if (x < THREE_HALVES_PI) {
        theta = x - PI;
        flipSign = true;
    }
    else {
        theta = TWO_PI - x;
        flipSign = true;
    }

    // Map theta (0..PI/2) to index range
    double t = theta / HALF_PI;  // [0,1]
    double exactIndex = t * (QSINE_SAMPLES - 1);
    int idx = (int)exactIndex;
    double frac = exactIndex - idx;

    // Linear interpolation between qsine_segmant_db[idx] and qsine_segmant_db[idx+1]
    double a = qsine_segmant_db[idx];
    double b = qsine_segmant_db[idx + 1];  // safe if QSINE_SAMPLES >= 2
    double result = a + (b - a) * frac;

    if (flipSign) result = -result;
    return result;
}


static inline double m_cos(double x) {
	// cos(x) = sin(x + PI/2)
	return m_sin(x + HALF_PI);
}

static inline double m_tan(double x) {
	// tan(x) = sin(x) / cos(x)
	double sin_x = m_sin(x);
	double cos_x = m_cos(x);
	if (cos_x == 0.0) {
		fprintf(stderr, "Error: Division by zero in tan(%f)\n", x);
		return 0.0; // or handle error appropriately
	}
	return sin_x / cos_x;
}
