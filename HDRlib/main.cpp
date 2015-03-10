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

#include "tonemapping/linearoperator.h"
#include "tonemapping/logoperator.h"
#include "tonemapping/expoperator.h"
using namespace cv;
using namespace std;


deque<LDRImage *> loadLDRImages(string name, string extension, int count, int start){
    deque<LDRImage *> d;
    for(int i = 0; i < count; i++){
        string fileName = name + to_string(start++) + "." + extension;
        cout << fileName << endl;
        LDRImage * ldrImage = new LDRImage(fileName.c_str());
        d.push_back(ldrImage);
    }
    return d;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);



    /*LDRImage * image = new LDRImage("img/1.JPG");
    LDRImage * image2 = new LDRImage("img/2.JPG");
    LDRImage * image3 = new LDRImage("img/3.JPG");*/


    deque<LDRImage *> ldrImage = loadLDRImages(string("img/"),string("JPG"),3,1);

    HDRCreator * hdr = new HDRCreator(ldrImage);

    DebevecMalikWF * wf = new DebevecMalikWF();

    /*hdr->addImage(image);
    hdr->addImage(image2);
    hdr->addImage(image3);*/
    hdr->setWeightFunction(wf);
    cout << wf->getWeight(8) << endl;

    HDRImage * out;
    try{
        out = hdr->buildHDR();
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


    ExpOperator * op = new ExpOperator(out);
    LDRImage * outputLDR = op->process(0.1,0.02);
    //cout << outputLDR->getImageMat();
    outputLDR->showImage();


    return a.exec();
}
