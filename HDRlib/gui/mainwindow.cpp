#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QString>

#include <iostream>
#include "gui/imageframe.h"
#include "gui/camerasettingsdialog.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initMenu();

    initLoadPage();


}

MainWindow::~MainWindow()
{
    delete ui;
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

    loadedBigImage->setText("File >> Open photo  -  to add photo");
    loadedBigImage->setAlignment(Qt::AlignCenter);

    loadImageLayout->addWidget(nextButton,2,1,1,1);
    loadImageLayout->addWidget(loadedImageScrollArea,1,1);
    loadImageLayout->addWidget(loadedBigImage,0,0,2,1);
    loadImageLayout->addWidget(imageExifInfo,2,0);
    loadImageLayout->addWidget(imageListHeader,0,1);

    connect(nextButton,SIGNAL(clicked()),this,SLOT(switchPage()));
}

void MainWindow::addImages(std::string filename){
    LDRImage * ldrImage = new LDRImage(filename.c_str());
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
        else
            ui->stackedWidget->setCurrentIndex(1);
    }
    else{
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void MainWindow::changeBigImage(ImageFrame * frame){
    for(int i = 0; i < imageList.size(); i++){
        imageList.at(i)->clearBorder();
    }
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
       loadedBigImage->setPixmap(QPixmap::fromImage(ldrImage->getQImage(width-30,height-30)));
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

}

void MainWindow::exit(){
    this->close();
}
