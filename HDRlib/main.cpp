#include <QCoreApplication>
#include "ldrimage.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

#include "hdrcreator.h"
#include "debevecmalikwf.h"
#include "exceptions/notenoughimagesexception.h"
#include "exceptions/imagesizeexception.h"
#include "hdrimage.h"
using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    /*Mat mat;
        mat = imread("1.tif", CV_LOAD_IMAGE_COLOR);   // Read the file

        if(! mat.data )                              // Check for invalid input
        {
            cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }




    Mat mat2;
        mat2 = imread("2.tif", CV_LOAD_IMAGE_COLOR);   // Read the file

        if(! mat2.data )                              // Check for invalid input
        {
            cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }




    Mat mat3;
        mat3 = imread("3.tif", CV_LOAD_IMAGE_COLOR);   // Read the file

        if(! mat3.data )                              // Check for invalid input
        {
            cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }



    LDRImage * image = new LDRImage(mat,0.5);
    LDRImage * image2 = new LDRImage(mat2,0.01);
    LDRImage * image3 = new LDRImage(mat3,0.005);*/

    LDRImage * image = new LDRImage("img/1.JPG");
    //image->showImage();
    LDRImage * image2 = new LDRImage("img/2.JPG");
    LDRImage * image3 = new LDRImage("img/3.JPG");

    //LDRImage *image2 = new LDRImage("img1.JPG");


    //cout << image->getImageMat() << endl;

    /*for(int y = 0; y < image->getHeight();y++){
        for(int x = 0; x < image->getWidth();x++){
            Vec3b  pixel = image->getPixel(x,y);
            cout << (int)pixel.val[0] << " " << (int)pixel.val[1] << " "<< (int)pixel.val[2] << endl;
        }
    }*/

    HDRCreator * hdr = new HDRCreator();

    DebevecMalikWF * wf = new DebevecMalikWF();

    hdr->addImage(image);
    hdr->addImage(image2);
    hdr->addImage(image3);
    hdr->setWeightFunction(wf);
    cout << wf->getWeight(8) << endl;
    try{
        HDRImage out = hdr->buildHDR();
        cout << "Done" << endl;
        /*cout << out.getMat() << endl;
        namedWindow( "Display window", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO );// Create a window for display.
        imshow( "Display window", out.getMat() );                   // Show our image inside it.*/
    }
    catch(NotEnoughImagesException e){
        cout << e.what() << endl;
    }
    catch(ImageSizeException e){
        cout << e.what() << endl;
    }
    catch(exception e){
        cout << "WF none"  << e.what() << endl;
    }



    return a.exec();
}
