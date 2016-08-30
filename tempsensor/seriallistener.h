/**
* @file seriallistener.h
*
* @brief Enthält die SerialListener Klasse
*
* seriallistener.h enthält die SerialListener Klasse. Sie fungiert als Thread, welcher die eingehende Kommunikation an der seriellen Schnittstelle durch den Arduino überwacht.
*/

#ifndef SERIALLISTENER_H
#define SERIALLISTENER_H

#include <QThread>
#include <QSerialPort>
#include <QString>


/**
* @class SerialListener
*
* @brief Thread. Liest Serielle Schnittstelle
*
* Der Seriallistener ist ein Thread der die serielle Schnittstelle überwacht und bei vorhandener Kommunikation durch den Arduino ein Signal aussendet.
*/
class SerialListener : public QThread{

    Q_OBJECT

public:
    SerialListener(QSerialPort *port);
    ~SerialListener();

protected:
    virtual void run();

private:
    void decodeSerialData();
    QSerialPort *port;

signals:
    void dataReceived(QString);
};

#endif // SERIALLISTENER_H
