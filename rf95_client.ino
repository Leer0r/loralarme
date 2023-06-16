// rf95_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_client
// Tested with Anarduino MiniWirelessLoRa



#include <SoftwareSerial.h>
#include <RH_RF95.h>
#include <rgb_lcd.h>

#define MAX_USER 2

#define HELP 0
#define RECIEVE 1
uint8_t actions[2] = {
  0,
  1
};

String users[MAX_USER] = {
  "Lilian",
  "JB"
};

uint8_t usersRecieve[MAX_USER] = {
  0,
  0
};

uint8_t user = 0;
uint8_t helpMessage = 0;

// Singleton instance of the radio driver
SoftwareSerial ss(5, 6);
RH_RF95 rf95(ss);
int buttonState = LOW;
int buttonPin = 4; // Broche du bouton (D4)
int isButtonPressed = 0;
int buttonPress = 0;
int memoryButton = 1;
unsigned long buttonHoldTime = 1000; // Temps en millisecondes pour réinitialiser le compteur
unsigned long buttonPressStartTime; // Heure de début du maintien du bouton
uint8_t lastMessageId = 0;

int led = 13;

rgb_lcd lcd; // initialize LCD library
int H = 2;
int W = 16;

int writeLine(String message, int line)
{
  if (line >= H)
  {
    return 1;
  }
  lcd.setCursor(0, line);
  lcd.print("                ");
  lcd.setCursor(0, line);
  lcd.print(message);
  return 0;
}

int writeNumber(int number, int line){
  if (line >= H)
  {
    return 1;
  }
  lcd.setCursor(0, line);
  lcd.print("                ");
  lcd.setCursor(0, line);
  lcd.print(number);
  return 0;
}

void resetUserList(uint8_t* userList){
  for(int i = 0; i < MAX_USER; i++){
    userList[i] = 0;
  }
}

void buttonPressedRemotely(){
  
  buttonPressed();
}

void buttonPressed(){
  buttonPress++;
  Serial.println("Button pressed !");
}

void resetButton(){
  buttonPress = 0;
  Serial.println("Reset !");
}

void sendMessage(uint8_t messageId,uint8_t action,uint8_t remoteUser){
      uint8_t data[3] = {
      messageId,
      action,
      remoteUser
    };
    rf95.send(data, sizeof(data));
   
    rf95.waitPacketSent();
}

void sendPressMessage() {
  Serial.println("Sending press to rf95_server");
    // Send a message to rf95_server
    lastMessageId += 1;
    uint8_t data[3] = {
      lastMessageId,
      HELP,
      user
    };
    rf95.send(data, sizeof(data));
   
    rf95.waitPacketSent();
}

void listenLoRa(){
  // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if(rf95.recv(buf, &len))
    {

        uint8_t messageId = buf[0];
        uint8_t action = buf[1];
        uint8_t remoteUser = buf[2];
        if(action == HELP){
          if(messageId == lastMessageId){
            return;
          }
          char message[16];
          Serial.println("RECIEVE HELP");
          writeLine("HELP: " + users[remoteUser], 1);
          helpMessage = 1;
          lastMessageId = messageId;
        }
        uint8_t data[] = "ok";
        rf95.send(data, sizeof(data));
        rf95.waitPacketSent();
    }
}

void setup()
{
    Serial.begin(9600);
    Serial.println("RF95 server test.");
    lcd.begin(16, 2); // Initialise l'écran LCD
    lcd.setRGB(0, 0, 255); // Définit la couleur de rétroéclairage (bleu dans cet exemple)

    pinMode(buttonPin, INPUT);
      
    if(!rf95.init())
    {
        Serial.println("init failed");
        while(1);
    }
    Serial.println("RF95 server enable.");
    writeLine("Alarm enabled", 0);
    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
    // you can set transmitter powers from 5 to 23 dBm:
    //rf95.setTxPower(13, false);
    
    rf95.setFrequency(534.0);
}

void loop()
{
  if(rf95.available())
  {
    listenLoRa();
  }

  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    isButtonPressed = 1;
  } else {
    if(isButtonPressed == 1){
      buttonPressed();
      sendPressMessage();
      isButtonPressed = 0;
    }
  }
}


