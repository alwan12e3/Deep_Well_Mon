 /*
  * File  : DFRobot_SIM7000_MQTT.ino
  * Power : SIM7000 needs 7-12V DC power supply
  * Brief : This example verify MQTT send verification
  *         With initialization completed, we connect to iot.dfrobot.com.cn
  *         Then send data to a topic
  *         Thus we finished the MQTT send verification
  * Note  : If you use Mega please connect PIN8 PIN10 and set PIN_RX = 10
  *         If you select another IOT, please confirm that the IOT is in the whitelist of your NB card
  *         The maximum length of data transmitted each time is 50
  */

#include <Wire.h>
#include <DFRobot_SIM7000.h>

//Login website (http://iot.dfrobot.com.cn) to register an account ,fill the following information based on your account
#define serverIP        "iot.dfrobot.com.cn"
#define IOT_CLIENT      " CLIENT NAME "
#define IOT_USERNAME    " USER   NAME "
#define IOT_KEY         " PASSWORD    "
#define IOT_TOPIC       " TOPIC       "

#define PIN_TX          7
#define PIN_RX          8

SoftwareSerial          mySerial(PIN_RX,PIN_TX);
DFRobot_SIM7000         sim7000;

byte statusLed    = 13;

byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;
unsigned long oldConTime;


void setup(){
    int signalStrength;
    Serial.begin(115200);
    while(!Serial);
    sim7000.begin(mySerial);
    Serial.println("Turn ON SIM7000......");
    if(sim7000.turnON()){                                        //Turn ON SIM7000
        Serial.println("Turn ON !");
    }
    Serial.println("Set baud rate......");
    while(1){
        if(sim7000.setBaudRate(19200)){                          //Set SIM7000 baud rate from 115200 to 19200 reduce the baud rate to avoid distortion
            Serial.println("Set baud rate:19200");
            break;
        }else{
            Serial.println("Faile to set baud rate");
            delay(1000);
        }
    }

    Serial.println("Check SIM card......");
    if(sim7000.checkSIMStatus()){                                //Check SIM card
        Serial.println("SIM card READY");
    }else{
        Serial.println("SIM card ERROR, Check if you have insert SIM card and restart SIM7000");
        while(1);
    }


    Serial.println("Set net mode......");
    while(1){
        if(sim7000.setNetMode(NB)){                              //Set net mod NB-IOT
            Serial.println("Set NB-IOT mode");
            break;
        }else{
            Serial.println("Fail to set mode");
            delay(1000);
        }
    }

    Serial.println("Get signal quality......");
    delay(500);
    signalStrength=sim7000.checkSignalQuality();                 //Check signal quality from (0-30)
    Serial.print("signalStrength =");
    Serial.println(signalStrength);
    delay(500);

    Serial.println("Attaching service......");
    while(1){
        if(sim7000.attacthService()){                            //Open the connection
            Serial.println("Attach service");
            break;
        }else{
            Serial.println("Fail to Attach service");
            delay(1000);
        }
    }
    //Water Flow Setup
     pinMode(statusLed, OUTPUT);
    digitalWrite(statusLed, HIGH);  // We have an active-low LED attached
  
    pinMode(sensorPin, INPUT);
    digitalWrite(sensorPin, HIGH);

    pulseCount        = 0;
    flowRate          = 0.0;
    flowMilliLitres   = 0;
    totalMilliLitres  = 0;
    oldTime           = 0;
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

void loop(){
    
    if ((millis() - oldConTime) > 3600000)
   {
      detachInterrupt(sensorInterrupt);

      oldConTime = millis();

    
    /* MQTT PROTOQOL
     *  *
     */
      Serial.print("Connect to :");
      Serial.println(serverIP);
      if(sim7000.openNetwork(TCP,serverIP,1883)){                  //Connect to server
        Serial.println("Connected !");
      }else{
        Serial.println("Failed to connect");
        return;
      }
      delay(200);

      Serial.print("Connect to : ");
      Serial.println(IOT_USERNAME);
      if(sim7000.mqttConnect(IOT_CLIENT,IOT_USERNAME,IOT_KEY)){    //MQTT connect request
          Serial.println("Connected !");
      }else{
          Serial.println("Failed to connect");
          return;
      }
      delay(200);

      Serial.println("Input data end with CRLF : ");
    // KIRIM DAATA DI SINI
      Serial.print("Send data : ");
      Serial.println(" ......");
      if(sim7000.mqttPublish(IOT_TOPIC,sendData)){                 //Send data to topic
          Serial.println("Send OK");
      }else{
          Serial.println("Failed to send");
        return;
      }
      delay(200);

      Serial.println("Close connection......");
      if(sim7000.closeNetwork()){                                  //Close connection
          Serial.println("Close connection !");
      }else{
          Serial.println("Fail to close connection !");
        return;
      }
      delay(2000);
      attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
   }else if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);
        
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
      
    unsigned int frac;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space

    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");        
    Serial.print(totalMilliLitres);
    Serial.println("mL"); 
    Serial.print("\t");       // Print tab space
    Serial.print(totalMilliLitres/1000);
    Serial.print("L");
    

    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
}

/*
Insterrupt Service Routine
 */
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
}
