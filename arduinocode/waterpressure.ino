#include "Wire.h" //allows communication over i2c devices
#include "LiquidCrystal_I2C.h" //allows interfacing with LCD screens
#include "SoftwareSerial.h"

const int pressureInput = A0; //select the analog input pin for the pressure transducer
/* senzor settings: G1/4 0-1.2 MPa Hydraulic Pressure Sensor */
/* the sensor is cheap and extraordinarily good. Search for models: HK1100C */
const float Voffset = 0.5;   /* Volts, depends on the stability of the supply voltage. ideal is 0.5V */
const float Vmin = 0.5;       /* Volts */
const float Vmax = 4.5;       /* Volts */
const float Pmin = 0;         /* MPa */
const float Pmax = 1.2;       /* MPa */
const float kPV = (Pmax - Pmin) / (Vmax - Vmin);

const float SupplyVoltage = 5.0;  /* depends on the power supply and the stability of the voltage conversion on the board */
const float MaxDAC = 1024.0;

unsigned long currentTime;
unsigned long cloopTime;

char c = ' ';

LiquidCrystal_I2C lcd(0x27, 16, 2); //sets the LCD I2C communication address; format(address, columns, rows)

void setup() //setup routine, runs once when system turned on or reset
{
  Serial.begin(9600); //initializes serial communication at set baud rate bits per second
  pinMode(pressureInput, INPUT);
  currentTime = millis();
  cloopTime = currentTime;
  lcd.init(); //initializes the LCD screen
  lcd.backlight();
  lcd.print("Start");
  delay(1000);
  lcd.clear();
  delay(500);
  lcd.print("Connecting Wifi");
  delay(1000);
  lcd.clear();
  delay(1000);
}

void loop() //loop routine runs over and over again forever
{
  currentTime = millis();
   // Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 1000))
{
  int sensorVal=analogRead(pressureInput);
  float voltage = (sensorVal* SupplyVoltage) / MaxDAC;
  float pressure_bar = abs(kPV *( (float)voltage - Voffset ) ) * 10 ;  /* x10 = MegaPascal to Bar; use abs() to avoid negative pressure */
  cloopTime = currentTime; // Updates cloopTime
  String kirimdata = String(pressure_bar);
  Serial.println(kirimdata); Serial.print(" | ");
  Serial.println("Sensor Value: "); Serial.print(pressure_bar);
  lcd.setCursor(0,0); //sets cursor to column 0, row 0
  lcd.print("Tekanan:"); //prints label
  lcd.print(pressure_bar); //prints pressure value to lcd screen, 1 digit on float
  lcd.print(" Bar"); //prints label after value
  lcd.print(""); //to clear the display after large values or negatives
  delay(1000); //delay in milliseconds between read values
 }
}
