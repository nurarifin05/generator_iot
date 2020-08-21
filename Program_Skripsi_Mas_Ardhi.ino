#include<DFRobotHighTemperatureSensor.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4); 

#define ssid "SUDO" 
#define password "saibudin" 
#define mqtt_server "202.154.63.168"
#define mqtt_username "ardi"
#define mqtt_password "iot"
#define clientID "espClient"

WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient);
long lastMsg = 100;
const int dataIN = 2; 

unsigned long prevmillis; 
unsigned long duration; 
unsigned long lcdrefresh; 
const float voltageRef = 3.300;
int HighTemperaturePin = A0; 
int rpm; 
boolean currentstate; 
boolean prevstate; 

DFRobotHighTemperature PT100 = DFRobotHighTemperature(voltageRef);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient_distance_sensor")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

  void setup()
  {
  setup_wifi(); 
  client.setServer(mqtt_server, 1883);  
  pinMode(dataIN,INPUT);
  Serial.begin(115200);   
  prevmillis = 0;
  prevstate = LOW;  
  lcd.begin();
  lcd.backlight();
  
  }

  void loop()
{
    if (!client.connected()) {
    reconnect();
  }
  client.loop();
 
  int temperature = PT100.readTemperature(HighTemperaturePin);  //Get temperature
  Serial.print("Temperature:  ");
  Serial.print(temperature);
  Serial.println("  ^C");
 
  delay(100); 
 
  currentstate = digitalRead(dataIN);
 if( prevstate != currentstate) 
   {
     if( currentstate == HIGH ) 
       {
         duration = ( micros() - prevmillis ); 
         rpm = (60000000/duration); 
         prevmillis = micros(); 
       }
   }
  prevstate = currentstate; 
  
 
  if( ( millis()-lcdrefresh ) >= 100 )
    {   
      
     Serial.println(rpm);
      lcdrefresh = millis();   
    }
    client.publish("ardi/rpm", String(rpm).c_str(), true); 
    client.publish("ardi/suhu", String(temperature).c_str(), true);

  lcd.setCursor(0,0);
  lcd.print("Temperature: "); 
  lcd.print(temperature);
  lcd.setCursor(0,1);
  lcd.print("RPM: "); 
  lcd.print(rpm);
    


      long now = millis();
  if (now - lastMsg > 100) {
    lastMsg = now;
  
  }
}
