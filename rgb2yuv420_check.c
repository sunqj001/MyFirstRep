#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include <stdint.h>  
#include "global.h"

void rotateImage(unsigned char *input, unsigned char *output, int angle, int width, int height); 
void rotateYUVImage(  
    unsigned char *inputY, unsigned char *inputU, unsigned char *inputV,  
    unsigned char *outputY, unsigned char *outputU, unsigned char *outputV, 
    int angle, 
    int width, int height); 
void SaveYuvImage(unsigned char *pImage, char* str, int len, int width, int height);
void InitRgbBuffer(void);
#if 0
// 简化的整数版RGB到YUV转换函数（适用于DSP，避免浮点运算）
void RGBtoYUV420(uint8_t *rgb, uint8_t *yuv420, int width, int height) 
{
    int y_index = 0;
    int uv_index = width * height;
    
    // 遍历每一行和每一列
    for (int j = 0; j < height; j++) 
    {
        for (int i = 0; i < width; i += 2) 
        { // 假设每两个像素取一个UV样本，以节省空间
            int r = rgb[(j * width + i) * 3];
            int g = rgb[(j * width + i) * 3 + 1];
            int b = rgb[(j * width + i) * 3 + 2];

            // 第一个像素的Y值
            int Y1 = (int)(RGB2YUV_Y_R * r + RGB2YUV_Y_G * g + RGB2YUV_Y_B * b);
            if (Y1 < 0) Y1 = 0;
            if (Y1 > 255) Y1 = 255;

            // 如果i+1在范围内，则计算第二个像素的Y值
            int Y2 = 0;
            if (i + 1 < width) 
            {
                r = rgb[((j * width) + (i + 1)) * 3];
                g = rgb[((j * width) + (i + 1)) * 3 + 1];
                b = rgb[((j * width) + (i + 1)) * 3 + 2];
                Y2 = (int)(RGB2YUV_Y_R * r + RGB2YUV_Y_G * g + RGB2YUV_Y_B * b);
                if (Y2 < 0) Y2 = 0;
                if (Y2 > 255) Y2 = 255;
            }

            // 写入Y值
            yuv420[y_index++] = (uint8_t)Y1;
            if (i + 1 < width) yuv420[y_index++] = (uint8_t)Y2;
            
            // 计算UV值（每两行每两列取一个样本）
            if ((j % 2 == 0) && (i % 2 == 0)) 
            {
                int R1 = r, G1 = g, B1 = b;
                int R2 = (i + 1 < width) ? rgb[((j * width) + (i + 1)) * 3] : R1;
                int G2 = (i + 1 < width) ? rgb[((j * width) + (i + 1)) * 3 + 1] : G1;
                int B2 = (i + 1 < width) ? rgb[((j * width) + (i + 1)) * 3 + 2] : B1;

                int U = (int)(RGB2YUV_U_R * R1 + RGB2YUV_U_G * G1 + RGB2YUV_U_G * G2 + RGB2YUV_U_R * R2) / 4 + 128.0f);
                int V = (int)(RGB2YUV_V_R * R1 + RGB2YUV_V_G * G1 + RGB2YUV_V_G * G2 + RGB2YUV_V_R * R2) / 4 + 128.0f);

                if (U < 0) U = 0;
                if (U > 255) U = 255;
                if (V < 0) V = 0;
                if (V > 255) V = 255;

                yuv420[uv_index++] = (uint8_t)U;
                yuv420[uv_index++] = (uint8_t)V;
            }
        }
    }
}
#endif

/*-----------------------------------------------------------------------------------------------------*/

#if 1
uint32_t rgbToYUV420(const uint8_t *rgb, 
                     uint8_t *yuv_y, 
                     uint8_t *yuv_u,
                     uint8_t *yuv_v,
                     int width, 
                     int height) {  
    int frameSize = width * height;  
    int chromaWidth = width / 2;  
    int chromaHeight = height / 2;  

    // Y分量  
    for (int i = 0; i < frameSize; i++) {  
        int r = rgb[i * 3 + 0];  
        int g = rgb[i * 3 + 1];  
        int b = rgb[i * 3 + 2];  

        // Y = 0.299*R + 0.587*G + 0.114*B  
        //yuv[i] = (uint8_t)((0.299 * r + 0.587 * g + 0.114 * b));  
        yuv_y[i] = (uint8_t)((0.299 * r + 0.587 * g + 0.114 * b));  
    }  

    // U和V分量  
    for (int j = 0; j < chromaHeight; j++) {  
        for (int i = 0; i < chromaWidth; i++) {  
            int rgbIndex = (j * 2 * width + i * 2) * 3;  
            int b1 = rgb[rgbIndex + 0];  
            int g1 = rgb[rgbIndex + 1];  
            int r1 = rgb[rgbIndex + 2];  

            int b2 = rgb[rgbIndex + 3];  
            int g2 = rgb[rgbIndex + 4];  
            int r2 = rgb[rgbIndex + 5];  

            int b3 = rgb[rgbIndex + width * 3 + 0];  
            int g3 = rgb[rgbIndex + width * 3 + 1];  
            int r3 = rgb[rgbIndex + width * 3 + 2];  

            int b4 = rgb[rgbIndex + width * 3 + 3];  
            int g4 = rgb[rgbIndex + width * 3 + 4];  
            int r4 = rgb[rgbIndex + width * 3 + 5];  

            // U分量 
            #if 1 
            int u = (uint8_t)((-0.14713 * r1 - 0.28886 * g1 + 0.436 * b1 +  
                               -0.14713 * r2 - 0.28886 * g2 + 0.436 * b2 +  
                               -0.14713 * r3 - 0.28886 * g3 + 0.436 * b3 +  
                               -0.14713 * r4 - 0.28886 * g4 + 0.436 * b4) / 4  + 128);  
            #endif
            #if 0
            int u =          ((-0.14713 * r1 - 0.28886 * g1 + 0.436 * b1 +  
                               -0.14713 * r2 - 0.28886 * g2 + 0.436 * b2 +  
                               -0.14713 * r3 - 0.28886 * g3 + 0.436 * b3 +  
                               -0.14713 * r4 - 0.28886 * g4 + 0.436 * b4) / 4);
                if(u < 0) u = 0;
                if(u > 255) u = 255;
            #endif
            //yuv[frameSize + (j * chromaWidth + i)] = (uint8_t)u;  
            yuv_u[(j * chromaWidth + i)] = (uint8_t)u;  

            // V分量  
            #if 1
            int v = (uint8_t)((0.615 * r1 - 0.51499 * g1 - 0.10001 * b1 +  
                               0.615 * r2 - 0.51499 * g2 - 0.10001 * b2 +  
                               0.615 * r3 - 0.51499 * g3 - 0.10001 * b3 +  
                               0.615 * r4 - 0.51499 * g4 - 0.10001 * b4) / 4 + 128);  
            #endif
            #if 0
           int v =           ((0.615 * r1 - 0.51499 * g1 - 0.10001 * b1 +  
                               0.615 * r2 - 0.51499 * g2 - 0.10001 * b2 +  
                               0.615 * r3 - 0.51499 * g3 - 0.10001 * b3 +  
                               0.615 * r4 - 0.51499 * g4 - 0.10001 * b4) / 4);  
            if(v < 0) u = 0;
            if(v > 255) u = 255;
          #endif                          
            //yuv[frameSize + frameSize / 4 + (j * chromaWidth + i)] = (uint8_t)v;  
            yuv_v[(j * chromaWidth + i)] = (uint8_t)v;  
        }  
    }  

    return (frameSize + frameSize / 2);
}      
#endif
// 函数：YUV420 转换为 RGB  
void YUV420ToRGB(uint8_t *yuv, uint8_t *rgb, int width, int height) 
{  
    int frameSize = width * height;  
    uint8_t *y = yuv;                        // Y分量  
    uint8_t *u = yuv + frameSize;           // U分量，位置在Y之后  
    uint8_t *v = yuv + frameSize + (frameSize / 4); // V分量，位置在U之后  

    for (int j = 0; j < height; j++) {  
        for (int i = 0; i < width; i++) {  
            int yIndex = j * width + i;    // Y分量索引  

            // YUV 中的 U 和 V 分量需要转换为对应的 U 和 V 下标  
            int uvIndex = (j / 2) * (width / 2) + (i / 2);  
            int Y = y[yIndex];  
            int U = u[uvIndex] - 128;      // U 为 0-255 范围，需要调整  
            int V = v[uvIndex] - 128;      // V 为 0-255 范围，需要调整  

            // 计算 RGB 分量  
            int R = Y + (1.402 * V);  
            int G = Y - (0.344136 * U) - (0.714136 * V);  
            int B = Y + (1.772 * U);  

            // 限制 RGB 范围在 0 到 255 之间  
            rgb[(j * width + i) * 3 + 0] = (R < 0) ? 0 : (R > 255) ? 255 : R;  
            rgb[(j * width + i) * 3 + 1] = (G < 0) ? 0 : (G > 255) ? 255 : G;  
            rgb[(j * width + i) * 3 + 2] = (B < 0) ? 0 : (B > 255) ? 255 : B;  
        }  
    }  
}


uint32_t rgbToYUV420_2(const uint8_t *rgb, 
                     uint8_t *yuv, 
                     int width, 
                     int height) {  
    int frameSize = width * height;  
    int chromaWidth = width / 2;  
    int chromaHeight = height / 2;  

    // Y分量  
    for (int i = 0; i < frameSize; i++) {  
        int r = rgb[i * 3 + 0];  
        int g = rgb[i * 3 + 1];  
        int b = rgb[i * 3 + 2];  

        //Y = 0.299*R + 0.587*G + 0.114*B  
        yuv[i] = (uint8_t)((0.299 * r + 0.587 * g + 0.114 * b));  
        
    }  

    // U和V分量  
    for (int j = 0; j < chromaHeight; j++) {  
        for (int i = 0; i < chromaWidth; i++) {  
            int rgbIndex = (j * 2 * width + i * 2) * 3;  
            int r1 = rgb[rgbIndex + 0];  
            int g1 = rgb[rgbIndex + 1];  
            int b1 = rgb[rgbIndex + 2];  

            int r2 = rgb[rgbIndex + 3];  
            int g2 = rgb[rgbIndex + 4];  
            int b2 = rgb[rgbIndex + 5];  

            int r3 = rgb[rgbIndex + width * 3 + 0];  
            int g3 = rgb[rgbIndex + width * 3 + 1];  
            int b3 = rgb[rgbIndex + width * 3 + 2];  

            int r4 = rgb[rgbIndex + width * 3 + 3];  
            int g4 = rgb[rgbIndex + width * 3 + 4];  
            int b4 = rgb[rgbIndex + width * 3 + 5];  

            // U分量 
            #if 0 
            int u = (uint8_t)((-0.14713 * r1 - 0.28886 * g1 + 0.436 * b1 +  
                               -0.14713 * r2 - 0.28886 * g2 + 0.436 * b2 +  
                               -0.14713 * r3 - 0.28886 * g3 + 0.436 * b3 +  
                               -0.14713 * r4 - 0.28886 * g4 + 0.436 * b4) / 4  + 128);  
            #endif
            #if 1
            int u = ((-0.14713 * r1 - 0.28886 * g1 + 0.436 * b1 +  
                               -0.14713 * r2 - 0.28886 * g2 + 0.436 * b2 +  
                               -0.14713 * r3 - 0.28886 * g3 + 0.436 * b3 +  
                               -0.14713 * r4 - 0.28886 * g4 + 0.436 * b4) / 4 + 128);
                if(u < 0) u = 0;
                if(u > 255) u = 255;
            #endif
            yuv[frameSize + (j * chromaWidth + i)] = (uint8_t)u;  
            //yuv_u[(j * chromaWidth + i)] = (uint8_t)u;  

            // V分量  
            #if 0
            int v = (uint8_t)((0.615 * r1 - 0.51498 * g1 - 0.10001 * b1 +  
                               0.615 * r2 - 0.51498 * g2 - 0.10001 * b2 +  
                               0.615 * r3 - 0.51498 * g3 - 0.10001 * b3 +  
                               0.615 * r4 - 0.51498 * g4 - 0.10001 * b4) / 4 + 128);  
            #endif
            #if 1
           int v = ((0.615 * r1 - 0.51498 * g1 - 0.10001 * b1 +  
                               0.615 * r2 - 0.51498 * g2 - 0.10001 * b2 +  
                               0.615 * r3 - 0.51498 * g3 - 0.10001 * b3 +  
                               0.615 * r4 - 0.51498 * g4 - 0.10001 * b4) / 4 + 128);  
            if(u < 0) u = 0;
            if(u > 255) u = 255;
          #endif                          
            yuv[frameSize + frameSize / 4 + (j * chromaWidth + i)] = (uint8_t)v;  
            //yuv_v[(j * chromaWidth + i)] = (uint8_t)v;  
        }  
    }  

    return (frameSize + frameSize / 2);
}      




void saveRGBImage(RGB* image, char* str, int width, int height);
int foo(unsigned int width, unsigned int height);

// RGB到YUV的转换系数（根据ITU-R BT.601标准）
#define RGB_TO_YUV_R_COEFF 0.299
#define RGB_TO_YUV_G_COEFF 0.587
#define RGB_TO_YUV_B_COEFF 0.114
#define RGB_TO_YUV_U_R_COEFF -0.169
#define RGB_TO_YUV_U_G_COEFF -0.331
#define RGB_TO_YUV_U_B_COEFF 0.5
#define RGB_TO_YUV_V_R_COEFF 0.5
#define RGB_TO_YUV_V_G_COEFF -0.419
#define RGB_TO_YUV_V_B_COEFF -0.081
// 缩放因子，将浮点结果转换为8位整数
#define YUV_SCALE 256
// RGB转YUV函数
void rgb_to_yuv(RGB rgb, YUV *pYuv) 
{
  // 计算Y分量
  pYuv->y = (uint8_t)round((RGB_TO_YUV_R_COEFF * rgb.r +
                          RGB_TO_YUV_G_COEFF * rgb.g +
                          RGB_TO_YUV_B_COEFF * rgb.b) * YUV_SCALE);
  
  // 计算U分量
  pYuv->u = (uint8_t)round((RGB_TO_YUV_U_R_COEFF * rgb.r +
                          RGB_TO_YUV_U_G_COEFF * rgb.g +
                          RGB_TO_YUV_U_B_COEFF * rgb.b) * YUV_SCALE + 128); // 加128是为了偏移到正数范围
  
  // 计算V分量
  pYuv->v = (uint8_t)round((RGB_TO_YUV_V_R_COEFF * rgb.r +
                          RGB_TO_YUV_V_G_COEFF * rgb.g +
                          RGB_TO_YUV_V_B_COEFF * rgb.b) * YUV_SCALE + 128); // 加128是为了偏移到正数范围
  
  return;
}
// 将RGB图像转换为YUV420格式
uint32_t rgb_to_yuv420(RGB* rgb_image, uint16_t width, uint16_t height, uint8_t* yuv420_image) 
{
  uint32_t frame_size = width * height;
  uint32_t uv_frame_size = frame_size / 4; // YUV420中U和V分量的大小是Y分量大小的1/4
  
  YUV* yuv_image = (YUV*)yuv420_image; // 临时指向YUV420的起始位置（Y分量）
  uint8_t* u_plane = yuv420_image + frame_size; // U分量的起始位置
  uint8_t* v_plane = u_plane + uv_frame_size;   // V分量的起始位置
  
  // 遍历每个像素，转换RGB到YUV
  for (uint16_t y = 0; y < height; y++) 
  {
      for (uint16_t x = 0; x < width; x++) 
      {
          YUV yuv;
          rgb_to_yuv(rgb_image[y * width + x], &yuv);
          yuv_image[y * width + x].y = yuv.y; // 存储Y分量
      }
  }
  
  // 下采样U和V分量（YUV420格式中U和V是4:2:0采样）
  for (uint16_t y = 0; y < height / 2; y++) 
  {
      for (uint16_t x = 0; x < width / 2; x++) 
      {
          // 简单的平均下采样，可以根据需要调整算法
          uint32_t r_sum_u = 0, r_sum_v = 0;
          for (int i = 0; i < 2; i++) 
          {
              for (int j = 0; j < 2; j++) 
              {
                  uint16_t rgb_index = ((y * 2 + i) * width + (x * 2 + j));
                  YUV yuv;
                  rgb_to_yuv(rgb_image[rgb_index], &yuv);
                  r_sum_u += yuv.u;
                  r_sum_v += yuv.v;
              }
          }
          u_plane[y * (width / 2) + x] = (uint8_t)(r_sum_u / 4); // 存储U分量
          v_plane[y * (width / 2) + x] = (uint8_t)(r_sum_v / 4); // 存储V分量
      }
  }

  return (frame_size + frame_size / 4);
}
int main() 
{
  printf("Hello1");
  usleep(1000);
  InitRgbBuffer();
  printf("Hello2");
  foo(IMG_WIDTH, IMG_HEIGHT);
  printf("\nEnd of main\n");
  printf("\n\n");
  //foo(32, 32);

  return 0;
}

/********************************************
 * 
 * 
 * 
 * return value : 0 no error.
 *                other error of width or height.
*/
RGB rgb_image_bk[IMG_WIDTH * IMG_HEIGHT * 3]; 
RGB rgb_image[IMG_WIDTH * IMG_HEIGHT * 3];

int foo2(ALLOT_AREA * pAllot, int width, int height)
{
    unsigned int iWidth;
    unsigned int iHeight;
    unsigned int cntWdPerCore;
    int i;

    usleep(1000);  
    printf("\ninput width = %d", width);
    printf("\ninput height = %d", height);

    iWidth  = (width  >> 2) << 2;
    iHeight = (height >> 2) << 2;

    usleep(1000);  
    printf("\niWidth = %d", iWidth);
    printf("\niHeight = %d", iHeight);

    if(iWidth < 4 || iHeight < 4)
    {
        printf("\nparam error!");
        return -1;
    }

    //**---- (iWidth / 4) / 8) * 4 ----*/
    cntWdPerCore = iWidth  >> 5;

    for(i = 0; i < 8; i++)
    {
        pAllot[i].count = cntWdPerCore * 4; 
        pAllot[i].height = iHeight;
    }

    /* add the remaining to the previous area */
    for(i = 0; i < ((iWidth >> 2) % 8); i++)
    {
        pAllot[i].count = pAllot[i].count + 4;
    }

    /* caculate offset */
    pAllot[0].offset = 0;
    for(i = 1; i < 8; i++)
    {
        pAllot[i].offset  = pAllot[i - 1].offset + pAllot[i - 1].count; 
    }
    
    usleep(1000);  
    #if 1  
    for(i = 0; i < 8; i++)
    {
        printf("\noffet = [%3.0d], count = [%3.0d], height = [%d]", 
                pAllot[i].offset, 
                pAllot[i].count,
                pAllot[i].height);
        usleep(1000);    
        
    }
    #endif

    return 0;
} 


int foo33(ALLOT_AREA * pAllot, int width, int height)
{
    unsigned int iWidth;
    unsigned int iHeight;
    unsigned int cntPer2Row;
    unsigned int colCntOfPerBlock;
    int i;
    unsigned int COUNT_PER_BLOCK;

    usleep(1000);  
    printf("\ninput width = %d", width);
    printf("\ninput height = %d", height);

    iWidth  = (width  >> 1) << 1;
    iHeight = (height >> 1) << 1;
    cntPer2Row = iHeight * 2 * 3;

    COUNT_PER_BLOCK = 2 * iWidth;
      /*  */
    i = 1;
    while(((cntPer2Row * i) % COUNT_PER_BLOCK != 0) &&
          (i * 2 <= iWidth))
    {
      i++;      
    }
    printf("\ni = %d\n", i);
    colCntOfPerBlock = i * 2;
    printf("\ncolCntOfPerBlock = %d\n", colCntOfPerBlock);
    
    if(colCntOfPerBlock >= iWidth)
    {
      printf("all in one core: colCntOfPerBlock = %d ", colCntOfPerBlock);
    }

    usleep(1000);  
    printf("\niWidth = %d", iWidth);
    printf("\niHeight = %d", iHeight);

    if(iWidth < 4 || iHeight < 4)
    {
        printf("\nparam error!");
        return -1;
    }

    for(i = 0; i < 8; i++)
    {
        pAllot[i].count = 0;
        pAllot[i].height = iHeight;
    }

    i = 0;
    int colInuse = 0;
    while((colInuse + colCntOfPerBlock) <= iWidth)
    {
        pAllot[i].count = pAllot[i].count + colCntOfPerBlock;
        colInuse = colInuse + colCntOfPerBlock;
        i = (i + 1) % 8;
    }

    if(colInuse < iWidth)
    {
        pAllot[7].count = pAllot[7].count + (iWidth - colInuse);
    }


    /* caculate offset */
    pAllot[0].offset = 0;
    for(i = 1; i < 8; i++)
    {
        pAllot[i].offset  = pAllot[i - 1].offset + pAllot[i - 1].count; 
    }
    
    usleep(1000);  
    #if 1  
    for(i = 0; i < 8; i++)
    {
        printf("\noffet = [%3.0d], count = [%3.0d], height = [%d]", 
                pAllot[i].offset, 
                pAllot[i].count,
                pAllot[i].height);
        usleep(1000);    
        
    }
    #endif

    return 0;
} 

int foo3(ALLOT_AREA * pAllot, int width, int height)
{
    unsigned int iWidth;
    unsigned int iHeight;
    unsigned int cntPer2Row;
    unsigned int colCntOfPerBlock;
    int i;
    unsigned int COUNT_PER_BLOCK;

    usleep(1000);  
    printf("\ninput width = %d", width);
    printf("\ninput height = %d", height);

    iWidth  = (width  >> 1) << 1;
    iHeight = (height >> 1) << 1;
    cntPer2Row = iWidth * 2 * 3;

    COUNT_PER_BLOCK = 2 * iHeight;
      /*  */
    i = 1;
    while(((cntPer2Row * i) % COUNT_PER_BLOCK != 0) &&
          (i * 2 <= iHeight))
    {
      i++;      
    }
    printf("\ni = %d\n", i);
    colCntOfPerBlock = i * 2;
    printf("\ncolCntOfPerBlock = %d\n", colCntOfPerBlock);
    
    if(colCntOfPerBlock >= iHeight)
    {
      printf("all in one core: colCntOfPerBlock = %d ", colCntOfPerBlock);
    }

    usleep(1000);  
    printf("\niWidth = %d", iWidth);
    printf("\niHeight = %d", iHeight);

    if(iWidth < 4 || iHeight < 4)
    {
        printf("\nparam error!");
        return -1;
    }

    for(i = 0; i < 8; i++)
    {
        pAllot[i].count = 0;
        pAllot[i].width = iWidth;
    }

    i = 0;
    int colInuse = 0;
    while((colInuse + colCntOfPerBlock) <= iHeight)
    {
        pAllot[i].count = pAllot[i].count + colCntOfPerBlock;
        colInuse = colInuse + colCntOfPerBlock;
        i = (i + 1) % 8;
    }

    if(colInuse < iHeight)
    {
        pAllot[7].count = pAllot[7].count + (iHeight - colInuse);
    }


    /* caculate offset */
    pAllot[0].offset = 0;
    for(i = 1; i < 8; i++)
    {
        pAllot[i].offset  = pAllot[i - 1].offset + pAllot[i - 1].count; 
    }
    
    usleep(1000);  
    #if 1  
    for(i = 0; i < 8; i++)
    {
        printf("\noffet = [%3.0d], count = [%3.0d], width = [%d]", 
                pAllot[i].offset, 
                pAllot[i].count,
                pAllot[i].width);
        usleep(1000);    
        
    }
    #endif

    return 0;
} 


uint8_t yuv420_image[(IMG_WIDTH * IMG_HEIGHT) + ((IMG_WIDTH * IMG_HEIGHT) / 2)]; 

#define DefineYuvCore(n)\
uint8_t yuv420_image_y_##n[(IMG_WIDTH * IMG_HEIGHT)]; \
uint8_t yuv420_image_u_##n[(IMG_WIDTH * IMG_HEIGHT) / 4]; \
uint8_t yuv420_image_v_##n[(IMG_WIDTH * IMG_HEIGHT) / 4]; \


#define Yuv420ImageY(core) yuv420_image_y_##core
#define Yuv420ImageU(core) yuv420_image_u_##core
#define Yuv420ImageV(core) yuv420_image_v_##core

DefineYuvCore(0);
DefineYuvCore(1);
DefineYuvCore(2);
DefineYuvCore(3);
DefineYuvCore(4);
DefineYuvCore(5);
DefineYuvCore(6);
DefineYuvCore(7);

int foo(unsigned int width, unsigned int height)
{
    ALLOT_AREA allotBlcok[8];
  
    uint32_t sizeOfYuv420[8];
    unsigned int iWidth;
    unsigned int iHeight;
    int sizeYuvFile;  

    unsigned int cntWdPerCore;
    unsigned int cntHgPerCore;
    unsigned int cntPer2Row;
    unsigned int colCntOfPerBlock;

    int i;

    foo3(allotBlcok, width, height);
  
  
    usleep(1000);  
    printf("\ninput width = %d", width);
    printf("\ninput height = %d", height);

    iWidth  = (width  >> 1) << 1;
    iHeight = (height >> 1) << 1;

    /* Save RGB data into file */
    saveRGBImage(rgb_image, "tstRgb.rgb", iWidth, iHeight);
    
    /* RGB TO YUV420 */
    int numOfCore = 8;
    uint8_t *pRgbImg = (uint8_t*)rgb_image;
    uint8_t *pYuv_y = (uint8_t*)yuv420_image;
    uint8_t *pYuv_u = (uint8_t*)yuv420_image + iWidth * iHeight;
    uint8_t *pYuv_v = (uint8_t*)yuv420_image + iWidth * iHeight + iWidth * iHeight / 4;

    #if 0
    sizeYuvFile = rgbToYUV420( pRgbImg, 
                              pYuv_y,
                              pYuv_u,
                              pYuv_v, 
                              iWidth, 
                              iHeight);
    printf("\nsizeYuvFile = %d\n", sizeYuvFile);                                  
    #endif

    #if 1
      #define Rgb2Yuv420(core) \
      {\
      sizeOfYuv420[core] = rgbToYUV420( pRgbImg + allotBlcok[core].offset * allotBlcok[core].width * 3, \
                                 Yuv420ImageY(core),\
                                 Yuv420ImageU(core),\
                                 Yuv420ImageV(core), \
                                 allotBlcok[core].width, \
                                 allotBlcok[core].count);\
      }

      Rgb2Yuv420(0);  
      Rgb2Yuv420(1);                                 
      Rgb2Yuv420(2);                                 
      Rgb2Yuv420(3);                                 
      Rgb2Yuv420(4);                                 
      Rgb2Yuv420(5);                                 
      Rgb2Yuv420(6);                                 
      Rgb2Yuv420(7);                

      #define CpYuvToGlbYuv(core) \
      {\
        /* ----Y---- */\
        memcpy(pYuv_y + allotBlcok[core].offset * allotBlcok[core].width, \
              Yuv420ImageY(core), \
              allotBlcok[core].count * allotBlcok[core].width);\
        \
        /* ----U---- */\
        memcpy(pYuv_u + allotBlcok[core].offset * allotBlcok[core].width / 4, \
              Yuv420ImageU(core), \
              allotBlcok[core].count * allotBlcok[core].width / 4);\
        \
        /* ----V---- */\
        memcpy(pYuv_v + allotBlcok[core].offset * allotBlcok[core].width / 4, \
              Yuv420ImageV(core), \
              allotBlcok[core].count * allotBlcok[core].width / 4);\
      }

      CpYuvToGlbYuv(0);
      CpYuvToGlbYuv(1);
      CpYuvToGlbYuv(2);
      CpYuvToGlbYuv(3);
      CpYuvToGlbYuv(4);
      CpYuvToGlbYuv(5);
      CpYuvToGlbYuv(6);
      CpYuvToGlbYuv(7);

    for(i = 0; i < numOfCore; i++)
    {      
      sizeYuvFile = sizeYuvFile + sizeOfYuv420[i];
      printf("\nsizeOfYuv420[%d] = %d\n", i, sizeOfYuv420[i]);
      usleep(1000);
    
    }
    #endif

    printf("\n -------yuv data------- \n");
#if 0
    for(i = 0; i < sizeYuvFile; i++ )
    {
      usleep(1000);
      printf("%x, ", yuv420_image[i]);
    }
#endif
    usleep(1000);
    printf("\n -------yuv data end------- \n");

    usleep(1000);
    printf("\nsizeYuvFile = %d\n", sizeYuvFile);
    usleep(1000);
    printf("\n rgb to yuv successed");

    /* Save yuv file */
    FILE* file_yuv = fopen("tstYuv.yuv", "wb");
    if (!file_yuv) {
        printf("\nFailed to open file_yuv");
        exit(-1);
    }

    fwrite(yuv420_image, 1, sizeYuvFile, file_yuv);
    fclose(file_yuv);
  /* Rotate yuv */
    unsigned char *pYuvImg = malloc(sizeYuvFile);
    if(pYuvImg == NULL)
    {
        printf("\nNo memory for yuv rotation");
        exit(-1);
    }

    rotateYUVImage(yuv420_image, 
                   yuv420_image + iWidth * iHeight,
                   yuv420_image + iWidth * iHeight + (iWidth * iHeight) / 4,
                   pYuvImg, 
                   pYuvImg + iWidth * iHeight,
                   pYuvImg + iWidth * iHeight + (iWidth * iHeight) / 4,
                   kRotateAngle_90,
                   width, iHeight);
    SaveYuvImage(pYuvImg, "yuvRote_90.yuv", iWidth * iHeight + (iWidth * iHeight) / 2, height, width);
    printf("\n");

    rotateYUVImage(yuv420_image, 
                   yuv420_image + iWidth * iHeight,
                   yuv420_image + iWidth * iHeight + (iWidth * iHeight) / 4,
                   pYuvImg, 
                   pYuvImg + iWidth * iHeight,
                   pYuvImg + iWidth * iHeight + (iWidth * iHeight) / 4,
                   kRotateAngle_180,
                   width, iHeight);
    SaveYuvImage(pYuvImg, "yuvRote_180.yuv", iWidth * iHeight + (iWidth * iHeight) / 2, width, height);
    printf("\n");

    rotateYUVImage(yuv420_image, 
                   yuv420_image + iWidth * iHeight,
                   yuv420_image + iWidth * iHeight + (iWidth * iHeight) / 4,
                   pYuvImg, 
                   pYuvImg + iWidth * iHeight,
                   pYuvImg + iWidth * iHeight + (iWidth * iHeight) / 4,
                   kRotateAngle_270,
                   width, iHeight);
    SaveYuvImage(pYuvImg, "yuvRote_270.yuv", iWidth * iHeight + (iWidth * iHeight) / 2, height, width);
    printf("\n");
    printf("\n");
    

    free(pYuvImg);

    usleep(1000);
    printf("\nSave yuv file successful");

    /* Yuv to rgb */
    unsigned char* pOutImg = malloc(iWidth * iHeight * 3);
    if(pOutImg == NULL)
    {
      printf("\n malloc failed!");
      return;
    }

    YUV420ToRGB(yuv420_image, rgb_image_bk, iWidth, iHeight);
    saveRGBImage(rgb_image_bk, "tstRgb3.rgb", iWidth, iHeight);
    saveRGBImage(rgb_image,     "tstRgb2.rgb", iWidth, iHeight);
    printf("\n");

    rotateImage( rgb_image, pOutImg, kRotateAngle_90, iWidth, iHeight);
    saveRGBImage(pOutImg, "tstRgb90.rgb", iHeight, iWidth);
    printf("\n");

    rotateImage( rgb_image, pOutImg, kRotateAngle_180, iWidth, iHeight);
    saveRGBImage(pOutImg, "tstRgb180.rgb", iWidth, iHeight);
    printf("\n");

    rotateImage( rgb_image, pOutImg, kRotateAngle_270, iWidth, iHeight);
    saveRGBImage(pOutImg, "tstRgb270.rgb", iHeight, iWidth);
    printf("\n");

    rotateImage( rgb_image, pOutImg, kRotateAngle_UD, iWidth, iHeight);
    saveRGBImage(pOutImg, "tstRgb_Ud.rgb", iWidth, iHeight);
    printf("\n");

    rotateImage( rgb_image, pOutImg, kRotateAngle_LR, iWidth, iHeight);
    saveRGBImage(pOutImg, "tstRgb_Lr.rgb", iWidth, iHeight);
    free(pOutImg);

    /* -------------------------ealarg image----------------------- */
    // float scale = 0.5;
    int new_width;
    int new_height;
    RGB *output_image;

    #define EnlargImage(scale, filename) \
    {\
    new_width = iWidth * scale;\
    new_height = iHeight * scale;\
    output_image = malloc(new_width * new_height * 3);\
    if(output_image == NULL)\
    {\
      printf("\nenlarg image failed!");\
      return;\
    }\
    enlarge_image(rgb_image, output_image, \
                  iWidth, iHeight, scale, \
                    &new_width, &new_height);\
    usleep(1000);\
    printf("\nnew_width = %d, new_height = %d", new_width, new_height);\
    saveRGBImage(output_image, filename, new_width, new_height);\
    printf("\n");\
    free(output_image);\
    }

    EnlargImage(0.5, "enlargImge_0_5.rgb");
    EnlargImage(1.5, "enlargImge_1_5.rgb");
    EnlargImage(1.7, "enlargImge_1_7.rgb");
    EnlargImage(2.0, "enlargImge_2_0.rgb");
    EnlargImage(4.0, "enlargImge_4_0.rgb");
    EnlargImage(6.0, "enlargImge_6_0.rgb");

    #define EnlargImage2(scale_w, scale_h, filename) \
    {\
    new_width = iWidth * (scale_w);\
    new_height = iHeight * (scale_h);\
    output_image = malloc(new_width * new_height * 3);\
    if(output_image == NULL)\
    {\
      printf("\nenlarg image failed!");\
      return;\
    }\
    enlarge_image2(rgb_image, output_image, \
                  iWidth, iHeight, \
                  scale_w, scale_h,\
                    &new_width, &new_height);\
    usleep(1000);\
    printf("\nnew_width = %d, new_height = %d", new_width, new_height);\
    saveRGBImage(output_image, filename, new_width, new_height);\
    printf("\n");\
    free(output_image);\
    }

    EnlargImage2(4.0, 1.0, "enlargImge_40_10.rgb");
    EnlargImage2(6.0, 1.0, "enlargImge_60_10.rgb");



  /* ----------------------shrink image---------------------------------*/
  #define ShrinkImage(scale, filename) \
    {\
    new_width = (int)(iWidth / (scale));\
    new_height = (int)(iHeight / (scale));\
    output_image = malloc(new_width * new_height * 3);\
    if(output_image == NULL)\
    {\
      printf("\nenlarg image failed!");\
      return;\
    }\
\
    shrink_image(rgb_image, output_image, \
                  iWidth, iHeight, (scale), \
                    &new_width, &new_height);\
    usleep(1000);\
    printf("\nnew_width = %d, new_height = %d", new_width, new_height);\
    saveRGBImage(output_image, filename, new_width, new_height);\
    printf("\n");\
    free(output_image);\
    }

    ShrinkImage(1.5, "shirnkImage_1_5.rgb");
    ShrinkImage(1.6, "shirnkImage_1_6.rgb");
    ShrinkImage(1.7, "shirnkImage_1_7.rgb");
    ShrinkImage(1.8, "shirnkImage_1_8.rgb");
    ShrinkImage(1.9, "shirnkImage_1_9.rgb");
    ShrinkImage(2.0, "shirnkImage_2_0.rgb");
    ShrinkImage(2.5, "shirnkImage_2_5_n.rgb");
    ShrinkImage(4.0, "shirnkImage_4_0.rgb");

  #define ShrinkImage2(scale_w, scale_h, filename) \
    {\
    new_width = (int)(iWidth / (scale_w));\
    new_height = (int)(iHeight / (scale_h));\
    output_image = malloc(new_width * new_height * 3);\
    if(output_image == NULL)\
    {\
      printf("\nenlarg image failed!");\
      return;\
    }\
\
    shrink_image2(rgb_image, output_image, \
                  iWidth, iHeight, \
                  (scale_w), (scale_h),\
                    &new_width, &new_height);\
    usleep(1000);\
    printf("\nnew_width = %d, new_height = %d", new_width, new_height);\
    saveRGBImage(output_image, filename, new_width, new_height);\
    printf("\n");\
    free(output_image);\
    }    

    ShrinkImage2(1.1, 2.0, "shirnkImage2_11_20.rgb");

    return 0;
}

void SaveYuvImage(unsigned char *pImage, char* str, int len, int width, int height)
{
    // 打开文件用于二进制写入
    //FILE* file = fopen("tstRgb.rgb", "wb");
    char filename[128];
    char* p;
    char ch = '.';

    filename[0] = 0;
    p = strchr(str, &ch);

    if(p == NULL)
    {
        sprintf(filename, "%s_%dx%d.yuv", str, width, height);
    }else
    {
        int pos = (int)(p - str);
        strncpy(filename, str, pos);
        sprintf(filename, "%s_%dx%d.yuv", filename, width, height);
    }

    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("\nFailed to open file: %s", filename);
        exit(-1);
    }


    // 写入图像数据
    fwrite(pImage, 1, len, file);
    fclose(file);
    printf("\nSave yuv file %s successed", filename);

    return;
}


void saveRGBImage(RGB* image, char* str, int width, int height)
{
    // 打开文件用于二进制写入
    //FILE* file = fopen("tstRgb.rgb", "wb");
    char filename[128];
    char* p;
    char ch = '.';

    filename[0] = 0;
    p = strchr(str, &ch);

    if(p == NULL)
    {
        sprintf(filename, "%s_%dx%d.rgb", str, width, height);
    }else
    {
        int pos = (int)(p - str);
        strncpy(filename, str, pos);
        sprintf(filename, "%s_%dx%d.rgb", filename, width, height);
    }
    

    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("\nFailed to open file: %s", filename);
        exit(-1);
    }

    // 写入图像宽度、高度和每个像素的字节数（3字节）
    // 这部分信息不是标准的PPM或BMP格式的一部分，但可以帮助我们理解图像数据
    // 在实际应用中，你可能需要根据具体的图像格式规范来写入文件头
    #if 0
    fwrite(&width, sizeof(int), 1, file);
    fwrite(&height, sizeof(int), 1, file);
    int pixelSize = 3; // RGB每个像素3个字节
    fwrite(&pixelSize, sizeof(int), 1, file);
    #endif

    // 写入图像数据
    fwrite(image, sizeof(RGB), width * height, file);
    fclose(file);
    printf("\nSave rgb file %s successed", filename);

    return;
}


void rotateImage(unsigned char *input, 
                unsigned char *output, 
                int angle, 
                int width, 
                int height) 
{
    int newX, x;  
    int newY, y;  

    switch(angle)
    {
      case kRotateAngle_90:
        for (y = 0; y < height; y++)
        {  
            for (x = 0; x < width; x++) 
            {  
                // 输入图像的坐标 (x, y) 转换为输出图像的坐标  
                newX = y;  
                newY = width - 1 - x;  

                output[(newY * height + newX) * 3 + 0] = input[(y * width + x) * 3 + 0]; // R  
                output[(newY * height + newX) * 3 + 1] = input[(y * width + x) * 3 + 1]; // G  
                output[(newY * height + newX) * 3 + 2] = input[(y * width + x) * 3 + 2]; // B  
            }  
        }  
        break;

      case kRotateAngle_180:
        for (y = 0; y < height; y++)
        {  
            for (x = 0; x < width; x++) 
            {  
                // 输入图像的坐标 (x, y) 转换为输出图像的坐标  
                newX = width - 1 - x;  
                newY = height - 1 - y;  

                output[(newY * width + newX) * 3 + 0] = input[(y * width + x) * 3 + 0]; // R  
                output[(newY * width + newX) * 3 + 1] = input[(y * width + x) * 3 + 1]; // G  
                output[(newY * width + newX) * 3 + 2] = input[(y * width + x) * 3 + 2]; // B  
            }  
        }  
        break;

      case kRotateAngle_270:
        for (y = 0; y < height; y++)
        {  
            for (x = 0; x < width; x++) 
            {  
                // 输入图像的坐标 (x, y) 转换为输出图像的坐标  
                newX = height - 1 - y;  
                newY = x;  

                output[(newY * height + newX) * 3 + 0] = input[(y * width + x) * 3 + 0]; // R  
                output[(newY * height + newX) * 3 + 1] = input[(y * width + x) * 3 + 1]; // G  
                output[(newY * height + newX) * 3 + 2] = input[(y * width + x) * 3 + 2]; // B  
            }  
        }  
        break;

      case kRotateAngle_UD:
        for (y = 0; y < height; y++)
        {  
            for (x = 0; x < width; x++) 
            {  
                // 输入图像的坐标 (x, y) 转换为输出图像的坐标  
                newX = x;  
                newY = height - 1 - y;  

                output[(newY * width + newX) * 3 + 0] = input[(y * width + x) * 3 + 0]; // R  
                output[(newY * width + newX) * 3 + 1] = input[(y * width + x) * 3 + 1]; // G  
                output[(newY * width + newX) * 3 + 2] = input[(y * width + x) * 3 + 2]; // B  
            }  
        }  
        break;

      case kRotateAngle_LR:
        for (y = 0; y < height; y++)
        {  
            for (x = 0; x < width; x++) 
            {  
                // 输入图像的坐标 (x, y) 转换为输出图像的坐标  
                newX = width - 1 - x;  
                newY = y;  

                output[(newY * width + newX) * 3 + 0] = input[(y * width + x) * 3 + 0]; // R  
                output[(newY * width + newX) * 3 + 1] = input[(y * width + x) * 3 + 1]; // G  
                output[(newY * width + newX) * 3 + 2] = input[(y * width + x) * 3 + 2]; // B  
            }  
        }  
        break;

      default:
        break;

    }

    printf("\nRotate image successed.");
}  

void printImage(unsigned char *image, int width, int height) {  
    for (int y = 0; y < height; y++) {  
        for (int x = 0; x < width; x++) {  
            printf("(%3d, %3d, %3d) ",   
                image[(y * width + x) * 3 + 0], // R  
                image[(y * width + x) * 3 + 1], // G  
                image[(y * width + x) * 3 + 2]); // B  
        }  
        printf("\n");  
    }  
}  

int main_1() {  
    // 假设输入图像的大小为 4x3  
    int width = 4;  
    int height = 3;  

    // 输入图像（每个像素的 RGB 值）  
    unsigned char inputImage[4 * 3 * 3] = {  
        255, 0, 0,   0, 255, 0,   0, 0, 255,   255, 255, 0, // 第一行  
        255, 0, 255, 0, 255, 255, 255, 255, 255, 0, 0, 0, // 第二行  
        128, 128, 128, 64, 64, 64, 32, 32, 32, 0, 0, 0  // 第三行  
    };  

    // 输出原始图像  
    printf("原始图像:\n");  
    printImage(inputImage, width, height);  

    // 旋转图像  
    //srotateImage90CW(inputImage, width, height);  

    // 输出旋转后的图像  
    printf("\n旋转后的图像:\n");  
    printImage(inputImage, height, width);  

    return 0;  
}


void rotateYUVImage(  
    unsigned char *inputY, unsigned char *inputU, unsigned char *inputV,  
    unsigned char *outputY, unsigned char *outputU, unsigned char *outputV,  
    int angle,
    int width, int height) 
{  
    int newWidth = height;  
    int x, y;
    int chromaWidth;  
    int chromaHeight;  

    switch(angle)
    {
      case kRotateAngle_90:
          // 旋转 Y 分量  
          for (y = 0; y < height; y++) 
          {  
              for (x = 0; x < width; x++) 
              {  
                  outputY[(x * height + (height - 1 - y))] = inputY[y * width + x];  
              }  
          }  

        // 旋转 U 分量  
        chromaWidth  = width / 2;  
        chromaHeight = height / 2;  

        for (y = 0; y < chromaHeight; y++) 
        {  
            for (x = 0; x < chromaWidth; x++) 
            {  
                // 输入坐标 (x, y) 转换为输出坐标  
                outputU[(x * chromaHeight + (chromaHeight - 1 - y))] = inputU[y * chromaWidth + x];  
            }  
        }

        // 旋转 V 分量  
        for (y = 0; y < chromaHeight; y++) 
        {  
            for (x = 0; x < chromaWidth; x++) 
            {  
                outputV[(x * chromaHeight + (chromaHeight - 1 - y))] = inputV[y * chromaWidth + x];  
            }  
        }        
            break;

      case kRotateAngle_180:
          // 旋转 Y 分量  
          for (y = 0; y < height; y++) 
          {  
              for (x = 0; x < width; x++) 
              {  
                  outputY[(height - 1 - y) * width + (width - 1 - x)] = inputY[y * width + x];  
              }  
          }  

        chromaWidth  = width / 2;  
        chromaHeight = height / 2;  
        // 旋转 U 分量  
        for (y = 0; y < chromaHeight; y++) 
        {  
            for (x = 0; x < chromaWidth; x++) 
            {  
                // 输入坐标 (x, y) 转换为输出坐标  
                outputU[(chromaHeight - 1 - y) * chromaWidth + (chromaWidth - 1 - x)] = inputU[y * chromaWidth + x];  
            }  
        }

        // 旋转 V 分量  
        for (y = 0; y < chromaHeight; y++) 
        {  
            for (x = 0; x < chromaWidth; x++) 
            {  
                outputV[(chromaHeight - 1 - y) * chromaWidth + (chromaWidth - 1 - x)] = inputV[y * chromaWidth + x];  
            }  
        }        
            break;

      case kRotateAngle_270:
          // 旋转 Y 分量  
          for (y = 0; y < height; y++) 
          {  
              for (x = 0; x < width; x++) 
              {  
                  outputY[(width - 1 - x) * height + y] = inputY[y * width + x];  
              }  
          }  

        chromaWidth  = width / 2;  
        chromaHeight = height / 2;  

        // 旋转 U 分量  
        for (y = 0; y < chromaHeight; y++) 
        {  
            for (x = 0; x < chromaWidth; x++) 
            {  
                // 输入坐标 (x, y) 转换为输出坐标  
                outputU[(chromaWidth - 1 - x) * chromaHeight + y] = inputU[y * chromaWidth + x];  
            }  
        }

        // 旋转 V 分量  
        for (y = 0; y < chromaHeight; y++) 
        {  
            for (x = 0; x < chromaWidth; x++) 
            {  
                outputV[(chromaWidth - 1 - x) * chromaHeight + y] = inputV[y * chromaWidth + x];  
            }  
        }        
            break;

    }
 
    return;
}  

void printYUV(unsigned char *Y, unsigned char *U, unsigned char *V, int width, int height) {  
    printf("Y:\n");  
    for (int y = 0; y < height; y++) {  
        for (int x = 0; x < width; x++) {  
            printf("%3d ", Y[y * width + x]);  
        }  
        printf("\n");  
    }  

    printf("U:\n");  
    for (int y = 0; y < height / 2; y++) {  
        for (int x = 0; x < width / 2; x++) {  
            printf("%3d ", U[y * (width / 2) + x]);  
        }  
        printf("\n");  
    }  

    printf("V:\n");  
    for (int y = 0; y < height / 2; y++) {  
        for (int x = 0; x < width / 2; x++) {  
            printf("%3d ", V[y * (width / 2) + x]);  
        }  
        printf("\n");  
    }  
}  

int main_3() {  
    // 假设 4x4 的 YUV420 图像  
    int width = 4;  
    int height = 4;  

    // 输入 YUV 数据  
    unsigned char inputY[4 * 4] = {  
        76,  149, 29, 80,  
        149,  76, 76, 176,  
        29,  76, 76, 80,  
        80,  149, 176, 29  
    };  

    unsigned char inputU[2 * 2] = {  
        127, 128,  
        128, 127  
    };  

    unsigned char inputV[2 * 2] = {  
        128, 127,  
        127, 128  
    };  

    // 输出数组  
    unsigned char *outputY = malloc(height * width);  
    unsigned char *outputU = malloc((height / 2) * (width / 2));  
    unsigned char *outputV = malloc((height / 2) * (width / 2));  

    // 打印原始图像  
    printf("原始图像:\n");  
    printYUV(inputY, inputU, inputV, width, height);  

    // 旋转图像  
    //rotateYUVImage(inputY, inputU, inputV, outputY, outputU, outputV, width, height);  

    // 打印旋转后的图像  
    printf("\n旋转后的图像:\n");  
    printYUV(outputY, outputU, outputV, height, width);  

    // 释放内存  
    free(outputY);  
    free(outputU);  
    free(outputV);  

    return 0;  
}

/*----------------------------read bmp file-----------------------------------*/

#pragma pack(push, 1) // 确保结构体对齐  
typedef struct {  
    uint16_t bfType;      // 文件类型，必须是 'BM' (0x4D42)  
    uint32_t bfSize;      // 文件大小  
    uint16_t bfReserved1; // 保留  
    uint16_t bfReserved2; // 保留  
    uint32_t bfOffBits;   // 从文件头到图像数据的偏移字节数  
} BMPFileHeader;  

typedef struct {  
    uint32_t biSize;          // 信息头大小  
    int32_t  biWidth;         // 图像宽度  
    int32_t  biHeight;        // 图像高度  
    uint16_t biPlanes;        // 颜色平面数，必须是1  
    uint16_t biBitCount;      // 每像素位数（24位）  
    uint32_t biCompression;   // 压缩方式，通常为0表示无压缩  
    uint32_t biSizeImage;     // 图像数据大小  
    int32_t  biXPelsPerMeter;  // 水平分辨率  
    int32_t  biYPelsPerMeter;  // 垂直分辨率  
    uint32_t biClrUsed;       // 使用的颜色数  
    uint32_t biClrImportant;  // 重要颜色数  
} BMPInfoHeader;  
#pragma pack(pop)  

void readBMP(const char *filename, char* filename2) 
{  
    // 打开 BMP 文件  
    if(filename == NULL)
    {
        return;
    }
    FILE *file = fopen(filename, "rb");  
    if (!file) {  
        perror("Unable to open file");  
        return;  
    }  

    // 读取文件头  
    BMPFileHeader fileHeader;  
    fread(&fileHeader, sizeof(BMPFileHeader), 1, file);  

    // 检查文件类型  
    if (fileHeader.bfType != 0x4D42) {  
        printf("Not a BMP file.\n");  
        fclose(file);  
        return;  
    }  

    // 读取信息头  
    BMPInfoHeader infoHeader;  
    fread(&infoHeader, sizeof(BMPInfoHeader), 1, file);  

    // 确保是24位图像  
    if (infoHeader.biBitCount != 24) {  
        printf("Only 24-bit BMP files are supported.\n");  
        fclose(file);  
        return;  
    }  

    // 获取图像数据偏移位置  
    fseek(file, fileHeader.bfOffBits, SEEK_SET);  

    // 读取像素数据  
    int width = infoHeader.biWidth;  
    int height = infoHeader.biHeight;  
    int rowSize = (width * 3 + 3) & (~3); // 每行必须是4的倍数  
    uint8_t *imageData = rgb_image;//(uint8_t *)malloc(rowSize * height);  
    if (!imageData) {  
        perror("Unable to allocate memory for image data");  
        fclose(file);  
        return;  
    }  
    printf("\nBMP file Width = %d, height = %d", width, height);
    printf("\nBMP file rowSize = %d", rowSize);

    // 读取图像数据  
    fread(imageData, sizeof(uint8_t), rowSize * height, file);  
    fclose(file);  

    FILE *fileW;
    if(filename2 == NULL)
    {
      fileW = fopen("tmp.rgb", "wb");  
    }else
    {
      fileW = fopen(filename2, "wb");  
    }

    if (!fileW) 
    {  
        perror("Unable to open file");  
        return;  
    }

    fwrite(imageData, 1, rowSize * height, fileW);
    fclose(fileW);

    // 打印每个像素的 RGB 值  
    // 注意：BMP 文件的像素是从下到上（底部到顶部）的存储顺序  
    for (int y = height - 1; y >= 0; y--) {  
        for (int x = 0; x < width; x++) {  
            int index = (y * rowSize) + (x * 3); // 计算每个像素的索引  
            uint8_t blue = imageData[index + 0];  
            uint8_t green = imageData[index + 1];  
            uint8_t red = imageData[index + 2];  
            //printf("Pixel (%d, %d): R=%d G=%d B=%d\n", x, y, red, green, blue);  
        }  
    }  

    // 释放内存  
    //free(imageData);  
}  

void InitRgbBuffer(void)
{
    readBMP("tu2.bmp", "tu3.bmp");
}

#if 0
int main(int argc, int8_t **argv) {  
    if (argc < 2) {  
        printf("Usage: %s <filename.bmp>\n", argv[0]);  
        return 1;  
    }  

    readBMP(argv[1], argv[2]);  
    return 0;  
}
#endif