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
    hdrImage = hdr->buildHDR();
}

HDRImage * BuildHDRThread::getHDRimage(){
    return hdrImage;
}

BuildHDRThread::~BuildHDRThread()
{

}

