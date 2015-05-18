#ifndef BUILDHDRTHREAD_H
#define BUILDHDRTHREAD_H
#include "hdrcreator.h"
#include "hdrimage.h"
#include <QThread>
#include <QtCore>

class BuildHDRThread : public QThread
{
    Q_OBJECT
public:
    BuildHDRThread();
    void addResult(HDRImage * hdrImage);
    void addHDRcreator(HDRCreator * hdr);
    HDRImage * getHDRimage();
    QTime timer;

    ~BuildHDRThread();
protected:
    void run();
private:
    HDRCreator * hdr;
    HDRImage * hdrImage;
};

#endif // BUILDHDRTHREAD_H
