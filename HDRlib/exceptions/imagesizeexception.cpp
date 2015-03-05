#include "imagesizeexception.h"

ImageSizeException::ImageSizeException()
{

}

const char * ImageSizeException::what(){
    return "Images are not same size";
}

ImageSizeException::~ImageSizeException()
{

}

