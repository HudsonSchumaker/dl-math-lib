#include "../include/mathlib.h"

#define MAX_DIVISOR 128
#define SIN_LUT_SIZE 2048
#define ANGLE_MASK (SIN_LUT_SIZE - 1)
#define TABLE_SCALE (SIN_LUT_SIZE / 360.0f)

static float sin_table[SIN_LUT_SIZE];
float reciprocal_table[MAX_DIVISOR + 1];

void build_trigo_tables(void) {
    for (int i = 0; i < SIN_LUT_SIZE; i++) {
        float angle =  ((float)i / SIN_LUT_SIZE) * ML_2PI;
        sin_table[i] = sinf(angle);
    }
}

int degrees_to_index(float degrees) {
    return (int)(degrees * TABLE_SCALE) & ANGLE_MASK;
}
float de_sinf(int angle) {
    return sin_table[angle & ANGLE_MASK];
}

float de_cosf(int angle) {
    return sin_table[(angle + (SIN_LUT_SIZE >> 2)) & ANGLE_MASK];
}

float de_tanf(int angle) {
    float cos = de_cosf(angle);
    if (cos > -0.0001f && cos < 0.0001f) {
        return 0.0f;
    }

    return de_sinf(angle) / cos;
}

void build_reciprocal_table(void) {
    reciprocal_table[0] = 0.0f;

    for (int i = 1; i <= MAX_DIVISOR; i++) {
        reciprocal_table[i] = 1.0f / (float)i;
    }
}
