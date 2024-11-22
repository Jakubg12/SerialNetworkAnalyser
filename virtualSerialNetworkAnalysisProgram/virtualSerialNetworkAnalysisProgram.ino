#include "SSBasicUtilities.h"
#include "ASSP.h"
//#define DEBUG /**runs print statements showing each action being performed*/
#define NUMNAMES 26             /**preprocessor for number of devices on network*/
unsigned long timeStrt;         /**holds time since boot at successful ASSPsend operation (in milliseconds), used in getTimeToDevice()*/
unsigned long timeFin;          /**holds time since boot at successful ASSPfetch operation (in milliseconds), used in getTimeToDevice()*/
unsigned long timeDif;          /**holds difference between 'timeStrt' and 'timeFin' (in milliseconds), represents time taken to send message. Used in getTimeToDevice()*/
int devicePort;                 /**holds port on which device is connected to, used in port for device*/
const int attempts = 50;        /**defines number of attempts of send/fetch operations allowed before 'timeout' fail*/
char* conDevices[NUMNAMES];     /**initialises array to store devices connected to ports (same size as 'names' array), used in task4()*/
const char* names[NUMNAMES] = { /**initialises and defines array storing devices on network*/
                                "Ann", "Bug", "Car", "Day",
                                "End", "Fax", "Gym", "Hub",
                                "Ink", "Jam", "Kit", "Lab",
                                "Mac", "Not", "Oak", "Pen",
                                "Qua", "Rob", "Set", "Tea",
                                "Use", "Van", "Web", "Xis",
                                "Yes", "Zip"
};
void setup() {
  Serial.begin(57600);
  Serial.print("Virtual Serial Network Analysis\n");
  SerialShield.setEmuMode(6); /**set emulation mode to simulate network*/
#ifdef DEBUG
  SerialShield.showASSPmessages = true; /**show verbose ASSP debugging, only if DEBUG mode enabled*/
#endif
  //task1(); /**checks which devices are on port 3*/
  //task2(); /**checks response time of devices on port 4*/
  task3(); /**checks which port each device is connected to, device by device*/
  //task4(); /**checks which port each device is connected to, port by port*/
}
void task1() {
  Serial.println("Doing task 1...");
  Serial.print("Device       Port3?\n");

  for (int i = 0; i < NUMNAMES; i++) { /**repeats for amount of devices (26), minus 1. done due to array indexing beginning at 0*/

#ifdef DEBUG /**states that ping is being done, and which device is being pinged. only done if debug mode enabled*/
    Serial.print("Pinging device '");
    Serial.print(names[i]);
    Serial.println("'...");
#endif

    if (isDeviceOnPort(names[i], 3)) { /**check if device at index i (current interation of loop) is on port 3. print with 'YES' if connected or 'NO' if not connected*/
      Serial.print(names[i]);
      Serial.println("          YES");
    } else {
      Serial.print(names[i]);
      Serial.println("          NO");
    }
  }
}

void task2() {
  Serial.println("Doing task 2...");
  Serial.print("Device       Port4 Time\n");

  for (int i = 0; i < NUMNAMES; i++) { /**repeats for amount of devices (26), minus 1. done due to array indexing beginning at 0*/

#ifdef DEBUG /**states that ping is being done, and which device is being pinged. only done if debug mode enabled*/
    Serial.print("Pinging device '");
    Serial.print(names[i]);
    Serial.println("'...");
#endif

    if (isDeviceOnPort(names[i], 4)) { /**first check if device is connected to port 4*/
      Serial.print(names[i]);
      Serial.print("          ");
      Serial.print(getTimeToDevice(names[i], 4)); /**if connected to port 4, check time take to ping device*/
      Serial.println("ms");                       /**print unit (milliseconds)*/
    } else {
      Serial.print(names[i]); /**if not connected to port 4, print device name with 'NC' ('Not Connected')*/
      Serial.println("          NC");
    }
  }
}

void task3() {
  Serial.println("Doing task 3...");
  Serial.print("Device       Port\n");
  for (int i = 0; i < NUMNAMES; i++) { /**repeats for amount of devices (26), minus 1. done due to array indexing beginning at 0*/

#ifdef DEBUG /**states that ping is being done, and which device is being pinged. only done if debug mode enabled*/
    Serial.print("Pinging device '");
    Serial.print(names[i]);
    Serial.println("'...");
#endif

    devicePort = getPortForDevice(names[i]); /**fetches which port device is connected to*/
    if (devicePort > 0) {                    /**print the device name just searched if the output of getPortForDevice() is more than 0 (if device is on port 1-4)*/
      Serial.print(names[i]);
      Serial.print("          ");
      Serial.println(devicePort);

    } else {
      Serial.print(names[i]); /**if function returns 0, print device name with 'NC' ('Not Connected')*/
      Serial.println("          NC");
    }
  }
}

void task4() {
  Serial.println("Doing task 4...");
  Serial.print("Port       Devices\n");
  for (int p = 1; p <= 4; p++) { /**loop for amount of ports available (4), 'p' = 'port'. start with port 1*/
    Serial.print(p);
    Serial.print("          ");
    for (int d = 0; d < NUMNAMES; d++) { /**loop for amount of devices available (26), 'd' = 'device'*/
      if (isDeviceOnPort(names[d], p)) { /**if device (d) is on port (p), add current device from names[] at index d to conDevices[] at same index (d). print device name from names[] */
        conDevices[d] = { names[d] };
        Serial.print(names[d]);
        Serial.print(" ");
      }
    }
    Serial.print("\n"); /**line break for formatting*/
  }
#ifdef DEBUG
  for (int i = 0; i <= sizeof(conDevices); i++) { /**print entire, unedited conDevices array. will print giberrish at certain points. only happens in debug mode */
    Serial.print("connected device: ");
    Serial.println(conDevices[i]);
  }
#endif

  Serial.print("NC        ");                                                  /**following prints devices not found to be connected to ports 1-4*/
  for (int i = 0; i < sizeof(names); i++) {                                    /**repeat for amount of items in names array, -1. done due to array indexing starting at 0*/
    if (!strcmp(conDevices[i], names[i]) == 0 && strlen(conDevices[i]) == 3) { /**print device name if values from conDevice[] and names[] at same index are not the same AND if length of conDevices value is 3*/
      Serial.print(names[i]);                                                  /**(length 3 check filters out gibberish)*/
      Serial.print(" ");
    }
  }
}
bool isDeviceOnPort(const char name[], int port) { /**returns true if ASSP successfully fetches 'ping' message sent to supplied device (name[]) to supplied port (port). else returns false*/
                                                   /**tries fetch operation 50 times, can be adjusted by changing 'attempts' variable on line 9*/

#ifdef DEBUG /**states that ping is being done, and which device is being pinged. only done if debug mode enabled*/
  Serial.print("Pinging ");
  Serial.print(name);
  Serial.print(" on port ");
  Serial.println(port);
#endif

  SerialShield.setMessageSender("Pinger");
  SerialShield.setMessageDestination(name);
  SerialShield.setMessageContent("ping");

  SerialShield.sendASSPMessage(port);


  for (int i = attempts; i >= 0; i--) {

#ifdef DEBUG /**prints how many more times fetch operation is allowed to repeat*/
    Serial.print(i);
    Serial.println(" fetch attempts left");
#endif
    if (SerialShield.fetchASSPMessage()) {

#ifdef DEBUG /**prints each part of message sent*/
      Serial.print(SerialShield.getMessageContent());
      Serial.println(SerialShield.getMessageDestination());
      Serial.println(SerialShield.getMessageSender());
      Serial.println(SerialShield.getMessagePort());
#endif

      return true;
    } else {
#ifdef DEBUG
      Serial.println("Message not fetched");
#endif
    }
  }
  return false;
}


unsigned long getTimeToDevice(const char name[], int port) { /**returns amount of time taken to successfully fetch message sent to device (name[]) on port (port)*/
  timeDif = 0;
  SerialShield.setMessageSender("Timer");
  SerialShield.setMessageDestination(name);
  SerialShield.setMessageContent("ping");
  if (SerialShield.sendASSPMessage(port)) {
    ;
    timeStrt = millis(); /**if message successfully sent, capture time since boot (in milliseconds)*/
  }

  for (int i = 0; i < attempts; i++) { /**try fetch 50 times*/
    if (SerialShield.fetchASSPMessage()) {
      timeFin = millis();           /**if fetch operation successfull, capture time since boot*/
      timeDif = timeFin - timeStrt; /**set value of timeDif to the difference between time at send operation (timeStrt) and fetch operation (timeFin). shows how much time it took for device to respond*/
      break;
    }
  }
  return timeDif; /**return difference between timeFin adn timeStrt. if fetch operation not succesfull, returns 0*/
}

int getPortForDevice(const char name[]) { /**finds which device is on which port, by looking at each port in turn and finding which device is connected to it. returns*/
  devicePort = 0;

  SerialShield.setMessageSender("Pinger");
  SerialShield.setMessageDestination(name);
  SerialShield.setMessageContent("ping");

  for (int i = 1; i <= 4; i++) {
    if (SerialShield.sendASSPMessage(i)) {
      SerialShield.fetchASSPMessage();
      devicePort = i;
    }
  }

#ifdef DEBUG
  for (int i = attempts; i >= 0; i--) {
    Serial.print(i);
    Serial.println(" fetch attempts left");

    if (SerialShield.fetchASSPMessage()) {
      Serial.print(SerialShield.getMessageContent());
      Serial.println(SerialShield.getMessageDestination());
      Serial.println(SerialShield.getMessageSender());
      Serial.println(SerialShield.getMessagePort());
      return true;
    } else {
      Serial.println("Message not fetched");
    }
  }
#endif

  return devicePort;
}


void loop() {
  // put your main code here, to run repeatedly:
}
