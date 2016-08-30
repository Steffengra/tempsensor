/**
* @file dialog.h
*
* @brief Enthält die Dialog Klasse
*
* dialog.h enthält die Dialog Klasse, die die Hauptklasse des Projektes ist und über die GUI und serielle Kommunikation mit dem Arduino im wesentlichen laufen.
*/

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>

#include "seriallistener.h"

namespace Ui {
class Dialog;
}

/**
* @class Dialog
*
* @brief Behandelt GUI und serielle Kommunikation
*
* Die Hauptklasse des Projekts. Über sie laufen das GUI und die serielle Kommunikation mit dem Arduino via slot read. Desweiteren erstellt sie einen SerialListener Thread und enthält Infos zum Verbinden mit dem Arduino.
*/
class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void read(QString ReceivedText);
    void setFreq();

private:
    Ui::Dialog *ui;
    QSerialPort *arduino;
    SerialListener *listener;
    static const quint16 arduino_vendor_id = 9025;      //Spezifisch für den benutzten ARDUINO UNO, für andere Produkte kann die markierte Stelle im Code wie in der Vorlesung gezeigt verallgemeinert werden.
    static const quint16 arduino_product_id = 67;       //^
    const char *frequency;

};

#endif // DIALOG_H
