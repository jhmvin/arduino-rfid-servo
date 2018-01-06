/**
 * Include Library Files
 */
#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>  // Library for Mifare RC522 Devices -> https://github.com/miguelbalboa/rfid
#include <Servo.h>  // Servo Library

/**
 * Common Cathod LED
 */
#define PIN_ON HIGH
#define PIN_OFF LOW

/**
 * Visual Output
 */
#define LED_RED 2
#define LED_GREEN 3
#define LED_BLUE 4 

/**
 * Servo Motor Pin
 */
#define SERVO_PIN 8
// servo instance
Servo myservo;
// servo angle
#define SERVO_OPEN 15
#define SERVO_CLOSE 115

/**
 * RFID Reader Pins
 */
#define MFRC522_RST 9
#define MFRC522_SDA 10
#define MFRC522_MOSI 11
#define MFRC522_MISO 12
#define MFRC522_SCK 13
// initialize instance
MFRC522 mfrc522(MFRC522_SDA, MFRC522_RST); // card reader instance
// RFID FLAGS
uint8_t successRead; // success read flag
#define BYTE_SIZE 4
byte readCard[BYTE_SIZE]; // read storage
byte entranceCard[BYTE_SIZE];
byte exitCard[BYTE_SIZE];


/**
 * Turn of all LED at the same time
 */
void turnOffAllLeds(){
  digitalWrite(LED_RED,PIN_OFF);
  digitalWrite(LED_GREEN,PIN_OFF);
  digitalWrite(LED_BLUE,PIN_OFF);
}

/**
 * Show RFID Reader Info
 */
void showReaderInfo() {
      // Get the MFRC522 software version
      byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
      Serial.print(F("MFRC522 Software Version: "));
      Serial.print(v, HEX);
      if (v == 0x91){
        Serial.print(F("  -- v1.0"));
      } else if (v == 0x92){
        Serial.print(F("  -- v2.0"));
      } else{
        Serial.print(F("  -- Unknown Version: Maybe a Chinese Clone"));
      }
      Serial.println("");
      // When 0x00 or 0xFF is returned, communication probably failed
      if ((v == 0x00) || (v == 0xFF)) {
        Serial.println(F("ERROR: CANNOT DETECT THE RFID READER"));
        Serial.println(F("INITILIZATION PROCESS WAS CANCELLED: CHECK YOUR CONNECTIONS"));
        turnOffAllLeds();
        while (true){
          digitalWrite(LED_RED,PIN_ON);
          delay(500);
          digitalWrite(LED_RED,PIN_OFF);
          delay(500);
        }
      }
}// end reader info

/**
 * Read RFID and store the value in readCard variable
 */
uint8_t readRFID() {
      // Getting ready for Reading PICCs
      if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
        return 0;
      }
      if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
        return 0;
      }
      // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
      // I think we should assume every PICC as they have 4 byte UID
      // Until we support 7 byte PICCs
      Serial.println(F("Scanned PICC's UID:"));
      for ( uint8_t i = 0; i < BYTE_SIZE; i++) {  //
        readCard[i] = mfrc522.uid.uidByte[i];
        Serial.print(readCard[i], HEX);
      }
      Serial.println("");
      mfrc522.PICC_HaltA(); // Stop reading
      return 1;
}// end rfid read

/**
 * Wait until there is a successful read.
 */
void waitForRead(){
    do {
      successRead = readRFID();
    }
    while (!successRead);
    digitalWrite(LED_BLUE,PIN_ON);
    delay(100);
    digitalWrite(LED_BLUE,PIN_OFF);
    delay(100);   
} // end wait for read

/**
 * Copy values of an array from a source.
 */
void cardCopy(byte *destination, byte *source){
  for (int n=0; n<BYTE_SIZE; n++){
    destination[n] = source[n];
  }
}

/**
 * test if two cards are equal
 */
boolean cardCompare(byte *card1, byte *card2){
  // test each element to be the same. if not, return false
  for (int n=0; n<BYTE_SIZE; n++){
    if (card1[n]!=card2[n]) return false;
  }
  //ok, if we have not returned yet, they are equal :)
  return true; 
}

/**
 * Servo Open Trigger.
 */
void openGate(){
  //--------------------------------------------------------------------------------------------
  //myservo.write(SERVO_OPEN);
  // open slow motion
  for (int pos = SERVO_CLOSE; pos >= SERVO_OPEN; pos--) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  //--------------------------------------------------------------------------------------------
  // delay
  delay(5000);
  //--------------------------------------------------------------------------------------------
  //myservo.write(SERVO_CLOSE);
  // close slow motion
  for (int pos = SERVO_OPEN; pos <= SERVO_CLOSE; pos++) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

/**
 * Setup Arduino
 */
void setup() {
  int x;
  //------------------------------------------------------------------------------------------------------------------------------------
  // initialize connection with the computer
  Serial.begin(9600);
  //------------------------------------------------------------------------------------------------------------------------------------
  Serial.println(F("Parking Simulation"));
  Serial.println(F("BSIT 4A-G1"));
  Serial.println(F("DE LA CRUZ, JOEMAR"));
  Serial.println(F("PANGANIBAN, LYKA LOUIS"));
  Serial.println(F("PERELLO, JHON MELVIN"));
  //------------------------------------------------------------------------------------------------------------------------------------
  // set output pins
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  Serial.println(F("LED PINS are set to output mode . . ."));
  //------------------------------------------------------------------------------------------------------------------------------------
  // boot sequence light all LED
  for(x = 0 ; x < 5; x++){
    turnOffAllLeds();
    delay(300);
    digitalWrite(LED_RED,PIN_ON);
    delay(200);
    digitalWrite(LED_GREEN,PIN_ON);
    delay(200);
    digitalWrite(LED_BLUE,PIN_ON);
    delay(200);
  }
  x = 0;
  Serial.println(F("Starting Program all LED's should be on . . ."));
  //------------------------------------------------------------------------------------------------------------------------------------
  // initialize servo on this pin
  myservo.attach(SERVO_PIN); // attach the servo on this pin
  // start servo as close
  myservo.write(SERVO_CLOSE);
  Serial.println(F("Servo Motor Initialized in close position . . ."));
  //------------------------------------------------------------------------------------------------------------------------------------
  // INITIALIZE RFID
  SPI.begin();           // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
  Serial.println(F("INITIALIZING RFID Reader . . ."));
  //------------------------------------------------------------------------------------------------------------------------------------
  // show card reader information
  showReaderInfo();
  //------------------------------------------------------------------------------------------------------------------------------------
  /**
   * SAVE Entrance Card.
   */
  // get entrance card
  Serial.println(F("Please Scan the ENTRANCE CARD -->"));
  // off all
  turnOffAllLeds();
  // green blue wait for card
  digitalWrite(LED_GREEN,PIN_ON);
  digitalWrite(LED_BLUE,PIN_ON);
  waitForRead();
  cardCopy(entranceCard,readCard);
  turnOffAllLeds();
  digitalWrite(LED_BLUE,PIN_ON);
  // success blink 3x green
  for(x = 0; x< 3 ; x++){
    digitalWrite(LED_GREEN,PIN_ON);
    delay(500);
    digitalWrite(LED_GREEN,PIN_OFF);
    delay(500);
  }
  turnOffAllLeds();
  x = 0;
  Serial.println(F("Entrance Card Capture ^^v"));
  //------------------------------------------------------------------------------------------------------------------------------------
  /**
   * Save Exit Card . ..
   */
  Serial.println(F("Please Scan the EXIT CARD -->"));
  // off all
  turnOffAllLeds();
  // red blue wait for card
  digitalWrite(LED_RED,PIN_ON);
  digitalWrite(LED_BLUE,PIN_ON);
  // check if entrance and exit card are the same
  // wait until a different card is scanned
  do{
      waitForRead();
      cardCopy(exitCard,readCard);
      digitalWrite(LED_BLUE,PIN_ON);
  }while(cardCompare(exitCard,entranceCard));
  // successfully saved exit card
  for(x = 0; x< 3 ; x++){
    digitalWrite(LED_RED,PIN_ON);
    delay(500);
    digitalWrite(LED_RED,PIN_OFF);
    delay(500);
  }
  turnOffAllLeds();
  x = 0;
  Serial.println(F("Exit Card Capture ^^v"));
  
} // end setup

//------------------------------------------------------------------------------------------------------------------------------------
int park_count = 0;
#define MAX_CAPACITY 5

/**
 * what to do on entrance
 */
void onEntrance(){
  // if exceed do nothing
  if(park_count >= MAX_CAPACITY){
    //
    Serial.println(F("Entrance Card Ignored: Parking Space already FULL . . ."));
    return;
  }
  //
  turnOffAllLeds();
  digitalWrite(LED_GREEN,PIN_ON);
  digitalWrite(LED_BLUE,PIN_ON);
  openGate();
  turnOffAllLeds();
  // add car
  park_count++;
}

/**
 * What to do when exit.
 */
void onExit(){
  // if empty do nothing
  if(park_count <= 0){
    //
    Serial.println(F("Exit Card Ignored: Parking Space already EMPTY . . ."));
    return;
  }
  //
  turnOffAllLeds();
  digitalWrite(LED_RED,PIN_ON);
  digitalWrite(LED_BLUE,PIN_ON);
  openGate();
  turnOffAllLeds();
  // add car
  park_count--;
}

void loop() {
  turnOffAllLeds();
  if(park_count <= 0){
    digitalWrite(LED_RED,PIN_ON);
  }else if(park_count >= MAX_CAPACITY){
    digitalWrite(LED_GREEN,PIN_ON);
  }else{
    // do nothing
  }

  // put your main code here, to run repeatedly:
  Serial.println(F("-------------------------"));
  Serial.println(F("Waiting for a card to be read . . ."));
  waitForRead();
  if(cardCompare(exitCard,readCard)){
    Serial.println(F("Exit Card Detected"));
    // DO SOMETHING WHEN EXIT
    onExit();
  }else if(cardCompare(entranceCard,readCard)){
    Serial.println(F("Entrance Card Detected"));
    // DO SOMETHING WHEN ENTER
    onEntrance();
  }else{
    Serial.println(F("UNREGISTERED CARD"));
  }
  Serial.println(F("A card was read . . ."));
  Serial.println(F("-------------------------"));
  Serial.print(F("Current Count: "));
  Serial.println(park_count);
  Serial.println(F("-------------------------"));
}
