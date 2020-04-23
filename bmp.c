/*************************
 
 *bmp.c文件
 
 *************************/
 
 #include "bmp.h"
 
  
 
 /*************************
 
 *fbp，映射内存起始地址
 
 *scrinfo，屏幕信息结构体
 
 *bmpname，.bmp位图文件名
 
 *************************/
 int show_photo(const char *fbp, struct fb_var_screeninfo *scrinfo, const char *bmpname)
 {
   if(NULL == fbp || NULL == scrinfo || NULL == bmpname)
     return -1;
 
   int line_x = 0, line_y = 0;
   unsigned long tmp = 0;
   int xres = scrinfo->xres_virtual;    //屏幕宽（虚拟）
   int bits_per_pixel = scrinfo->bits_per_pixel;  //屏幕位数
   BitMapFileHeader FileHead;
   BitMapInfoHeader InfoHead;
   RgbQuad rgb;    
 
   unsigned long location = 0;
   printf("1111\n");
   //打开.bmp文件
   FILE *fb = fopen(bmpname, "rb");
   if (fb == NULL)
   {
     printf("fopen bmp error\r\n");
     return -1;
   }
 
   //读文件信息
   if (1 != fread( &FileHead, sizeof(BitMapFileHeader),1, fb))
   {
     printf("read BitMapFileHeader error!\n");
     fclose(fb);
     return -1;
   }
   if (memcmp(FileHead.bfType, "BM", 2) != 0)
   {
     printf("it's not a BMP file\n");
     fclose(fb);
     return -1;
   }
   
   //读位图信息
   if (1 != fread( (char *)&InfoHead, sizeof(BitMapInfoHeader),1, fb))
   {
     printf("read BitMapInfoHeader error!\n");
     fclose(fb);
     return -1;
   }
   
   printf("BitMapFileHeader : \n");
   printf("bfType  %s \n",FileHead.bfType);
   printf("bfSize %u\n",FileHead.bfSize);
   printf("bfReserved1 %d\n",FileHead.bfReserved1);
   printf("bfReserved2 %d\n",FileHead.bfReserved2);
   printf("bfOffBits %u\n",FileHead.bfOffBits);
   
   
   printf("BitMapInfoHeader :\n");
   printf("biSize %u\n",InfoHead.biSize);
   printf("biWidth %ld\n",InfoHead.biWidth);
   printf("biHeight %ld\n",InfoHead.biHeight);
   printf("biPlanes %d\n",InfoHead.biPlanes);
   printf("biBitCount %d\n",InfoHead.biBitCount);
   printf("biCompression %u\n",InfoHead.biCompression);
   printf("biSizeImage %u\n",InfoHead.biSizeImage);
   printf("biXPelsPerMeter %ld\n",InfoHead.biXPelsPerMeter);
   printf("biYPelsPerMeter %ld\n",InfoHead.biYPelsPerMeter);
   printf("biClrUsed %u\n",InfoHead.biClrUsed);
   printf("biClrImportant %u\n",InfoHead.biClrImportant);
   

   //跳转至数据区
   fseek(fb, FileHead.bfOffBits, SEEK_SET);
   
   int len = InfoHead.biBitCount / 8;    //原图一个像素占几字节
   int bits_len = bits_per_pixel / 8;    //屏幕一个像素占几字节  //循环显示
 
   while(!feof(fb))
   {
     tmp = 0;
     rgb.Reserved = 0xFF;
   
     if (len != fread((char *)&rgb, 1, len, fb))
       break;
   
     //计算该像素在映射内存起始地址的偏移量
     location = line_x * bits_len + (InfoHead.biHeight - line_y - 1) * xres * bits_len;
   
     tmp |= rgb.Reserved << 24 | rgb.Red << 16 | rgb.Green << 8 | rgb.Blue;
   
     *((unsigned long *)(fbp + location)) = tmp;    
   
     line_x++;    
     if (line_x == InfoHead.biWidth )
     {
       line_x = 0;
       line_y++;
       if(line_y == InfoHead.biHeight)    
         break;    
     }    
   }
   
   fclose(fb);
 
   return 0;
 }