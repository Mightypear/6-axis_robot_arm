#include <AccelStepper.h>
#include <TimerThree.h>
#include <MultiStepper.h>
#include "Sensors.h"
#include <Servo.h>
//SERVOKOD
Servo myServo;
int servoPin = 4;
int currentAngle = 30;
//SERVOKOD

MultiStepper msteppers;


//Sensors sensors;
// Create an array to hold 6 AccelStepper objects
AccelStepper steppers[6];
long stepperPos[6] = { 0, 0, 0, 0, 0, 0 };    // Target position for each stepper
long homePosition[6] = { 0, 0, 0, 0, 0, 0 };  // Home position for each stepper
long P1[6] = { 0, 0, 0, 0, 0, 0 };
long P2[6] = { 0, 0, 0, 0, 0, 0 };
long P3[6] = { 0, 0, 0, 0, 0, 0 };
long P4[6] = { 0, 0, 0, 0, 0, 0 };

//long stepsPerFullTurn[6] = { 200, 200, 200, 200, 200, 200 };
long stepsPerFullTurn[6] = { 31500, 31500, 31500, 31500, 31500, 200 };  // Steps per full turn
unsigned long delayTime = millis();                                     // Last time stepper position was updated



// Function to create and return a new AccelStepper object
AccelStepper newStepper(int stepPin, int dirPin, int enablePin, bool invertEnablePin, int maxSpeed, int acceleration) {
  AccelStepper stepper = AccelStepper(AccelStepper::DRIVER, stepPin, dirPin);
  stepper.setEnablePin(enablePin);
  stepper.setPinsInverted(false, false, invertEnablePin);
  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(acceleration);
  stepper.enableOutputs();
  return stepper;
}



Sensors sensors;

void setup() {
  Serial.begin(9600);



  //SERVOKOD
  myServo.attach(servoPin);
  myServo.write(currentAngle);
  Serial.begin(9600);
  Serial.println("Skriv en absolut vinkel (0-180) eller ett relativt kommando (+/-).");
  //SERVOKOD

  //SENSORKOD
  sensors = Sensors();
  for (int i = 0; i < 6; i++) {
    sensors.init(i);
  }
  //SENSORKOD



  // Initialize steppers with pins and settings
  steppers[0] = newStepper(54, 55, 38, true, 1000, 5000);
  steppers[1] = newStepper(60, 61, 56, true, 1000, 5000);
  steppers[2] = newStepper(46, 48, 62, true, 1000, 5000);
  steppers[3] = newStepper(26, 28, 24, true, 800, 2000);
  steppers[4] = newStepper(36, 34, 30, true, 800, 2000);
  //steppers[4] = newStepper(32, 47, 45, true, 1500, 3000);
  steppers[5] = newStepper(14, 2, 3, true, 1500, 3000);

msteppers.addStepper(steppers[0]);
msteppers.addStepper(steppers[1]);
msteppers.addStepper(steppers[2]);
msteppers.addStepper(steppers[3]);
msteppers.addStepper(steppers[4]);



  Serial.println("Welcome to the Robot test lab");
  Serial.println("------------------------------------------------");
  Serial.println(" ");
  DisplayHelp();  // Assuming you have a DisplayHelp() function for help messages

  // Initialize Timer3 to call RunSteppers every 0.5ms
  Timer3.initialize(500);  // 500 microseconds
  Timer3.attachInterrupt(RunSteppers);
}

void loop() {
  ReadSerial();  // Process serial commands

  if (millis() - delayTime > 10) {
    delayTime = millis();

    float k_p = 0.5;
    float k_i = 0.1;
    float k_d = 0.00001;
    for (int i = 0; i < 6; i++) {
      long SensorPosition = sensors.getAngle(i) * stepsPerFullTurn[i]/360; //steps
      long motorPosition = steppers[i].currentPosition(); //steps
      
      long error = SensorPosition - motorPosition;
      
      long newSetPosition = motorPosition;

      // Serial.print(stepperPos[i]);
      // Serial.print(" ");
      // Serial.print(SensorPosition);
      // Serial.print(" ");
      // Serial.print(motorPosition);
      // Serial.print(" ");
      // Serial.print(error);
      // Serial.print(" ");
      // Serial.print(newSetPosition);
      // Serial.println("");

      steppers[i].setCurrentPosition((0.7*motorPosition + 0.3*SensorPosition)); //komp
      steppers[i].setCurrentPosition(newSetPosition);
      //steppers[i].moveTo(stepperPos[i]);
      msteppers.moveTo(stepperPos);  //BYT EVENTYEKKT

      
    }
    //steppers[i].moveTo(stepperPos[i]);
    // //Check if 10ms have passed to update the stepper positions
    // if (millis() - delayTime > 10) {
    //   for (int i = 0; i < 6; i++) {
    //     steppers[i].moveTo(stepperPos[i]);
    //   }
    //   delayTime = millis();  // Update the time
  }
}

// Function that gets called by Timer3 to run each stepper motor
void RunSteppers() {
  for (int i = 0; i < 6; i++) {
    //steppers[i].run();  // Keep each stepper running
    msteppers.run();
  }
}

// Function to handle serial input
void ReadSerial() {
  if (Serial.available() == 0) { return; }

  const byte numChars = 32;
  char receivedChars[numChars];
  static byte ndx = 0;
  bool newData = false;

  while (Serial.available() > 0 && !newData) {
    char receivedChar = Serial.read();
    if (receivedChar != '\n') {
      receivedChars[ndx++] = receivedChar;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    } else {
      receivedChars[ndx] = '\0';  // Null-terminate the string
      ndx = 0;
      newData = true;
    }
  }

  if (!newData) return;

  // Process the command received via serial
  Serial.println(receivedChars);
  char* strtokIndx = strtok(receivedChars, " ");
  char command[32] = { 0 };
  strcpy(command, strtokIndx);
  Serial.println(command);
  Serial.println(strcmp(command, "DisableAll"));
  Serial.println(strcmp(command, "DisableAll") == 0);
  if (strcmp(command, "GoTo") == 0) {
    Serial.println("Processing GoTo...");
    strtokIndx = strtok(NULL, " ");
    int stepperID = atoi(strtokIndx);  // Get the stepper ID
    strtokIndx = strtok(NULL, " ");
    float deg = atof(strtokIndx);  // Get the degrees to move
    GoTo(stepperID, deg);
  }
  //NYTT TILL ATT FÅ POSITIONER OSV
  else if (strcmp(command, "Go") == 0) {
    //Go t0 t1 t2 t3 t4 t5
    strtokIndx = strtok(NULL, " ");
    float theta0 = atof(strtokIndx);

    strtokIndx = strtok(NULL, " ");
    float theta1 = atof(strtokIndx);

    strtokIndx = strtok(NULL, " ");
    float theta2 = atof(strtokIndx);

    strtokIndx = strtok(NULL, " ");
    float theta3 = atof(strtokIndx);

    strtokIndx = strtok(NULL, " ");
    float theta4 = atof(strtokIndx);

    strtokIndx = strtok(NULL, " ");
    float theta5 = atof(strtokIndx);

    // Serial.println(theta0);
    // Serial.println(theta1);
    // Serial.println(theta2);
    // Serial.println(theta3);
    // Serial.println(theta4);
    // Serial.println(theta5);

    stepperPos[0] = stepsPerFullTurn[0] * theta0 / 360.0;
    stepperPos[1] = stepsPerFullTurn[1] * theta1 / 360.0;
    stepperPos[2] = stepsPerFullTurn[2] * theta2 / 360.0;
    stepperPos[3] = stepsPerFullTurn[3] * theta3 / 360.0;
    stepperPos[4] = stepsPerFullTurn[4] * theta4 / 360.0;
    stepperPos[5] = stepsPerFullTurn[5] * theta5 / 360.0;

    Serial.print("Going to positions:");
    Serial.print(theta0);
    Serial.print(" ");
    Serial.print(theta1);
    Serial.print(" ");
    Serial.print(theta2);
    Serial.print(" ");
    Serial.print(theta3);
    Serial.print(" ");
    Serial.print(theta4);
    Serial.print(" ");
    Serial.println(theta5);
    PrintStepperPositions();
  }


  else if (strcmp(command, "GoRel") == 0) {
    strtokIndx = strtok(NULL, " ");
    int stepperID = atoi(strtokIndx);
    strtokIndx = strtok(NULL, " ");
    float degrees = atof(strtokIndx);
    //if (stepperID == 0)
    MoveRelativeDegrees(stepperID, degrees);
  } else if (strcmp(command, "Enable") == 0) {
    Serial.println("Processing Enable...");
    strtokIndx = strtok(NULL, " ");
    int stepperID = atoi(strtokIndx);
    Enable(stepperID);
  } else if (strcmp(command, "Disable") == 0) {
    Serial.println("Processing Disable...");
    strtokIndx = strtok(NULL, " ");
    int stepperID = atoi(strtokIndx);
    Disable(stepperID);
  } else if (strcmp(command, "EnableAll") == 0) {
    EnableAll();
  } else if (strcmp(command, "DisableAll") == 0) {
    DisableAll();
  } else if (strcmp(command, "SetHome") == 0) {
    SetHome();
  } else if (strcmp(command, "GoHome") == 0) {
    GoHome();
  } else if (strcmp(command, "SetP1") == 0) {
    SetP1();  //NY
  } else if (strcmp(command, "GoP1") == 0) {
    GoP1();  //NY
  } else if (strcmp(command, "SetP2") == 0) {
    SetP2();  //NY
  } else if (strcmp(command, "GoP2") == 0) {
    GoP2();  //NY
  } else if (strcmp(command, "SetP3") == 0) {
    SetP3();  //NY
  } else if (strcmp(command, "GoP3") == 0) {
    GoP3();  //NY
  } else if (strcmp(command, "SetP4") == 0) {
    SetP4();  //NY
  } else if (strcmp(command, "GoP4") == 0) {
    GoP4();  //NY
  } else if (strcmp(command, "GetPosition") == 0) {
    strtokIndx = strtok(NULL, " ");
    int stepperID = atoi(strtokIndx);
    GetPosition(stepperID);  //ska stå (stepperID) men fungerar inte?
  }                          // NYTT
  else if (strcmp(command, "GetAllPositions") == 0) {
    GetAllPositions();

    // for (int i = 0; i < 6; i++) {

    //   if (i == 5) {
    //     Serial.print(steppers[i].currentPosition() * 360.0 / stepsPerFullTurn[i]);
    //   } else {
    //     Serial.print(steppers[i].currentPosition() * 360.0 / stepsPerFullTurn[i]);
    //   }
    //   Serial.print(" ");
    //}

    //steppers[stepperID].currentPosition()*360.0/stepsPerFullTurn[stepperID];
  }  //NYTT
  else if (strcmp(command, "GoServo") == 0) {
    Serial.println("Processing GoServo...");
    strtokIndx = strtok(NULL, " ");
    long degrees = atoi(strtokIndx);
    myServo.write(degrees);
  } else if (strcmp(command, "TestCommand") == 0) {
    strtokIndx = strtok(NULL, " ");
    int val1 = atoi(strtokIndx);
    strtokIndx = strtok(NULL, " ");
    float val2 = atof(strtokIndx);
    strtokIndx = strtok(NULL, " ");
    bool val3 = atoi(strtokIndx);
    TestCommand(val1, val2, val3);
  }
}

// Function to move the stepper to a specified degree
void GoTo(int stepperID, float degrees) {
  if (stepperID < 0 || stepperID > 5) {
    Serial.println("Error: stepperID must be between 0 and 5!");
    return;
  }

  Serial.print("Moving motor ");
  Serial.print(stepperID);
  Serial.print(" to ");
  Serial.print(degrees);
  Serial.println(" degrees...");

  stepperPos[stepperID] = stepsPerFullTurn[stepperID] * degrees / 360.0;
}

// Function to move a stepper motor by relative degrees
void MoveRelativeDegrees(int stepperID, float degrees) {
  if (stepperID < 0 || stepperID > 5) {
    Serial.println("Error: stepperID must be between 0 and 5!");
    return;
  }

  Serial.print("Moving motor ");
  Serial.print(stepperID);
  Serial.print(" by ");
  Serial.print(degrees);
  Serial.println(" relative degrees...");

  stepperPos[stepperID] += stepsPerFullTurn[stepperID] * degrees / 360.0;
}

// Set the maximum speed of a specific stepper
void SetMaxSpeed(int stepperID, long speed) {
  if (stepperID < 0 || stepperID > 5) {
    Serial.println("Error: stepperID must be between 0 and 5!");
    return;
  }
  steppers[stepperID].setMaxSpeed(speed);
}

// Set the acceleration of a specific stepper
void SetAcceleration(int stepperID, long acceleration) {
  if (stepperID < 0 || stepperID > 5) {
    Serial.println("Error: stepperID must be between 0 and 5!");
    return;
  }
  steppers[stepperID].setAcceleration(acceleration);
}

// Function to enable a stepper motor
void Enable(int stepperID) {
  if (stepperID < 0 || stepperID > 5) {
    Serial.println("Error: stepperID must be between 0 and 5!");
    return;
  }
  steppers[stepperID].enableOutputs();
  Serial.print("Enabled motor ");
  Serial.println(stepperID);
}

// Function to disable a stepper motor
void Disable(int stepperID) {
  if (stepperID < 0 || stepperID > 5) {
    Serial.println("Error: stepperID must be between 0 and 5!");
    return;
  }
  steppers[stepperID].disableOutputs();
  Serial.print("Disabled motor ");
  Serial.println(stepperID);
}

void PrintStepperPositions() {
  for (int i = 0; i < 6; i++) {
    if (i == 5) {
      Serial.println(stepperPos[i]);
    } else {
      Serial.print(stepperPos[i]);
    }
    Serial.print(" ");
  }
}

void GetAllPositions() {

  for (int i = 0; i < 6; i++) {
    float pos_i = sensors.getAngle(i); //Degrees
    if (i == 5) {
      Serial.println(pos_i);
    } else {
      Serial.print(pos_i);
    }
    Serial.print(" ");
  }

  //steppers[stepperID].currentPosition()*360.0/stepsPerFullTurn[stepperID];
}

// Function to enable all steppers
void EnableAll() {
  for (int i = 0; i < 6; i++) {
    steppers[i].enableOutputs();
  }
  Serial.println("Enabled all motors");
}

// Function to disable all steppers
void DisableAll() {
  for (int i = 0; i < 6; i++) {
    steppers[i].disableOutputs();
    
  }
  Serial.println("Disabled all motors");
}

// Set the current position as home
void SetHome() {
  for (int i = 0; i < 6; i++) {
    homePosition[i] = sensors.getAngle(i);
    //homePosition[i] = stepperPos[i];
  }
  Serial.println("Set current position as home");
}
//NY
void SetP1() {
  for (int i = 0; i < 6; i++) {
    //homePosition[i] = sensors.getAngle(i);
    P1[i] = stepperPos[i];
    // P1[i] = sensors.getAngle(i) * stepsPerFullTurn[i]/360;
    //P1[i] = sensors.getAngle(i);
  }
  Serial.println("Set current position as P1");
}
void GoP1() {
  for (int i = 0; i < 6; i++) {
    stepperPos[i] = P1[i];
  }
  Serial.println("Returning to P1");
}
void SetP2() {
  for (int i = 0; i < 6; i++) {
    //homePosition[i] = sensors.getAngle(i);
    P2[i] = stepperPos[i];
  }
  Serial.println("Set current position as P2");
}
void GoP2() {
  for (int i = 0; i < 6; i++) {
    stepperPos[i] = P2[i];
  }
  Serial.println("Returning to P2");
}
void SetP3() {
  for (int i = 0; i < 6; i++) {
    //homePosition[i] = sensors.getAngle(i);
    P3[i] = stepperPos[i];
  }
  Serial.println("Set current position as P3");
}
void GoP3() {
  for (int i = 0; i < 6; i++) {
    stepperPos[i] = P3[i];
  }
  Serial.println("Returning to P3");
}
void SetP4() {
  for (int i = 0; i < 6; i++) {
    //homePosition[i] = sensors.getAngle(i);
    P4[i] = stepperPos[i];
  }
  Serial.println("Set current position as P4");
}
void GoP4() {
  for (int i = 0; i < 6; i++) {
    stepperPos[i] = P4[i];
  }
  Serial.println("Returning to P4");
}
//NY
// Move all steppers to their home position
void GoHome() {
  for (int i = 0; i < 6; i++) {
    stepperPos[i] = homePosition[i];
  }
  Serial.println("Returning to home position");
}

// Get the current position of all steppers
// void GetPosition() {
//   for (int i = 0; i < 6; i++) {
//     Serial.print("Stepper ");
//     Serial.print(i);
//     Serial.print(" position: ");
//     Serial.println(sensors.getAngle(i));
//    // Serial.println(steppers[steppers].currentPosition()*360.0/stepsPerFullTurn[steppers]);
//   }
// }
void GetPosition(int stepperID) {

  Serial.print("Position for motor");
  Serial.print(stepperID);
  Serial.print(": ");
  Serial.print(steppers[stepperID].currentPosition() * 360.0 / stepsPerFullTurn[stepperID]);
  Serial.print(", target: ");
  Serial.println(stepperPos[stepperID] * 360.0 / stepsPerFullTurn[stepperID]);

  //Serial.println(sensors.getAngle(i));
  // Serial.println(steppers[steppers].currentPosition()*360.0/stepsPerFullTurn[steppers]);
}

// Custom test command
void TestCommand(int val1, float val2, bool val3) {
  Serial.print("TestCommand received: val1=");
  Serial.print(val1);
  Serial.print(", val2=");
  Serial.print(val2);
  Serial.print(", val3=");
  Serial.println(val3 ? "true" : "false");
}

// Function to display help messages
void DisplayHelp() {
  Serial.println("Valid commands are:");
  Serial.println("Disable (int)stepperID");
  Serial.println("Enable (int)stepperID");
  Serial.println("DisableAll");
  Serial.println("EnableAll");
  Serial.println("SetHome");
  Serial.println("GoHome");
  Serial.println("SetP1");  //NY
  Serial.println("GoP1");   //NY
  Serial.println("SetP2");  //NY
  Serial.println("GoP2");   //NY
  Serial.println("SetP3");  //NY
  Serial.println("GoP3");   //NY
  Serial.println("SetP4");  //NY
  Serial.println("GoP4");   //NY
  Serial.println("GetPosition");
  Serial.println("GoTo (int)stepperID (float)degrees, absolute movement");
  Serial.println("GoRel (int)stepperID (float)degrees, relative movement");
  Serial.println("SetMaxSpeed (int)stepperID (int)speed, steps per second");
  Serial.println("SetAcceleration (int)stepperID (int)acceleration, steps per second^2");
  Serial.println("TestCommand (int)val1 (float)val2 (bool)val3");
}
