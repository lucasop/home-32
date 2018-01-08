/*----------------------------------------------------------
    SHARP GP2Y1010AU0F Dust Sensor  SAMPLE
  ----------------------------------------------------------*/

/*  2017 LucaSOp */

#include "Arduino.h"

const char* DESC = "nodo_01-Dust-soggiorno";
const char* VERS = "v1.0.0";
const char* IP = "";
const char* MAC = "";
const char* HA_ENTITY_ID = "";


/* File password in libraries/Secret/Secret.h */
#include <Secret.h>

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//#include <GP2Y1010_DustSensor.h>
#include <ESP8266Influxdb.h>

const int led_pin = 21;   // GPIO21
//const int analog_pin = 2; // ADC12 on GPIO2 con il WIFI attivo ADC1 viene inibito
const int analog_pin = 35; // ADC1-CH7 on GPI35

/******************* influxdb  *********************/
const char *INFLUXDB_HOST = S_INFLUXDB_HOST;
const uint16_t INFLUXDB_PORT = S_INFLUXDB_PORT;
const char *DATABASE = S_INFLUXDB_DATABASE;
const char *DB_USER = S_INFLUXDB_DB_USER;
const char *DB_PASSWORD = S_INFLUXDB_DB_PASSWORD;

Influxdb influxdb(INFLUXDB_HOST, INFLUXDB_PORT);




/******************* WIFI *********************/
const char* wifi_ssid     = S_WIFI_SSID;
const char* wifi_password = S_WIFI_PASSWORD;

/******************* MQTT  *********************/
#define mqtt_server S_MQTT_HOST
#define mqtt_user S_MQTT_USER
#define mqtt_password S_MQTT_PASSWORD

//WiFiClient client;

//GP2Y1010_DustSensor dustsensor;
int n = 0;
/*----------------------------------------------------------
    SHARP GP2Y1010AU0F Dust Sensor  setup
  ----------------------------------------------------------*/
void setup() {
  Serial.begin(115200);

  // StartUp Banner
  Serial.println("#######################################################################");

  Serial.print( "Descrizione: ");
  Serial.println( DESC );
  
  Serial.print( "Version: ");
  Serial.println( VERS );
  Serial.println( "GITHUB repository HOME_ONE" );

  Serial.print( "IP: ");
  Serial.println( IP );

  Serial.print( "MAC_ADD: ");
  Serial.println( MAC );

  Serial.print( "HA_ENTITY_ID: ");
  Serial.println( HA_ENTITY_ID );

  Serial.println("#######################################################################");

  setup_wifi();
  Serial.print("Configuring OTA device...");
    //TelnetServer.begin();   //Necesary to make Arduino Software autodetect OTA device
    ArduinoOTA.onStart([]() {Serial.println("OTA starting...");});
    ArduinoOTA.onEnd([]() {Serial.println("OTA update finished!");Serial.println("Rebooting...");});
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {Serial.printf("OTA in progress: %u%%\r\n", (progress / (total / 100)));});
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    Serial.println("OK");


  //dustsensor.setInputVolts(3.3);
  analogSetWidth(10);                           // 10Bit resolution
  analogSetAttenuation((adc_attenuation_t)2);   // -6dB range tutti gli ADC
  //analogSetPinAttenuation(analog_pin,(adc_attenuation_t)2); // -6dB range 
  
  pinMode(led_pin,OUTPUT);
  //dustsensor.begin(led_pin, analog_pin);

   influxdb.opendb(DATABASE, DB_USER, DB_PASSWORD);
  
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

 

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

float dust = 0.0;
float diffd = 0.1;


/*----------------------------------------------------------
    SHARP GP2Y1010AU0F Dust Sensor  loop
  ----------------------------------------------------------*/
void loop() {

  ArduinoOTA.handle();




 // float dust = dustsensor.getDustDensity();                      // ADC12 on GPIO2
  //for(n=1; n<8; n++) dust += dustsensor.getDustDensity();
  //dust /= 8;
  //Serial.print("Dust Density: "); Serial.print(dust); Serial.println(" ug/m3");

//  Serial.println("led low");
  digitalWrite(led_pin, LOW);
  delayMicroseconds(280);

  float mesured = analogRead(analog_pin);  //read analog pin / Dust value
  delayMicroseconds(40);

 // Serial.println("led hiht");
  digitalWrite(led_pin, HIGH);
  delayMicroseconds(9680);


  int analog_bit = 10;
  double analog_bit_num = pow(2., (double)analog_bit);
  float inputvolts = 3.3;

// culc dust density
// pow(2., (double)analog_bit) = 2^10 =1024
// inputvolts = 3.3 
/*
calcVoltage = voMeasured * (3.3v / 1024.0);
dustDensity = (0.17 * calcVoltage - 0.1)*1000

So in my case when voltage for sensor is 3.3v and measure 221

calcVoltage = 221* (3.3/ 1024.0);
dustDensity = (0.17 * 0,71 - 0.1)*1000
-----------------------------------------------
26,7mg/m3

*/

  float dust1 = (0.17 * (mesured * (inputvolts / analog_bit_num)) - 0.1) * 1000.;
  if( dust1<0 )  dust1=0.;

  float newDust = dust1; 

   if (checkBound(newDust, dust, diffd)) {
      dust = newDust;
      Serial.print("New dust:");
      Serial.println(String(dust).c_str());
      // Create field object with measurment name=power_read
      FIELD dataObj("Dust_table");
      dataObj.addTag("method", "Field_object"); // Add method tag
      dataObj.addTag("dust", "D0"); // Add pin tag
      dataObj.addField("value", dust); // Add value field
//Serial.print("INFLUXDB_HOST: "); Serial.print(INFLUXDB_HOST);Serial.print(" INFLUXDB_PORT: "); Serial.println(INFLUXDB_PORT);
      Serial.println(influxdb.write(dataObj) == DB_SUCCESS ? "Object write success" : "Writing failed");
      // Empty field object.
      dataObj.empty();
      //client.publish(humidity_topic, String(hum).c_str(), true);
      Serial.print(" Dust Density: "); Serial.print(newDust); Serial.println(" ug/m3");
    }  


  
  delay(3000); // misura ogni 1 secondi
}

