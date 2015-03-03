#include <QCoreApplication>
#include "ldrimage.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    Mat mat;
        mat = imread("3x3.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file

        if(! mat.data )                              // Check for invalid input
        {
            cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }



    LDRImage * image = new LDRImage(mat,100);

    image->toString();
    //image->showImage();

    LDRImage *image2 = new LDRImage("img1.JPG");
    image2->toString();
    //image2->showImage();

    for(int y = 0; y < image->getHeight();y++){
        for(int x = 0; x < image->getWidth();x++){
            Vec3b  pixel = image->getPixel(x,y);
            cout << (int)pixel.val[0] << " " << (int)pixel.val[1] << " "<< (int)pixel.val[2] << endl;
        }
    }

    return a.exec();
}
