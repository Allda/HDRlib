#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qwidget.h>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>

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
    QLabel * loadedBigImage;
    QLabel * imageExifInfo;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QAction * openFileAct;
    QMenu * fileMenu;

private slots:
    void openFile();
    void switchPage();
};

#endif // MAINWINDOW_H
