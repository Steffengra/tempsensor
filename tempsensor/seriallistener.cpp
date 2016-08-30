#include "seriallistener.h"
#include <QDebug>

/**
 * @brief Konstruktor
 * @param port
 */
SerialListener::SerialListener(QSerialPort *port){
    this->port = port;
}

/**
 * @brief Destruktor
 */
SerialListener::~SerialListener(){

}

/**
 * @brief Startet die listener-Schleife
 */
void SerialListener::run() {
    if(port == nullptr || !port->isOpen()) {    //Kein oder falscher QSerialPort übergeben -> Ende
        return;
    }

    port->setDataTerminalReady(true);

    while(isRunning() && port->isOpen()) {  //Jede Sekunde wird decodeSerialData aufgerufen
        decodeSerialData();
        msleep(1000);
    }
}

/**
 * @brief Liest die Daten aus dem QSerialPort in ein Array, und trimmed dieses Array dann in einen QString. Im Erfolgsfall (Daten ausgelesen) wird das Signal dataReceived
 * ausgesendet.
 */
void SerialListener::decodeSerialData() {
    if(!port->isDataTerminalReady()) {
        return;
    }

    QByteArray bytes = port->readAll();
    QString text = QString::fromUtf8(bytes).trimmed();
    if(text.isEmpty()) {
        return;
    }
    //
    emit dataReceived(text);
}
