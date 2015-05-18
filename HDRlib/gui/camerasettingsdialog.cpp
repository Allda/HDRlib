#include "camerasettingsdialog.h"
#include <QFrame>
#include <QFile>
#include <iostream>
#include <QTextStream>

CameraSettingsDialog::CameraSettingsDialog(QWidget *parent)
    :QDialog(parent)
{
    layout = new QGridLayout(this);
    //this->setLayout(layout);
    horizontalLine = new QFrame();
    horizontalLine->setFrameStyle(QFrame::HLine);
    horizontalLine->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QFont f( "Arial", 14, QFont::Bold);

    // labels and input boxes
    networkLabel = new QLabel(this);
    networkLabel->setText("Network setting");
    networkLabel->setFont(f);
    localIPlabel = new QLabel(this);
    localIPlabel->setText("Local IP");
    cameraIPlabel = new QLabel(this);
    cameraIPlabel->setText("Camera IP");
    portLabel = new QLabel(this);
    portLabel->setText("Port");
    localIPinput = new QLineEdit(this);
    cameraIPinput = new QLineEdit(this);
    portInput = new QLineEdit(this);

    imageLabel = new QLabel("Image setting", this);
    imageLabel->setFont(f);
    apertureLabel = new QLabel("Aperture", this);
    gainLabel = new QLabel("Gain", this);
    apertureInput = new QLineEdit(this);
    gainInput = new QLineEdit(this);

    horizontalLine2 = new QFrame();
    horizontalLine2->setFrameStyle(QFrame::HLine);
    horizontalLine2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    exposureTimeLabel = new QLabel("Exposure sequence", this);
    exposureTimeLabel->setFont(f);
    exposure1Label = new QLabel("Image 1", this);
    exposure2Label = new QLabel("Image 2", this);
    exposure3Label = new QLabel("Image 3", this);
    exposure1Input = new QLineEdit(this);
    exposure2Input = new QLineEdit(this);
    exposure3Input = new QLineEdit(this);

    submit = new QPushButton("Submit", this);
    cancel = new QPushButton("Cancel", this);

    layout->addWidget(networkLabel,0,0);
    layout->addWidget(localIPlabel,1,0);
    layout->addWidget(localIPinput,1,1);
    layout->addWidget(cameraIPlabel,2,0);
    layout->addWidget(cameraIPinput,2,1);
    layout->addWidget(portLabel,1,2);
    layout->addWidget(portInput,1,3);

    layout->addWidget(horizontalLine,3,0,1,4);

    layout->addWidget(imageLabel,4,0);
    layout->addWidget(apertureLabel,5,0);
    layout->addWidget(gainLabel,6,0);

    layout->addWidget(horizontalLine2,7,0,1,4);

    layout->addWidget(exposureTimeLabel,8,0,1,2);
    layout->addWidget(exposure1Label,9,0);
    layout->addWidget(exposure2Label,9,2);
    layout->addWidget(exposure3Label,10,0);

    layout->addWidget(submit,11,3);
    layout->addWidget(cancel,11,2);





    layout->addWidget(apertureInput,5,1);
    layout->addWidget(gainInput,6,1);

    layout->addWidget(exposure1Input,9,1);
    layout->addWidget(exposure2Input,9,3);
    layout->addWidget(exposure3Input,10,1);

    connect(submit,SIGNAL(clicked()),this,SLOT(submitClickSlot()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(cancelClickSlot()));
    loadConfigData();
}

CameraSettingsDialog::~CameraSettingsDialog()
{

}

void CameraSettingsDialog::loadConfigData(){
    QFile inputFile("multiexposureCamera/configuration.txt");
    QString wholeFileText;
    if (inputFile.open(QIODevice::ReadWrite))
    {
       QTextStream in(&inputFile);
       // each line starts with label
       while (!in.atEnd())
       {
          QString line = in.readLine();
          if(line.startsWith("LOCAL_IP")){
              localIPinput->setText(line.split("=")[1].trimmed());
          }
          else if(line.startsWith("CAMERA_IP")){
              cameraIPinput->setText(line.split("=")[1].trimmed());
          }
          else if(line.startsWith("PORT")){
              portInput->setText(line.split("=")[1].trimmed());
          }
          else if(line.startsWith("CAMERA_APERTURE")){
              apertureInput->setText(line.split("=")[1].trimmed());
          }
          else if(line.startsWith("CAMERA_GAIN")){
              gainInput->setText(line.split("=")[1].trimmed());
          }
          else if(line.startsWith("CAMERA_MULTI_EXPOSURE_0")){
              exposure1Input->setText(line.split("=")[1].trimmed());
          }
          else if(line.startsWith("CAMERA_MULTI_EXPOSURE_1")){
              exposure2Input->setText(line.split("=")[1].trimmed());
          }
          else if(line.startsWith("CAMERA_MULTI_EXPOSURE_2")){
              exposure3Input->setText(line.split("=")[1].trimmed());
          }

       }
       inputFile.close();
    }
}

// submit button slot - save values to configuration file
void CameraSettingsDialog::submitClickSlot(){
    QFile inputFile("multiexposureCamera/configuration.txt");
    QString wholeFileText;
    if (inputFile.open(QIODevice::ReadWrite))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
           // if line is known -> rewrite value
          QString line = in.readLine();
          if(line.startsWith("LOCAL_IP")){
              line = "LOCAL_IP = " +localIPinput->text();
          }
          else if(line.startsWith("CAMERA_IP")){
              line = "CAMERA_IP = " +cameraIPinput->text();
          }
          else if(line.startsWith("PORT")){
              line = "PORT = " +portInput->text();
          }
          else if(line.startsWith("CAMERA_APERTURE")){
              line = "CAMERA_APERTURE = " +apertureInput->text();
          }
          else if(line.startsWith("CAMERA_GAIN")){
              line = "CAMERA_GAIN = " +gainInput->text();
          }
          else if(line.startsWith("CAMERA_MULTI_EXPOSURE_0")){
              line = "CAMERA_MULTI_EXPOSURE_0 = " +exposure1Input->text();
          }
          else if(line.startsWith("CAMERA_MULTI_EXPOSURE_1")){
              line = "CAMERA_MULTI_EXPOSURE_1 = " +exposure2Input->text();
          }
          else if(line.startsWith("CAMERA_MULTI_EXPOSURE_2")){
              line = "CAMERA_MULTI_EXPOSURE_2 = " +exposure3Input->text();
          }
          wholeFileText += line + "\n";
       }
       // write whole content
       inputFile.seek(0); // go to the beginning of the file
       inputFile.write(wholeFileText.toUtf8()); // write the new text back to the file*/
       inputFile.close();
    }
    this->hide();
    delete this;
}


void CameraSettingsDialog::cancelClickSlot(){
    this->hide();
    delete this;
}

