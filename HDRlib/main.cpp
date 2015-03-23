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

#include "multiexposureCamera/camera_controller.h"

#include "tonemapping/linearoperator.h"
#include "tonemapping/logoperator.h"
#include "tonemapping/expoperator.h"
#include "tonemapping/reinhardglobaloperator.h"

#include <QApplication>
#include "gui/mainwindow.h"

#include <QDir>
#include <QStringList>
using namespace cv;
using namespace std;


deque<LDRImage *> loadLDRImages(string name, string extension, int count, int start, bool expand){
    deque<LDRImage *> d;
    for(int i = 0; i < count; i++){
        string fileName;
        if(expand && start < 10)
            fileName = name + "0" +to_string(start++) + "." + extension;
        else
            fileName = name + to_string(start++) + "." + extension;
        cout << fileName << endl;
        LDRImage * ldrImage = new LDRImage(fileName.c_str());
        d.push_back(ldrImage);
    }
    return d;
}


vector<LDRImage *> cameraInit(){
    CameraController camera_controller;
    vector<LDRImage *> ldrImageList;


    if ( ! camera_controller.loadConfiguration( "multiexposureCamera/configuration.txt" ) )
        return ldrImageList;


    if ( ! camera_controller.connectToCamera() )
        return ldrImageList;

    if ( ! camera_controller.configureCamera() )
        return ldrImageList;

    try{
        ldrImageList = camera_controller.getImages();
    }catch(exception e){
        cout << "Ouups " << e.what() << endl;
    }

    for(unsigned i = 0; i < ldrImageList.size();i++){
        LDRImage * ldrImage = ldrImageList.at(i);
        cout << ldrImage->getExposureTime() <<endl;
        ldrImage->showImage();
    }
    return ldrImageList;

}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    /*vector<LDRImage *> ldrImages =  cameraInit();
    deque<LDRImage *> ldrImages2;
    for(unsigned i= 0; i < ldrImages.size();i++){
        Mat colorMat;
        LDRImage * ldrIm = ldrImages.at(i);
        cvtColor(ldrIm->getImageMat(),colorMat,COLOR_GRAY2BGR);
        ldrImages2.push_back(new LDRImage(colorMat, ldrIm->getExposureTime()));
    }*/
    /*LDRImage * image = new LDRImage("img/1.JPG");
    LDRImage * image2 = new LDRImage("img/2.JPG");
    LDRImage * image3 = new LDRImage("img/3.JPG");*/


    /*deque<LDRImage *> ldrImage = loadLDRImages(string("img/chram"),string("jpg"),3,1, false);



    HDRCreator * hdr = new HDRCreator(ldrImage);

    DebevecMalikWF * wf = new DebevecMalikWF();


    hdr->setWeightFunction(wf);
    cout << wf->getWeight(8) << endl;

    HDRImage * out;
    try{
        out = hdr->buildHDR();
        //cout << "Done" << endl;
        //cout << out.getMat() << endl;
        namedWindow( "Display window", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO );// Create a window for display.
        imshow( "Display window", out->getMat() );                   // Show our image inside it.
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


    ReinhardGlobalOperator * op = new ReinhardGlobalOperator(out);
    LDRImage * outputLDR = op->process();
    //cout << outputLDR->getImageMat();
    outputLDR->showImage();*/


    return a.exec();
}
