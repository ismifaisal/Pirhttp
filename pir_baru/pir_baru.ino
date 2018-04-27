#include <SPI.h>
#include <Ethernet.h>

int LED1 = 13; 
int pirPin = 5;
int pirPin2 = 6;
int row = 0;
int gerakan = 0;
int gerakan2 = 0;

// Local Network Settings
byte mac[]     = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Must be unique on local network
byte ip[]      = { 192, 168, 100, 102 };      // Must be unique on local network
byte gateway[] = { 192, 168, 100, 114 };        // 192.168.100.102    
byte subnet[]  = { 255, 255, 255, 0 };

// Server Settings
char server[] = "pramudyo.com"; 
const int updateInterval = 1000;            // Time interval in milliseconds to update ThingSpeak   

// Variable Setup
long lastConnectionTime = 0; 
boolean lastConnected = false;
int failedCounter = 0;

// Initialize Arduino Ethernet Client
EthernetClient client;

void setup()
{
  Serial.begin(9600);
  pinMode(pirPin,INPUT);
  pinMode(pirPin2,INPUT);
  pinMode(LED1,OUTPUT);
  
  Ethernet.begin(mac, ip, gateway, subnet);
  delay(1000);
  Serial.print("ETHERNET SHIELD ip  is     : ");
  Serial.println(Ethernet.localIP()); 
  // Start Ethernet on Arduino
  startEthernet();
}

void loop()
{  
  gerakan = digitalRead(pirPin);
  gerakan2 = digitalRead(pirPin2);
  
  String pirPin = String(gerakan);
  String pirPin2 = String(gerakan2);
    
  // Print Update Response to Serial Monitor
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }
  // Disconnect from Server
  if (!client.connected() && lastConnected)
  {
    Serial.println();
    Serial.println("...disconnected.");
    Serial.println();
    client.stop();
  }
  // Update Server
  if(!client.connected() && (millis() - lastConnectionTime > updateInterval))
  {
    updateServer(("&satu="+pirPin)+("&dua="+pirPin2)); 
    //+("&tiga="+pirPin3)+("&empat="+pirPin4)+("&lima="+pirPin5)+("&enam="+pirPin6)+("&tujuh="+pirPin7)+("&delapan="+pirPin8)+("&sembilan="+pirPin9)+("&sepuluh="+pirPin10)+("&sebelas="+pirPin11)+("&duabelas="+pirPin12)+("&tigabelas="+pirPin13)+("&empatbelas="+pirPin14)+("&limabelas="+pirPin15)+("&enambelas="+pirPin16)+("&tujuhbelas="+pirPin17)+("&delapanbelas="+pirPin18)+("&sembilanbelas="+pirPin19)+("&duapuluh="+pirPin20)+("&duasatu="+pirPin21));
  }
  lastConnected = client.connected();
}

void updateServer(String tsData)
{
  if (client.connect(server, 80)) // +tsData
  { 
    client.print("POST /monitor.php HTTP/1.1\n");
    client.print("Host: pramudyo.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);
    
    lastConnectionTime = millis();
    
    if (client.connected())
    {
      Serial.println("Connecting to Server...");
      Serial.println();
      
      failedCounter = 0;
    }
    else
    {
      failedCounter++;
  
      Serial.println("Connection to Server failed ("+String(failedCounter, DEC)+")");   
      Serial.println();
    }
    
  }
  else
  {
    failedCounter++;
    
    Serial.println("Connection to Server Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
    
    lastConnectionTime = millis(); 
  }
}

void startEthernet()
{
  
  client.stop();

  Serial.println("Connecting Arduino to network...");
  Serial.println();  

  delay(1000);
  
  // Connect to network amd obtain an IP address using DHCP
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("DHCP Failed, reset Arduino to try again");
    Serial.println();
  }
  else {
    Serial.println("Arduino connected to network using DHCP");
    Serial.println();
    Serial.println("Data being uploaded to Server.......");
    Serial.println();
  }
  
  delay(1000);
}
