#include "imageframe.h"

#include <iostream>

ImageFrame::ImageFrame(LDRImage *ldrImage , QWidget *parent, bool clickable)
    :QLabel(parent)
{
    this->ldrImage = ldrImage;
    this->clickable = clickable;
    active = false;
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
        active = true;
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
    active = true;
    this->setStyleSheet("border: 3px solid #A00");
}

void ImageFrame::clearBorder(){
    this->setStyleSheet("border: 3px solid white");
    active = false;
}

bool ImageFrame::isActive(){
    return active;
}

