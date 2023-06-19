/************************************************************************************
	NAME: HARIS HASSAN
	USERNAME: 190230894

*************************************************************************************/

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServerSecure.h>
#include <WiFiUdp.h>
#include <rgb_lcd.h>  // include the grove RGB LCD library


/***************** Define Tasks Times********************************************/
#define buttonL 5
#define updateLCDL 1000
#define avgFootfallL 1000


/***************** Define PINS********************************************/
#define Led 15
#define Button 15
#define ADC A0
#define SOCKET 8800
#define BUFFERLEN 255
#define ssid          "Mewtwo"    // your hotspot SSID or name
#define password      "harishassan"    // your hotspot password
/***************** Define macros/typedefs********************************************/

/************************** Function prototypes ********************************/
void MQTTconnect ( void );
void ButtonPoll ( void );

/********************* Global instances / variables********************************/
WiFiClientSecure client;    // create a class instance for the MQTT server
const char domainname[] = "mewtwo";

bool current = false; // For button Press
bool last = false; // For button Press

WiFiUDP UDP;
char inBUFFER[BUFFERLEN]; //Buffer to store incoming Packets

int red = 0, green = 128, blue = 128;
rgb_lcd LCD;

static const char cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIC8jCCAlugAwIBAgIUYOIOrtubcFsmeN3TrSfpvaJoMHAwDQYJKoZIhvcNAQEL\
BQAwfjELMAkGA1UEBhMCVUsxFjAUBgNVBAgMDVdFU1QgTUlETEFORFMxEzARBgNV
BAcMCkJJUk1JTkdIQU0xGjAYBgNVBAoMEUFzdG9uIFVuaXZlcnNpdHkgMQ0wCwYD
VQQLDARFRVBFMRcwFQYDVQQDDA51c2VybmFtZS5sb2NhbDAeFw0yMDAyMTUxOTQ5
MzZaFw00NzA3MDMxOTQ5MzZaMH4xCzAJBgNVBAYTAlVLMRYwFAYDVQQIDA1XRVNU
IE1JRExBTkRTMRMwEQYDVQQHDApCSVJNSU5HSEFNMRowGAYDVQQKDBFBc3RvbiBV
bml2ZXJzaXR5IDENMAsGA1UECwwERUVQRTEXMBUGA1UEAwwOdXNlcm5hbWUubG9j
YWwwgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBAKZPMudaME7JDhWSUhH+ZfOQ
dRZ6Pa4I1Xt0RJCb9F9PUX4vU7cyVWufoYpBlyOxzPyn1kMAzBmEQ0vAwXG0m7PU
urft8ZB/ZZallIfscgEI8HDcOVdwt/uEbfsONp7R4BAGKGhijx+niMgWLL3aoIF1
Tj7AqzjU85e/v2kPrdR5AgMBAAGjbTBrMB0GA1UdDgQWBBTAz79RjP3Mbs4Y32Ga
9p44Vycp+zAfBgNVHSMEGDAWgBTAz79RjP3Mbs4Y32Ga9p44Vycp+zAPBgNVHRMB
Af8EBTADAQH/MBgGA1UdEQQRMA+CDXVzZXJuYW1lLmxvY2EwDQYJKoZIhvcNAQEL
BQADgYEABmkelyDzfZRcqVGM8edRJM5dAQvBlHnjnsMPhvhgkG6MUisBa+vtVhKC
cOE4gAFvv8/hCaWIQwtFdWNfyle5meRX/Tj4L9N1aHASyPfTBcqKTGpjq6GiqtmL
rTINar7hi15E+adMVRRg/1VNfnBXKxh6IYch5hgeHj5TRgGwgAs=
-----END CERTIFICATE-----
)EOF";

static const char key[] PROGMEM =  R"EOF(
-----BEGIN PRIVATE KEY-----
MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAKZPMudaME7JDhWS
UhH+ZfOQdRZ6Pa4I1Xt0RJCb9F9PUX4vU7cyVWufoYpBlyOxzPyn1kMAzBmEQ0vA
wXG0m7PUurft8ZB/ZZallIfscgEI8HDcOVdwt/uEbfsONp7R4BAGKGhijx+niMgW
LL3aoIF1Tj7AqzjU85e/v2kPrdR5AgMBAAECgYBgNFpW+JYPTUDne6AcJpSlY8BH
w2jgvt13r9dl68FeTQzwOMJtrCE7w7j3uF+M13KkCRbp5ZErhZZEQPnmI7sZSkal
3TIWmLQq9g9mIscdIjc6rsfWJ7DAdpVgdsClS3sHPxv3D3RTy6Z40vzPgb2977/y
26pUHHJVEG27dqthbQJBANJPu7fm8EOUcvg1/wwnp3p+dCHEmZ7snQlUerMGbw3+
fSJTqFhrwEXYhfzj2VQUqZyaDBM+tZ9vUQU+BreM+fcCQQDKcFUVeUh6PzJJ1uSJ
F3gabOc1IoIPyotQ9RWAU70vaj/aAuIXJd6PsUt+0DucYfcg1G5YteUqJGl/5Mpv
CokPAkADNLHw2LVa4l1qSTBtGAGmjVzp0txgnsy6Aq6oIfX5aaKwrkPHrUTOC8Hn
G/YJIROAzpxWgsMz/fdnNA3YKG77AkEAiA7NsJwOMVNuKiCLAvTKHQCauKSTw6c+
0U+XfuNJIKgJeC495I7oMa1Yb0fm+KkDHoaID4lZF2TXn0SXJeBv0wJBAKhRejgb
R/KkoohDnWQCNkI9TOMPtTohGjoqMakVcaFHB+SOYhKi7iI0+coxNv/1dMydaE5X
xkjlush/Q6zlliA=
-----END PRIVATE KEY-----
)EOF";


int cardpresent = 0, lastcardpresent = 0; // To read tag from reader module
int stored_id[4] = {0}; //To store UID recieved from UDP
int stored_id2[4] = {0}; //To store UID recieved from Reader Module
int LowtoHighTrans=0;

int tff=0,avtotal; // Total footfall.
int affpm; //Average footfall per minute.
int sui; //The sensors unique identifier (for localisation purposes)
static const char *fingerprint PROGMEM = "77 00 54 2D DA E7 D8 03 27 31 23 99 EB 27 DB CB A5 4C 57 18"; 
BearSSL::ESP8266WebServerSecure SERVER(443);


/*********************** Adafruit IO defines*************************************/
#define NOPUBLISH //comment this out once publishing at less than 10 second intervals
#define ADASERVER     "io.adafruit.com"     // do not change this
#define ADAPORT       8883                  // do not change this 
#define ADAUSERNAME   "mewtwo"               // ADD YOUR username here between the qoutation marks
#define ADAKEY        "aio_alhc06lHv1UfUx7BqWHOae8zinpw" // ADD YOUR Adafruit key here betwwen marks

Adafruit_MQTT_Client MQTT(&client, ADASERVER, ADAPORT, ADAUSERNAME, ADAKEY);

/***************************** Feeds ***************************************/
//Feeds we publish to

// Setup a feed called LED to subscibe to HIGH/LOW changes
Adafruit_MQTT_Publish POT = Adafruit_MQTT_Publish(&MQTT, ADAUSERNAME "/feeds/pot");
Adafruit_MQTT_Publish hum = Adafruit_MQTT_Publish(&MQTT, ADAUSERNAME "/feeds/humidity");
Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&MQTT, ADAUSERNAME "/feeds/temp");

/***************** Setup Function ********************************************/
void setup()
{
  pinMode(Led, OUTPUT);
  pinMode(ADC, INPUT);
  
  Serial.begin(115200);                         // open a serial port at 115,200 baud
  
  Serial.print("Attempting to connecting to "); // Inform of us connecting
  Serial.print(ssid);                           // print the ssid over serial
  WiFi.begin(ssid, password);                   // attemp to connect to the access point SSID with the password
  while (WiFi.status() != WL_CONNECTED)         // whilst we are not connected
  {
    delay(500);                                 // wait for 0.5 seconds (500ms)
    Serial.print(".");                          // print a .
    digitalWrite (Led, HIGH); //set the pin connected to the LED (15) as an output
  }
  digitalWrite (Led, LOW); //set the pin connected to the LED (15) as an output

  Serial.print("\n");                           // print a new line

  Serial.println("Succesfully connected");      // let us now that we have now connected to the access point
  Serial.print("Mac address: ");                // print the MAC address
  Serial.println(WiFi.macAddress());            // note that the arduino println function will print all six mac bytes for us
  Serial.print("IP:  ");                        // print the IP address
  Serial.println(WiFi.localIP());               // In the same way, the println function prints all four IP address bytes
  Serial.print("Subnet mask: ");                // Print the subnet mask
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");                    // print the gateway IP address
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS: ");                        // print the DNS IP address
  Serial.println(WiFi.dnsIP());

  /***************** UDP ********************************************/
  if (UDP.begin(SOCKET)) {
    Serial.println("Successfully Opened Port 8800");
  } else {
    Serial.println("Unsuccessful.....");
  }

  /***************** LCD ********************************************/
  LCD.begin(16, 2);
  LCD.setRGB(red, green, blue);

  client.setFingerprint(fingerprint);

  //MQTT.subscribe(&LED);  // subscribe to the LED feed


      if ( MDNS.begin( domainname ) ){
      Serial.print( "Access your server at https://" );
      Serial.println(domainname);
      Serial.print(".local");
      }

      SERVER.getServer().setRSACert(new BearSSL::X509List(cert), new BearSSL::PrivateKey(key) );
      SERVER.on("/", respond );
      SERVER.begin();

      Serial.println("Server should now be running ");
}

void loop()
{
  unsigned long curmil;
  static unsigned long ButtonPollCount = 0, updateLCDCount=0, avgFootfallCount=0;
  
  curmil = millis();

  if ((unsigned long)curmil - ButtonPollCount >= buttonL) {
    ButtonPollCount = curmil;
    ButtonPoll();
  }
    if ((unsigned long)curmil - avgFootfallCount >= avgFootfallL) {
    avgFootfallCount = curmil;
    avgFootfall();
  }
  if ((unsigned long)curmil - updateLCDCount >= updateLCDL) {
     updateLCDCount = curmil;
     updateLCD();
  }

  SERVER.handleClient();
  MDNS.update();
}

void ButtonPoll ( void )
{
  pinMode(Button, INPUT);
  current = digitalRead(Button);

  if ( current && !last ) {
    delay(5);
    /***************** If statement is true when Button is pressed ********************************************/
    if ( digitalRead(Button) ) {
      pinMode(Led, OUTPUT);
      Serial.println("the button is pressed\n");
      LowtoHighTrans++;
      avtotal++;
      tff=LowtoHighTrans;
      last = current;
    }
  }
  else {
    pinMode(Led, OUTPUT);
    last = current;
  }
}

void avgFootfall ( void )
{
  affpm = (float) avtotal / 1000;                      // calculate average 
  Serial.println(affpm);                            // print it
  avtotal = 0;    
}

void updateLCD ( void )
{
      LCD.setCursor(0, 1);
      LCD.print(WiFi.localIP());
      LCD.setCursor(0, 0);
      LCD.setRGB(red, green, blue);
      LCD.print("Av FF/m: ");
      LCD.print(analogRead(affpm));
  }

void task2 ( void )
{
  int packetSize = 0;              // A temporary variable to store the size of received packets
  packetSize = UDP.parsePacket();  // Check for the presence of a UDP packet and return the size of it

  if (packetSize) {                // If the a packet is present i.e. size greater than 0
    UDP.read(inBUFFER, BUFFERLEN); // Read the content of the UDP opacket to inBUFFER with a max length of BUFFERLEN
    inBUFFER[packetSize] = '\0';   // Add a Null operator at the end of buffer to ensure the correct reading of string length
    Serial.print("Received ");
    Serial.print(packetSize);
    Serial.println(" Bytes");

    Serial.print("Contents: ");
    Serial.println(inBUFFER);

    /***************** Saving received RFID UID and storing it in integer array and printing on LCD ********************************************/
    LCD.setCursor(0, 1);
    sscanf(inBUFFER, "%d%d%d%d", &stored_id[0], &stored_id[1], &stored_id[2], &stored_id[3]);
    LCD.print(inBUFFER);
    /*************************************************************************************/

    Serial.print("From IP ");
    Serial.println(UDP.remoteIP());

    Serial.print("From Port ");
    Serial.println(UDP.remotePort() );
  }
}


/******************************* MQTT connect *************************************/
void MQTTconnect ( void )
{
  unsigned char tries = 0;

  // Stop if already connected.
  if ( MQTT.connected() )
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ( MQTT.connect() != 0 )                                        // while we are
  {
    Serial.println("Will try to connect again in five seconds");   // inform user
    MQTT.disconnect();                                             // disconnect
    delay(5000);                                                   // wait 5 seconds
    tries++;
    if (tries == 3)
    {
      Serial.println("problem with communication, forcing WDT for reset");
      while (1)
      {
        ;   // forever do nothing
      }
    }
  }

  Serial.println("MQTT succesfully connected!");
}


  void servepage ( void )
{
    String reply;

    reply += "<!DOCTYPE HTML>";
    reply += "<html>";
    reply += "<head>";
    reply += "<title>Forrest's Sensor</title>";
    reply += "</head>";
    reply += "<body>";
    reply += "<h1> This is Forrest's Wireless Sensor Node </h1>";
    reply += "You can add content here to represent various system parameters etc";
    reply += "<h2>" + String(millis()) + "</h2>";
    reply += "</body>";
    reply += "</html>";
    
    SERVER.send(200, "text/html", reply);
}

  void servePageRespond ()
{
    String reply;

    reply += "<!DOCTYPE HTML>";
    reply += "<html>";
    reply += "<head>";
    reply += "<title>Forrest's Sensor</title>";
    reply += "</head>";
    reply += "<body>";
    reply += "<h1> This is 190230894's Wireless Sensor Node </h1>";
    reply += "<h2>Total footfall is: " + String(tff) + "</h2>";
    reply += "<h2>Average footfall per minute is: " + String(affpm) + "</h2>";
    reply += "<h2>The sensors unique identifier: " + String(sui) + "</h2>";
    reply += "</body>";
    reply += "</html>";
    
    SERVER.send(200, "text/html", reply);
}

void respond(void) {
  servePageRespond();
  }
