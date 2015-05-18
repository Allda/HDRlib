#include "buildhdrthread.h"
#include <iostream>

BuildHDRThread::BuildHDRThread()
{

}

void BuildHDRThread::addHDRcreator(HDRCreator *hdr){
    this->hdr = hdr;
}
void BuildHDRThread::addResult(HDRImage *hdrImage){
    this->hdrImage = hdrImage;
}

void BuildHDRThread::run(){
    timer.start();
    hdrImage = hdr->buildHDR();
    std::cout << "HDR build time: " << timer.elapsed() << std::endl;
}

HDRImage * BuildHDRThread::getHDRimage(){
    return hdrImage;
}

BuildHDRThread::~BuildHDRThread()
{

}

