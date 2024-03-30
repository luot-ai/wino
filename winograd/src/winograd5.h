#ifndef __WINOGRAD5_H__
#define __WINOGRAD5_H__

void winograd5_2d(float* U, float* d, float* result);
void convolutional_winograd5(float* transformed_g, float* d, float* result, int height, int width, int channels, int n,
                             int m, int r);
void winograd5_2d_custom(float* U, float* d, float* result);

static void ld_tile0(int addr);
static void ld_tile1(int addr);
static void ld_tile2(int addr);
static void ld_tile3(int addr);
static void ld_tile4(int addr);
static void ld_tile5(int addr);
static void ld_tile6(int addr);
static void ld_tile7(int addr);
static void ld_tile8(int addr);
static void wb_tile(int addr);
static void aamul_02();
static void aamul_31();
static void aamul_1221();
static void triadd_012();
static void triadd_321();
static void oacc();   


#endif /* __WINOGRAD5_H__ */                             