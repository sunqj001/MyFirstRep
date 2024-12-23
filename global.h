#ifndef _GLOBAL_H_
#define _GLOBAL_H_



// RGB 到 YUV 的转换系数（基于 ITU-R BT.601 标准）
#define RGB2YUV_Y_R    0.299f
#define RGB2YUV_Y_G    0.587f
#define RGB2YUV_Y_B    0.114f
#define RGB2YUV_U_R    -0.14713f
#define RGB2YUV_U_G    -0.28886f
#define RGB2YUV_V_R    0.615f
#define RGB2YUV_V_G    -0.51499f
#define RGB2YUV_SCALE  255.0f

//typedef  char int8_t ;
typedef  short int16_t; 
typedef  int   int32_t; 

typedef unsigned char uint8_t ;
typedef unsigned short uint16_t; 
typedef unsigned int   uint32_t; 

#define IMG_WIDTH   (256)
#define IMG_HEIGHT  (128)


typedef enum
{
  kRotateAngle_0   = 0,
  kRotateAngle_90  = 1,
  kRotateAngle_180 = 2,
  kRotateAngle_270 = 3,
  kRotateAngle_360 = 4,

  kRotateAngle_UD = 5,
  kRotateAngle_LR = 6,

}eRotateAngle;

     // 定义RGB和YUV的数据类型
     typedef struct {
         uint8_t r;
         uint8_t g;
         uint8_t b;
     } RGB;
     
     typedef struct {
         uint8_t y;
         uint8_t u;
         uint8_t v;
     } YUV;

     typedef struct {
         uint32_t count;
         uint32_t offset;
         uint32_t height;
         uint32_t width;
     } ALLOT_AREA;

extern void enlarge_image(RGB* input_image, RGB* output_image, 
                    int width, int height, float scale, 
                    int* new_width, int* new_height);  

extern void enlarge_image2(RGB* input_image, RGB* output_image, 
                    int width, int height, 
                    float scale_w, 
                    float scale_h, 
                    int* new_width, int* new_height);

extern void shrink_image(RGB* input_image, RGB* output_image, 
                    int width, int height, float scale, 
                    int* new_width, int* new_height);

extern void shrink_image2(RGB* input_image, RGB* output_image, 
                    int width, int height, 
                    float scale_width, 
                    float scale_height, 
                    int* new_width, int* new_height); 

/***************************************************************
*  end of header file
****************************************************************/
#endif 