#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QString>

#include <iostream>
#include "gui/imageframe.h"
#include "gui/camerasettingsdialog.h"
#include "gui/buildhdrthread.h"
#include <QMessageBox>
#include "tonemapping/reinhardglobaloperator.h"
#include <QSpacerItem>
#include <QProgressDialog>
#include "stdio.h"
#include "rgbe.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initMenu();

    initLoadPage();
    initGenerateHDRpage();
    initStyleSheet();
    ui->stackedWidget->setCurrentIndex(0);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initStyleSheet(){
    this->setObjectName("main");
    //this->setStyleSheet("background-color: #444; color: #fff");
    this->setStyleSheet("* {background-color: #444; color: #fff} QPushButton{background-color:#777; font-weight:bold} QPushButton:disabled{color:#aaa}");
    ui->menuBar->setStyleSheet("background-color: #2D2D2D;");
    ui->statusBar->setStyleSheet("background-color: #2D2D2D;");
    //scrollAreaWidgetContents->setStyleSheet("border: 2px solid #fff");
    tonemappingWidgetContents->setObjectName("tonemappingWidget");
    scrollAreaWidgetContents->setObjectName("tonemappingWidget");
    scrollAreaWidgetContents->setStyleSheet("#tonemappingWidget{border: 2px solid #fff}");
    tonemappingWidgetContents->setStyleSheet("#tonemappingWidget{border: 2px solid #fff}");
    finalImage->setStyleSheet("background-color: #555");
    imageExifInfo->setStyleSheet("background-color:#666;");
    ui->menuBar->setStyleSheet("QMenu::item:selected{background:#a00} QMenu::item:disabled{color:#AAA} QMenuBar{background-color:2D2D2D}");
}

void MainWindow::initMenu(){
    fileMenu = menuBar()->addMenu(tr("&File"));
    cameraMenu = menuBar()->addMenu(tr("&Camera"));
    openFileAct = new QAction(tr("&Open file"),this);
    loadCameraImagesAct = new QAction(tr("&Import camera images"),this);
    openHDRAct = new QAction(tr("&Open HDR"),this);
    saveHDRAct = new QAction(tr("&Save as HDR"),this);
    exportAct = new QAction(tr("&Export image"),this);
    saveSequenceAct = new QAction(tr("&Save sequence"),this);
    exitAct = new QAction(tr("&Exit"), this);
    fileMenu->addAction(openFileAct);
    fileMenu->addAction(loadCameraImagesAct);
    fileMenu->addAction(openHDRAct);
    fileMenu->addAction(saveHDRAct);
    fileMenu->addAction(exportAct);
    fileMenu->addAction(saveSequenceAct);
    fileMenu->addAction(exitAct);
    cameraSettingsAct = new QAction(tr("&Camera settings"),this);
    cameraPreviewAct = new QAction(tr("&Camera preview"),this);
    cameraMenu->addAction(cameraSettingsAct);
    cameraMenu->addAction(cameraPreviewAct);
    connect(openFileAct,SIGNAL(triggered()),this,SLOT(openFile()));
    connect(openHDRAct,SIGNAL(triggered()),this,SLOT(openHDR()));
    connect(loadCameraImagesAct,SIGNAL(triggered()), this, SLOT(importCameraImages()));
    connect(saveHDRAct,SIGNAL(triggered()),this,SLOT(saveHDR()));
    connect(exportAct, SIGNAL(triggered()),this,SLOT(exportImage()));
    connect(saveSequenceAct,SIGNAL(triggered()),this,SLOT(saveSequence()));
    connect(exitAct,SIGNAL(triggered()), this, SLOT(exit()));
    connect(cameraSettingsAct,SIGNAL(triggered()),this,SLOT(openCameraSettingsDialog()));
    connect(cameraPreviewAct,SIGNAL(triggered()),this,SLOT(cameraPreview()));
    saveSequenceAct->setEnabled(false);
    exportAct->setEnabled(false);
    saveHDRAct->setEnabled(false);
}

void MainWindow::initLoadPage(){
    loadImageLayout = new QGridLayout(ui->loadImagePage);

    loadedImageScrollArea = new QScrollArea(ui->loadImagePage);
    loadedImageScrollArea->setMaximumSize(QSize(250, 16777215));
    loadedImageScrollArea->setMinimumWidth(276);
    loadedImageScrollArea->setWidgetResizable(true);
    scrollAreaWidgetContents = new QWidget();
    scrollAreaWidgetContents->setGeometry(QRect(0, 0, 248, 0));

    scrollImagesLayout = new QVBoxLayout(scrollAreaWidgetContents);
    scrollImagesLayout->setContentsMargins(11, 11, 11, 11);
    scrollImagesLayout->setSpacing(6);
    scrollImagesLayout->setAlignment(scrollAreaWidgetContents, Qt::AlignCenter);

    imageListHeader = new QLabel("Image list",ui->loadImagePage);

    loadedImageScrollArea->setWidget(scrollAreaWidgetContents);

    loadedBigImage = new ImageFrame(NULL, ui->loadImagePage,false);
    imageExifInfo = new QLabel(ui->loadImagePage);
    imageExifInfo->setStyleSheet("background-color:white;");
    QPushButton * nextButton = new QPushButton(ui->loadImagePage);
    nextButton->setText("Next");
    QPushButton * deleteButton = new QPushButton("Delete", ui->loadImagePage);
    deleteButton->setStyleSheet("background-color:#a00");
    connect(deleteButton,SIGNAL(clicked()), this, SLOT(deleteImage()));


    loadedBigImage->setText("File >> Open photo  -  to add photo");
    loadedBigImage->setAlignment(Qt::AlignCenter);

    loadImageLayout->addWidget(nextButton,3,1,1,1);
    loadImageLayout->addWidget(loadedImageScrollArea,1,1,1,1);
    loadImageLayout->addWidget(loadedBigImage,0,0,3,1);
    loadImageLayout->addWidget(imageExifInfo,3,0);
    loadImageLayout->addWidget(imageListHeader,0,1);
    loadImageLayout->addWidget(deleteButton,2,1);

    connect(nextButton,SIGNAL(clicked()),this,SLOT(switchPage()));
}

void MainWindow::initGenerateHDRpage(){
    generateHDRlayout =  new QGridLayout(ui->generateHDRpage);

    tonemappingScrollArea = new QScrollArea(ui->generateHDRpage);
    tonemappingScrollArea->setMaximumSize(QSize(250, 16777215));
    tonemappingScrollArea->setMinimumWidth(276);
    tonemappingScrollArea->setWidgetResizable(true);
    tonemappingWidgetContents = new QWidget();
    tonemappingWidgetContents->setGeometry(QRect(0, 0, 248, 0));

    QVBoxLayout * boxLayout = new QVBoxLayout(tonemappingWidgetContents);
    QWidget * topPart = new QWidget(tonemappingWidgetContents);
    boxLayout->addWidget(topPart);
    QSpacerItem * spacer =  new QSpacerItem(1,1,QSizePolicy::Fixed,QSizePolicy::Expanding);
    boxLayout->addSpacerItem(spacer);

    tonemappingLayout = new QGridLayout(topPart);
    tonemappingLayout->setContentsMargins(11, 11, 11, 11);
    tonemappingLayout->setSpacing(6);
    tonemappingLayout->setAlignment(tonemappingWidgetContents, Qt::AlignCenter);

    tonemappingScrollArea->setWidget(tonemappingWidgetContents);

    QPushButton * backButton = new QPushButton("Back",ui->generateHDRpage);
    generateButton = new QPushButton("Start tone mapping", ui->generateHDRpage);
    generateButton->setEnabled(false);

    finalImage = new ImageFrame(NULL, ui->generateHDRpage,false);
    finalImage->setAlignment(Qt::AlignCenter);
    finalImage->setSizePolicy(QSizePolicy ::Expanding , QSizePolicy ::Expanding );
    finalImage->setMinimumSize(10,10);
    //finalImage->setBorder();
    //tonemappingScrollArea->setStyleSheet("border: 3px solid white");
    generateHDRlayout->addWidget(tonemappingScrollArea,1,0,1,1);
    generateHDRlayout->addWidget(backButton,0,0);
    generateHDRlayout->addWidget(generateButton,2,0);
    generateHDRlayout->addWidget(finalImage,0,2,3,1);

    connect(backButton,SIGNAL(clicked()),this,SLOT(switchPage()));
    connect(generateButton,SIGNAL(clicked()),this,SLOT(toneMapping()));

    radioGroup = new QButtonGroup(tonemappingWidgetContents);
    linearMappingRadio = new QRadioButton("Linear mapping", tonemappingWidgetContents);
    logMappingRadio = new QRadioButton("Logarithmic mapping", tonemappingWidgetContents);
    expMappingRadio = new QRadioButton("Exponencial mapping", tonemappingWidgetContents);
    adaptiveLogMapRadio = new QRadioButton("Adaptive logarithmic mapping", tonemappingWidgetContents);
    reinhardMappingRadio = new QRadioButton("Reinhard mapping", tonemappingWidgetContents);
    linearMappingRadio->setChecked(true);
    radioGroup->addButton(linearMappingRadio);
    radioGroup->addButton(logMappingRadio);
    radioGroup->addButton(expMappingRadio);
    radioGroup->addButton(adaptiveLogMapRadio);
    radioGroup->addButton(reinhardMappingRadio);

    QLabel * logQ = new QLabel("Q:", tonemappingWidgetContents);
    logQ->setStyleSheet("margin-left:30px");
    QLabel * logK = new QLabel("K:", tonemappingWidgetContents);
    logK->setStyleSheet("margin-left:30px");
    logQslider = new QSlider(Qt::Horizontal,tonemappingWidgetContents);
    logQVal = new QLabel("0",tonemappingWidgetContents);
    connect(logQslider,SIGNAL(valueChanged(int)),logQVal, SLOT(setNum(int)));
    logKslider = new QSlider(Qt::Horizontal,tonemappingWidgetContents);
    logKVal = new QLabel("0",tonemappingWidgetContents);
    connect(logKslider,SIGNAL(valueChanged(int)),logKVal, SLOT(setNum(int)));

    QLabel * expQ = new QLabel("Q:", tonemappingWidgetContents);
    QLabel * expK = new QLabel("K:", tonemappingWidgetContents);
    expQ->setStyleSheet("margin-left:30px");
    expK->setStyleSheet("margin-left:30px");
    expQslider = new QSlider(Qt::Horizontal,tonemappingWidgetContents);
    expQVal = new QLabel("0",tonemappingWidgetContents);
    connect(expQslider,SIGNAL(valueChanged(int)),expQVal, SLOT(setNum(int)));
    expKslider = new QSlider(Qt::Horizontal,tonemappingWidgetContents);
    expKVal = new QLabel("0",tonemappingWidgetContents);
    connect(expKslider,SIGNAL(valueChanged(int)),expKVal, SLOT(setNum(int)));

    QLabel * adaptiveLogBias = new QLabel("Bias:", tonemappingWidgetContents);
    QLabel * adaptiveLogLdmax = new QLabel("Ld max:", tonemappingWidgetContents);
    adaptiveLogBias->setStyleSheet("margin-left:30px");
    adaptiveLogLdmax->setStyleSheet("margin-left:30px");
    adaptiveLogLdmaxSlider = new QSlider(Qt::Horizontal,tonemappingWidgetContents);
    adaptiveLogLdmaxSlider->setMaximum(20000);
    adaptiveLogLdmaxVal = new QLabel("0",tonemappingWidgetContents);
    connect(adaptiveLogLdmaxSlider,SIGNAL(valueChanged(int)),adaptiveLogLdmaxVal, SLOT(setNum(int)));
    adaptiveLogBiasSlider = new QSlider(Qt::Horizontal,tonemappingWidgetContents);
    adaptiveLogBiasVal = new QLabel("0",tonemappingWidgetContents);
    connect(adaptiveLogBiasSlider,SIGNAL(valueChanged(int)),adaptiveLogBiasVal, SLOT(setNum(int)));



    tonemappingLayout->addWidget(linearMappingRadio,0,0,1,3);
    tonemappingLayout->addWidget(logMappingRadio,1,0,1,3);
    tonemappingLayout->addWidget(logQ,2,0);
    tonemappingLayout->addWidget(logQslider,2,1);
    tonemappingLayout->addWidget(logQVal,2,2);
    tonemappingLayout->addWidget(logK,3,0);
    tonemappingLayout->addWidget(logKslider,3,1);
    tonemappingLayout->addWidget(logKVal,3,2);
    tonemappingLayout->addWidget(expMappingRadio,4,0,1,3);
    tonemappingLayout->addWidget(expQ,5,0);
    tonemappingLayout->addWidget(expQslider,5,1);
    tonemappingLayout->addWidget(expQVal,5,2);
    tonemappingLayout->addWidget(expK,6,0);
    tonemappingLayout->addWidget(expKslider,6,1);
    tonemappingLayout->addWidget(expKVal,6,2);
    tonemappingLayout->addWidget(adaptiveLogMapRadio,7,0,1,3);
    tonemappingLayout->addWidget(adaptiveLogLdmax,8,0);
    tonemappingLayout->addWidget(adaptiveLogLdmaxSlider,8,1);
    tonemappingLayout->addWidget(adaptiveLogLdmaxVal,8,2);
    tonemappingLayout->addWidget(adaptiveLogBias,9,0);
    tonemappingLayout->addWidget(adaptiveLogBiasSlider,9,1);
    tonemappingLayout->addWidget(adaptiveLogBiasVal,9,2);
    tonemappingLayout->addWidget(reinhardMappingRadio,10,0,1,3);
}

void MainWindow::addImages(std::string filename){
    LDRImage * ldrImage = new LDRImage(filename.c_str());
    addImages(ldrImage);

}

void MainWindow::addImages(LDRImage *ldrImage){
    ImageFrame * frame = new ImageFrame(ldrImage);
    connect(frame,SIGNAL(clicked(ImageFrame *)),this,SLOT(changeBigImage(ImageFrame *)));
    // first image = set border, set as bigImage
    if(imageList.size() == 0){
        emit frame->clicked(frame);
        saveSequenceAct->setEnabled(true);
    }
    imageList.append(frame);

    frame->setCursor(Qt::PointingHandCursor);

    QImage qimage = ldrImage->getQImage(230,200);
    frame->setPixmap(QPixmap::fromImage(qimage));
    frame->setMaximumSize(qimage.width(),qimage.height());
    frame->setAlignment(Qt::AlignCenter);

    scrollImagesLayout->addWidget(frame);
}

void MainWindow::openFile(){
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    ui->stackedWidget->setCurrentIndex(0);
    for(int i = 0; i < fileNames.length();i++){
        addImages(fileNames[i].toUtf8().constData());
    }

}


void MainWindow::switchPage(){
    int currentIndex = ui->stackedWidget->currentIndex();
    if(currentIndex == 0){
        if(imageList.size() < 3){
            QMessageBox msgBox(this);
            msgBox.setText("Not enough images                        ");
            msgBox.setInformativeText("Must contain at least 3 images");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }
        else{
            ui->stackedWidget->setCurrentIndex(1);
            finalImage->clear();
            generateButton->setEnabled(false);
            hdr->setWeightFunction(wf);
            hdr->addVector(getLDRImageList());
            thread = new BuildHDRThread();
            thread->addHDRcreator(hdr);
            thread->addResult(hdrImage);
            connect(thread,SIGNAL(finished()), this, SLOT(HDRdone()));
            thread->start();
            ui->statusBar->showMessage("Building HDR ...");

        }
    }
    else{
        ui->stackedWidget->setCurrentIndex(0);
        exportAct->setEnabled(false);
        saveHDRAct->setEnabled(false);
        generateButton->setEnabled(true);
    }
}

void MainWindow::changeBigImage(ImageFrame * frame){
    for(int i = 0; i < imageList.size(); i++){
        imageList.at(i)->clearBorder();
    }
    frame->setBorder();
    LDRImage * ldrImage = frame->getLDRImage();
    int width = loadedBigImage->width();
    int height = loadedBigImage->height();
    loadedBigImage->setLDRImage(ldrImage);
    loadedBigImage->setPixmap(QPixmap::fromImage(ldrImage->getQImage(width-30,height-30)));
    imageExifInfo->clear();
    int imageWidth = ldrImage->getWidth();
    int imageHeight = ldrImage->getHeight();
    QString exifInfo =  QString("Resolution: %1x%2\nExposure time: %3s")
            .arg(QString::number(imageWidth), QString::number(imageHeight),
                 QString::number(ldrImage->getExposureTime()));
    imageExifInfo->setText(exifInfo);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    if(loadedBigImage->getLDRImage() != NULL){
        LDRImage * ldrImage = loadedBigImage->getLDRImage();
        int width = loadedBigImage->width();
        int height = loadedBigImage->height();
        loadedBigImage->setPixmap(QPixmap::fromImage(ldrImage->getQImage()).scaled(width-30,height-30,Qt::KeepAspectRatio));
    }
    if(finalImage->getLDRImage() != NULL){
        finalImage->clear();
        LDRImage * ldrImage = finalImage->getLDRImage();
        int width = finalImage->width();
        int height = finalImage->height();
        finalImage->setPixmap(QPixmap::fromImage(ldrImage->getQImage()).scaled(width-30,height-30,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    }

}

string MainWindow::int2String(int value){
    //char *intStr = itoa(value);
    //return string(intStr);
}

string MainWindow::double2String(double val){
    std::ostringstream strs;
    strs << val;
    return strs.str();
}

void MainWindow::openCameraSettingsDialog(){
    CameraSettingsDialog * camDialog = new CameraSettingsDialog();
    camDialog->setModal(true);
    camDialog->show();
}

void MainWindow::importCameraImages(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Input filter", "Use bayer filter?",
                                QMessageBox::Yes|QMessageBox::No);


    CameraController camera_controller;
    vector<LDRImage *> ldrImageList;

    QMessageBox msgBox(this);
    ui->statusBar->showMessage("Loading configuration file..");
    msgBox.setIcon(QMessageBox::Critical);
    if ( ! camera_controller.loadConfiguration( "multiexposureCamera/configuration.txt" ) ){
        msgBox.setText("Can't load config file                        ");
        msgBox.exec();
        return;
    }
    ui->statusBar->showMessage("Connecting to camera..");



    if ( ! camera_controller.connectToCamera() ){
        msgBox.setText("Unable to connect to the camera           ");
        msgBox.exec();
        return;
    }

    ui->statusBar->showMessage("Configure camera..");

    if ( ! camera_controller.configureCamera() ){
        msgBox.setText("Unable to configure the camera           ");
        msgBox.exec();
        return;
    }

    ui->statusBar->showMessage("Downloading images..");

    try{
        ldrImageList = camera_controller.getImages();
    }catch(exception e){
            msgBox.setText("Unable to get images           ");
            msgBox.exec();
            return;
    }

    for(unsigned i = 0; i < ldrImageList.size();i++){
        LDRImage * ldrImage = ldrImageList.at(i);
        Mat origMat = ldrImage->getImageMat();
        Mat colorMat;
        if (reply == QMessageBox::Yes) {
            cvtColor(origMat, colorMat,CV_BayerGR2BGR);
        }
        else {
            cvtColor(origMat,colorMat,CV_GRAY2BGR);
        }

        ldrImage->setImageMat(colorMat);
        this->addImages(ldrImage);
    }
    ui->statusBar->showMessage("Done",5000);



}

void MainWindow::exit(){
    this->close();
}

vector<LDRImage *> MainWindow::getLDRImageList(){
    ldrImageList.clear();
    for(int i = 0; i< imageList.size();i++){
        ldrImageList.push_back(imageList.at(i)->getLDRImage());
    }
    return ldrImageList;
}

void MainWindow::HDRdone(){
    this->hdrImage = thread->getHDRimage();
    delete thread;
    ui->statusBar->showMessage("HDR done",10000);
    generateButton->setEnabled(true);
    saveHDRAct->setEnabled(true);
}

void MainWindow::toneMapping(){

    ui->statusBar->showMessage("Tonemapping");
    if(linearMappingRadio->isChecked()){
        LinearOperator * linOp = new LinearOperator(hdrImage);
        finalLDRimage = linOp->process();
    }
    else if(logMappingRadio->isChecked()){
        LogOperator * logOp = new LogOperator(hdrImage);
        double q,k;
        q = logQslider->value() * 0.01;
        k = logKslider->value() * 0.01;
        finalLDRimage = logOp->process(q,k);
    }
    else if(expMappingRadio->isChecked()){
        ExpOperator * expOp = new ExpOperator(hdrImage);
        double q,k;
        q = expQslider->value() * 0.01;
        k = expKslider->value() * 0.01;
        finalLDRimage = expOp->process(q,k);
    }
    else if(adaptiveLogMapRadio->isChecked()){
        AdaptiveLogOperator * adaptiveLogOp = new AdaptiveLogOperator(hdrImage);
        double q,k;
        q = adaptiveLogLdmaxSlider->value();
        k = adaptiveLogBiasSlider->value() * 0.01;
        finalLDRimage = adaptiveLogOp->process(q,k);
    }
    else if(reinhardMappingRadio->isChecked()){
        ReinhardGlobalOperator * reinhardOp = new ReinhardGlobalOperator(hdrImage);
        finalLDRimage = reinhardOp->process();
    }

    int width = finalImage->width();
    int height = finalImage->height();
    finalImage->setLDRImage(finalLDRimage);
    finalImage->setPixmap(QPixmap::fromImage(finalLDRimage->getQImage(width-30,height-30)));
    ui->statusBar->showMessage("Tonemapping done",10000);
    exportAct->setEnabled(true);
}

void MainWindow::saveHDR(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               ".",
                               tr("Images (*.hdr)"));
    QFileInfo f(fileName);
    if(f.suffix() == "")
        fileName += ".hdr";
    std::cout << fileName.toStdString() << std::endl;
    FILE * file;
    file = fopen(fileName.toStdString().c_str(),"wb");
    if(file == NULL)
        return;

    RGBE_WriteHeader(file,hdrImage->getMat().cols ,hdrImage->getMat().rows,NULL);
    RGBE_WritePixels(file,(float*)hdrImage->getMat().data,hdrImage->getMat().cols * hdrImage->getMat().rows);
    fclose(file);
}

void MainWindow::openHDR(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open HDR"),
                               ".",
                               tr("Images (*.hdr)"));
    FILE *f;
    f = fopen(fileName.toStdString().c_str() ,"rb");
    int width, height;
    float* image;
    RGBE_ReadHeader(f,&width,&height,NULL);
    image = (float *)malloc(sizeof(float)*3*width*height);
    RGBE_ReadPixels_RLE(f,image,width,height);
    Mat mat = Mat(height,width,CV_32FC3,image);
    hdrImage = new HDRImage(mat);
    ui->stackedWidget->setCurrentIndex(1);
    finalImage->clear();
    generateButton->setEnabled(true);
    ui->statusBar->showMessage("HDR file loaded");

    fclose(f);
}

void MainWindow::exportImage(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               ".",
                               tr("Images (*.png *.jpg)"));
    QFileInfo f(fileName);
    if(f.suffix() == "")
        fileName += ".jpg";
    std::cout << fileName.toStdString() << std::endl;
    imwrite(fileName.toStdString(),finalLDRimage->getImageMat());
}

void MainWindow::deleteImage(){
    for(int i = 0; i < imageList.size();i++){
        ImageFrame * frame = imageList.at(i);
        if(frame->isActive()){
            imageList.removeAt(i);
            delete frame;
            if(imageList.size() > 0){
                emit imageList.at(0)->clicked(imageList.at(0));
            }
            else{
                loadedBigImage->clear();
                saveSequenceAct->setEnabled(false);
                imageExifInfo->clear();
            }
        }
    }
}

void MainWindow::cameraPreview(){
    CameraController camera_controller;

    QMessageBox msgBox(this);

    msgBox.setIcon(QMessageBox::Critical);
    if ( ! camera_controller.loadConfiguration( "multiexposureCamera/configuration.txt" ) ){
        msgBox.setText("Can't load config file                        ");
        msgBox.exec();
        return;
    }
    if ( ! camera_controller.connectToCamera() ){
        msgBox.setText("Unable to connect to the camera           ");
        msgBox.exec();
        return;
    }

    if ( ! camera_controller.configureCamera() ){
        msgBox.setText("Unable to configure the camera           ");
        msgBox.exec();
        return;
    }
    camera_controller.show();

}

void MainWindow::saveSequence(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               ".",
                               tr("Images (*.png *.jpg)"));
    QFileInfo f(fileName);
    if(f.suffix() == "")
        fileName += ".jpg";

    for(int i=0; i< getLDRImageList().size();i++){
        QString newPath = f.absoluteDir().absolutePath() + QDir::separator() + f.baseName() + QString::number(i);;
        if (!f.completeSuffix().isEmpty())
            newPath += "." + f.completeSuffix();
        else
            newPath += ".jpg";
        std::cout << newPath.toStdString() <<std::endl;
        LDRImage * ldrImage = getLDRImageList().at(i);
        imwrite(newPath.toStdString(),ldrImage->getImageMat());
    }

}
