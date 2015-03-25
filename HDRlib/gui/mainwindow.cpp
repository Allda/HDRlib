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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initMenu();

    initLoadPage();
    initGenerateHDRpage();
    initStyleSheet();
    ui->statusBar->showMessage("Nejaky text",10000);
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
    openFileAct = new QAction(tr("&Open file"),this);
    connect(openFileAct,SIGNAL(triggered()),this,SLOT(openFile()));
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openFileAct);
    cameraSettingsAct = new QAction(tr("&Camera settings"),this);
    cameraMenu = menuBar()->addMenu(tr("&Camera"));
    cameraMenu->addAction(cameraSettingsAct);
    connect(cameraSettingsAct,SIGNAL(triggered()),this,SLOT(openCameraSettingsDialog()));
    loadCameraImagesAct = new QAction(tr("&Import camera images"),this);
    fileMenu->addAction(loadCameraImagesAct);
    connect(loadCameraImagesAct,SIGNAL(triggered()), this, SLOT(importCameraImages()));
    exitAct = new QAction(tr("&Exit"), this);
    fileMenu->addAction(exitAct);
    connect(exitAct,SIGNAL(triggered()), this, SLOT(exit()));
    saveHDRAct = new QAction(tr("&Save as HDR"),this);
    fileMenu->addAction(saveHDRAct);
    connect(saveHDRAct,SIGNAL(triggered()),this,SLOT(saveHDR()));
    exportAct = new QAction(tr("&Export image"),this);
    fileMenu->addAction(exportAct);
    connect(exportAct, SIGNAL(triggered()),this,SLOT(exportImage()));
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
    reinhardMappingRadio = new QRadioButton("Reinhard mapping", tonemappingWidgetContents);
    linearMappingRadio->setChecked(true);
    radioGroup->addButton(linearMappingRadio);
    radioGroup->addButton(logMappingRadio);
    radioGroup->addButton(expMappingRadio);
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
    tonemappingLayout->addWidget(reinhardMappingRadio,7,0,1,3);
}

void MainWindow::addImages(std::string filename){
    LDRImage * ldrImage = new LDRImage(filename.c_str());
    addImages(ldrImage);

}

void MainWindow::addImages(LDRImage *ldrImage){
    ImageFrame * frame = new ImageFrame(ldrImage);
    connect(frame,SIGNAL(clicked(ImageFrame *)),this,SLOT(changeBigImage(ImageFrame *)));
    // first image = set border, set as bigImage
    if(imageList.size() == 0)
        emit frame->clicked(frame);
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
    /*QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Input filter", "Use bayer filter?",
                                QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {

    }
    else {

    }*/
    int numTasks = 100;
    QProgressDialog *  progress = new QProgressDialog("Task in progress...", "Cancel", 0, numTasks);
    /*progress->setWindowModality(Qt::WindowModal);
    progress->show();
    //progress->setMinimumDuration(5000);
    progress->setAutoClose(false);
    //progress->setAutoClose(false);
    for (int i = 0; i < numTasks; i+=25) {
        progress->setValue(i);
        if (progress->wasCanceled()){
            break;
        }
        for(int j=0;j < 100000; j++)
            std::cout << j << std::endl;
        progress->setValue(numTasks);
        if(i % 10 == 0) QApplication::processEvents();
    }
    progress->close();*/
    CameraController camera_controller;
    vector<LDRImage *> ldrImageList;

    QMessageBox msgBox(this);

    msgBox.setIcon(QMessageBox::Critical);
    if ( ! camera_controller.loadConfiguration( "multiexposureCamera/configuration.txt" ) ){
        msgBox.setText("Can't load config file                        ");
        //msgBox.setInformativeText("Must contain at least 3 images");
        msgBox.exec();
        return;
    }



    if ( ! camera_controller.connectToCamera() ){
        msgBox.setText("Unable to connect to the camera           ");
        //msgBox.setInformativeText("Must contain at least 3 images");
        msgBox.exec();
        return;
    }

    if ( ! camera_controller.configureCamera() )
        msgBox.setText("Unable to configure the camera           ");
        //msgBox.setInformativeText("Must contain at least 3 images");
        msgBox.exec();
        return;

    try{
        ldrImageList = camera_controller.getImages();
    }catch(exception e){
            msgBox.setText("Unable to get images           ");
            //msgBox.setInformativeText("Must contain at least 3 images");
            msgBox.exec();
            return;
    }

    for(unsigned i = 0; i < ldrImageList.size();i++){
        LDRImage * ldrImage = ldrImageList.at(i);

        ldrImage->showImage();
    }


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
                               tr("Images (*.png *.xpm *.jpg)"));
    std::cout << fileName.toStdString() << std::endl;
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
            else
                loadedBigImage->clear();
        }
    }
}
