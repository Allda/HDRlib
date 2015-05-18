#include "tonemapping.h"

ToneMapping::ToneMapping(HDRImage * image)
{
    this->image = image;
    height = image->getMat().rows;
    width = image->getMat().cols;
    channels = image->getMat().channels();
}

ToneMapping::~ToneMapping()
{

}

