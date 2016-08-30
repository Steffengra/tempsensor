#include "dialog.h"
#include "ui_dialog.h"
#include <QSerialPort>
#include <qserialportinfo>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QSound>

QFile logfile("log.txt");       //Erstellt ein Logfile im Arbeitsverzeichnis


Dialog::Dialog(QWidget *parent) :
    //UI Setup
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setFixedSize(285, 160);
    ui->LEDPushButton->setEnabled(0);
    ui->lineEdit_6->setEnabled(0);
    ui->lineEdit_6->insert("n/A");
    ui->lineEdit_100->setEnabled(0);
    ui->lineEdit_100->insert("n/A");


    //QSerialport und listener Thread werden erstellt
    arduino = new QSerialPort(this);
    listener = new SerialListener(arduino);
    this->frequency = "";       //Variable Frequency wird auf leer gesetzt um Misinterpretation vorhandener Daten zu vermeiden

    connect(listener, SIGNAL(dataReceived(QString)), this, SLOT(read(QString)));        //Slot read wird nun durch Signal dataReceived aufgerufen
    connect(ui->frequencyPushButton, SIGNAL(clicked()), this, SLOT(setFreq()));         //Slot setFreq wird nun durch Signal clicked des frequencyPushButton aufgerufen (also Button wurde angeklickt)

    //Es wird versucht sich mit einem Arduino zu verbinden, wenn ein Arduino mit bekannter productID und vendorID über die serielle Schnittstelle verbunden ist
    bool connected = false;
    QString arduino_port;
    //Für Geräte mit anderer vendorID bzw productID kann der Code an dieser Stelle wie im Vorlesungsbeispiel verändert werden.
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){  //checks each port for the correct vendor and product id to figure out if arduino is connected and on which port
        if((serialPortInfo.productIdentifier() == arduino_product_id) && (serialPortInfo.vendorIdentifier() == arduino_vendor_id)){
            connected = true;
            arduino_port = serialPortInfo.portName();
        }
    }

    //Der erstellte QSerialport arduino wird zur Arduinokommunikation konfiguriert
    if (connected){
        arduino->setPortName(arduino_port);
        arduino->open(QSerialPort::ReadWrite);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        listener->start();  //listenerthread wird gestartet
    }
    else {
        qDebug() << "Error, couldn't find Arduino";     //Falls kein verbundener Arduino entsprechender vendorID und productID gefunden werden konnte wird eine Fehlermeldung ausgegeben
    }
}

/**
 * @brief Destruktor für Dialog
 *
 * Schliesst QSerialport und SerialListener thread falls offen, schliesst dann das UI.
 */
Dialog::~Dialog()
{
    if (arduino->isOpen()){
        arduino->close();           //close port if it was opened
        listener->terminate();
        delete listener;          //close listener
    }
    delete ui;
}

/**
 * @brief Koordiniert Kommunikation mit dem Arduino
 * @param ReceivedText
 *
 * read wird durch das Signal dataReceived des Threads aufgerufen. In diesem Fall hat der Arduino ein Signal über die Serielle Schnittstelle gesendet.
 * Dies kann verschiedene Ursachen haben. Funktion read interpretiert das angekommene Signal und handelt entsprechend.
 */
void Dialog::read(QString ReceivedText){
    QSound::play("://alert.wav");   //Alarm
    ui->frequencyPushButton->setEnabled(0); //frequencyPushbutton wird kurzzeitig deaktiviert, um Probleme im Programmablauf zu vermeiden
    qDebug() << ReceivedText;   //Der empfangene String wird in der Debug Konsole ausgegeben
    if (!strcmp(qPrintable(ReceivedText), "sync")){     //sync ist der Signalstring mit dem das Arduino eine Datensynchronisation anfordert
        QString syncstring = "-d" + QString::number(QDateTime::currentDateTime().date().day())      //Wir bauen uns einen String der das derzeitige Datum sowie Signalchars enthält..
                            + "o" + QString::number(QDateTime::currentDateTime().date().month())
                            + "y" + QString::number(QDateTime::currentDateTime().date().year())
                            + "h" + QString::number(QDateTime::currentDateTime().time().hour())
                            + "i" + QString::number(QDateTime::currentDateTime().time().minute());
        arduino->write(qPrintable(syncstring));     //..und schicken diesen String zurück ans Arduino
    }
    if (ReceivedText[0] == 'E'){        //E ist der Signalchar mit dem das Arduino eigene Daten übergibt: Farbe der LED, letzte Durchschnittstemperatur
        int color = (ReceivedText.at(1).toLatin1() - '0');      //Wir wandeln den char in eine Zahl um..
        switch (color) {        //..und passen den LEDPushButton entsprechend an.
            case 0:
                ui->LEDPushButton->setStyleSheet("background-color: white");
                break;
            case 1:
                ui->LEDPushButton->setStyleSheet("background-color: red");
                break;
            case 2:
                ui->LEDPushButton->setStyleSheet("background-color: blue");
                break;
            case 3:
                ui->LEDPushButton->setStyleSheet("background-color: green");
                break;
        }

        //Wir öffnen das Logfile und hängen die letzte empfangene Durchschnittstemperatur an
        logfile.open(QIODevice::Append);
        QTextStream logstream(&logfile);
        logstream << ((ReceivedText[3].toLatin1() - '0')*10 + (ReceivedText[4].toLatin1() -'0')) << "\r\n";
        logfile.close();
        logfile.open(QIODevice::ReadOnly);
        QTextStream logreadstream(&logfile);
        int q = 0;
        QString line[1000];
        while (!logreadstream.atEnd()){ //wir lesen alle Zeilen des logfiles ein
            line[q] = logreadstream.readLine();
            q++;
        }
        logfile.close();
        long sum = 0;
        int r;
        if (q > 100) {  //Wenn wir mehr als 100 Durchschnittstemperaturen aus dem logfile eingelesen haben errechnen wir den Schnitt der letzten 100 und geben diesen im UI lineEdit_100 aus
            for (r = 0; r < 100; r++){
                sum += line[q-r-1].toInt();
            }
            ui->lineEdit_100->clear();
            ui->lineEdit_100->insert(QString::number(sum/100));
        }
        if (q > 7) {    //Wenn wir mehr als 7 Durchschnittstemperaturen aus dem logfile eingelesen haben errechnen wir den Schnitt der letzten 100 und geben diesen im UI lineEdit_6 aus
            sum = 0;
            for (r = 0; r < 6; r++) {
                sum += line[q-r-1].toInt();
            }
            ui->lineEdit_6->clear();
            ui->lineEdit_6->insert(QString::number(sum/6));
        }

    }
    ui->frequencyPushButton->setEnabled(1); //Der frequencyPushButton wird wieder freigegeben.
}

/**
 * @brief Funktion zum einlesen von Userinput des lineEdits, wird direkt an das Arduino gesendet
 *
 * Die Funktion setFreq wird durch einen Knopfdruck des Users aufgerufen. Sie liest den Inhalt des frequencyLineEdits und, falls dieses nicht leer ist, schickt den Inhalt direkt
 * zur weiteren Bearbeitung an das Arduino.
 */
void Dialog::setFreq(){
    if (!ui->frequencyLineEdit->text().isEmpty()){
        arduino->write(qPrintable(ui->frequencyLineEdit->text()));
    }
}
