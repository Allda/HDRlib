#ifndef CAMERASETTINGSDIALOG_H
#define CAMERASETTINGSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <QFrame>
#include <QPushButton>
#include <QLineEdit>

class CameraSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    CameraSettingsDialog(QWidget * parent =0);
    void loadConfigData();
    ~CameraSettingsDialog();
private:
    QGridLayout * layout;
    QFrame * horizontalLine;
    QFrame * horizontalLine2;
    QLabel * networkLabel;
    QLabel * localIPlabel;
    QLabel * cameraIPlabel;
    QLabel * portLabel;
    QLineEdit * localIPinput;
    QLineEdit * cameraIPinput;
    QLineEdit * portInput;

    QLabel * imageLabel;
    QLabel * apertureLabel;
    QLabel * gainLabel;
    QLineEdit * apertureInput;
    QLineEdit * gainInput;

    QLabel * exposureTimeLabel;
    QLabel * exposure1Label;
    QLabel * exposure2Label;
    QLabel * exposure3Label;
    QLineEdit * exposure1Input;
    QLineEdit * exposure2Input;
    QLineEdit * exposure3Input;

    QPushButton * submit;
    QPushButton * cancel;

private slots:
    void submitClickSlot();
    void cancelClickSlot();
};

#endif // CAMERASETTINGSDIALOG_H
