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


    for (int i = 0; i < 4; i++)
        BTd[i] = d[0 + i] - d[8 + i];
    for (int i = 0; i < 4; i++)
        BTd[4 + i] = d[4 + i] + d[8 + i];
    for (int i = 0; i < 4; i++)
        BTd[8 + i] = -d[4 + i] + d[8 + i];
    for (int i = 0; i < 4; i++)
        BTd[12 + i] = d[4 + i] - d[12 + i];

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
    // for (int i= 0; i<4;i++)
    // {
    //     printf("group %d : ",i);
    //     for ( int j=0;j<4;j++)
    //     {
    //         printf("%f ",UV[i*4+j]);
    //     }
    //     printf("\n");
    // }

    for (int i = 0; i < 4; i++)
        ATUV[i] = UV[0 + i] + UV[4 + i] + UV[8 + i];
    for (int i = 0; i < 4; i++)
        ATUV[4 + i] = UV[4 + i] - UV[8 + i] - UV[12 + i];

    result[0] += (ATUV[0] + ATUV[1] + ATUV[2]);
    result[2] += (ATUV[4] + ATUV[5] + ATUV[6]);
    result[1] += (ATUV[1] - ATUV[2] - ATUV[3]);
    result[3] += (ATUV[5] - ATUV[6] - ATUV[7]);

}
void convolutional_winograd5(float* transformed_g, float* d, float* result, int height, int width, int channels, int n,
                             int m, int r ,int pad) {
    int height_col = (height - 2 + 2 * pad) / m;  //纵坐标上有多少个tile
    int width_col = (width - 2 + 2 * pad) / m;    //横坐标上有多少个tile

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
        temp_d_nn = nn * height_col_width_col_4;
        for (int c = 0; c < channels; c++)  //卷积核通道循环
        {
            temp_U_c = c * height_col_width_col_16;
            for (int h = 0; h < height_col; h++) {
                temp_U_h = h * width_col_16;
                temp_d_h = h * width_col_4;
                for (int w = 0; w < width_col; w++)
                    winograd5_2d(nn * channels * 16 + c * 16 + transformed_g, temp_U_c + temp_U_h + w * 16 + d,
                                 temp_d_nn + temp_d_h + w * 4 + result);  
            }
        }
    }
}

void convolutional_winograd5_cus(float* transformed_g, float* d, float* result, int height, int width, int channels, int n,
                             int m, int r ,int pad) {
                                
    int height_col = (height - 2 + 2*pad) / m;  //纵坐标上有多少个tile
    //printf("height_col = %d\n",height_col);
    int width_col = (width - 2 + 2*pad) / m;    //横坐标上有多少个tile
    //printf("width_col = %d\n",width_col);
    int thw = height_col * width_col ;
    int width_col_16 = width_col * 16;
    int height_col_width_col_16 = height_col * width_col_16;
    int channels_height_col_width_col_16 = channels * height_col_width_col_16;
    int width_col_4 = width_col * 4;
    int height_col_width_col_4 = height_col * width_col_4;
    // int channels_height_col_width_col_4 = channels * height_col_width_col_4;
    int temp_U_nn, temp_U_c, temp_U_h;
    int temp_d_nn, temp_d_c, temp_d_h;


    int wcnt =0;
    int daddr = 0;
    int oaddr = 0 ;
    for (int nn = 0; nn < n; nn++)  //卷积核个数循环
    {
        for (int c = 0; c < channels; c++)  //卷积核通道循环
        {
            float* ker_addr = nn * channels * 16 + c * 16 + transformed_g;
            //m5_dump_reset_stats(0,0);
            ld_tile4(ker_addr);
            ld_tile5(ker_addr+4);
            ld_tile6(ker_addr+8);
            ld_tile7(ker_addr+12);
            //m5_dump_reset_stats(0,0);
            temp_U_h = 0 ;
            temp_d_h =  0;
            m5_dump_reset_stats(0,0);
            float* dinline = daddr +d;
            for (int hw=0 ; hw < thw ; hw ++)
            {
                ld_tile0(daddr +d);
                ld_tile1(daddr +d + 4);
                ld_tile2(daddr +d + 8);
                ld_tile3(daddr +d + 12);
                aamul_02();
                aamul_12();
                aamul_21();
                aamul_31();
                triadd_012();
                ld_tile8(oaddr + result);
                triadd_321();
                daddr += 16;
                oacc();
                wb_tile(oaddr + result);
                oaddr += 4;
            }
            m5_dump_reset_stats(0,0);
            oaddr -= height_col_width_col_4;
        }
        oaddr+=height_col_width_col_4;
    }
}

void convolutional_winograd5_cus_fuse(float* transformed_g, float* d, float* result, int height, int width, int channels, int n,
                             int m, int r ,int pad) {
                                
    int height_col = (height - 2 + 2*pad) / m;  //纵坐标上有多少个tile
    //printf("height_col = %d\n",height_col);
    int width_col = (width - 2 + 2*pad) / m;    //横坐标上有多少个tile
    //printf("width_col = %d\n",width_col);
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
        temp_d_nn = nn * height_col_width_col_4;
        for (int c = 0; c < channels; c++)  //卷积核通道循环
        {
            temp_U_c = c * height_col_width_col_16;
            float* ker_addr = nn * channels * 16 + c * 16 + transformed_g;
            //m5_dump_reset_stats(0,0);
            m5_dump_reset_stats(0,0);
            ld_tile4(ker_addr);
            ld_tile5(ker_addr+4);
            ld_tile6(ker_addr+8);
            ld_tile7(ker_addr+12);
            //ld_tile0(d);
            m5_dump_reset_stats(0,0);
            //m5_dump_reset_stats(0,0);
            for (int h = 0; h < height_col; h++) {
                temp_U_h = h * width *2;
                temp_d_h = h * width_col_4;
                m5_dump_reset_stats(0,0);
                for (int w = 0; w < width_col; w++)
                {
                    m5_dump_reset_stats(0,0);
                    printf("width=%d\n",temp_U_c + temp_U_h + w * 2); 
                    winograd5_2d_custom_fuse(temp_U_c + temp_U_h + w * 2 + d,temp_d_nn + temp_d_h + w * 4 + result,width);
                    //printf("%d:%f",h*width_col+w,result[2]);
                }
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
    //m5_dump_reset_stats(0,0);
    // for (int i = 0; i < 4; i++) {
    //    printf("%f ", result1[i]);
    // }
    winograd5_2d(g,d,result1);
    //m5_dump_reset_stats(0,0);
    for (int i = 0; i < 4; i++) {
       printf("%f ", result1[i]);
    }
    //m5_dump_reset_stats(0,0);
    ld_tile4(g);
    ld_tile5(g+4);
    ld_tile6(g+8);
    ld_tile7(g+12);
    // for (int i = 0; i < 4; i++) {
    //    printf("%f ", result2[i]);
    // }
    winograd5_2d_custom(d,result2);
    m5_dump_reset_stats(0,0);
    for (int i = 0; i < 4; i++) {
       printf("%f ", result2[i]);
    }
}

void winograd5_2d_custom_fuse(float* d, float* result, int width) {
    // for(int i=0;i<16;i++){
    //     printf("d[%d]=%f\n",i,d[i]);
    // }
    //m5_dump_reset_stats(0,0);
    ld_tile8(result);
    ld_tile0(d);
    ld_tile3(d+6);
    ld_tile1(d);
    ld_tile2(d);

    aamul_02();
    aamul_31();
    aamul_12();
    aamul_21();
    triadd_012();
    triadd_321();
    oacc();

    wb_tile(result);
}


void winograd5_2d_custom(float* d, float* result) {
    // for(int i=0;i<16;i++){
    //     printf("d[%d]=%f\n",i,d[i]);
    // }
    //m5_dump_reset_stats(0,0);
    
    ld_tile0(d);
    ld_tile1(d+4);
    ld_tile2(d+8);
    ld_tile3(d+12);
    
    aamul_02();
    aamul_12();
    aamul_21();
    aamul_31();
    ld_tile8(result);
    triadd_012();
    triadd_321();
    oacc();

    wb_tile(result);
}

void winograd_2d_cus(float* im, float* kernel, float* ofmap, int h, int w, int c, int n,
                            int out_w,int out_h, int pad,int stride)
{
    int m = 2 ;
    int r = 3 ;
    int size = 3 ;
    float* d_5 = im ;
    float* g_5 = kernel ;
    float* transformed_d_5 =
        (float*)malloc((w - 2 + 2 * pad) / 2 * (h - 2 + 2*pad) / 2 * c * 16 * sizeof(float));  //存储经过im2col的输入feature map
    float* transformed_g_5 = calloc(n * c * 16, sizeof(float));
    float* output_temp_5 = calloc(out_w * out_h * n, sizeof(float));
    m5_dump_reset_stats(0,0);
    transforme_g_winograd2(g_5, transformed_g_5, c, n);
    im2col_winograd1(d_5, c, h, w, size, stride, 2, 3, transformed_d_5, pad);
    convolutional_winograd5_cus(transformed_g_5, transformed_d_5, output_temp_5, h, w, c, n, 2, 3,pad);
    col2im_winograd1(output_temp_5, n, h, w, size, stride, pad, m, ofmap);
    m5_dump_reset_stats(0,0);
    free(transformed_d_5);
    free(transformed_g_5);
    free(output_temp_5);
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
static void aamul_12() {
    __asm__ __volatile__(
        "aamulb x0,x0,x0"
        :
        :
    );
}
static void aamul_21() {
    __asm__ __volatile__(
        "aamulc x0,x0,x0"
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