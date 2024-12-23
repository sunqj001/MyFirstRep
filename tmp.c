#include <stdio.h>  
#include <stdlib.h>  

typedef struct {  
    unsigned char r;  
    unsigned char g;  
    unsigned char b;  
} RGB;  

/*-----------------------------------------*/
// 图像放大函数  
void enlarge_image(RGB* input_image, RGB* output_image, 
                    int width, int height, float scale, 
                    int* new_width, int* new_height) 
{  
    *new_width = (int)(width * scale);  // 新宽度  
    *new_height = (int)(height * scale);  // 新高度  
    
    // 双线性插值  
    for (int new_y = 0; new_y < *new_height; new_y++) {  
        for (int new_x = 0; new_x < *new_width; new_x++) {  
            // 原图中的对应位置  
            float old_x = new_x / scale;  
            float old_y = new_y / scale;  
            int x1 = (int)old_x;  
            int y1 = (int)old_y;  
            int x2 = (x1 + 1 < width) ? x1 + 1 : x1; // 确保不越界  
            int y2 = (y1 + 1 < height) ? y1 + 1 : y1; // 确保不越界  

            // 计算权重  
            float dx = old_x - x1;  
            float dy = old_y - y1;  

            // 获取四个角的像素  
            RGB p1 = input_image[y1 * width + x1];  
            RGB p2 = input_image[y1 * width + x2];  
            RGB p3 = input_image[y2 * width + x1];  
            RGB p4 = input_image[y2 * width + x2];  

            // 计算新像素的 RGB 值  
            output_image[new_y * (*new_width) + new_x].r = (unsigned char)(p1.r * (1 - dx) * (1 - dy) + p2.r * dx * (1 - dy) + p3.r * (1 - dx) * dy + p4.r * dx * dy);  
            output_image[new_y * (*new_width) + new_x].g = (unsigned char)(p1.g * (1 - dx) * (1 - dy) + p2.g * dx * (1 - dy) + p3.g * (1 - dx) * dy + p4.g * dx * dy);  
            output_image[new_y * (*new_width) + new_x].b = (unsigned char)(p1.b * (1 - dx) * (1 - dy) + p2.b * dx * (1 - dy) + p3.b * (1 - dx) * dy + p4.b * dx * dy);  
        }  
    }  

    return;  
}  

void enlarge_image2(RGB* input_image, RGB* output_image, 
                    int width, int height, 
                    float scale_w, 
                    float scale_h, 
                    int* new_width, int* new_height) 
{  
    *new_width = (int)(width * scale_w);  // 新宽度  
    *new_height = (int)(height * scale_h);  // 新高度  
    
    // 双线性插值  
    for (int new_y = 0; new_y < *new_height; new_y++) {  
        for (int new_x = 0; new_x < *new_width; new_x++) {  
            // 原图中的对应位置  
            float old_x = new_x / scale_w;  
            float old_y = new_y / scale_h;  
            int x1 = (int)old_x;  
            int y1 = (int)old_y;  
            int x2 = (x1 + 1 < width) ? x1 + 1 : x1; // 确保不越界  
            int y2 = (y1 + 1 < height) ? y1 + 1 : y1; // 确保不越界  

            // 计算权重  
            float dx = old_x - x1;  
            float dy = old_y - y1;  

            // 获取四个角的像素  
            RGB p1 = input_image[y1 * width + x1];  
            RGB p2 = input_image[y1 * width + x2];  
            RGB p3 = input_image[y2 * width + x1];  
            RGB p4 = input_image[y2 * width + x2];  

            // 计算新像素的 RGB 值  
            output_image[new_y * (*new_width) + new_x].r = (unsigned char)(p1.r * (1 - dx) * (1 - dy) + p2.r * dx * (1 - dy) + p3.r * (1 - dx) * dy + p4.r * dx * dy);  
            output_image[new_y * (*new_width) + new_x].g = (unsigned char)(p1.g * (1 - dx) * (1 - dy) + p2.g * dx * (1 - dy) + p3.g * (1 - dx) * dy + p4.g * dx * dy);  
            output_image[new_y * (*new_width) + new_x].b = (unsigned char)(p1.b * (1 - dx) * (1 - dy) + p2.b * dx * (1 - dy) + p3.b * (1 - dx) * dy + p4.b * dx * dy);  
        }  
    }  

    return;  
}  

/*-----------------------------------------------------*/

// 图像缩小函数  
void shrink_image(RGB* input_image, RGB* output_image, 
                    int width, int height, float scale, 
                    int* new_width, int* new_height) 
{  
    // 计算新宽高  
    //*new_width = (int)((width / scale) / 4) * 4;  
    //*new_height = (int)((height / scale) / 4) * 4;  
    *new_width = (int)((width / scale));  
    *new_height = (int)((height / scale));  

    // 分配内存给新图像  
    #if 0
    Pixel* output_image = (Pixel*)malloc((*new_width) * (*new_height) * sizeof(Pixel));  
    if (!output_image) {  
        fprintf(stderr, "内存分配失败\n");  
        return NULL;  
    }  
    #endif

    // 平均法缩小图像  
    for (int new_y = 0; new_y < *new_height; new_y++) 
    {  
        for (int new_x = 0; new_x < *new_width; new_x++) 
        {  
            int old_x_start = (int)(new_x * scale);  
            int old_y_start = (int)(new_y * scale);  
            int old_x_end = (int)((new_x + 1) * scale);  
            int old_y_end = (int)((new_y + 1) * scale);  

            // 初始化累加器  
            int sum_r = 0, sum_g = 0, sum_b = 0;  
            int count = 0;  

            // 累加缩小区域内的像素  
            for (int y = old_y_start; y <= old_y_end; y++) 
            {  
                for (int x = old_x_start; x <= old_x_end; x++) 
                {  
                    if (x < width && y < height) 
                    { // 确保不越界  
                        RGB p = input_image[y * width + x];  
                        sum_r += p.r;  
                        sum_g += p.g;  
                        sum_b += p.b;  
                        count++;  
                    }  
                }  
            }  

            // 计算平均值并赋值  
            output_image[new_y * (*new_width) + new_x].r = (unsigned char)(sum_r / count);  
            output_image[new_y * (*new_width) + new_x].g = (unsigned char)(sum_g / count);  
            output_image[new_y * (*new_width) + new_x].b = (unsigned char)(sum_b / count);  
        }  
    }  

    return;  
}  


// 图像缩小函数  
void shrink_image2(RGB* input_image, RGB* output_image, 
                    int width, int height, 
                    float scale_width, 
                    float scale_height, 
                    int* new_width, int* new_height) 
{  
    // 计算新宽高  
    //*new_width = (int)((width / scale_width) / 4) * 4;  
    //*new_height = (int)((height / scale_height) / 4) * 4;  
    *new_width = (int)((width / scale_width));  
    *new_height = (int)((height / scale_height));  

    // 平均法缩小图像  
    for (int new_y = 0; new_y < *new_height; new_y++) 
    {  
        for (int new_x = 0; new_x < *new_width; new_x++) 
        {  
            int old_x_start = (int)(new_x * scale_width);  
            int old_y_start = (int)(new_y * scale_height);  
            int old_x_end = (int)((new_x + 1) * scale_width);  
            int old_y_end = (int)((new_y + 1) * scale_height);  

            // 初始化累加器  
            int sum_r = 0, sum_g = 0, sum_b = 0;  
            int count = 0;  

            // 累加缩小区域内的像素  
            for (int y = old_y_start; y <= old_y_end; y++) 
            {  
                for (int x = old_x_start; x <= old_x_end; x++) 
                {  
                    if (x < width && y < height) 
                    { // 确保不越界  
                        RGB p = input_image[y * width + x];  
                        sum_r += p.r;  
                        sum_g += p.g;  
                        sum_b += p.b;  
                        count++;  
                    }  
                }  
            }  

            // 计算平均值并赋值  
            output_image[new_y * (*new_width) + new_x].r = (unsigned char)(sum_r / count);  
            output_image[new_y * (*new_width) + new_x].g = (unsigned char)(sum_g / count);  
            output_image[new_y * (*new_width) + new_x].b = (unsigned char)(sum_b / count);  
        }  
    }  

    return;  
}  
