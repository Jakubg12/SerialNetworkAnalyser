/*
   ASSP - Library for Aberystwyth Software Serial Shield
   Author Neal Snooke (nns@aber.ac.uk)

   This class implements a 'reliable' protocol for software serial on multiple ports of
   a single Arduino.

     A 'Clear to send' - 'Ready to send' message exchange protocol is used to ensure
     that a message will eventually be successfully sent/received, despite the fact
     that an arduino can only receive on a single software serial port at a time,
     and data arriving on any port that is not actively listening will not be seen,
     because there is NO buffering.

     Clearly both sender and receiver must be using this library and frequently
     calling the "fetchASSPMessage" function.

     If a receiver does not call fetchASSPMessage frequently enough then the receiver
     will get a false result from the sendASSPMessage function. The fetchASSPMessage
     function will repeatedly check all ports for incoming messages if no port is
     specified. The sendASSPMessage function requires a port to be specified.
*/

/*
 *   NOTE: For Nano compilation, #define NANO_DAUGHTER_BOARD in ASSP.h
 */

/* HISTORY
 * 
 * 2015-11-20   V1.0    NNS Initial version
 * 2020-11-09   V1.1    LGT Fixed null pointer bug
 *                          Tidied & unified code
 */

#include "Arduino.h"
#include "ASSP.h"
#include <SoftwareSerial.h>

#define LED_RED 3
#define LED_YELLOW 5

/*
   constructor
*/

ASSP::ASSP(int baud) {
#ifdef PRINT_FUNCTION_ENTRY
  Serial.println(F("Start constructor ASSP "));
#endif

  if (baud > 0) {
    start(baud);
  }

  setMessageContent("");
  setMessageSender("");
  setMessageDestination("");

#ifdef PRINT_FUNCTION_ENTRY
  Serial.println(F("End constructor ASSP "));
#endif
}

void ASSP::start(int baud) {
  // set the data rate for the SoftwareSerial port
  for (int i = 0; i < PORT_COUNT; i++) {
    // set the data rate for the SoftwareSerial port
    serialConnections[i].begin(baud);

    //serialConnections[i].listen();
    //while (serialConnections[i].available()){
    //  serialConnections[i].read();
    //}
  }
}

/*

*/
void ASSP::setMessageContent(const char * content) {
#ifdef DEBUG_ASSP
  Serial.println(F("ASSP Set Message Content(c*)"));
  Serial.println(content);
#endif
  strcpy(ASSPContent, content);
}

/*
*/
void ASSP::setMessageContent(const String & content) {
#ifdef DEBUG_ASSP
  Serial.println(F("ASSP Set Message Content(S)"));
  Serial.println(content);
#endif
  content.toCharArray(ASSPContent, ASSPMAXMESSAGELENGTH - ASSPMAXNAMELENGTH * 2 - 4);
}

/*

*/
void ASSP::setMessageSender(const char * sender) {
#ifdef DEBUG_ASSP
  Serial.println(F("ASSP Set Message Sender(c*)"));
  Serial.println(sender);
#endif
  strcpy(ASSPSender, sender);
}


/*

*/
void ASSP::setMessageSender(const String & sender) {
#ifdef DEBUG_ASSP
  Serial.println(F("ASSP Set Message Sender(S)"));
  Serial.println(sender);
#endif
  sender.toCharArray(ASSPSender, ASSPMAXNAMELENGTH);
}

/*

*/
void ASSP::setMessageDestination(const char * destination) {
#ifdef DEBUG_ASSP
  Serial.println(F("ASSP Set Message Dest(c*)"));
  Serial.println(destination);
#endif
  strcpy(ASSPDestination, destination);
}

/*
*/
void ASSP::setMessageDestination(const String & destination) {
#ifdef DEBUG_ASSP
  Serial.println(F("ASSP Set Message Dest(S)"));
  Serial.println(destination);
#endif
  destination.toCharArray(ASSPDestination, ASSPMAXNAMELENGTH);
}

/*

*/
char * ASSP::getMessageContent() {
  return ASSPContent;
}

/*

*/
char * ASSP::getMessageSender() {
  return ASSPSender;
}

/*

*/
char * ASSP::getMessageDestination() {
  return ASSPDestination;
}

/*
   create a complete message from its parts
   individual parts can be empty strings if required
   returns a reference to the result stored globally in ASSPMessage
*/
void ASSP::ASSPCreateUserMessage() {
#ifdef PRINT_FUNCTION_ENTRY
  Serial.println(F("Start ASSPCreateUserMessage"));
#endif

#ifdef DEBUG_ASSP
  Serial.println(F("ASSPCreateUserMessage"));
  Serial.println(ASSPContent);
  Serial.println(ASSPDestination);
  Serial.println(ASSPSender);
#endif

  strcpy(ASSPMessage, "MSG,");
  strcat(ASSPMessage, ASSPContent);
  strcat(ASSPMessage, ",");
  strcat(ASSPMessage, ASSPDestination);
  strcat(ASSPMessage, ",");
  strcat(ASSPMessage, ASSPSender);

#ifdef DEBUG_ASSP
  Serial.print(F("message created "));
  Serial.println(ASSPMessage);
#endif
#ifdef PRINT_FUNCTION_ENTRY
  Serial.println(F("End ASSPCreateUserMessage"));
#endif
}

/*
   split the message string into its parts
   these are accessed as global strings called
   ASSPType
   ASSPMessage
   ASSPContent
   ASSPSender
   ASSPDestination
*/
void ASSP::ASSPExtractMessage() {
#ifdef DEBUG_ASSP
  Serial.print(F("ASSPExtractMessage"));
  Serial.println(ASSPMessage);
#endif

  //clear the result strings
  //ASSPType[0] = '\0';
  ASSPContent[0] = '\0';
  ASSPSender[0] = '\0';
  ASSPDestination[0] = '\0';

  char* next = ASSPMessage; //walk string

  // use a string tokenizer with the comma as a delimeter
  char * pch;
  pch = strsep (&next, ",");
  //strcpy(ASSPType, pch);

  if (pch != NULL) {
    pch = strsep (&next, ",");
    strcpy(ASSPContent, pch);
  }

  if (pch != NULL) {
    pch = strsep (&next, ",");
    strcpy(ASSPDestination, pch);
  }

  if (pch != NULL) {
    pch = strsep (&next, ",");
    strcpy(ASSPSender, pch);
  }

#ifdef DEBUG_ASSP
  //Serial.print(F("Type: "));
  //Serial.println(ASSPType);
  Serial.print(F("Content: "));
  Serial.println(ASSPContent);
  Serial.print(F("Destination: "));
  Serial.println(ASSPDestination);
  Serial.print(F("Sender: "));
  Serial.println(ASSPSender);
  Serial.println();
#endif
}

/*
   make a CTS message type
*/
void ASSP::ASSPCreateCTS() {
  strcpy(ASSPMessage, "CTS");
}


/*
   make an RTS message type
*/
void ASSP::ASSPCreateRTS() {
  strcpy(ASSPMessage, "RTS");
}

/*
   test if the string is a message type
*/
boolean ASSP::isUserMessage() {
  return strncmp(ASSPMessage, "MSG", 3) == 0;
}

/*
   test if the string is a CTS message
*/
boolean ASSP::isClearToSend() {
  //Serial.println(F("isClearToSend"));
  return strncmp(ASSPMessage, "CTS", 3) == 0;
}

/*
   test if the string is a RTS message
*/
boolean ASSP::isReadyToSend() {
  //Serial.println(F("isReadyToSend"));
  return strncmp(ASSPMessage, "RTS", 3) == 0;
}

/**
   fetch a message from any port
   port numbers 1-4
*/
boolean ASSP::fetchASSPMessage() {
#ifdef PRINT_FUNCTION_ENTRY
  Serial.print(F("Start fetchASSPMessage all ports: "));
#endif

  if (showASSPmessages) {
#ifndef NANO_DAUGHTER_BOARD
    Serial.println(F("Fetching message from all ports: "));
#endif
  }

  ASSPMessage[0] = '\0';
  ASSPContent[0] =  '\0';
  ASSPDestination[0] =  '\0';
  ASSPSender[0] =  '\0';

  for (int portNumber = 1; portNumber <= PORT_COUNT; portNumber++) {
    changePort(portNumber);

    if (fetchUserMessage(serialConnections[currentListeningPort], _timeoutLoops)) {
      ASSPExtractMessage();
      ASSPPort = portNumber;
      return true;
    } else {
      // copy any message that might have been received to content (as a raw message)
      setMessageContent(ASSPMessage);
    }

  }

#ifdef PRINT_FUNCTION_ENTRY
  Serial.println(F("End fetchASSPMessage"));
#endif
  return false;
}


/*
 *
 */
boolean ASSP::fetchUserMessage(SoftwareSerial& port, byte attempts) {
#ifdef PRINT_FUNCTION_ENTRY
  Serial.println(F("Start fetchUserMessage"));
#endif

  if (checkSoftwareSerial(port)) {
#ifdef UNO_SHIELD
    digitalWrite(LED_YELLOW, HIGH);
#endif

    if (isReadyToSend()) {

      if (showASSPmessages) {
        Serial.println(F("Received RTS message"));
      }
      // need to reply with a Clear To Send
      ASSPCreateCTS();
      port.println(ASSPMessage);

      if (showASSPmessages) {
        Serial.println(F("Sent CTS message"));
      }

#ifdef DEBUG_ASSP
      Serial.println(ASSPMessage);
#endif

      while (attempts > 0) {
        //Serial.println("waiting for message");

        if (checkSoftwareSerial(port)) {

          if (isUserMessage()) {
            // tell us what is happening via the terminal

            if (showASSPmessages) {
              Serial.print(F("Got message: "));
              Serial.println(ASSPMessage);
            }

#ifdef UNO_SHIELD
            digitalWrite(LED_YELLOW, LOW);
#endif
            return true;
          } else {
            // send another CTS in case it was corrupted
            ASSPCreateCTS();
            port.println(ASSPMessage);

            if (showASSPmessages) {
              Serial.println(F("Sent replacement CTS message"));
            }
          }
        } // while

        delay(50); //wait for a possible response
        attempts--;
      }
    }

#ifdef UNO_SHIELD
    digitalWrite(LED_YELLOW, LOW);
#endif

    return false;
  }
  else {
    return false;
  }

#ifdef PRINT_FUNCTION_ENTRY
  Serial.println("End fetchASSPMessage");
#endif
}

/**
   port numbers 1-4
   port 0 sent to all
*/
boolean ASSP::sendASSPMessage(int port) {
  boolean success = false;

#ifdef PRINT_FUNCTION_ENTRY
  Serial.print(F("Start sendASSPMessage  port: "));
  Serial.println(port);
#endif

  if (showASSPmessages) {
    Serial.print(F("Sending message to port: "));
    Serial.println(port);
  }

  if (port > 0 && port <= 4) {
    changePort(port); //must listen because we have a RTS/CTS exchange...

    ASSPCreateUserMessage();

    if (showASSPmessages) {
      Serial.print(F("Message is: "));
      Serial.println(ASSPMessage);
    }

    success = sendUserMessage(serialConnections[currentListeningPort], _timeoutLoops);
  }

#ifdef PRINT_FUNCTION_ENTRY
  Serial.println(F("End sendASSPMessage"));
#endif
  return success;
}

/*
   to send a string to a port
   NOT using the ASSP protocol
*/
void ASSP::sendRawMessage(int portNumber) {
#ifdef UNO_SHIELD
  digitalWrite(LED_RED, HIGH);
#endif

  serialConnections[portNumber - 1].println(ASSPContent);

#ifdef UNO_SHIELD
  digitalWrite(LED_RED, LOW);
#endif
}

/*
   send message to a software serial port
   requires that we wait until the port is listening
   10 tries at 50mS gaps then give up
*/
boolean ASSP::sendUserMessage(SoftwareSerial& port, byte attempts) {

#ifdef PRINT_FUNCTION_ENTRY
  Serial.println(F("Start sendUserMessage"));
  Serial.println(ASSPMessage);
#endif

  digitalWrite(LED_RED, HIGH);

  boolean notSent = true;
  while (notSent && attempts > 0) {

    // send RTS
    ASSPCreateRTS();
    port.println(ASSPMessage);

    if (showASSPmessages) {
      Serial.println(F("Sent RTS message"));
    }

#ifdef DEBUG_ASSP
    Serial.print(F("sendUserMessage sent: "));
    Serial.println(ASSPMessage);
#endif

    delay(50); //wait for a possible response

    if (checkSoftwareSerial(port)) {

#ifdef DEBUG_ASSP
      Serial.print(F("sendUserMessage received: "));
      Serial.println(ASSPMessage);
#endif

#ifdef PRINT_RX_CHARS
      printStringCharacterCodes(ASSPMessage);
#endif
      if (isClearToSend()) {

        if (showASSPmessages) {
          Serial.println(F("CTS response message received"));
        }

        delay(50); //wait for buffered messages at receiver to be processed

        // creates ASSPMessage
        ASSPCreateUserMessage();
        port.println(ASSPMessage);

        if (showASSPmessages) {
          Serial.print(F("Message sent: "));
          Serial.println(ASSPMessage);
        }

        notSent = false;
      } else {
        if (showASSPmessages) {
          Serial.print(F("Unexpected message received: "));
          Serial.println(ASSPMessage);
        }
      }

      // deal with an RTS is received instead of a CTS?
      if (isReadyToSend()) {

        // tell us what is happening via the terminal
        Serial.println(F("\nOh No - we are sending to each other !!: "));
        Serial.println(F("i.e. Recieved RTS when sending RTS"));

        blinkTXRXLeds(500, 40);
      }
    }

    attempts--;
  }

  if (attempts == 0) {
    if (showASSPmessages) {
      Serial.println(F("TIMEOUT: no response to RTS. Message not sent"));
    }
  }

  digitalWrite(LED_RED, LOW);

#ifdef PRINT_FUNCTION_ENTRY
  Serial.println("End sendUserMessage");
#endif
  return !notSent;
}

#ifdef NANO_DAUGHTER_BOARD
/**
   fetch a message from the required port
   port numbers 1-4
*/
boolean ASSP:: fetchASSPMessage(int portNumber) {
#ifdef PRINT_FUNCTION_ENTRY
  Serial.print(F("Start fetchASSPMessage port: "));
  Serial.println(portNumber);
#endif

  if (showASSPmessages) {
    Serial.print(F("Fetching message from port: "));
    Serial.println(portNumber);
  }

  ASSPMessage[0] = '\0';
  changePort(portNumber);

  boolean result = fetchUserMessage(serialConnections[currentListeningPort], _timeoutLoops);

  if (result) {
    ASSPExtractMessage();
  } else {
    // copy any message that might have been received to content (as a raw message)
    setMessageContent(ASSPMessage);
  }

#ifdef PRINT_FUNCTION_ENTRY
  Serial.println(F("End fetchASSPMessage"));
#endif
  return result;
}
#endif

/*
   simply check software serial port for a string
   finishing with /0
*/
boolean ASSP::fetchRawMessage(int port) {
#ifdef UNO_SHIELD
  digitalWrite(LED_YELLOW, HIGH);
#endif

  changePort(port);
  setMessageContent("");
  boolean result = checkSoftwareSerial(serialConnections[currentListeningPort]);

  if (result) {
    //strcpy(ASSPContent, ASSPMessage);
    setMessageContent(ASSPMessage);
    setMessageSender("");
    setMessageDestination("");
    ASSPPort = port;
  }
#ifdef UNO_SHIELD
  digitalWrite(LED_YELLOW, LOW);
#endif
  return result;
}

/**
   function to check for software serial input
   returns true when a complete line of input is available in the global serialBuffer
   returns false if no data was available
*/
boolean ASSP::checkSoftwareSerial(SoftwareSerial & port) {
  // empty the string buffer
  byte sbposition = 0;

  // is there any data available?
  if (port.available()) {
    // read the character and append the character to the buffer
    char nextCharacter = port.read();

    // read all the characters up to nextline
    long int time = millis();

#ifdef PRINT_RX_CHARS
    Serial.print(nextCharacter);
    Serial.print(F(" -- "));
    Serial.println(nextCharacter, DEC);
#endif

    while (nextCharacter != '\n') {
      // is there a character available yet? (they might arrive quite slowly)
      if (port.available()) {

        if  (nextCharacter != 13) {
          ASSPMessage[sbposition] = nextCharacter;
          sbposition++;
          //Serial.println(softwareSerialBuffer);

#ifdef PRINT_RX_CHARS
          Serial.print("stored ");
          Serial.print(nextCharacter);
          Serial.print(F(" "));
          Serial.println(nextCharacter, DEC);
#endif

        }

        // read the character
        nextCharacter = port.read();

#ifdef PRINT_RX_CHARS
        Serial.print(nextCharacter);
        Serial.print(F(" "));
        Serial.println(nextCharacter, DEC);
#endif

      }

      // timeout
      if (millis() - time > 1000) {
        ASSPMessage[sbposition] = '\0';

        if (showASSPmessages) {
          Serial.println(F("Incomplete message - no newline received (connected to unpowered Arduino?)"));
        }

        return false;
      }
    }

    ASSPMessage[sbposition] = '\0';

#ifdef DEBUG_ASSP
    Serial.print(F("checkSoftwareSerial received: "));
    Serial.println(ASSPMessage);
#endif

    // signal that a string was read
    return true;
  }

  // signal that no string was available to read
  //softwareSerialBuffer = "";
  return false;
}

/*
   change the active port
*/
void ASSP::changePort(int port) {
  if (currentListeningPort != (port - 1)) {

#ifdef DEBUG_ASSP
    Serial.print(F("Changing Port:"));
    Serial.println(port);
#endif

    currentListeningPort = port - 1;
    serialConnections[currentListeningPort].listen();
    ASSPMessage[0] = '\0'; //clear recieve buffer

    delay(20); // wait for something to arrive
  }
}

int ASSP::getMessagePort() {
  return ASSPPort;
}

/**
   Blinks the TX/ RX  (red yellow LEDS in sequence
*/
void ASSP::blinkTXRXLeds(int flashes, int theDelay) {
  //Serial.println("Blink TX/RX LEDS... ");

  for (int flashcount = 0; flashcount < flashes; flashcount++) {
#ifdef UNO_SHIELD

    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_YELLOW, HIGH);

    delay(theDelay);

    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);

    delay(theDelay);
#endif
  }
}

/*

*/
void ASSP::printStringCharacterCodes(const String & message) {
  for (int i = 0; i < message.length(); i++) {
    Serial.print(message.charAt(i));
    Serial.print(":");
    Serial.println(message.charAt(i), DEC);
  }
}
/*
   utility function to tell the Nano to change mode
*/
void ASSP::setEmuMode(int mode) {
  pinMode(EMU, OUTPUT);
  digitalWrite(EMU, HIGH);
  delay(1000);
  digitalWrite(EMU, LOW);
  delay(100);
  digitalWrite(EMU, HIGH);
  delay(mode * 100);
  digitalWrite(EMU, LOW);

  // to prevent too rapid signals
  delay (800);
}

ASSP SerialShield(4800);
