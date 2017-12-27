
// Library Files
// #include <EEPROM.h>     // Saving the data to the Arduino Memory // not going to use just use a predefined card numbers
#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>  // Library for Mifare RC522 Devices -> https://github.com/miguelbalboa/rfid
#include <Servo.h>  // Servo Library

// Common Cathod Setup
#define PIN_ON HIGH
#define PIN_OFF LOW
// Commonly used for serial communications with the PC
// PIN 0 (RX)
// PIN 1 (TX)
// Start using PIN 2
// For Visuals or System Outputs
#define BLUE_LED 2 // for for programming mode
#define GREEN_LED 3 // success read of card and Not Full Parking Space
#define RED_LED 4 // error reading card and Full Parking Space
#define BUZZER 5 // when the card is not read
// PIN 7 and 8 are vacant
// For Servo
#define SERVO_PIN 6 // controls the servo
// For MFRC522
#define MFRC522_RST 9
#define MFRC522_SDA 10
#define MFRC522_MOSI 11
#define MFRC522_MISO 12
#define MFRC522_SCK 13

//Defining Visuals
// BLUE indicates programming mode.
// BLUE WITH GREEN asking to set the entrance card
// BLUE WITH RED asking to set the exit card
// GREEN - card accepted rotate servo motor for entrance (Buzz 1x)
// RED - card accepted rotate servo for exit (Buzz 1x)
// RED BUZZER - card read failed try to scan code again (Buzz 3x)

//---------------------------------------------------------------------------------------------------------
// VISUALS SEGMENT
//---------------------------------------------------------------------------------------------------------

/**
 * Turn off all visuals.
 */
void offVisuals(){
  digitalWrite(GREEN_LED, PIN_OFF);
  digitalWrite(BLUE_LED, PIN_OFF);
  digitalWrite(RED_LED, PIN_OFF);
}

/**
 * buzz for 0.5 seconds
 */
void buzz(){
  // make sure its off
  digitalWrite(BUZZER, PIN_OFF);
  digitalWrite(BUZZER, PIN_ON);
  delay(250);
  digitalWrite(BUZZER, PIN_OFF);
}

//---------------------------------------------------------------------------------------------------------
// RFID READER SEGMENT
//---------------------------------------------------------------------------------------------------------

MFRC522 mfrc522(MFRC522_SDA, MFRC522_RST); // card reader instance
uint8_t successRead; // success read flag
byte readCard[4]; // read storage

/**
 * Initializes the RFID Reader
 */
void initializeRFID(){
      //Protocol Configuration
      Serial.begin(9600);  // Initialize serial communications with PC
      SPI.begin();           // MFRC522 Hardware uses SPI protocol
      mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
    
      //If you set Antenna Gain to Max it will increase reading distance
      //mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
    
      Serial.println(F("Access Control Example v0.1"));   // For debugging purposes
} // end initialize

/**
 * Check the card reader details if the reader was not recognizable beep forever.
 */
void showReaderInfo() {
      // Get the MFRC522 software version
      byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
      Serial.print(F("MFRC522 Software Version: 0x"));
      Serial.print(v, HEX);
      if (v == 0x91){
        Serial.print(F(" = v1.0"));
      } else if (v == 0x92){
        Serial.print(F(" = v2.0"));
      } else{
        Serial.print(F(" (unknown),probably a chinese clone?"));
      }
      Serial.println("");
      // When 0x00 or 0xFF is returned, communication probably failed
      if ((v == 0x00) || (v == 0xFF)) {
        Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
        Serial.println(F("SYSTEM HALTED: Check connections."));
        // Visualize system is halted
        offVisuals();
        digitalWrite(RED_LED, PIN_ON);   // Turn on red LED
        // make the program in forever loop
        while (true){
          buzz();
        }
      }
}// end reader info

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
      for ( uint8_t i = 0; i < 4; i++) {  //
        readCard[i] = mfrc522.uid.uidByte[i];
        Serial.print(readCard[i], HEX);
      }
      Serial.println("");
      mfrc522.PICC_HaltA(); // Stop reading
      return 1;
}// end rfid read

/**
 * loop until there is a successful read
 */
void waitForRead(){
    do {
      successRead = readRFID();
    }
    while (!successRead);   
} // end wait for read

//---------------------------------------------------------------------------------------------------------
// SERVO
//---------------------------------------------------------------------------------------------------------
Servo myservo; // servo motor instance
int servoPosition; // the position of the servo motor
#define SERVO_LEFT 0
#define SERVO_TOP 90
#define SERVO_RIGHT 180

/**
 * Creates the servo motor instance
 */
void attachServo(){
  myservo.attach(SERVO_PIN); // attach the servo on this pin
  servoPosition = 0;
  // make the servo position default as close 180
  myservo.write(SERVO_RIGHT);
}

//---------------------------------------------------------------------------------------------------------
// SETUP
//---------------------------------------------------------------------------------------------------------
byte entranceCard[4];
byte exitCard[4];
/**
 * Sets the entrance and exit card
 */
void setupEntranceExitCard(){
  // get entrance
  offVisuals(); // turn off visuals
  digitalWrite(BLUE_LED, PIN_ON); // light up blue for programming
  digitalWrite(GREEN_LED, PIN_ON); // green to ask for the entrance card
  waitForRead(); // wait until there is a successfuly read
  entranceCard[4] = readCard; // save the entrance card
  buzz();
  // get exit
  offVisuals(); // turn off visuals
  digitalWrite(BLUE_LED, PIN_ON); // light up blue for programming
  digitalWrite(RED_LED, PIN_ON); // green to ask for the entrance card
  waitForRead(); // wait until there is a successfuly read
  exitCard[4] = readCard; // save the entrance card
  buzz();
  offVisuals(); // turn off visuals
}

void setup() {
  //--------------------------
  /**
   * Define the following pins as output mode
   */
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  //--------------------------

  /**
   * Initializes the RFID reader and all components.
   */
  initializeRFID(); // initialize RFID
  /**
   * Displays the RFID reader information. if there was an error with the connction it will
   * run the buzzer forever.
   */
  showReaderInfo(); // show reader information
  /**
   * Attaches the servo motor to the arduino board.
   */
  attachServo(); // initialize servo
  /**
   * Lights up the BLUE led for programming mode that inidicates the system needs the entrance
   * and exit cards.
   * 
   * BLUE with GREEN scan the card for entrance
   * BLUE with RED scan the card for exit
   */
  setupEntranceExitCard(); // save card values
  
} // end setup

//---------------------------------------------------------------------------------------------------------
// LOOP
//---------------------------------------------------------------------------------------------------------
void loop() {
  // put your main code here, to run repeatedly:

} // end loop
