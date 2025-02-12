#include <SPI.h>
#include <LoRa.h>
#include <SoftwareSerial.h>
#include "DHT.h"
#define IN1LP 36 //Motor left positive
#define IN2LN 38 //Motor left negative
#define IN3RP 40 //Motor right positive
#define IN4RN 42 //Motor right negative
#define ENA 44 //Control motor left
#define ENB 46 //Control motor right
#define SS 53 //LoRa
#define RST 9 //LoRa
#define DIO0 8 //LoRa
#define DHTPIN 21 //DHT OUT Pin
#define DHTTYPE DHT22 //DHT Type
#define MQ_PIN                       (5)     //define which analog input channel you are going to use
#define RL_VALUE                     (5)     //define the load resistance on the board, in kilo ohms
#define RO_CLEAN_AIR_FACTOR          (9.83)  //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
//which is derived from the chart in datasheet
/***********************Software Related Macros************************************/
#define CALIBARAION_SAMPLE_TIMES     (50)    //define how many samples you are going to take in the calibration phase
#define CALIBRATION_SAMPLE_INTERVAL  (500)   //define the time interal(in milisecond) between each samples in the
//cablibration phase
#define READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define READ_SAMPLE_TIMES            (5)     //define the time interal(in milisecond) between each samples in 
//normal operation
/**********************Application Related Macros**********************************/
#define GAS_LPG                      (0)
int ch1;
int ch2;
int ch3;
int ch4;
int ch5;
int ch6;
int motorPin1 = 10;// Blue   - 28BYJ48 pin 1
int motorPin2 = 11;// Pink   - 28BYJ48 pin 2
int motorPin3 = 12;// Yellow - 28BYJ48 pin 3
int motorPin4 = 13;// Orange - 28BYJ48 pin 4
int motorSpeed = 1;     //variable to set stepper speed
int Pin1 = 30;
float val1 = 0;
int Pin2 = 32;
float val2 = 0;
int Pin3 = 34;
float val3 = 0;
int gas = 0;
//int analogPin = 5; //ประกาศตัวแปร ให้ analogPin แทนขา analog ขาที่5
float LPGCurve[3]  =  {2.3, 0.21, -0.47};
float Ro           =  10;
char lpg;
DHT dht(DHTPIN, DHTTYPE);


int contemp;
byte temp;
byte dectemp;
byte detect1;
byte detect2;
byte detect3;
byte gasdetect;
long dec;


  
  
float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE * (1023 - raw_adc) / raw_adc));
}
float MQCalibration(int mq_pin)
{
  int i;
  float val = 0;

  for (i = 0; i < CALIBARAION_SAMPLE_TIMES; i++) {      //take multiple samples
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val / CALIBARAION_SAMPLE_TIMES;                 //calculate the average value

  val = val / RO_CLEAN_AIR_FACTOR;                      //divided by RO_CLEAN_AIR_FACTOR yields the Ro
  //according to the chart in the datasheet
  return val;
}
float MQRead(int mq_pin)
{
  int i;
  float rs = 0;
  for (i = 0; i < READ_SAMPLE_TIMES; i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
  rs = rs / READ_SAMPLE_TIMES;
  return rs;
}
int MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
  if ( gas_id == GAS_LPG ) {
    return MQGetPercentage(rs_ro_ratio, LPGCurve);
  }
  else {
    return 0;
  }
}
int  MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10, ( ((log(rs_ro_ratio) - pcurve[1]) / pcurve[2]) + pcurve[0])));
}


void setup() {
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(IN1LP, OUTPUT);
  pinMode(IN2LN, OUTPUT);
  pinMode(IN3RP, OUTPUT);
  pinMode(IN4RN, OUTPUT);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);
  LoRa.setPins(SS, RST, DIO0);
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  Serial.print("Calibrating...\n");
  Ro = MQCalibration(MQ_PIN);                      //Calibrating the sensor. Please make sure the sensor is in clean air
  //when you perform the calibration
  Serial.print("Calibration is done...\n");
  Serial.print("Ro=");
  Serial.print(Ro);
  Serial.print("kohm");
  Serial.print("\n");
  dht.begin();
  // set up lora
  while (!Serial);
  Serial.println("LoRa Sender");
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
 
}


void loop() {

   float temp = dht.readTemperature();
int contemp = int (temp);
  dec = 100 * (temp - contemp);
  String tempval;
  tempval = String(contemp) + "." + String(dec);
  // -------------
  
  
  if (isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.print(temp);
  Serial.println( F("°C ") );
  Serial.println((" "));
  Serial.print("Test String Temp: ");
  Serial.println(tempval);
delay(300);
  // PIR data
  val1 = digitalRead(Pin1);
  val2 = digitalRead(Pin2);
  val3 = digitalRead(Pin3);
  String motiondetect;
  if (val1 == 0 && val2 == 0 && val3 == 0) {
    motiondetect = "No Motion Detect";
  } else if (val1 == 1 || val2 == 0 || val3 == 0) {
    motiondetect = "Motion Detect";
  }
  else if (val1 == 0 || val2 == 1 || val3 == 0) {
    motiondetect = "Motion Detect";
  }
  else if (val1 == 0 || val2 == 0 || val3 == 1) {
    motiondetect = "Motion Detect";
  }
  Serial.print("Motion detect Back : ");
  Serial.println(val1); //หลัง
  Serial.print("Motion detect Right : ");
  Serial.println(val2); //ขวา
  Serial.print("Motion detect Left : ");
  Serial.println(val3); //ซ้าย
  Serial.println(motiondetect );
  delay(300);
  //Gas Data
  Serial.print("LPG:");
  Serial.print(MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_LPG) );
  Serial.println( "ppm" );
  //gas = analogRead(analogPin);
  lpg = MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_LPG);
  String Gas;
  if (lpg > 0) {
    Gas = "Have Gas";
  } else {
    Gas = "No Gas";
  }
  Serial.println( lpg );
delay(300);
  //----------------
  // get signal from rc
  ch1 = pulseIn(2, HIGH, 25000);
  ch2 = pulseIn(3, LOW, 25000);
  ch3 = pulseIn(4, HIGH, 25000);
  ch4 = pulseIn(5, LOW, 25000);
  ch5 = pulseIn(6, HIGH, 25000);
  ch6 = pulseIn(7, LOW, 25000);

  boolean ch = ch1 > 0 && ch2 > 0;
  if (ch == 1) { 
    if (ch4 > 18800) { //left
      clockwise();  //หมุนวนซ้าย
    }
    else if (ch4 < 18100) { //right
      counterclockwise(); //หมุนวนขวา
    }
    else {
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, LOW);
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, LOW);
    }
    if (ch2 < 18200) { //forward
      analogWrite(ENA, 150);
      analogWrite(ENB, 150);
      digitalWrite(IN1LP, HIGH);
      digitalWrite(IN2LN, LOW);
      digitalWrite(IN3RP, HIGH);
      digitalWrite(IN4RN, LOW);
    }
    else if (ch2 > 18700) { //backward
      analogWrite(ENA, 150);
      analogWrite(ENB, 150);
      digitalWrite(IN1LP, LOW);
      digitalWrite(IN2LN, HIGH);
      digitalWrite(IN3RP, LOW);
      digitalWrite(IN4RN, HIGH);
    }
    else if (ch1 < 1150) { //Left
      analogWrite(ENA, 135);
      analogWrite(ENB, 200);
      digitalWrite(IN1LP, HIGH);
      digitalWrite(IN2LN, LOW);
      digitalWrite(IN3RP, LOW);
      digitalWrite(IN4RN, HIGH);
    }
    else if (ch1 > 1830) { //Right
      analogWrite(ENA, 200);
      analogWrite(ENB, 135);
      digitalWrite(IN1LP, LOW);
      digitalWrite(IN2LN, HIGH);
      digitalWrite(IN3RP, HIGH);
      digitalWrite(IN4RN, LOW);
    }
  
    else { //Stop
      analogWrite(ENA, 0);
      analogWrite(ENB, 0);
      digitalWrite(IN1LP, LOW);
      digitalWrite(IN2LN, LOW);
      digitalWrite(IN3RP, LOW);
      digitalWrite(IN4RN, LOW);
    }
  }
  else {
   
    
    temp = contemp;
    dectemp = dec;
    detect1 = val1;
    detect2 = val2;
    detect3 = val3;
    gasdetect = lpg;
    LoRa.beginPacket();
    LoRa.write(temp);
    LoRa.write(dectemp);
    LoRa.write(detect1);
    LoRa.write(detect2);
    LoRa.write(detect3);
    LoRa.write(gasdetect);
    LoRa.endPacket();
   
  }
  delay(300);
}
void counterclockwise(){
  // 1
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay(motorSpeed);
  // 2
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay (motorSpeed);
  // 3
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  delay(motorSpeed);
  // 4
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
  delay(motorSpeed);
  // 5
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
  delay(motorSpeed);
  // 6
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, HIGH);
  delay (motorSpeed);
  // 7
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
  delay(motorSpeed);
  // 8
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
  delay(motorSpeed);
}
void clockwise(){
  // 1
  digitalWrite(motorPin4, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin1, LOW);
  delay(motorSpeed);
  // 2
  digitalWrite(motorPin4, HIGH);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin1, LOW);
  delay (motorSpeed);
  // 3
  digitalWrite(motorPin4, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin1, LOW);
  delay(motorSpeed);
  // 4
  digitalWrite(motorPin4, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin1, LOW);
  delay(motorSpeed);
  // 5
  digitalWrite(motorPin4, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin1, LOW);
  delay(motorSpeed);
  // 6
  digitalWrite(motorPin4, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin1, HIGH);
  delay (motorSpeed);
  // 7
  digitalWrite(motorPin4, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin1, HIGH);
  delay(motorSpeed);
  // 8
  digitalWrite(motorPin4, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin1, HIGH);
  delay(motorSpeed);
}
