#include <SoftwareSerial.h>
SoftwareSerial gprsSerial(7, 8);

void setup()
{
  gprsSerial.begin(19200);
  Serial.begin(19200);

  Serial.println("Config SIM900...");
  delay(2000);
  Serial.println("Done!...");
  gprsSerial.flush();
  Serial.flush();

  // attach or detach from GPRS service 
  gprsSerial.println("AT+CGATT?");
  delay(100);
  toSerial();
  
  gprsSerial.println("AT+CSQ");
  delay(1000);
  toSerial();


  // bearer settings
  gprsSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  delay(2000);
  toSerial();

  // bearer settings
  gprsSerial.println("AT+SAPBR=3,1,\"APN\",\"internet\""); //internet = APN 
  delay(2000);
  toSerial();
  //CLose bearer
  gprsSerial.println("AT+SAPBR=0,1");
  delay(5000);
  toSerial();
  // open bearer
  gprsSerial.println("AT+SAPBR=1,1");
  delay(5000);
  toSerial();
  
  //IP address
  gprsSerial.println("AT+SAPBR=2,1");
  delay(2000);
  toSerial();
}


void loop()
{
  /*
   // initialize http service
   gprsSerial.println("AT+CIPSHUT");
   toSerial();
   delay (1000);

   gprsSerial.println("AT+CIPSTATUS");
   toSerial();
   delay(1000);

   gprsSerial.println("AT+CIPMUX=0");
   toSerial();
   delay(2000);

   gprsSerial.println("AT+CSTT=\"internet\",\"wap\",\"\"");
   toSerial();
   delay(5000);

   gprsSerial.println("AT+CIICR");
   toSerial();
   delay(5000);

   gprsSerial.println("AT+CIFSR");
   toSerial();
   delay(4000);
   //toSerial();
   delay(2000);
   
   gprsSerial.print("AT+CIPSTART=\"TCP\",");
   toSerial();
   delay(1000);
   gprsSerial.print("\"script.go");
   toSerial();
   delay(2000);
   gprsSerial.print("ogle.com/macros/s/AKfycbxXylolgHQu1UUmEctB4");
   toSerial();
   delay(3000);
   gprsSerial.print("fQQgDnNvPOrlnw9q9ohbIR6X13JpIIS/exec\"");
   toSerial();
   //delay(1000);
   gprsSerial.println(",80");
   
   
 //  gprsSerial.println("AT+CIPSTART=\"TCP\",\"www.google.com\",80");
   toSerial();
   delay(6000);

   delay(100);
   gprsSerial.println("");
   gprsSerial.println("AT+CIPSEND");
   toSerial();
   delay(2000);

   gprsSerial.print("?debtData=909");
   gprsSerial.write(26);
   toSerial();
   delay(3000);

   gprsSerial.println("AT+CIPSHUT");
   toSerial();
   //delay(10000);*/
    gprsSerial.println("AT+HTTPTERM");
   toSerial();
   delay(300);

   gprsSerial.println("AT+HTTPINIT");
   delay(10000); 
   toSerial();

   gprsSerial.println("AT+HTTPPARA=\"CID\",1");
   delay(2000); 
   toSerial();
 
   // set http param value
   gprsSerial.print("AT+HTTPPARA=\"URL\",");
   toSerial();
   delay(100);
   gprsSerial.print("\"http://api.thingspeak.com/update?");
   toSerial();
   delay(100);
   gprsSerial.print("api_key=MK4IM3GO19KC2LUB&field1=");
   toSerial();
   delay(100);
   int debit = 15;
   gprsSerial.print(debit);
   delay(100);
   gprsSerial.println("\"");
   
   toSerial();
   delay(1000);
   
    
   // set http action type 0 = GET, 1 = POST, 2 = HEAD
   gprsSerial.println("AT+HTTPACTION=0");
   delay(10000);
   toSerial();

   // read server response
   gprsSerial.println("AT+HTTPREAD"); 
   delay(5000);
   toSerial();

   gprsSerial.println("");
   gprsSerial.println("AT+HTTPTERM");
   toSerial();
   delay(300);

   gprsSerial.println("");
   
   delay(10000);
   
}

void toSerial()
{
  while(gprsSerial.available()!=0)
  {
    Serial.write(gprsSerial.read());
  }
}
