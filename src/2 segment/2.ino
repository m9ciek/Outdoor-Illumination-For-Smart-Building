#include <SPI.h>
#include "mcp_can.h"
#include <FastLED.h>

#define DATA_PIN    6
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    100
#define BRIGHTNESS  220

CRGB leds[NUM_LEDS];
const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

unsigned char hue=0;
unsigned char len = 0;
unsigned char buf[4];
int functionNumber = 0;
int diodeNumber = 0;
boolean flag = true;

void setup()
{
    Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS))              
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");
    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    Serial.println("Led Strip Init OK!");
}

void loop()
{
    //odczytanie przychodzących wiadomości i uruchomienie odpowiedniego efektu
    //jedynie przy wykonywaniu funkcji przejścia, ten segment przekazuje dane
    if(CAN_MSGAVAIL == CAN.checkReceive())
    {
        CAN.readMsgBuf(&len, buf);

        unsigned long canId = CAN.getCanId();
        
        Serial.println("-----------------------------");
        Serial.print("Get data from ID: 0x");
        Serial.println(canId, HEX);
        if(canId == 51){
          for(int i = 0; i<len; i++){
              Serial.print(buf[i]);
              Serial.print("\t");
          }
              if(buf[0]==99){FastLED.clear(); FastLED.show();}
              if(buf[3]==1){hue = buf[2]; colorSwitch(hue);}
              if(buf[3]==2){hue = buf[2]; diodeNumber = buf[1];
                 singleDot(hue,diodeNumber);}
              if(buf[3]==3){hue = buf[2]; rainbow(hue);}
              if(buf[3]==5){hue = buf[2]; palette(hue);}
              if(buf[3]==6){sparkles();}
              if(buf[3]==7){hue=buf[2]; moveDot(hue);}
              if(buf[3]==8){fireEffect();}
          
          Serial.println();
        }else{
          Serial.println("Unknown Device");
        }
    }
}

void colorSwitch(int ghue){
    int brightness = 220;
    FastLED.setBrightness(brightness);
    fill_solid( leds, NUM_LEDS, CHSV(ghue,255,brightness));
    FastLED.show();
}

void singleDot(int hue, int diodeNumber){
    int i = diodeNumber;
    fadeToBlackBy( leds, NUM_LEDS, 20);
    leds[i] = CHSV( hue, 255, 192);
    FastLED.show();
}

void rainbow(int ghue) {
  fill_rainbow( leds, NUM_LEDS, ghue, 12);
  FastLED.show();
}

CRGBPalette16 myPal = OceanColors_p;
void palette(uint8_t heatindex){
  FastLED.clear();
  for(int i=0;i<=NUM_LEDS;i++){
    leds[i] = ColorFromPalette( myPal, heatindex);
    heatindex++;
    delay(15);
    FastLED.show();
    Serial.print(heatindex);
  }
//  FastLED.clear();
//  FastLED.show();
  unsigned char message[4] = {0, 0, heatindex, 5};
  CAN.sendMsgBuf(52, 0, 4, message);
}

void sparkles(){
//    fadeToBlackBy(leds,NUM_LEDS,20);
    int randomDiode = random8(NUM_LEDS);
    leds[randomDiode]=CHSV(random8(),255,200);
    leds[randomDiode].nscale8(200);
    delay(30);
    FastLED.show();
}

void moveDot(uint8_t ghue){
  FastLED.clear();
  for(int i=0;i<=NUM_LEDS;i++){
    leds[i,i+1,i+2] = CHSV(ghue,255,200);
    FastLED.show();
    ghue++;
    delay(12);
    leds[i]= CRGB::Black;
    FastLED.show();
//    fadeToBlackBy(leds,NUM_LEDS,20);
  }

  unsigned char message[4] = {0, 0, ghue, 7};
  CAN.sendMsgBuf(52, 0, 4, message);
}

CRGBPalette16 firePal = heatmap_gp;
void fireEffect(){
  FastLED.setBrightness(200);
  uint8_t heatindex = random(100,230);
  int i = random(0,100);
  leds[i]=ColorFromPalette(firePal,heatindex);
  FastLED.show();
}
