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

// Load LDR images from storage
deque<LDRImage *> loadLDRImages(string name, string extension, int count, int start, bool expand){
    deque<LDRImage *> d;
    for(int i = 0; i < count; i++){
        string fileName;
        if(expand && start < 10)
            fileName = name + "0" +to_string(start++) + "." + extension;
        else
            fileName = name + to_string(start++) + "." + extension;
        LDRImage * ldrImage = new LDRImage(fileName.c_str());
        d.push_back(ldrImage);
    }
    return d;
}

// Connect to camera and grap 3 frames.. for comand line only
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
        ldrImage->showImage();
    }
    return ldrImageList;

}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // GUI application
    MainWindow w;
    w.show();

    return a.exec();
}
