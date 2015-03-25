#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qwidget.h>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include "ldrimage.h"
#include "gui/imageframe.h"
#include <QList>
#include "debevecmalikwf.h"
#include "hdrcreator.h"
#include "gui/buildhdrthread.h"
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QSlider>
#include "tonemapping/linearoperator.h"
#include "tonemapping/logoperator.h"
#include "tonemapping/expoperator.h"
#include "tonemapping/reinhardglobaloperator.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QScrollArea * loadedImageScrollArea;
    QWidget * scrollAreaWidgetContents;
    QVBoxLayout * scrollImagesLayout;


public:
    ImageFrame * loadedBigImage;
    QLabel * imageExifInfo;
    explicit MainWindow(QWidget *parent = 0);
    void addImages(std::string filename);
    void resizeEvent(QResizeEvent* event);
    string int2String(int value);
    string double2String(double val);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QAction * openFileAct;
    QAction * cameraSettingsAct;
    QAction * loadCameraImagesAct;
    QAction * exitAct;
    QAction * saveHDRAct;
    QAction * exportAct;
    QMenu * fileMenu;
    QMenu * cameraMenu;
    QList<ImageFrame *> imageList;
    std::vector<LDRImage *>ldrImageList;
    QGridLayout * loadImageLayout;
    QLabel * imageListHeader;

    QGridLayout * generateHDRlayout;
    QScrollArea * tonemappingScrollArea;
    QWidget * tonemappingWidgetContents;
    QGridLayout * tonemappingLayout;
    ImageFrame * finalImage;
    void initMenu();
    void initLoadPage();
    void initGenerateHDRpage();
    void initStyleSheet();
    vector<LDRImage *> getLDRImageList();
    HDRCreator * hdr = new HDRCreator();
    DebevecMalikWF * wf = new DebevecMalikWF();
    HDRImage * hdrImage = NULL;
    BuildHDRThread * thread;

    QLabel  * statusBarText;
    QPushButton * generateButton;

    QButtonGroup * radioGroup;
    QRadioButton * linearMappingRadio;
    QRadioButton * logMappingRadio;
    QRadioButton * expMappingRadio;
    QRadioButton * reinhardMappingRadio;
    QSlider * logQslider;
    QSlider * logKslider;
    QLabel * logQVal;
    QLabel * logKVal;

    QSlider * expQslider;
    QSlider * expKslider;
    QLabel * expQVal;
    QLabel * expKVal;


private slots:
    void openFile();
    void openCameraSettingsDialog();
    void importCameraImages();
    void exit();
    void switchPage();
    void changeBigImage(ImageFrame *frame);
    void HDRdone();
    void toneMapping();
    void saveHDR();
};

#endif // MAINWINDOW_H
