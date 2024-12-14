/*
Name: Tiwari Ramesh Sanjana
Date: 19-6-2024
Title: Home Automation using MQTT
Description: MQTT protocol follows pub-sub communication model. Node-red acts as a publisher,
the Arduino Uno IDE is both the subscriber and publisher, and the data is controlled by the MQTT broker.
Arduino as a subscriber: 2 lights of 2 different romms, simulated in PICSIMlab are controlled using switches
designed in Node-red dashboard, over the MQTT protocol. Arduino as publisher, the temperature reading
obtained from temperature sensor placed in the room are displayed on the dashboard.
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define ROOM1_LIGHT1          2
#define ROOM2_LIGHT1          3

/*Update these with values suitable for your network.*/
byte mac[] = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 100);

/*Temperature sensor setup*/
const char* lm35_topic = "temperature_topic";
int lm35Pin = A0; // LM35 sensor connected to analog pin A0

/*Broker address*/
IPAddress server(192,168,31, 89);

/*Callback function header*/
void callback(char* topic, byte* payload, unsigned int length);

/*Create rthernet client instance*/
EthernetClient ethClient;
/*Create pubsub client instance*/
PubSubClient client(server, 1883, callback, ethClient);


/*Callback function*/
void callback(char* topic, byte* payload, unsigned int length)  {
  char topic1[30]="homedemo/room1/light1";
  char topic2[30]="homedemo/room2/light1";

  if(strcmp(topic, topic1) == 0)  {
    /*Turn the LED ON if the payload is '1' and publish to the MQTT server a confirmation message*/
    if(payload[0] == '1') {
      digitalWrite(ROOM1_LIGHT1, HIGH);
      client.publish("home/feedback", "Room1 Light1 On");
      Serial.print("Message: Room1 Light1 On");
    }  
    /*Turn the LED OFF if the payload is '0' and publish to the MQTT server a confirmation message*/
    else if(payload[0] == '0')  {
      digitalWrite(ROOM1_LIGHT1, LOW);
      client.publish("home/feedback", "Room1 Light1 Off");
    }
  }
  else if(strcmp(topic, topic2) == 0) {
    /*turn the LED ON if the payload is '1' and publish to the MQTT server a confirmation message*/
    if(payload[0] == '1') {
      digitalWrite(ROOM2_LIGHT1, HIGH);
      client.publish("home/feedback", "Room2 Light1 On");
      Serial.print("Message: Room2 Light1 On");
    }
    /*turn the LED OFF if the payload is '0' and publish to the MQTT server a confirmation message*/
    else if(payload[0] == '0')  {
      digitalWrite(ROOM2_LIGHT1, LOW);
      client.publish("home/feedback", "Room2 Light1 Off");
    }
  }
} 

void reconnect() 
{
  /*Loop until we're reconnected */
  while(!client.connected())  {
    Serial.print("Attempting MQTT connection...");
    /*Create a random client ID */
    String clientId = "Home_automation_UNO";
    clientId += String(random(0xffff), HEX);
    /*Attempt to connect*/
    if(client.connect(clientId.c_str()))  {
      Serial.println("connected");
      /*Once connected, publish an announcement and resubscribe*/
      client.subscribe("homedemo/room1/light1");
      client.subscribe("homedemo/room2/light1");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      /*Wait 5 seconds before retrying*/
      delay(5000);
    }
  }
}

void setup()
{
  /*Configure output pins*/
  pinMode(ROOM1_LIGHT1, OUTPUT);
  pinMode(ROOM2_LIGHT1, OUTPUT);
  /*Initial pin state*/
  digitalWrite(ROOM1_LIGHT1, LOW);
  digitalWrite(ROOM2_LIGHT1, LOW);
   
   /*Initialise serila communication*/
  Serial.begin(9600);
  
  /*Connecting the board to ethernet*/
  Ethernet.begin(mac, ip);

  if(Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if(Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }
}

void loop()
{
  /*If arduino  is connected to mosquitto*/
  if(!client.connected()) {
    /*Attempting to connect loop*/
    reconnect();
  }
  client.loop();

  /*Pulish temperature sensor readings on Node-red dashboard*/
  float temperature = analogRead(lm35Pin) * 5.0 / 1023.0 * 100.0;
  client.publish(lm35_topic, String(temperature).c_str());
}