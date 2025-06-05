//colocar un capacitor entre VMOT y GND (100micros), la esp se puede dañar al operarla a 5v

/*
 * Pinout:
 * ESP32-DevKitC V4 WROOM-32U
 * 
 * IR Sensor GP2Y0A21YK (10-80cm, 5V) attached as follows:
 * - Sense - GPIO36 (VP, ADC input)
 * 
 * Turntable stepper motor driver board (A4988 Pololu):
 * - STEP   - GPIO14
 * - DIR    - GPIO27
 * - MS1    - GPIO26
 * - MS2    - GPIO25
 * - ENABLE - GPIO33
 * 
 * Z-Axis stepper motor driver board (A4988 Pololu):
 * - STEP   - GPIO32
 * - DIR    - GPIO21
 * - MS1    - GPIO22
 * - MS2    - GPIO23
 * - ENABLE - GPIO19
 * 
 * OLED Display 0.96" SPI (SSD1306 controller):
 * - MOSI   - GPIO13 //cambiar 
 * void setup() {
  SPI.begin(OLED_CLK, -1, OLED_MOSI, OLED_CS);  // -1 porque no usamos MISO
  display.begin(SSD1306_SWITCHCAPVCC);
  display.display();
  } 
 * 
 * - MISO   - GPIO19 
 * - SCK    - GPIO18
 * - CS     - GPIO5
 * - DC     - GPIO17
 * - RESET  - GPIO16
 */

//#include "FS.h" //manejo de archivos y SD, pero los datos se envían al PC por serial.
#include "SPI.h"
//librerias para la OLED SPI (SSD1306)
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"


String filename="scn000.txt";        
int sensePin = 36;     

int tStep = 14; // Ajustar
int tDir  = 27;
int tMS1  = 26;
int tMS2  = 25;
int tEnable = 33;

int zStep = 32;
int zDir  = 21;
int zMS1  = 22;
int zMS2  = 23;
int zEnable = 19;
//conexiones al display OLED SPI
int OLED_RESET = 16;
int OLED_DC = 17;
int OLED_CS = 5;

Adafruit_SSD1306 display(128, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS);

void setup() 
{ 
  //Define stepper pins as digital output pins
  pinMode(tStep, OUTPUT);
  pinMode(tDir, OUTPUT);
  pinMode(tMS1, OUTPUT);  
  pinMode(tMS2, OUTPUT);
  pinMode(tEnable, OUTPUT);
  pinMode(zStep, OUTPUT);
  pinMode(zDir, OUTPUT);
  pinMode(zMS1, OUTPUT);
  pinMode(zMS2, OUTPUT);
  pinMode(zEnable, OUTPUT);
  //
  digitalWrite(tMS1, LOW);
  digitalWrite(tMS2, LOW);
  digitalWrite(zMS1, LOW);
  digitalWrite(zMS2, LOW);
  //
  digitalWrite(tEnable, LOW);
  digitalWrite(zEnable, LOW);

  initDisplay();  
  
  digitalWrite(tMS1,LOW);
  digitalWrite(tMS2,LOW);
  
  
  digitalWrite(zMS1,LOW);
  digitalWrite(zMS2,LOW);

  //Set rotation direction of motors
  //digitalWrite(tDir,HIGH);
  //digitalWrite(zDir,LOW);

  //Enable motor controllers
  digitalWrite(tEnable,LOW);
  digitalWrite(zEnable,LOW);
    
  // Open serial communications
  Serial.begin(9600);
  
}

void loop() 
{
  int vertDistance=10; //Total desired z-axis travel
  int noZSteps=20; //No of z-steps per rotation.  Distance = noSteps*0.05mm/step
  int zCounts=(200/1*vertDistance)/noZSteps; //Total number of zCounts until z-axis returns home
  int thetaCounts=200;

  // Scan object
  digitalWrite(zDir,LOW); 
  for (int j=0; j<zCounts; j++) //Rotate z axis loop
  {
    for (int i=0; i<thetaCounts; i++)   //Rotate theta motor for one revolution, read sensor and store
    {
      rotateMotor(tStep, 1); //Rotate theta motor one step
      myDelay(200);
      double senseDistance=0; //Reset senseDistanceVariable;
      senseDistance=readAnalogSensor(); //Read IR sensor, calculate distance
      writeToSerial(senseDistance, i, j);
    }
  
    rotateMotor(zStep, noZSteps); //Move z carriage up one step
    myDelay(1000);
  }

  // Scan complete.  Rotate z-axis back to home and pause.
  digitalWrite(zDir,HIGH);
  myDelay(10);  
  for (int j=0; j<zCounts; j++)
  {
    rotateMotor(zStep, noZSteps);
    myDelay(10);
  }

  for (int k=0; k<3600; k++) //Pause for one hour (3600 seconds), i.e. freeze until power off because scan is complete.
  {
    myDelay(1000); 
  } 
}

void rotateMotor(int pinNo, int steps)
{
  
  for (int i=0; i<steps; i++)
  {
    digitalWrite(pinNo, LOW); //LOW to HIGH changes creates the
    myDelay(1);
    digitalWrite(pinNo, HIGH); //"Rising Edge" so that the EasyDriver knows when to step.
    myDelay(1);
  }
}

double readAnalogSensor()
{
  int noSamples=100;
  int sumOfSamples=0;

  int senseValue=0;
  double senseDistance=0;
  
  for (int i=0; i<noSamples; i++)
  {
    senseValue=analogRead(sensePin); //Perform analogRead
    myDelay(2); //Delay to let analogPin settle -- not sure if necessary
    sumOfSamples=sumOfSamples+senseValue; //Running sum of sensed distances
  }
  
  senseValue=sumOfSamples/noSamples; //Calculate mean raw
  senseDistance=senseValue; //Convert to double
  senseDistance = mapDouble(senseDistance, 0.0, 4095.0, 0.0, 3.3); //Convert analog pin reading to voltage
  senseDistance = 4800.0 / (senseDistance * 1000.0 - 20.0);//Convert voltage to distance in cm via cubic fit of Sharp sensor datasheet calibration
  return senseDistance;
}
void writeToSerial(double senseDistance, int thetaStep, int zStep)
{
  // Convierte pasos en ángulo y altura
  float thetaDeg = (360.0 / 200.0) * thetaStep; // suponiendo 200 pasos por vuelta
  float zMM = (0.05 * zStep); // 0.05 mm por paso

  Serial.print(senseDistance, 2);
  Serial.print(",");
  Serial.print(thetaDeg, 2);
  Serial.print(",");
  Serial.println(zMM, 2);
}

double mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("No se pudo inicializar la pantalla OLED");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Escaner 3D listo");
  display.display();
}

void myDelay(int Delay){
  unsigned long previous = millis();
  if(millis() - previous < Delay){
    NULL;
  }
}
