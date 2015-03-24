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
    QMenu * fileMenu;
    QMenu * cameraMenu;
    QList<ImageFrame *> imageList;
    QGridLayout * loadImageLayout;
    QLabel * imageListHeader;
    void initMenu();
    void initLoadPage();

private slots:
    void openFile();
    void openCameraSettingsDialog();
    void importCameraImages();
    void exit();
    void switchPage();
    void changeBigImage(ImageFrame *frame);
};

#endif // MAINWINDOW_H
