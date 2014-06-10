#include <TinyZtLib.h>

#include <TinyDHT.h>
#include <TinyWireM.h>
#include <avr/power.h>  
#include <USI_TWI_Master.h>

// include the library code:
// Connect via i2c, default address #0 (A0-A2 not jumpered)

#define DHTTYPE DHT22 
#define DHT_PIN 1
#define OLED_ADDRESS 0x27
#define TEMPTYPE 0

DHT dht(DHT_PIN,DHTTYPE);

TinyZtlib led((uint8_t )0x27);

int8_t i;
int disp=8888;
int8_t hMin=BAD_HUM,hMax=BAD_HUM;
int16_t tMin=BAD_TEMP,tMax=BAD_TEMP;

void setup() {
  // Set CPU to 16Mhz
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  // set the LCD type
  //  lcd.setMCPType(LTI_TYPE_MCP23017); 
  // set up the LCD's number of rows and columns:
  // Print a message to the LCD.
  dht.begin();
  i=0;
  led.I2cInit();
  //led.Seg8b4a036aUnSleep(OLED_ADDRESS);
  //led.Seg8b4a036aSetBrightness(OLED_ADDRESS,0,1);
  //lled.Seg8b4a036aDisplayDec(OLED_ADDRESS,disp,5,0);
}



void loop() {
  //led.Seg8b4a036aDisplayDec(OLED_ADDRESS,disp,5,0);
  int8_t h=dht.readHumidity();
  int16_t t=dht.readTemperature(0);
  if(i%2){
    disp=h;
  }else{
    disp=t;
  }
  delay(500);
  led.Seg8b4a036aUnSleep(OLED_ADDRESS);
  led.Seg8b4a036aSetBrightness(OLED_ADDRESS,0,1);
  led.Seg8b4a036aDisplayDecAndCode(OLED_ADDRESS,disp,(i%2?0x76:0x63),5,0);
  delay(1500);
  led.Seg8b4a036aSleep(OLED_ADDRESS);
  if (i++>10){i=0;}
}


