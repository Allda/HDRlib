#ifndef IMAGEFRAME_H
#define IMAGEFRAME_H
#include <QLabel>
#include "ldrimage.h"


class ImageFrame : public QLabel
{
    Q_OBJECT
public:
    ImageFrame(LDRImage * ldrImage , QWidget * parent = 0, bool clickable = true);
    ~ImageFrame();
    LDRImage * getLDRImage();
    void setLDRImage(LDRImage * ldrImage);
    void setBorder();
    void clearBorder();
private:
    LDRImage * ldrImage;
    bool clickable;
signals:
    void clicked(ImageFrame * frame);

private slots:
    void slotClicked();

protected:
    void mousePressEvent ( QMouseEvent * event ) ;

};

#endif // IMAGEFRAME_H
