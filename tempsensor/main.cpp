/**
 * \mainpage Dokumentation
 *
 * \section Zielsetzung
 *
 * Der im letzten Semester als Beispiel zur Nutzung kleiner Elektronikbauteile etwa in SmartHome-Anwendungen gebaute
 * Arduino-Temperatursensor sollte mit serieller Kommunikation und einer graphischen Oberfläche erweitert werden. Er bietet nun die
 * erstellung eines Logfiles, graphische Ausgabe der Messergebnisse und des Temperatur-LED Zustandes, direkte Anpassung der Messfrequenz
 * im laufenden Betrieb sowie live-Synchronisation des Datums mit einem verbundenen Server, was über lange Messzeiträume deutlich
 * genauere Messprotokollierung ermögllicht. Weiterhin vorhandene Features sind die Ausgabe der Messwerte direkt am Arduino über eine LED
 * und in Textform auf einem LCD und Messung der Luftfeuchtigkeit.
 *
 * Zur Umsetzung genutzt wurden ein Arduino UNO board, ein LCD, ein DHT11 Temperatur- und Feuchtigkeitssensor, eine RGB-LED, ein Laptop und ein Verbindungskabel.
 *
 * \section a How-to: Aufbau und Inbetriebnahme
 *
 * \image html aufbau.jpg "Abbildung 1"
 *
 * Abbildung 1 zeigt ein Bild des Aufbaus. Folgendes muss verbunden werden:
 *  - DHT11-Sensor: drei Pins (vlnr)
 *      - Sensor -> Digitaler Arduino Port, standardmäßig 10
 *      - VCC -> Power Arduino Port (5V)
 *      - GND -> GND
 *  - LCD: Vier Pins
 *      - GND -> GND
 *      - VCC -> Power Arduino Port (5V)
 *      - SDA -> Analoger Arduino Port, standardmäßig A4
 *      - SCL -> Analoger Arduino Port, standardmäßig A5
 *  - RGB-LED: vier Pins (vlnr)
 *      - R -> Digitaler Arduino Port mit PWM, standardmäßig 6
 *      - G -> Digitaler Arduino Port mit PWM, standardmäßig 5
 *      - B -> Digitaler Arduino Port mit PWM, standardmäßig 3
 *      - GND -> GND
 *
 * Anschliessend muss das Arduino noch über die serielle Schnittstelle mit einem Rechner verbunden und der tempsensor-code auf diesem Rechner ausgeführt werden.
 *
 * Nun wird der Arduino in bestimmten Intervallen (voreingestellt eine Sekunde, über die GUI veränderbar) Messwerte ermitteln, und alle 5 Intervalle synchronisieren sich die Geräte.
 * Gleichzeitig zeigt das LCD die letzten ermittelten 5-Intervall-Durchschnittswerte und die LED interpretiert diese in Farbe und Intensität.
 *
 * \section Umsetzung
 *
 * Der auskommentierte Quellcode befindet sich im Ordner "Temperatursensor" (Arduino-code) bzw "tempsensor" (QT-code)
 * und auf <a href="https://github.com/Steffengra/tempsensor">Github</a>. Dort wird in größerem Detail auf die
 * Funktionsweise eingegangen.
 *
 * Die größte Hürde bei der Erweiterung des Projektes war die serielle Kommunikation. Glücklicherweise bietet QT eine Klasse QSerialPort, welche die serielle
 * Kommunikation deutlich vereinfacht. Ein Objekt dieser Klasse wird erstellt und mit den entsprechenden Informationen, die sie zum Verbinden
 * mit dem Arduino benötigt, versorgt. Dieses Objekt mit seinen Funktionen wirkt im Weiteren dann als Schnittstelle, über die unser Quellcode die
 * serielle Schnittstelle bearbeiten kann. Wir entschieden uns für ein Synchronisationsmodell, in dem beide verbundenen Geräte bis auf eine Ausnahme nur zu
 * bestimmten Zeitpunkten miteinander kommunizieren und durch bestimmte bekannte keywords ihre jeweilige Absicht ausdrücken. Sendet bspw. das Arduino "sync",
 * so ist dies das Signal für den Server, dass der Arduino bereit für die synchronisation der Uhrzeit ist. Das Arduino weiss wiederum, dass danach bestimmte
 * Daten in einer bestimmten Form mit Sicherheits-bits zu erwarten sind usw., es gibt also ein Kommunikations-Protokoll. Um das Überwachen der
 * seriellen Schnittstelle auf eingehende Kommunikation durch den Arduino kümmert sich ein Thread, welcher bei Empfang von Daten ein Signal aussendet, um das
 * sich der Hauptthread dann per Slot kümmern kann.
 *
 * Das Erstellen der graphischen Oberfläche war nicht weiter problematisch. Das UI ist größtenteils passiv. Ausnahme ist die Möglichkeit, das Messintervall live
 * zu ändern. Wird das Intervall vom User geändert, so geschieht dies direkt und wird vom Arduino vor der nächsten Synchronisation bearbeitet.
 *
 * \section b To-do: Probleme und mögliche Erweiterungen
 *
 * Im letzten Semester war das größte Problem der Anwendung die auf lange Zeit ungenaue interne Zeitberechnung des Arduino. Dieses Problem konnte durch die
 * Synchronisation mit dem Server gelöst werden.
 *
 * Insgesamt ist das UI etwas spartanisch geraten und könnte leicht verschönert und verbessert werden. Obwohl die serielle Kommunikation in unseren Tests
 * keine Probleme verursachte, so kann diese sicherlich deutlich robuster gestaltet werden. Auch die permanente Verbindung per Kabel ist nicht ideal für
 * eine realistische SmartHome-Anwendung.
 *
 * Insgesamt ist dieses Projekt natürlich beliebig erweiterbar und insbesondere verbesserbar.
 */


#include "dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    return a.exec();
}
