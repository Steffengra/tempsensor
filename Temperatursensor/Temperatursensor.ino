/* Grundlagen der Informatik I, WS 2015/16
  Projektaufgabe Temperatursensor
  Autor: Steffen Gracla, Alec Prinz, Merlin Eggers
  Matrikelnummer: 4248668, 4269955, 4221777
  Datum: 12.02.2016
  Allgemeine Beschreibung der Loesung und eventuelle Referenzen
  (mind. 2-3 Saetze): siehe Dokumentation
  - Arduino
  - Breadboard
  - Kabel
  - RGB LED (r -> pin 6, g -> pin 5, b -> pin 3) wichtig: Pin mit PWM benutzen
  - DHT11 Temperature and humidity module (s -> pin10)
  - liquid crystal display-display (SDA -> pin A4, pin A5)
*/
byte current_date[3] = {0, 0, 0};   //0 = Tag, 1 = Monat, 2 = Jahr, Startdatum
int current_minute   = 0;          //60*h + min, Startzeit
int current_hour     = 0;
byte pin[4] = {10, 6, 5, 3};          //0 = temp, 1 = red, 2 = green, 3 = blue, Pinbelegung für Digitalpins

#include "struct.h"                   //Dort befindet sich die für den Ringspeicher genutzte Struct
#include <LiquidCrystal_I2C.h>        //Library fürs LED

struct speichereinheit ring[100];     //Ringspeicherdatenbank
float inTemp[2][5];                   //0 = humid, 1 = temp, fünfminütiger Messspeicher
byte in_sensor[5];                    //0 = humid, 2 = temp, 4 = control byte, 1 and 3 are always zero
byte z = 99;                          //Zählvariable für Ringspeicher
long sleep = 1000;
byte LEDstate = 0;                    //0 = weiss, 1 = rot, 2 = blau, 3 = grün
String syncstring;

LiquidCrystal_I2C lcd(0x27, 16, 2);   //für das LCD relevante Informationen (16 chars, 2 lines)


void setup() {
  Serial.begin(9600);           //Prepare serial communication
  pinMode(pin[1], OUTPUT);      //Prepare pin red
  pinMode(pin[2], OUTPUT);      //Prepare pin green
  pinMode(pin[3], OUTPUT);      //Prepare pin blue
  lcd.begin();                  //Prepare LCD
  set_LED(0, 0);                //Signals board is working
  LEDstate = 0;
}

void loop() {                           //Wiederholt sich unendlich oft
  z = (z + 1) % 100;                    //Hält z in einem Bereich von 0 bis 100
  for (byte i = 0; i < 5; i++) {        //Fünf Messungen werden gemacht
    read_Heatsensor();                  //read_Heatsensor wird aufgerufen
    inTemp[0][i] = in_sensor[0];        //in in_sensor schreibt read_Heatsensor seine empfangenen Werte, diese nehmen wir uns nun für unseren 5 Minuten Speicher
    inTemp[1][i] = in_sensor[2];        //Wie eine Zeile vorher, hier wird die gemessene Temperatur kopiert
    delay(sleep);                       //60000 deviates 31.51 miliseconds per minute, eine Minute Pause pro Messung.
  }
  set_ring(z);                          //set_ring(z) errechnet den fünfminütigen Durchschnitt und aktualisiert dann alle Bestandteile der Ringspeichereinheit an der Stelle des aktuellen Schleifendurchlaufes mit den aktuellen Werten
  
  //Dieser if/else Komplex steuert das Verhalten der RGB-LED abhängig von der errechneten Durchschnittstemperatur
  if (ring[z].temp > 25) {                //Zu hoch:
    LEDstate = 1;
    set_LED(1, ring[z].temp - 26);
  }      
  else if (ring[z].temp < 15) {           //Zu kalt:
    LEDstate = 2;
    set_LED(3, ring[z].temp + 14);      //Blau leuchten mit einer Intensitätsstufe von +1 pro einem Grad unter der Schwelltemperatur
  }
  else {                              //Temperature OK
    LEDstate = 3;
    set_LED(2, 0);                   
  }
  
  if (Serial.available() > 0){
    sleep = Serial.parseInt();
  }
  Serial.print("sync");
  syncstring = Serial.readString();
  if (syncstring[0] == '-'){
    Sync();
  }
  set_ring(z);
  set_LCD(z);                           //set_LCD aktualisiert das LCD mit den Werten aus der aktuellen Ringspeichereinheit
}

void Sync() {         //synchronises time, date, LEDstate, last temperature with computer
  int d, o, y, h, i;
  d = Find('d');
  o = Find('o');
  y = Find('y');
  h = Find('h');
  i = Find('i');
  if (syncstring.length() - i == 3){
    current_minute = (syncstring[i+1] - '0')*10 + (syncstring[i+2] - '0');
  }
  else {
    current_minute = (syncstring[i+1] - '0');
  }
  if (i - h == 3) {
    current_hour = (syncstring[h+1] - '0')*10 + (syncstring[h+2] - '0');
  }
  else {
    current_hour = (syncstring[h+1] - '0');
  }
  current_date[2] = (syncstring[y+3] - '0')*10 + (syncstring[y+4] - '0');
  if (y - o == 3) {
    current_date[1] = (syncstring[o+1] - '0')*10 + (syncstring[o+2] - '0');
  }
  else {
    current_date[1] = (syncstring[o+1] - '0');
  }
  if (o - d == 3) {
    current_date[0] = (syncstring[d+1] - '0')*10 + (syncstring[d+2] - '0');
  }
  else {
    current_date[0] = (syncstring[d+1] - '0');
  }
  syncstring = 'E' + String(LEDstate) + 't' + String(ring[z].temp) + 'h' + String(ring[z].humid);
  Serial.print(syncstring);
}

int Find(char c){           //finds the iterator position of a certain character in syncstring
  int i;
  for (i = 0; i < syncstring.length(); i++){
    if (syncstring[i] == c){
      return i;
    }
  }
  return -1;
}

void set_ring(byte z) {                                                                               //sets the current ring to the correct values
  ring[z].humid = (inTemp[0][0] + inTemp[0][1] + inTemp[0][2] + inTemp[0][3] + inTemp[0][4]) / 5;     //Ringspeichereinheit z humid wird mit dem Durchschnitt der 5 in inTemp[0] gespeicherten Werte überschrieben
  ring[z].temp = (inTemp[1][0] + inTemp[1][1] + inTemp[1][2] + inTemp[1][3] + inTemp[1][4]) / 5;      //Ringspeichereinheit z temp wird mit dem Durchschnitt der 5 in inTemp[1] gespeicherten Werte überschrieben
  ring[z].hour = current_hour;
  ring[z].minute = current_minute;                                                                    //Ringspeichereinheit z minute wird mit der aktuellen Zeit überschrieben
  ring[z].day = current_date[0];                                                                      //Ringspeichereinheit z day wird mit dem aktuellen Tag überschrieben
  ring[z].month = current_date[1];                                                                    //Ringspeichereinheit z month wird mit dem aktuellen Monat überschrieben
  ring[z].year = current_date[2];                                                                     //Ringspeichereinheit z year wird mit dem aktuellen Jahr überschrieben
}

void set_LED(byte color, byte intensity) { //color 0 = white, 1 = red, 2 = green, 3 = blue, 255 = off, intensity 0-3, set_LED aktualisiert die LED mit einer übergebenen Farbe und Intensität
  analogWrite(pin[1], 0);                  //Rote LED wird ausgeschaltet
  analogWrite(pin[2], 0);                  //Grüne LED wird ausgeschaltet
  analogWrite(pin[3], 0);                  //Blaue LED wird ausgeschaltet
  if (intensity > 3)                       //Wir wollen die Intensität in 4 Stufen regelbar machen. Dafür begrenzen wir Intensitätswerte bei 3
    intensity = 3;
  intensity = intensity * 50 + 100;        //Die Intensität kann von 0 bis 255 gesetzt werden. Wir entschieden uns für einen Festbetrag von 100 bei Intensität der Stufe 0, und +50 Intensität pro Stufe.
  switch (color) {                         //Welche Farbe war gewünscht?
    case 0:                                //Weiss: Alle LEDs leuchten gleichstark
      analogWrite(pin[1], intensity);
      analogWrite(pin[2], intensity);
      analogWrite(pin[3], intensity);
      return;                              //In diesem Fall sind wir fertig mit dem Auftrag
    case 1:                                //Rot: Nur die rote LED leuchtet mit der gewünschten Intensität
      analogWrite(pin[1], intensity);
      return;                              //In diesem Fall sind wir fertig mit dem Auftrag
    case 2:                                //Grün: Nur die grüne LED leuchtet mit der gewünschten Intensität
      analogWrite(pin[2], intensity);      
      return;                              //In diesem Fall sind wir fertig mit dem Auftrag
    case 3:                                //Blau: Nur die blaue LED leuchtet mit der gewünschten Intensität
      analogWrite(pin[3], intensity);
      return;                              //In diesem Fall sind wir fertig mit dem Auftrag
  }
}

void set_LCD(byte z) {                     //set_LCD aktualisiert das LCD mit den Werten aus der aktuellen Ringspeichereinheit 
  lcd.setCursor(0, 0);                     //Schreibe in Zeile 1
  if (ring[z].day < 10)                    //Schreibe eine 0 wenn der Tag <10 ist (01, 02, 09 etc.)
    lcd.print("0");
  lcd.print(ring[z].day);                  //Schreibe den Aktuellen Tag
  lcd.print(".");                          //Schreibe einen Punkt
  if (ring[z].month < 10)
    lcd.print("0");
  lcd.print(ring[z].month);
  lcd.print(".");
  if (ring[z].year < 10)
    lcd.print("0");
  lcd.print(ring[z].year);
  lcd.print(" - ");
  if (ring[z].hour < 10)
    lcd.print("0");
  lcd.print(ring[z].hour);
  lcd.print(":");
  if (ring[z].minute % 60 < 10)
    lcd.print("0");
  lcd.print(ring[z].minute % 60);
  lcd.setCursor(0, 1);                    //Schreibe nun in Zeile 2
  lcd.print("Tmp:");
  lcd.print(ring[z].temp);
  lcd.write(223); //°
  lcd.print("C  Hm:");
  lcd.print(ring[z].humid);
  lcd.print("%");
}

int expect_Pulse(bool level) {                              //waits for signal(LOW or HIGH) and counts length, or returns timeout. Genutzt für read_Heatsensor. Inspiriert durch ADAFRUIT dht11 library
  int timeout = 0;                                          //wait for reply, else -2 = timeout
  while (digitalRead(pin[0]) == level) {                    //level = HIGH or LOW
    if (timeout++ >= microsecondsToClockCycles(1000))       //Inkrementiere Timeout, überprüfe ob wir über dem timeout-Wert sind, falls ja:
      return -2;                                            //Error timeout
  }
  return timeout;                                           //return counted signal length
}

bool read_Heatsensor() {                                    //reads heatsensor  Inspiriert durch ADAFRUIT dht11 library
  in_sensor[0] = in_sensor[2] = in_sensor[4] = 0;           //Überschreibt die vorhandenen Werte mit 0
  //request data
  pinMode(pin[0], OUTPUT);                          //Die Abfolge eines Signals LOW(10ms), HIGH(30micros) signalisiert dem Sensor eine Messung zu machen und die Werte zu übertragen
  digitalWrite(pin[0], LOW);
  delay(10);
  digitalWrite(pin[0], HIGH);
  delayMicroseconds(30);
  pinMode(pin[0], INPUT);                                   //Nach dem Signal schalten wir auf input

  if (expect_Pulse(LOW) == -2)                              //Es kommt nun ein kurzes LOW dann HIGH Signal als Antwort. Nach diesem Signal kommen die Informationen. Kommt dieses Signal nicht -> ERROR
    return 1;                                               //1 = timeout error, Abbruch
  if (expect_Pulse(HIGH) == -2)
    return 1;

  byte cycle[80];
  for (int i = 0; i < 80; i += 2) {                         //read 40 bits of info (16 bits humidity (8 empty), 16 bits temperature (8 empty), 8 parity bits
    cycle[i] = expect_Pulse(LOW);                           //Es kommen abwechselnd Signale LOW und HIGH. Es werden 80 Signale erwartet. Die Länge der 80 Signale speichern wir in cycle zwischen.
    cycle[i + 1] = expect_Pulse(HIGH);
  }
  for (int i = 0; i < 40; ++i) {                            //Nun interpretieren wir die Signallängen. LOW bedeutet nächstes bit, HIGH bedeutet 0 oder 1. War das HIGH Signal länger als das LOW Signal(welches immer etwa gleich lang sein soll), so wird dies als 1 interpretiert
    int low = cycle[2*i];                                   //Aktuelles LOW Signal wird zwischengespeichert
    int high = cycle[2*i+1];                                //Aktuelles HIGH Signal wird zwischengespeichert
    in_sensor[i/8] <<= 1;                                   //in_sensor hat 5 Fächer, jedes Fach soll ein byte speichern. Wir bauen uns das byte indem wir 8 mal bitwise um eine Stelle nach links verschieben, dann entscheiden 0 oder 1.
    if (high > low)       //Wenn HIGH länger als LOW war (d.h. 1)
      in_sensor[i/8] |= 1;    //Baue eine 1 in das byte ein
  }
  if (in_sensor[4] != (in_sensor[0] + in_sensor[2]))  //Unser 5. gebautes byte ist das checksum byte. Ist es nicht das gleiche wie das erste + das dritte byte (das zweite und vierte sind für Nachkommastellen, dieser Sensor misst aber nicht so genau, daher immer 0)
    return 1;                                         //1 = checksum error dann
  return 0;                                           //Sonst lief alles super.
}

