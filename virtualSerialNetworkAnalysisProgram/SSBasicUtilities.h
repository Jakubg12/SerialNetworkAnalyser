/*
 * Some useful functions to read from the serial and software serial ports
 * 
 * This file provides the following functions, read the function comments for more information:
 * boolean checkSerial();
 * void printStringCharacterCodes(String message)
 * 
 * The following global variable is defined and contains the data read from the checkSerial function
 * char serialBuffer[71];
 * 
 */
#include <SoftwareSerial.h>

// diagnostic to print all aharacters arriving from the serial ports
//#define PRINT_RX_CHARS
//#define DEBUG_SSUTILITIES //do not delete
//#define PRINT_FUNCTION_ENTRY //do not delete

// globals variables used by the serial port functions
char serialBuffer[71];

/**
 * function to check for serial input from the terminal
 * returns true when a complete line of input is available in the global serialBuffer
 * returns false if no data was available
 */
boolean checkSerial(){
  // empty the string buffer
  byte sbposition = 0;

  // is there any data available?
  if (Serial.available()){
    // read the character and append the character to the buffer
    char nextCharacter = Serial.read();

#ifdef PRINT_RX_CHARS
    Serial.print(nextCharacter);   
    Serial.print(F(" -- "));
    Serial.println(nextCharacter,DEC);
#endif

    while (nextCharacter != '\n'){
      // is there a character available yet? (they might arrive quite slowly)
      if (Serial.available()){

        if  (nextCharacter != 13){
          serialBuffer[sbposition] = nextCharacter;
          sbposition++;
          //Serial.println(softwareSerialBuffer);

#ifdef PRINT_RX_CHARS
          Serial.print(F("stored "));
          Serial.print(nextCharacter);   
          Serial.print(F(" "));
          Serial.println(nextCharacter,DEC);
#endif

        }

        // read the character
        nextCharacter = Serial.read();

#ifdef PRINT_RX_CHARS
        Serial.print(nextCharacter);
        Serial.print(F(" "));
        Serial.println(nextCharacter,DEC);
#endif

      }
    }

    serialBuffer[sbposition] = '\0';

#ifdef DEBUG_SSUTILITIES
    Serial.print(F("Received: "));
    Serial.println(serialBuffer);
#endif

    // signal that a string was read
    return true;
  }

  // signal that no string was available to read
  return false;
} 

/*
 *
 */
void printStringCharacterCodes(String message){
  for (int i=0; i<message.length(); i++){
    Serial.print(i);
    Serial.print(":");
    Serial.println(message.charAt(i), DEC);
  }
}
