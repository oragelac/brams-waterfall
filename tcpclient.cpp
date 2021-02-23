#include "tcpclient.h"

TcpClient::TcpClient(unsigned int fftSize, QString host, unsigned short int port) : socket(this), fftSize(fftSize), fftSize90(round((double)fftSize / 10)) {
    
    idata = new int16_t[fftSize];
    data = new double[fftSize];

    socket.connectToHost(QHostAddress(host), port);
    QObject::connect(&socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    QObject::connect(&socket, SIGNAL(connected()), this, SLOT(connected()));
    QObject::connect(&socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    length = 0;
    timer.start();
}

TcpClient::~TcpClient() {

    delete idata;
    delete data;
}

void TcpClient::connected() {}
void TcpClient::disconnected() {}

void TcpClient::onReadyRead() {

    int previousLength = byteArray.length();
    byteArray.append(socket.readAll());
    length += byteArray.length() - previousLength;

    if(timer.hasExpired(5000)) {

        std::cout << ((double) length / timer.elapsed()) * 500 << " samples/second" << std::endl;
        length = 0;
        timer.restart();
    }
    
    if(byteArray.length() >= sizeof(int16_t) * fftSize) {

        getDataFromByteArray();
    }
}

void TcpClient::getDataFromByteArray() {

    memcpy(idata, byteArray.data(), sizeof(int16_t) * fftSize);
    
    for(unsigned int i = 0; i < fftSize; ++i) {

        data[i] = idata[i];
    }

    emit fftDataReady(data);
    byteArray.remove(0, sizeof(int16_t) * fftSize90); 
}