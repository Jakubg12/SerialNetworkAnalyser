#ifndef ASSP_h
#define ASSP_h

/* 2020-11-09   V1.1 */

// select the software serial board to use (pins differ)
// if NANO_DAUGHTER_BOARD is not defined, assume Arduino UNO

//#define NANO_DAUGHTER_BOARD

#ifndef NANO_DAUGHTER_BOARD
#ifndef UNO_SHIELD
#define UNO_SHIELD
#endif
#endif

#define EMU A1 //pin for emulator mode signal

//#define DEBUG_ASSP //do not delete
//#define PRINT_FUNCTION_ENTRY //do not delete
//#define LOCAL_ECHO //do not delete. echo hardware terminal input back at the character level
// diagnostic to print all aharacters arriving from the software serial ports
//#define PRINT_RX_CHARS

#include "Arduino.h"
#include <SoftwareSerial.h>

class ASSP
{
  public:

    //set flag to show the ASSP protocol messages
    boolean showASSPmessages = false;
    
    // constructor
    ASSP(int baud);
    void start(int baud);

    //returns true if message received
    boolean fetchASSPMessage();
#ifdef NANO_DAUGHTER_BOARD
    boolean fetchASSPMessage(int port);
#endif
    boolean fetchRawMessage(int port);

    //returns true if sent successfully
    boolean sendASSPMessage(int port);
    void sendRawMessage(int port);

    // set very approximate timeout in milliseconds (
    void giveUpAfter(int);

    char * getMessageContent();
    char * getMessageSender();
    char * getMessageDestination();
    int getMessagePort();

    void setMessageContent(const char *);
    void setMessageContent(const String &);
    void setMessageSender(const char *);
    void setMessageSender(const String &);
    void setMessageDestination(const char *);
    void setMessageDestination(const String &);

    void setEmuMode(int); //set emulator mode for Nano

  private:
    int currentListeningPort = -1; //the port that is listening
    int _timeoutLoops = 50; //about 20*50mS
    void init(int baud);

    // software serial port definitions
    // the software serial port definitions

#ifdef NANO_DAUGHTER_BOARD
    static const int PORT_COUNT = 4;
    SoftwareSerial serialConnections[4] = {
      SoftwareSerial(8, 9), // RX pin 8, TX pin 9 --> RX A3, TX A2 (SV4 on sch, PORT 1 on board)
      SoftwareSerial(6, 7), // RX pin 6, TX pin 7 --> RX 13, TX 12 (SV3 on sch, PORT 2 on board)
      SoftwareSerial(4, 5), // RX pin 4, TX pin 5 --> RX 8, TX 7 (SV2 on sch, PORT 3 on board)
      SoftwareSerial(2, 3), // RX pin 2, TX pin 3 --> RX 4, TX 2 (SV1 on sch, PORT 4 on board)
    };
#endif
    
#ifdef UNO_SHIELD
    static const int PORT_COUNT = 4;
    SoftwareSerial serialConnections[PORT_COUNT] = {
      SoftwareSerial(A3, A2), // RX pin , TX pin 
      SoftwareSerial(13, 12),
      SoftwareSerial(8, 7), 
      SoftwareSerial(4, 2), 
    };
#endif

    // ASSP data
#define ASSPMAXMESSAGELENGTH 70 // max size content part of message including source data etc
#define ASSPMAXNAMELENGTH 10

    //the message parts
    char ASSPContent[ASSPMAXMESSAGELENGTH - ASSPMAXNAMELENGTH * 2 - 4];
    char ASSPDestination[ASSPMAXNAMELENGTH];
    char ASSPSender[ASSPMAXNAMELENGTH];
    int ASSPPort; // the port a message arrived on
    char ASSPMessage[ASSPMAXMESSAGELENGTH];     // the whole message

    void ASSPCreateCTS();
    void ASSPCreateRTS();
    void ASSPCreateUserMessage();

    boolean isUserMessage();
    boolean isClearToSend();
    boolean isReadyToSend();

    void ASSPExtractMessage();
    boolean sendUserMessage(SoftwareSerial&, byte);
    boolean fetchUserMessage(SoftwareSerial& port, byte attempts);
    boolean checkSoftwareSerial(SoftwareSerial &);
    void blinkTXRXLeds(int flashes, int theDelay);
    void changePort(int port);
    void printStringCharacterCodes(const String & message);
};

extern ASSP SerialShield;

#endif
