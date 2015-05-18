#include "hdrimage.h"

HDRImage::HDRImage(Mat mat)
{
    this->image = mat;

}

// Load HDR image from HDRE format
HDRImage::HDRImage(QString fileName){
    FILE *f;
    f = fopen(fileName.toStdString().c_str() ,"rb");
    int width, height;
    float* imageData;
    RGBE_ReadHeader(f,&width,&height,NULL);
    imageData = (float *)malloc(sizeof(float)*3*width*height);
    RGBE_ReadPixels_RLE(f,imageData,width,height);
    image = Mat(height,width,CV_32FC3,imageData);
    fclose(f);
}

Mat HDRImage::getMat(){
    return image;
}

HDRImage::~HDRImage()
{

}

// Save HDR as HDRE format
void HDRImage::saveHDR(QString fileName){
    FILE * file;
    file = fopen(fileName.toStdString().c_str(),"wb");
    if(file == NULL)
        return;

    RGBE_WriteHeader(file,image.cols ,image.rows,NULL);
    RGBE_WritePixels(file,(float*)image.data,image.cols * image.rows);
    fclose(file);
}

