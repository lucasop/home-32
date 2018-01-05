/*----------------------------------------------------------
    SHARP GP2Y1010AU0F Dust Sensor  SAMPLE
  ----------------------------------------------------------*/

#include <GP2Y1010_DustSensor.h>

const int led_pin = 21;   // GPIO21
const int analog_pin = 2; // ADC12 on GPIO2


GP2Y1010_DustSensor dustsensor;
int n = 0;
/*----------------------------------------------------------
    SHARP GP2Y1010AU0F Dust Sensor  setup
  ----------------------------------------------------------*/
void setup() {
  Serial.begin(9600);
  dustsensor.setADCbit(10);
  dustsensor.setInputVolts(3.3);
  analogSetAttenuation((adc_attenuation_t)2);   // -6dB range tutti gli ADC
  //analogSetPinAttenuation(analog_pin,(adc_attenuation_t)2); // -6dB range
  
  dustsensor.begin(led_pin, analog_pin);
}

/*----------------------------------------------------------
    SHARP GP2Y1010AU0F Dust Sensor  loop
  ----------------------------------------------------------*/
void loop() {
  float dust = dustsensor.getDustDensity();                      // ADC12 on GPIO2
  for(n=1; n<8; n++) dust += dustsensor.getDustDensity();
  dust /= 8;
  Serial.print("Dust Density: "); Serial.print(dust); Serial.println(" ug/m3");
  delay(3000); // misura ogni 3 secondi
}

