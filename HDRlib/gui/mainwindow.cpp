#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>

#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    openFileAct = new QAction(tr("&Open file"),this);
    connect(openFileAct,SIGNAL(triggered()),this,SLOT(openFile()));
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openFileAct);

    QGridLayout * loadImageLayout = new QGridLayout(ui->loadImagePage);


    loadedImageScrollArea = new QScrollArea(ui->loadImagePage);
    loadedImageScrollArea->setMaximumSize(QSize(250, 16777215));
    loadedImageScrollArea->setMinimumWidth(276);
    loadedImageScrollArea->setWidgetResizable(true);
    scrollAreaWidgetContents = new QWidget();
    scrollAreaWidgetContents->setGeometry(QRect(0, 0, 248, 0));

    scrollImagesLayout = new QVBoxLayout(scrollAreaWidgetContents);
    scrollImagesLayout->setContentsMargins(11, 11, 11, 11);
    scrollImagesLayout->setSpacing(6);

    loadedImageScrollArea->setWidget(scrollAreaWidgetContents);

    loadedBigImage = new QLabel(ui->loadImagePage);
    imageExifInfo = new QLabel(ui->loadImagePage);
    imageExifInfo->setStyleSheet("background-color:white;");
    QPushButton * nextButton = new QPushButton(ui->loadImagePage);
    nextButton->setText("Next");

    loadedBigImage->setText("File >> Open photo  -  to add photo");
    loadedBigImage->setAlignment(Qt::AlignCenter);
    //QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    //loadedBigImage->setSizePolicy(sizePolicy);

    loadImageLayout->addWidget(nextButton,1,1,1,1);
    loadImageLayout->addWidget(loadedImageScrollArea,0,1,1,1);
    loadImageLayout->addWidget(loadedBigImage,0,0,1,1);
    loadImageLayout->addWidget(imageExifInfo,1,0,1,1);

    connect(nextButton,SIGNAL(clicked()),this,SLOT(switchPage()));


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::openFile(){
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    //dialog.setNameFilter(trUtf8("Splits (*.000 *.001)"));
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    //QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.*)"));
    for(int i = 0; i < fileNames.length();i++){
    //ui->tab->addPhotos(fileNames[i].toUtf8().constData());
    }

}


void MainWindow::switchPage(){
    int currentIndex = ui->stackedWidget->currentIndex();
    std::cout << currentIndex << std::endl;
    if(currentIndex == 0){
        ui->stackedWidget->setCurrentIndex(1);
    }
    else{
        ui->stackedWidget->setCurrentIndex(0);
    }
}
