#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstdint>
#include <iostream>

#include <QTcpSocket>
#include <QHostAddress>
#include <QElapsedTimer>

#define FFTSIZE 16384


class TcpClient : public QObject {

    Q_OBJECT

public slots:

    void onReadyRead();
    void connected();
    void disconnected();
    

public:

    TcpClient(unsigned int, QString, unsigned short int);
    ~TcpClient();

    void getDataFromByteArray();


signals:

    void fftDataReady(double*);

private:

    int16_t* idata;
    double* data;
    QTcpSocket socket;
    QByteArray byteArray;
    unsigned int fftSize, fftSize90;
    QElapsedTimer timer;
    int length;
};

#endif // DATAGENERATOR_H
