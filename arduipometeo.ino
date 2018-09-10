/* Stazione meteo ipogea
   Raffaello Di Martino IZ0QWM
   http://www.kwos.it */

//Libraries
#include <DHT.h>
#include "RTClib.h"
#include <DS3231.h> // http://www.rinkydinkelectronics.com/library.php?id=73
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <stdio.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

//Costanti generiche
int i = 0;
#define PIN_LED 9

//Costanti per termometro
#define DHTPIN 7     // PIN a cui e' collegato il DHT22
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

//Costanti per anemometro
#define PIN_ANEMOMETER 3 // Digital 3
// Tempo di aggiornamento della velocita' del vento
#define MSECS_CALC_WIND_SPEED 5000

//Variabili per anemometro
volatile int numRevsAnemometer = 0; // Incrementato nella funzione countAnemometer()

File logfile;

//Start Modulo RTC 
DS3231  rtca(SDA, SCL); //Solo per leggere
RTC_DS3231 rtc; //RTC DS3231 object per settare l'orario

//Start modulo BMP280
Adafruit_BMP280 bme; // su I2C

void setup()
{
  
  Serial.begin(9600);
  //Led
  pinMode(PIN_LED, OUTPUT);
  //Anemometro
  pinMode(PIN_ANEMOMETER, INPUT);
  digitalWrite(PIN_ANEMOMETER, HIGH);
  attachInterrupt(1, countAnemometer, FALLING);
  // Realtime Clock
  rtca.begin(); // Solo per leggere
  rtc.begin(); // Solo per settare l'orario
  // Sensore pressione
  bme.begin();
  //Serial.print("---------------------------------------------------\n");
  //Serial.print("INIZIALIZZAZIONE\n");
  //Serial.print("---------------------------------------------------\n");
  //Serial.print("Setto l'orario per il log file...\n");
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Prendiamo l'orario dal PC

  //SD Card
  //Serial.print("Inizializzazione SD Card...\n");

  if (!SD.begin(4)) {// Usiamo il pin 4 dove c'e' il CS per capire se la scheda e' inserita
    //Serial.println("Inizializzazione fallita!\n");
    while (1);
  }
  //Serial.println("Inizializzazione OK.\n");  
  
}

void loop()
{
    digitalWrite(PIN_LED, HIGH); 
    // create a new file
    char filename[] = "LOGGER00.CSV";
    for (uint8_t i = 0; i < 100; i++) {
      filename[6] = i/10 + '0';
      filename[7] = i%10 + '0';
      if (! SD.exists(filename)) {
        //logfile.close();
        logfile = SD.open(filename, FILE_WRITE); 
        // Serial.println("GIORNO,ORA,HUM,TEMPOUT,TEMPINT,WIND,TEMPBARO,PRESS");
        logfile.println("GIORNO,ORA,HUM,TEMPOUT,TEMPINT,WIND,TEMPBARO,PRESS\n");
        logfile.close();
        delay(1000);
      }
      logfile = SD.open(filename, FILE_WRITE);
      delay(1000); 
      //Serial.println(logfile.size());
      if (logfile.size() < 200000) {
        break;  // leave the loop!
      }
    } 
    //delay(1000);
    String TempHum = calcTempHumidity();
    //Prendo la stringa dalla funzione WindSpeed
    String Wind = calcWindSpeed();
    //Prendo la stringa dalla funzione calcPressure
    String Pressure = calcPressure();
    //Creo la stringa completa
    //String total = DataOra + "," + TempHum + "," + Wind + "," + Pressure;
    logfile.print(rtca.getDateStr());
    logfile.print(",");
    logfile.print(rtca.getTimeStr());
    //logfile.print(DataOra);
    logfile.print(",");
    logfile.print(TempHum);
    logfile.print(",");
    logfile.print(Wind);
    logfile.print(",");
    logfile.print(Pressure);
    logfile.println();
    logfile.close();

    /*Serial.print(rtca.getDateStr());
    Serial.print(",");
    Serial.print(rtca.getTimeStr());
    Serial.print(",");
    Serial.print(TempHum);
    Serial.print(",");
    Serial.print(Wind);
    Serial.print(",");
    Serial.print(Pressure);
    Serial.print(" -> ");
    Serial.print(filename);
    Serial.println();*/
    
    digitalWrite(PIN_LED, LOW);
    
    // Azzero le rivoluzioni dell'anemometro
    numRevsAnemometer = 0;
    
    delay(5000); //Delay 5 sec.

}

//=======================================================
// Interrupt handler for anemometer. Richiamato ogni volta che il reedswitch
// si attiva (un giro - revolution).
//=======================================================
void countAnemometer() {
numRevsAnemometer++;
}

String calcTempHumidity() {
    //Variabili per termometro
    float hum;  //Valore umidita' DHT
    float tempOut; //Valore temperatura DHT
    float tempIn;//Valore temperatura DS3231
    dht.begin();
    tempIn = rtca.getTemp();
    //Leggi i dati di temperatura e umidita' e inserisci nelle variabili
    hum = dht.readHumidity();
    tempOut = dht.readTemperature();
    String separatore = ",";
    String hum_temp = hum + separatore + tempOut + separatore + tempIn;
    //Serial.println(hum_temp);
    return hum_temp;
  
}


String calcPressure() {
    float tempBmp;
    tempBmp = bme.readTemperature();
    //Serial.print(bme.readTemperature());
    float pascal, hpa;
    pascal = bme.readPressure();
    //Serial.print(bme.readPressure());
    hpa = pascal * 0.01;
    
    String separatore = ",";
    //Serial.print(meters);
    String temp_press = tempBmp + separatore + hpa;
    //Serial.print(pascal);
    return temp_press;   
} 

//=======================================================
// Calcola la velocita' del vento.
// 1 rev/sec = 2.5 mph
//=======================================================
String calcWindSpeed() {
int y, x, iSpeed;
// This will produce mph * 10
// (didn't calc right when done as one statement)
long speed = 25000;//2.5 mph * 1000
speed *= numRevsAnemometer;
speed /= MSECS_CALC_WIND_SPEED;
iSpeed = speed; // Need this for formatting below

x = iSpeed / 10; // Valore in mph
y = x * 1.60934; // Trasformo in Km/h
String punto = ".";
//Serial.print('.');
x = iSpeed % 10;
String wind = y + punto + x;
return wind;
//Serial.println(wind);
numRevsAnemometer = 0; // Resetta il contatore
}

   
