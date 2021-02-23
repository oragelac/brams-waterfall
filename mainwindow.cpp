#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), initialized(false) {

    QDialog dialog(this);
    QFormLayout form(&dialog);
    host = new QLineEdit;
    port = new QLineEdit;
    dialog.setModal(true);

    form.addRow(QString("Host"), host);
    form.addRow(QString("Port"), port);

    port->setValidator(new QIntValidator(0, 65536, &dialog));
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    QObject::connect(&dialog, SIGNAL(rejected()), this, SLOT(close()));

    if(dialog.exec() == QDialog::Rejected) {

        host->setText("127.0.0.1");
        port->setText("4321");

    }

    this->statusBar()->showMessage(tr("Initializing"));


    QString string = "Connecting to ";
    string.append(host->text());
    string.append(":");
    string.append(port->text());

    this->statusBar()->showMessage(string, 3000);
    this->setWindowState(Qt::WindowMaximized);
    this->setMinimumSize(750, 300);

    tcpClient = new TcpClient(FFTSIZE, host->text(), port->text().toInt());

    fft = new FFT(FFTSIZE, SAMPLERATE);

    /* Spectrogram Layout */

    spectrogramLayout = new QHBoxLayout();

    scaleLabel = new QLabel();
    scaleLabel->setMaximumWidth(50);

    scaleLabelRight = new QLabel();
    scaleLabelRight->setMaximumWidth(50);

    spectrogramLabel = new QLabel();
    spectrogram = new Spectrogram(spectrogramLabel, FFTSIZE, SAMPLERATE);

    paletteLabel = new QLabel();
    paletteLabel->setMaximumWidth(40);

    spectrogramLayout->addWidget(scaleLabel);
    spectrogramLayout->addWidget(spectrogramLabel);
    spectrogramLayout->addWidget(scaleLabelRight);
    spectrogramLayout->addWidget(paletteLabel);

    /* Slider Layout */

    sliderLayout = new QGridLayout();

    brightnessSlider = new QSlider(Qt::Horizontal);
    brightnessSlider->setValue(0);
    brightnessSlider->setMinimum(-128);
    brightnessSlider->setMaximum(+128);

    sliderLayout->addWidget(new QLabel("Brightness"), 0, 0);
    sliderLayout->addWidget(brightnessSlider, 0, 1);

    contrastSlider = new QSlider(Qt::Horizontal);
    contrastSlider->setValue(0);
    contrastSlider->setMinimum(-128);
    contrastSlider->setMaximum(+128);

    sliderLayout->addWidget(new QLabel("Contrast"), 1, 0);
    sliderLayout->addWidget(contrastSlider, 1, 1);

    /* Frequency Layout */

    freqLayout = new QGridLayout();

    freqFrom = new QSpinBox();
    freqFrom->setMinimum(0);
    freqFrom->setMaximum(2755);
    freqFrom->setValue(FREQFROM);

    freqLayout->addWidget(new QLabel("From"), 0, 0);
    freqLayout->addWidget(freqFrom, 0, 1);
    freqLayout->addWidget(new QLabel("Hz"), 0, 2);

    freqTo = new QSpinBox();
    freqTo->setMinimum(1);
    freqTo->setMaximum(2756);
    freqTo->setValue(FREQTO);

    freqLayout->addWidget(new QLabel("To"), 1, 0);
    freqLayout->addWidget(freqTo, 1, 1);
    freqLayout->addWidget(new QLabel("Hz"), 1, 2);

    freqRangeButton = new QPushButton("Set Frequency Range");
    defaultFreqRangeButton = new QPushButton("Reset Frequency Range");

    freqLayout->addWidget(defaultFreqRangeButton);
    freqLayout->addWidget(freqRangeButton);

    /* Settings Layout */

    settingsLayout = new QHBoxLayout();
    settingsLayout->addLayout(sliderLayout);
    settingsLayout->addLayout(freqLayout);

    /* Main Layout */

    mainLayout = new QVBoxLayout();
    mainLayout->addLayout(spectrogramLayout);
    mainLayout->addLayout(settingsLayout);

    centralWidget = new QWidget();
    centralWidget->setLayout(mainLayout);
    this->setCentralWidget(centralWidget);
    this->layout()->setSizeConstraint(QLayout::SetNoConstraint);

    /* Connecting signals to slots */

    QObject::connect(tcpClient, SIGNAL(fftDataReady(double*)), fft, SLOT(compute(double*)));
    QObject::connect(fft, SIGNAL(done(double*)), spectrogram, SLOT(draw(double*)));
    QObject::connect(brightnessSlider, SIGNAL(valueChanged(int)), spectrogram, SLOT(adjustBrightness(int)));
    QObject::connect(brightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(notifyBrightnessChange(int)));
    QObject::connect(contrastSlider, SIGNAL(valueChanged(int)), spectrogram, SLOT(adjustContrast(int)));
    QObject::connect(contrastSlider, SIGNAL(valueChanged(int)), this, SLOT(notifyContrastChange(int)));
    QObject::connect(defaultFreqRangeButton, SIGNAL(clicked()), this, SLOT(resetFreqRange()));
    QObject::connect(freqRangeButton, SIGNAL(clicked()), this, SLOT(updateFreqRange()));
    QObject::connect(spectrogram, SIGNAL(scalingSpectrogram(QString, int)), this->statusBar(), SLOT(showMessage(QString, int)));
    QObject::connect(spectrogram, SIGNAL(scalingDone()), this->statusBar(), SLOT(clearMessage()));

    updateFreqRange();
    this->statusBar()->showMessage(tr("Ready"), 3000);
}

MainWindow::~MainWindow() {

    delete tcpClient;
    delete fft;
    delete spectrogram;
}

void MainWindow::resizeEvent(QResizeEvent*) {

    std::cout << "resize event" << std::endl;

    paletteLabel->setPixmap(QPixmap::fromImage(spectrogram->generatePalette(paletteLabel->width(), paletteLabel->height())));
    QPixmap pixmap = QPixmap::fromImage(spectrogram->generateFreqScale(scaleLabel->width(), scaleLabel->height()));
    scaleLabel->setPixmap(pixmap);
    scaleLabelRight->setPixmap(pixmap);

}

void MainWindow::updateFreqRange() {

    if(freqFrom->value() >= freqTo->value()) {
        
        resetFreqRange();
    }

    else {

        std::cout << "update freq range" << std::endl;

        spectrogram->setFreqRange(freqFrom->value(), freqTo->value());
        QPixmap pixmap = QPixmap::fromImage(spectrogram->generateFreqScale(scaleLabel->width(), scaleLabel->height()));
        scaleLabel->setPixmap(pixmap);
        scaleLabelRight->setPixmap(pixmap);
        this->statusBar()->showMessage(tr("Frequency range set"), 3000);
    }
}

void MainWindow::resetFreqRange() {

    std::cout << "reset freq range" << std::endl;

    freqFrom->setValue(FREQFROM);
    freqTo->setValue(FREQTO);
    spectrogram->setFreqRange(freqFrom->value(), freqTo->value());
    QPixmap pixmap = QPixmap::fromImage(spectrogram->generateFreqScale(scaleLabel->width(), scaleLabel->height()));
    scaleLabel->setPixmap(pixmap);
    scaleLabelRight->setPixmap(pixmap);
    this->statusBar()->showMessage(tr("Frequency range reset"), 3000);
}

void MainWindow::notifyBrightnessChange(int value) {

    this->statusBar()->showMessage(QString::number(value), 3000);
    paletteLabel->setPixmap(QPixmap::fromImage(spectrogram->generatePalette(paletteLabel->width(), paletteLabel->height())));

}
void MainWindow::notifyContrastChange(int value) {

    this->statusBar()->showMessage(QString::number(value), 3000);
    paletteLabel->setPixmap(QPixmap::fromImage(spectrogram->generatePalette(paletteLabel->width(), paletteLabel->height())));
}
