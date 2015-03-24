#include "imageframe.h"

#include <iostream>

ImageFrame::ImageFrame(LDRImage *ldrImage , QWidget *parent, bool clickable)
    :QLabel(parent)
{
    this->ldrImage = ldrImage;
    this->clickable = clickable;
    if(clickable){
        setStyleSheet("border: 3px solid white");
        connect(this, SIGNAL(clicked(ImageFrame*)),this,SLOT(slotClicked()));
    }
}

ImageFrame::~ImageFrame()
{

}

void ImageFrame::setLDRImage(LDRImage * ldrImage){
    this->ldrImage = ldrImage;
}

void ImageFrame::mousePressEvent ( QMouseEvent * event )
{
    if(clickable){
        emit clicked(this);
        setBorder();
    }
}

void ImageFrame::slotClicked(){
    setBorder();
}

LDRImage * ImageFrame::getLDRImage(){
    return ldrImage;
}

void ImageFrame::setBorder(){
    this->setStyleSheet("border: 3px solid #AAA");
}

void ImageFrame::clearBorder(){
    this->setStyleSheet("border: 3px solid white");
}

