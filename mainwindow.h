#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QList>
#include <QCameraViewfinder>
#include <QCameraInfo>
#include <QCamera>
#include <QCameraImageCapture>
#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QFileInfo>
#include <QImageReader>
#include <QMimeData>
#include <QUrl>
#include <QMouseEvent>
#include <QSettings>
#include <QScrollBar>
#include <QComboBox>
#include <QDebug>
#include <QPalette>

#include "QZXing.h"

/*Kamera*/
#define TIMER_DELAY 1000
/*Kamera*/

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    /*QZXING Kütüphanesi*/
    void ac();
    void coz();
    //void kapat();
    /*QZXING Kütüphanesi*/

private slots:

    /*Kamera*/
    void kameraBtnSlot();
    void rsmTemizleBtnSlot();
    void dosyaYoluSecBtnSlot();
    void cekmeBtnSlot();
    void rsmAcBtnSlot();

    void kameraCekmeBtnSlot(int id, QImage image);
    void cekmeHataSlot(int id, QCameraImageCapture::Error error, QString errorString);
    void kameraDurumSlot(QCamera::State state);
    void kameraHataSlot(QCamera::Error error);

    void timerSlot();
    /*Kamera*/

    /*QZXING Kütüphanesi*/
    void on_qrAcBtn_clicked();
    void on_qrCikBtn_clicked();
    /*QZXING Kütüphanesi*/

private:
    Ui::MainWindow *ui;

    /*Kamera*/
    QStringList kameraIsimleri;
    QList<QCameraInfo> kameraBilgileri;

    QCamera *camera;
    QCameraImageCapture *capture;
    QCameraViewfinder *finder;

    QTimer *timer;


    void baslangicGoruntusu();
    void kameraGuncelle();
    /*Kamera*/


    /*QZXING Kütüphanesi*/
    QZXing zxing;
    //QPoint lastPos;
    /*QZXING Kütüphanesi*/


};
#endif // MAINWINDOW_H
