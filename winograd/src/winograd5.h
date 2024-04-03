#ifndef __WINOGRAD5_H__
#define __WINOGRAD5_H__

void winograd5_2d(float* U, float* d, float* result);
void convolutional_winograd5(float* transformed_g, float* d, float* result, int height, int width, int channels, int n,
                             int m, int r);
void winograd5_2d_custom(float* d, float* result);
void test_inline(float* d);
static void ld_tile0(float* addr);
static void ld_tile1(float* addr);
static void ld_tile2(float* addr);
static void ld_tile3(float* addr);
static void ld_tile4(float* addr);
static void ld_tile5(float* addr);
static void ld_tile6(float* addr);
static void ld_tile7(float* addr);
static void ld_tile8(float* addr);
static void wb_tile(float* addr);
static void aamul_02();
static void aamul_31();
static void aamul_1221();
static void triadd_012();
static void triadd_321();
static void oacc();   


#endif /* __WINOGRAD5_H__ */                             