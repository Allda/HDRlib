#include "notenoughimagesexception.h"

NotEnoughImagesException::NotEnoughImagesException()
{

}

const char * NotEnoughImagesException::what(){
    return "Not enough images. Must contain at least 3 images";
}

NotEnoughImagesException::~NotEnoughImagesException()
{

}

