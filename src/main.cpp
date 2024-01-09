//arduino
#include <Arduino.h>
#include <SoftwareSerial.h>
//MPU6050
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

<<<<<<< Updated upstream

=======
// Définitions des constantes
#define TOLERANCE_ANGLE_CHANGE 15    // Tolérance de changement d'angle après calibration
#define TOLERANCE_VIBRATION 0.1      // Seuil de détection de vibration (probablement en radians)
#define DURATION_CALIB_GYRO 200     // Durée de calibration du gyroscope en millisecondes
#define BLINKER_PIN 1                // Pin pour la détection de clignotement (recalibration)
>>>>>>> Stashed changes


#define tolerance_changement_angle 10 //degrés en +/- des extremas de la calibration
#define serial_fox_rx 4 //rx de l'arduino, tx du sigfox, etc.
#define serial_fox_tx 3
#define delais_sigfox 60000 //délais pendant lequel le système ne renvoie pas d'alerte radio (millisecondes)
#define duree_calibration_gyro  100 //durée de callibration du gyroscope (millisecondes)
#define duree_sirene 1 //duree de la douce mélodie (millisecondes)
#define siren_pin 17 //pin de la gate du mofset de la sirene
#define delais_sirene 0 //délais pendant lequel le système ne fera pas 2 déclenchements de la mélodie (à partir du début de la précédente)



/*@brief  Each bit represents an error. See here for more info
@details 0 -> everything is right, the code is sent by sigfox or the alarm if != 0, 1 low importance, 2 critical
bit decimal erreur             //action 

0  1: 
1  2: 
2  4:      GPS Disconnected    // nothing                                                 ok
3  8:
4  16:
5  32:     debugSendSig        // sigfox                                                  a voir
6  64:     Low battery         // sigfox +       + specific music (like CO alarms)        todo
7  128:         
8  256:    sigfox disconnected // sigfox + siren                                          todo
9  512:    
10 1024:
11 2048:   
12 4096:   
13 8192:   AccelDetect         // sigfox + siren                                          ok
14 16384:  GyroDetect          // sigfox + siren                                          ok
15 32768:  MPU fail            // sigfox + siren                                          ok je crois

ca pourrait être 16, mais pas grave

*/
int32_t error_code = 0; 
int32_t sigfoxErrorLevelTrigger = 32 ; // définit à partir de quel niveau envoyer message sigfox, ex:  tous les error code > 32 sont envoyés par sigfox
int32_t  sirenErrorLevelTrigger = 256; // définit à partir de quel niveau faire sirene, ex: tous les error code > 32  allument la sirene

//faire un melody trigger pour economiser la batterie, facon alarme incendie qui a plus de piles






//gyro
Adafruit_MPU6050 mpu;
//ref de l'angle
double refXmin, refXmax, refYmin, refYmax, refZmin, refZmax;  //extremas trouvés pendant calibration
double toleranceAngle = tolerance_changement_angle;//degrés en +/- des extremas de la calibration
unsigned long dureeCalibration = duree_calibration_gyro; //durée en millisecondes de callibration
sensors_event_t g, a, temp;  // ici c'est ok ???

//WISOL WSSFM10R1
SoftwareSerial SerFox(serial_fox_rx, serial_fox_tx); //inversé par rapport a arduino rx puis tx arduino ---- tx puis rx module
//bitSet(error_code, 5);//DEBUGAGE : envois un message sigfox dans tous les cas
unsigned long debutSigfox = 0; 
unsigned long delaisSigfox = delais_sigfox; //millisecondes 

//siren
int siren = siren_pin; //pin sur l'arduino nano - gate of the n mofset
unsigned long debutSirene = 0; 
unsigned long dureeSirene = duree_sirene; //millisecondes 
unsigned long delaisSirene = delais_sirene; //millisecondes 

<<<<<<< Updated upstream
=======
// Création de l'instance de la sirène
Siren mySiren(SIREN_PIN, SIREN_FREQ, SIREN_DURATION_MINIMAL, SIREN_MAX_DURATION, SIREN_INTERVAL_DURATION, SIREN_MIN_DELAY_BETWEEN_TWO_TRIGGERS, SIREN_HAS_BEEN_PLAYING_FOR_TOO_LONG_MINIMUM_DELAY_WITHOUT_ERROR, SIREN_LOG_SIZE);
>>>>>>> Stashed changes

/*
cheatsheet millis :
https://www.norwegiancreations.com/2018/10/arduino-tutorial-avoiding-the-overflow-issue-when-using-millis-and-micros/
while  : (millis() - start < ms) ;
before : (millis() - start > ms) ;
*/

<<<<<<< Updated upstream

/* ---------------------------------------------- */
=======
    // 1er setup tone
    mySiren.playQuickTone();
    
    // Initialisation et calibration des capteurs
    verifyMPU();
    setupMPUTiltDetection();
    calibrateAccel();
    calibrateGyro();

    // 2eme setup tone
    mySiren.playQuickTone();
>>>>>>> Stashed changes

  


void gyroCali(sensors_event_t g){ // calibrate orientation during the first dureeCalibration seconds
  //set reference values
  mpu.getEvent(&g, &a, &temp);
  refXmin = g.gyro.x;
  refXmax = g.gyro.x;
  refYmin = g.gyro.y;
  refYmax = g.gyro.y;
  refZmin = g.gyro.z;
  refZmax = g.gyro.z;
  unsigned long tiktak = millis();
  while (millis() - tiktak < dureeCalibration) { //continue tant qu'on dépasse pas
    //x
    // record the maximum sensor value
    if (g.gyro.x > refXmax) {
      refXmax = g.gyro.x;
    }
    // record the minimum sensor value
    if (g.gyro.x < refXmin) {
      refXmin = g.gyro.x;
    }
    //y
    // record the maximum sensor value
    if (g.gyro.y > refYmax) {
      refYmax = g.gyro.y;
    }
    // record the minimum sensor value
    if (g.gyro.y < refYmin) {
      refYmin = g.gyro.y;
    }
    //z
    // record the maximum sensor value
    if (g.gyro.z > refZmax) {
      refZmax = g.gyro.z;
    }
    // record the minimum sensor value
    if (g.gyro.z < refZmin) {
      refZmin = g.gyro.z;
    }
  }
}

void verifMPU(){ 
  if (!mpu.begin()) {
    bitSet(error_code, 15);//Failed to find MPU6050 chip
  }
}

void setupMPUShockDetection(){ //TODO VVVV
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(2);// a calibrer
  mpu.setMotionDetectionDuration(2);// a calibrer
  mpu.setInterruptPinLatch(true);  // Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);
}

void setupMPUTiltDetection(){ //TODO VVVV
  //---------setup angle change detection---------------//
  /* Get new sensor events with the readings */
  // set accelerometer range to +-8G
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  // set gyro range to +- 500 deg/s
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  // set filter bandwidth to 21 Hz
  mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
}

void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);

  //Begin serial communication with Arduino and WISOL WSSFM10R1
  SerFox.begin(9600); // !!!!!! le moniteur série doit etre en CR

  //verifMPU(); dans loop
  setupMPUShockDetection();
  setupMPUTiltDetection();
  mpu.getEvent(&g, &a, &temp);
  gyroCali(g);



  //siren
  pinMode(siren, OUTPUT);
  digitalWrite(siren, LOW);
}

void updateSerial() { //uniquement pour du debug avec le pc
  delay(1000);//super important pour pas corrompre
  while (Serial.available()) {
    SerFox.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(SerFox.available()) {
    Serial.write(SerFox.read());//Forward what Software Serial received to Serial Port
  }
}

void serFoxFloatPrint(float f) { //conversion float -> hex
  byte * b = (byte *) &f;
  if(b[0] < 16) SerFox.print('0');
  SerFox.print(b[0], HEX);
  if(b[1] < 16) SerFox.print('0');
  SerFox.print(b[1], HEX);
  if(b[2] < 16) SerFox.print('0');
  SerFox.print(b[2], HEX);
  if(b[3] < 16) SerFox.print('0');
  SerFox.print(b[3], HEX);
}

void serFoxInt32Print(int32_t i) { //conversion int32_t -> hex
  byte * b = (byte *) &i;
  if(b[0] < 16) SerFox.print('0');
  SerFox.print(b[0], HEX);
  if(b[1] < 16) SerFox.print('0');
  SerFox.print(b[1], HEX);
  if(b[2] < 16) SerFox.print('0');
  SerFox.print(b[2], HEX);
  if(b[3] < 16) SerFox.print('0');
  SerFox.print(b[3], HEX);
}

void gyroGraph(){
  /* Get new sensor events with the readings */
  //sensors_event_t g, a, temp;
  mpu.getEvent(&g, &a, &temp);

  /* Print out the values *
  Serial.print(a.acceleration.x);
  Serial.print(",");
  Serial.print(a.acceleration.y);
  Serial.print(",");
  Serial.print(a.acceleration.z);
  Serial.print(", ");//*/

  Serial.print(",");
  Serial.print(g.gyro.x);
  Serial.print(",");
  Serial.print(refXmax + toleranceAngle);
  Serial.print(",");
  Serial.print(refXmin - toleranceAngle);
  Serial.print(",");
  Serial.print(g.gyro.y);
  Serial.print(",");
  Serial.print(refYmax + toleranceAngle);
  Serial.print(",");
  Serial.print(refYmin - toleranceAngle);
  Serial.print(",");
  Serial.print(g.gyro.z);
  Serial.print(",");
  Serial.print(refZmax + toleranceAngle);
  Serial.print(",");
  Serial.print(refZmin - toleranceAngle);
  Serial.print(",");

  Serial.println("");
}

void msgSigfox(int32_t error,float lat, float lng){
  SerFox.listen(); //Only one SoftwareSerial object can listen at a time
  SerFox.print("AT$SF=");
  //intégrer code d'erreur
  serFoxInt32Print(error);
  serFoxFloatPrint(lat); //32 bits - 4 bytes 
  serFoxFloatPrint(lng);
  SerFox.write(0x0D);    
  updateSerial();
}

void doSiren(){
  if (error_code >=  sirenErrorLevelTrigger){ //le code se clear en fin de loop
    if(millis() - debutSirene > delaisSirene ){ // après la durée de pause
      debutSirene = millis(); //debut du delais entre deux melodies
      digitalWrite(siren, HIGH);
    }
  }
  //ici car le reset DOIT se faire VV
  if(millis() - debutSirene > dureeSirene ){ // après la durée de mélodie
    digitalWrite(siren, LOW);
  }
  

  // TODO: delais_sirene
}

void sendAlert() {
  if ( error_code >=  sigfoxErrorLevelTrigger ){ //  voir declaration  , se clear en fin de loop
    if(millis() - debutSigfox >  delaisSigfox ){ //reset interdiction de l'envoi temporaire sans gps, imaginer que ce if est à la fin
      debutSigfox = millis(); //debut du delais entre deux envois
      msgSigfox(error_code, float(0.0),float(0.0)); //avant c'etait latitude et longitude dans les float
    }
  }
}

void accelDetect(){
  if(mpu.getMotionInterruptStatus()){ //detects accelerations, not motion!
  bitSet(error_code, 13);//accel interrupt
    Serial.print("shock\r\n");
  }
}

void gyroDetect(){

  mpu.getEvent(&g, &a, &temp);
  
  if( (g.gyro.x > refXmax + toleranceAngle) || (g.gyro.x < refXmin - toleranceAngle)){ 
    bitSet(error_code, 14);//tilt changed
  }
  if( (g.gyro.y > refYmax + toleranceAngle) || (g.gyro.y < refYmin - toleranceAngle)){
    bitSet(error_code, 14);//tilt changed
  }
  if( (g.gyro.z > refZmax + toleranceAngle) || (g.gyro.z < refZmin - toleranceAngle)){
    bitSet(error_code, 14);//tilt changed
  }
}




void loop()
{
  

      msgSigfox(error_code, float(0.0),float(0.0)); //avant c'etait latitude et longitude dans les float
  //mpu6050
  verifMPU();
  accelDetect();
  gyroDetect();

  //conditions et envoi
  //doSiren(); //ajouter des check a chaque étape pour vérifier que tout va bien
  sendAlert();

 

  //debug
  gyroGraph();
  //debug
  Serial.print(error_code);
  Serial.print(",");
  Serial.print(error_code, BIN);
  Serial.print(",");

  error_code = 0; // l'erreur s'additionne jusqu'à la sirène et le sigfox puis est reset, ca n'influe pas les délais
}

