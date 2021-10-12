#include "mainwindow.h"
#include "ui_mainwindow.h"

/*QZXING Kütüphanesi*/


//Çözümlenecek olan resimin filtresi
const QString getFilter()
{
    static QString filter;
    filter += "Images (";
    foreach ( const QByteArray & format, QImageReader::supportedImageFormats() )
        filter += " *." + format;
    filter += " )";

    return filter;
}
const QString getIniPath()
{
    const static QString iniPath( qApp->applicationDirPath() + "/settings.ini" );
    return iniPath;
}
/*QZXING Kütüphanesi*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*Kamera*/
    this->baslangicGoruntusu();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerSlot()));
    timer->start(TIMER_DELAY);

    /*QZXING Kütüphanesi*/

    //connect( ui->actionOpen, SIGNAL( triggered() ), SLOT( ac() ) );
    //connect( ui->actionQuit, SIGNAL( triggered() ), SLOT( kapat() ) );

    zxing.setDecoder( QZXing::DecoderFormat_QR_CODE );

    QSettings ini( getIniPath(), QSettings::IniFormat );
    ui->splitter->restoreState( ini.value( ui->splitter->objectName() ).toByteArray() );
    restoreState( ini.value( "State" ).toByteArray() );
    restoreGeometry( ini.value( "Geometry" ).toByteArray() );
    /*QZXING Kütüphanesi*/

    /*GİZLENEN ARAÇLAR*/
    //ui->QRKodResim->hide();
    //ui->rsmCiktisi->hide();
    /*GİZLENEN ARAÇLAR*/
}

MainWindow::~MainWindow()
{
    /*Kamera*/
    if(timer->isActive())
        timer->stop();

    delete ui;
    delete finder;
    delete timer;
    /*Kamera*/
}

/*Kamera*/
void MainWindow::baslangicGoruntusu()
{
    this->setWindowTitle("QR Kod Okuyucu");
    ui->kaydetYoluEdit->setText(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0]);
    finder = new QCameraViewfinder();
    ui->kameraGoruntusu->addWidget(finder);

    this->timerSlot();

    connect(ui->kameraBtn, SIGNAL(clicked(bool)), this, SLOT(kameraBtnSlot()));
    connect(ui->rsmTemizleBtn, SIGNAL(clicked(bool)), this, SLOT(rsmTemizleBtnSlot()));
    connect(ui->dosyaYoluSecBtn, SIGNAL(clicked(bool)), this, SLOT(dosyaYoluSecBtnSlot()));
    connect(ui->cekmeBtn, SIGNAL(clicked(bool)), this, SLOT(cekmeBtnSlot()));
    connect(ui->rsmAcBtn, SIGNAL(clicked(bool)), this, SLOT(rsmAcBtnSlot()));
}

void MainWindow::kameraGuncelle()
{
    kameraBilgileri.clear();
    kameraIsimleri.clear();
    kameraIsimleri << "";
    foreach (const QCameraInfo &cameraInfo, QCameraInfo::availableCameras())
    {
        kameraBilgileri << cameraInfo;
        kameraIsimleri << cameraInfo.description();
    }
}

void MainWindow::kameraBtnSlot()
{
    if(ui->kameraCBox->isEnabled())
        if(ui->kameraCBox->currentIndex() == 0)
            QMessageBox::information(this, "Uyarı", "Lütfen bir kamera seçin");
        else
        {
            camera = new QCamera(kameraBilgileri[ui->kameraCBox->currentIndex()-1]);
            capture = new QCameraImageCapture(camera);

            capture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
            camera->setCaptureMode(QCamera::CaptureStillImage);

            connect(capture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(kameraCekmeBtnSlot(int,QImage)));
            connect(capture, SIGNAL(error(int,QCameraImageCapture::Error,QString)), this, SLOT(cekmeHataSlot(int,QCameraImageCapture::Error,QString)));
            connect(camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(kameraDurumSlot(QCamera::State)));
            connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(kameraHataSlot(QCamera::Error)));

            camera->setViewfinder(finder);
            camera->start();

            if(camera->state() == QCamera::ActiveState)
            {
                ui->kameraCBox->setEnabled(false);
                ui->kameraBtn->setText("Kapat");
                finder->show();
            }
            else
                QMessageBox::information(this, "Uyarı", "Kamera başlatılamıyor");
        }
    else
    {
        finder->hide();
        ui->kameraCBox->setEnabled(true);
        ui->kameraBtn->setText("Başlat");

        delete capture;
        delete camera;
    }
}

void MainWindow::rsmTemizleBtnSlot()
{
    ui->rsmCiktisi->clear();
}

void MainWindow::dosyaYoluSecBtnSlot()
{
    QString path, choosePath;

    path = ui->kaydetYoluEdit->text();

    if(QFileInfo::exists(path) || QFileInfo::exists(path + ".jpg"))//QFileInfo(path).exists() || QFileInfo(path + ".jpg").exists())
        choosePath = path;
    else
        choosePath = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0];

    choosePath = QFileDialog::getExistingDirectory(this, "Ekran görüntüsünü kaydetmek için yolu seçin", choosePath);

    if(choosePath.isEmpty())
        ui->kaydetYoluEdit->setText(path);
    else
        ui->kaydetYoluEdit->setText(choosePath);
}

void MainWindow::cekmeBtnSlot()
{
    if(ui->kameraCBox->isEnabled())
        QMessageBox::information(this, "Uyarı", "Lütfen kamerayı açın ve tekrar deneyin");
    else if(capture->isReadyForCapture())
        capture->capture(ui->kaydetYoluEdit->text());
    else
        QMessageBox::information(this, "Uyarı", "Ekran görüntüsü alınamadı, lütfen daha sonra tekrar deneyin");
}

void MainWindow::rsmAcBtnSlot()
{
    QString path;
    if(ui->kaydetYoluEdit->text().isEmpty())
        path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0];
    else
        path = ui->kaydetYoluEdit->text();

    if(QFileInfo::exists(path)) // QFileInfo(path).exists())
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    else if(QFileInfo::exists(path + ".jpg")) //QFileInfo(path + ".jpg").exists())
        QDesktopServices::openUrl(QUrl::fromLocalFile(path + ".jpg"));
    else
        QMessageBox::information(this, "Uyarı", "Mevcut yol mevcut değil, lütfen yolu kontrol edip tekrar deneyin");
}

//Fotoğrafı çeken fonksiyon
void MainWindow::kameraCekmeBtnSlot(int id, QImage image)
{
    Q_UNUSED(id)

    qDebug() << "Agaaa ahanda bu: " << image;

    //çözümleme kısmına aktarılan fotoğraf
    ui->rsmCiktisi->setPixmap(QPixmap::fromImage(image.scaled(ui->rsmCiktisi->geometry().size(), Qt::KeepAspectRatio)));

    ui->QRKodResim->setPixmap(QPixmap::fromImage(image.scaled(ui->rsmCiktisi->geometry().size(), Qt::KeepAspectRatio)));
    //çözümleme fonksiyonu
    coz();
}

void MainWindow::cekmeHataSlot(int id, QCameraImageCapture::Error error, QString errorString)
{
    if(id == 1 && error == QCameraImageCapture::ResourceError)
        QMessageBox::information(this, "Kayıt başarısız", "Ekran görüntüsü kaydedilemedi, lütfen yolu kontrol edin ve tekrar deneyin");
    else
        QMessageBox::information(this, "Yakalama hatası", errorString);
}

void MainWindow::kameraDurumSlot(QCamera::State state)
{
    switch (state)
    {
    case QCamera::ActiveState:
        break;
    case QCamera::UnloadedState:
        if(!ui->kameraCBox->isEnabled())
        {
            this->finder->hide();
            ui->kameraCBox->setEnabled(true);
            ui->kameraBtn->setText("Başlat");

            delete capture;
            delete camera;
        }
        break;
    case QCamera::LoadedState:
        break;
    }
}

void MainWindow::kameraHataSlot(QCamera::Error error)
{
    if(error != QCamera::NoError && !ui->kameraCBox->isEnabled())
    {
        this->finder->hide();
        ui->kameraCBox->setEnabled(true);
        ui->kameraBtn->setText("Başlat");
        delete capture;
        delete camera;
        QMessageBox::information(this, "Kamera Hatası", camera->errorString());
    }
}

void MainWindow::timerSlot()
{
    this->kameraGuncelle();

    while(ui->kameraCBox->count() > kameraIsimleri.size())
        ui->kameraCBox->removeItem(kameraIsimleri.size());
    while(ui->kameraCBox->count() < kameraIsimleri.size())
        ui->kameraCBox->addItem(NULL);
    for(int i = 0; i < kameraIsimleri.size(); i++)
        ui->kameraCBox->setItemText(i, kameraIsimleri[i]);
}

/*Kamera*/


/*QZXING Kütüphanesi*/


//Çözümlenecek fotoğrafı açan fonksiyon
void MainWindow::ac()
{
    QString path = QFileDialog::getOpenFileName( this, QString(), QString(), getFilter() );
    if ( path.isEmpty() )
        return;

    ui->QRKodResim->setPixmap( QPixmap( path ) );
    coz();
}

//Çözümleyen fonksiyon
void MainWindow::coz()
{
    QImage qrImage = ui->QRKodResim->pixmap()->toImage();
    const QString & decodeText = zxing.decodeImage( qrImage );

    ui->QRKodCikti->setPlainText( decodeText );
    ui->etiketBoyutuTxt->setText( QString::number( decodeText.size() ) );
}

void MainWindow::on_qrAcBtn_clicked()
{
    ac();
}
void MainWindow::on_qrCikBtn_clicked()
{
    close();
}
/*QZXING Kütüphanesi*/
