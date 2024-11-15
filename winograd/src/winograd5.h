#ifndef __WINOGRAD5_H__
#define __WINOGRAD5_H__

void winograd_2d_cus(float* im, float* kernel, float* ofmap, int h, int w, int c, int n,
                            int out_w,int out_h, int pad,int stride); 
void winograd5_2d(float* U, float* d, float* result);
void convolutional_winograd5(float* transformed_g, float* d, float* result, int height, int width, int channels, int n,
                             int m, int r,int pad);
void convolutional_winograd5_cus(float* transformed_g, float* d, float* result, int height, int width, int channels, int n,
                             int m, int r , int pad); 
void convolutional_winograd5_cus_fuse(float* transformed_g, float* d, float* result, int height, int width, int channels, int n,
                             int m, int r ,int pad);             
void winograd5_2d_custom(float* d, float* result);
void winograd5_2d_custom_fuse(float* d, float* result,int width);
void test_inline1(float* d);
void test_inline2(float* d,float* g,float* result1,float* result2);
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
static void aamul_12();
static void aamul_21();
static void aamul_1221();
static void triadd_012();
static void triadd_321();
static void oacc();   


#endif /* __WINOGRAD5_H__ */                             