#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define BACKLIGHT_PIN 3 //Podswietlenie wyswietlacza
#define ONE_WIRE_BUS 22 //Wejscie z czujnika temp
#define DIODA_GRZANIE_PIN 26 
#define DIODA_GAZ_PIN 34
#define ANALOG_IN_PIN A14 //Wejscie z czujnika gazu

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

bool grzanie;
bool wentylacja;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7);  // ustawienie adresu LCD i kolejnych wejsc w I2C

void setup(){

  grzanie=0;
  Serial.begin(9600);
  sensors.setResolution(insideThermometer, 9);
  sensors.getAddress(insideThermometer, 0);
  
  lcd.begin(16, 2);
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);

  pinMode(DIODA_GRZANIE_PIN, OUTPUT);
  pinMode(DIODA_GAZ_PIN, OUTPUT);
  pinMode(ANALOG_IN_PIN, INPUT);
}

void gas_sensor(float max, float min)
{
  float gasSensor;  //wartosc na analogu
  gasSensor = analogRead(ANALOG_IN_PIN);
  
  Serial.print("Wartosc na analogu: "); //wypisywanie na konsole
  Serial.print(gasSensor);
  Serial.print("\n");
              
  if (gasSensor > max)
  {
    digitalWrite(DIODA_GAZ_PIN, HIGH);
    wentylacja=1;
  }

  if ((wentylacja==1)&&(gasSensor < min))
  {
    digitalWrite(DIODA_GAZ_PIN, LOW);
    wentylacja=0;
  }
  
}

void printTemperature(DeviceAddress deviceAddress){
  
  float tempC = sensors.getTempC(deviceAddress);
  lcd.home();
  lcd.print("Temp: ");
  lcd.print(tempC);
  lcd.print(" C");
}

void loop(){
  
float MIN_TEMP=24;    
float MAX_TEMP=26;

float MIN_AIR=300;    //czulosc czujnika gazu
float MAX_AIR=400;

  sensors.requestTemperatures();
  printTemperature(insideThermometer);

  gas_sensor(MAX_AIR, MIN_AIR);
 
  
         if((wentylacja==0)&&((sensors.getTempC(insideThermometer)) < (MIN_TEMP)))
          {
            grzanie=1;
          }
          
         if((wentylacja==1)||((grzanie==1)&&((sensors.getTempC(insideThermometer)) > (MAX_TEMP))))
          {
            grzanie=0;
          }

   //wypisywanie info na LCD:
         if(grzanie==1)
          {
            lcd.setCursor(0,1);
            lcd.print("Grzeje    ");
            digitalWrite(DIODA_GRZANIE_PIN, HIGH);
          }
          else 
          {
            digitalWrite(DIODA_GRZANIE_PIN,LOW);
            
              if(wentylacja==1)
                    {
                      lcd.setCursor(0,1);
                      lcd.print("Wentylacja");
                     }
          
                    else
                    {
                      lcd.setCursor(0,1);
                      lcd.print("          ");
                    } 
          }       
}

