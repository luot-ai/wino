/*
 * @Author: lt 1035768203@qq.com
 * @Date: 2024-03-16 09:53:17
 * @LastEditors: lt 1035768203@qq.com
 * @LastEditTime: 2024-03-30 10:08:04
 * @FilePath: \accler\darknet\LearnAndTry\winograd\src\winograd5.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "winograd5.h"
#include "common.h"
#include "gem5/m5ops.h"

void winograd5_2d(float* U, float* d, float* result) {
    float BTd[16] = {0};
    float V[16] = {0};
    float UV[16] = {0};
    float ATUV[8] = {0};
    for (int i = 0; i<4;i++){
        printf("ld_vec %i :",i);
        for (int j =0 ; j<4;j++)
        {
            printf("%f,",d[i*4+j]);
        }
        printf("\n");
    }
    for (int i = 0; i<4;i++){
        printf("kernel_vec %i :",i);
        for (int j =0 ; j<4;j++)
        {
            printf("%f,",U[i*4+j]);
        }
        printf("\n");
    }
    // dot(BT, 4, 4, d, 4, 4, BTd);
    for (int i = 0; i < 4; i++)
        BTd[i] = d[0 + i] - d[8 + i];
    for (int i = 0; i < 4; i++)
        BTd[4 + i] = d[4 + i] + d[8 + i];
    for (int i = 0; i < 4; i++)
        BTd[8 + i] = -d[4 + i] + d[8 + i];
    for (int i = 0; i < 4; i++)
        BTd[12 + i] = d[4 + i] - d[12 + i];

    // dot(BTd, 4, 4, B, 4, 4, V);
    for (int i = 0; i < 4; i++)
        V[0 + i * 4] = BTd[0 + i * 4] - BTd[2 + i * 4];
    for (int i = 0; i < 4; i++)
        V[1 + i * 4] = BTd[1 + i * 4] + BTd[2 + i * 4];
    for (int i = 0; i < 4; i++)
        V[2 + i * 4] = -BTd[1 + i * 4] + BTd[2 + i * 4];
    for (int i = 0; i < 4; i++)
        V[3 + i * 4] = BTd[1 + i * 4] - BTd[3 + i * 4];

    // for (int i=0;i<16;i++)
    //     UV[i]=U[i]*V[i];
    multi(U, 4, 4, V, 4, 4, UV);
    for (int i = 0; i<4;i++){
        printf("vec %i :",i);
        for (int j =0 ; j<4;j++)
        {
            printf("%f,",UV[i*4+j]);
        }
        printf("\n");
    }

    // dot(AT, 2, 4, UV, 4, 4, ATUV);
    for (int i = 0; i < 4; i++)
        ATUV[i] = UV[0 + i] + UV[4 + i] + UV[8 + i];
    for (int i = 0; i < 4; i++)
        ATUV[4 + i] = UV[4 + i] - UV[8 + i] - UV[12 + i];

    for (int i = 0; i<2;i++){
        printf("atuv vec %i :",i);
        for (int j =0 ; j<4;j++)
        {
            printf("%f,",ATUV[i*4+j]);
        }
        printf("\n");
    }
    result[0] += (ATUV[0] + ATUV[1] + ATUV[2]);
    result[2] += (ATUV[4] + ATUV[5] + ATUV[6]);
    result[1] += (ATUV[1] - ATUV[2] - ATUV[3]);
    result[3] += (ATUV[5] - ATUV[6] - ATUV[7]);
    // result[0] = (ATUV[0] + ATUV[1] + ATUV[2]);
    // result[2] = (ATUV[4] + ATUV[5] + ATUV[6]);
    // result[1] = (ATUV[1] - ATUV[2] - ATUV[3]);
    // result[3] = (ATUV[5] - ATUV[6] - ATUV[7]);
}

void convolutional_winograd5(float* transformed_g, float* d, float* result, int height, int width, int channels, int n,
                             int m, int r) {
    int height_col = (height - 2) / m;  //纵坐标上有多少个tile
    int width_col = (width - 2) / m;    //横坐标上有多少个tile

    int width_col_16 = width_col * 16;
    int height_col_width_col_16 = height_col * width_col_16;
    int channels_height_col_width_col_16 = channels * height_col_width_col_16;
    int width_col_4 = width_col * 4;
    int height_col_width_col_4 = height_col * width_col_4;
    // int channels_height_col_width_col_4 = channels * height_col_width_col_4;
    int temp_U_nn, temp_U_c, temp_U_h;
    int temp_d_nn, temp_d_c, temp_d_h;

    for (int nn = 0; nn < n; nn++)  //卷积核个数循环
    {
        // temp_U_nn = nn * channels_height_col_width_col_16;
        temp_d_nn = nn * height_col_width_col_4;
        for (int c = 0; c < channels; c++)  //卷积核通道循环
        {
            temp_U_c = c * height_col_width_col_16;
            // float* ker_addr = nn * channels * 16 + c * 16 + transformed_g;
            // ld_tile4(ker_addr);
            // ld_tile5(ker_addr+4);
            // ld_tile6(ker_addr+8);
            // ld_tile7(ker_addr+12);
            for (int h = 0; h < height_col; h++) {
                temp_U_h = h * width_col_16;
                temp_d_h = h * width_col_4;
                for (int w = 0; w < width_col; w++)
                    // winograd5_2d_custom(temp_U_c + temp_U_h + w * 16 + d,temp_d_nn + temp_d_h + w * 4 + result);
                    winograd5_2d(nn * channels * 16 + c * 16 + transformed_g, temp_U_c + temp_U_h + w * 16 + d,
                                 temp_d_nn + temp_d_h + w * 4 + result);  // temp_U_nn ++ temp_d_c
            }
        }
    }
}

void test_inline1(float* d) {
    m5_dump_reset_stats(0,0);
    ld_tile8(d);
    m5_dump_reset_stats(0,0);
    wb_tile(d);
    m5_dump_reset_stats(0,0);
    for (int i = 0; i < 4; i++) {
       printf("%f ", d[i]);
    }
    m5_dump_reset_stats(0,0);
}

void test_inline2(float* d,float* g,float* result1,float* result2) {
    m5_dump_reset_stats(0,0);
    // for (int i = 0; i < 4; i++) {
    //    printf("%f ", result1[i]);
    // }
    winograd5_2d(g,d,result1);
    for (int i = 0; i < 4; i++) {
       printf("%f ", result1[i]);
    }
    m5_dump_reset_stats(0,0);
    ld_tile4(g);
    ld_tile5(g+4);
    ld_tile6(g+8);
    ld_tile7(g+12);
    // for (int i = 0; i < 4; i++) {
    //    printf("%f ", result2[i]);
    // }
    winograd5_2d_custom(d,result2);
    for (int i = 0; i < 4; i++) {
       printf("%f ", result2[i]);
    }
    m5_dump_reset_stats(0,0);
}

void winograd5_2d_custom(float* d, float* result) {

    ld_tile0(d);
    ld_tile3(d+12);
    ld_tile1(d+4);
    ld_tile2(d+8);
    ld_tile8(result);

    aamul_02();
    aamul_31();
    aamul_1221();
    triadd_012();
    triadd_321();
    oacc();

    wb_tile(result);
}

static void ld_tile0(float* addr){
	__asm__ __volatile__(
        "ldtilea x0,%0,x0"
		:
		:"r"(addr)
		);
}
static void ld_tile1(float* addr){
	__asm__ __volatile__(
        "ldtileb x0,%0,x0"
		:
		:"r"(addr)
		);
}
static void ld_tile2(float* addr) {
    __asm__ __volatile__(
        "ldtilec x0,%0,x0"
        :
        :"r"(addr)
    );
}
static void ld_tile3(float* addr) {
    __asm__ __volatile__(
        "ldtiled x0,%0,x0"
        :
        :"r"(addr)
    );
}
static void ld_tile4(float* addr) {
    __asm__ __volatile__(
        "ldtilee x0,%0,x0"
        :
        :"r"(addr)
    );
}
static void ld_tile5(float* addr) {
    __asm__ __volatile__(
        "ldtilef x0,%0,x0"
        :
        :"r"(addr)
    );
}
static void ld_tile6(float* addr) {
    __asm__ __volatile__(
        "ldtileg x0,%0,x0"
        :
        :"r"(addr)
    );
}
static void ld_tile7(float* addr) {
    __asm__ __volatile__(
        "ldtileh x0,%0,x0"
        :
        :"r"(addr)
    );
}
static void ld_tile8(float* addr) {
    __asm__ __volatile__(
        "ldtileo x0,%0,x0"
        :
        :"r"(addr)
    );
}
static void wb_tile(float* addr) {
    __asm__ __volatile__(
        "wbtile x0,%0,x0"
        :
        :"r"(addr)
    );
}

static void aamul_02() {
    __asm__ __volatile__(
        "aamula x0,x0,x0"
        :
        :
    );
}
static void aamul_31() {
    __asm__ __volatile__(
        "aamuld x0,x0,x0"
        :
        :
    );
}
static void aamul_1221() {
    __asm__ __volatile__(
        "aamulbc x0,x0,x0"
        :
        :
    );
}
static void triadd_012() {
    __asm__ __volatile__(
        "triadda x0,x0,x0"
        :
        :
    );
}
static void triadd_321() {
    __asm__ __volatile__(
        "triaddb x0,x0,x0"
        :
        :
    );
}
static void oacc() {
    __asm__ __volatile__(
        "oacc x0,x0,x0"
        :
        :
    );
}