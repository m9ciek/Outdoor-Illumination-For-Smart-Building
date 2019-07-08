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

//zmienne pomocnicze do obsługi efektów i magistrali CAN
unsigned char hue=0;
unsigned char len = 0;
unsigned char buf[4];
int functionNumber = 1;
boolean flag = true;
long currentMillis, previousMillis;
long interval = 16000; //zmiana funkcji co 15 sekund

void setup()
{   
    Serial.begin(115200);
    while (CAN_OK != CAN.begin(CAN_500KBPS))              // inicjalizacja CANBUS 500Kb/s
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
       if(CAN_MSGAVAIL == CAN.checkReceive()){
        
          CAN.readMsgBuf(&len, buf);
  
          unsigned long canId = CAN.getCanId();
          
          Serial.println("-----------------------------");
          Serial.print("Get data from ID: 0x");
          Serial.println(canId, HEX);
  
          for(int i = 0; i<len; i++){
              Serial.print(buf[i], HEX);
              Serial.print("\t");
          }
          
          if(buf[0]==7 && buf[3]==7){flag=true;}
          Serial.println();
        }
        
      //w zależności od numeru funkcji, uruchomi się odpowiednia część programu
      if(functionNumber==1){
        if(flag){moveDot();flag=false;}
      }
      if(functionNumber==2){palette();}
      if(functionNumber==3){sparkles();}
      if(functionNumber==4){rainbow();}
      if(functionNumber==5){fireEffect();}
      if(functionNumber==6){colorSwitch();}
      if(functionNumber==7){singleDot();}

      /*stiwch i case */
      /*
        switch(functionNumber){
          case 1:
          if(flag){moveDot();flag=false;}
          break;
          case 2: palette(); break;
          case 3: sparkles(); break;
          case 4: rainbow(); break;
          case 5: fireEffect(); break;
          case 6: colorSwitch(); break;
          case 7: singleDot(); break;
          default: rainbow();
        }
      if(functionNumber==2){palette();}
      if(functionNumber==3){sparkles();}
      if(functionNumber==4){rainbow();}
      if(functionNumber==5){fireEffect();}
      if(functionNumber==6){colorSwitch();}
      if(functionNumber==7){singleDot();}
       */

    //płynna zmiana wartości koloru co 20 ms  
    EVERY_N_MILLISECONDS( 20 ) { hue++; }

    //zliczanie czasu w celu zmiany efektu co pewien czas
    if(millis() - previousMillis > interval){
      functionNumber++;
      previousMillis = millis();
      if(functionNumber==8){
        resetMessage();
        flag=true;
        functionNumber=1;
      }
    }
}

//płynne zmiany kolorów
void colorSwitch(){
    unsigned char message[4] = {0, 0, hue, 1};
    CAN.sendMsgBuf(51, 0, 4, message);
    
    int brightness = 220;
    FastLED.setBrightness(brightness);
    fill_solid( leds, NUM_LEDS, CHSV(hue,255,brightness));
    FastLED.show();
}

//pojedyncza dioda na każdym fragmencie, 
//funkcja przekazuje aktualny kolor oraz numer diody do pozostałych modułów
void singleDot(){  
    int i = beatsin16( 11, 0, NUM_LEDS-1 );
    unsigned char j = (unsigned char)i;
    
    unsigned char message[4] = {0, j, hue, 2};
    CAN.sendMsgBuf(51, 0, 4, message);
    
    fadeToBlackBy( leds, NUM_LEDS, 20);
    leds[i] = CHSV( hue, 255, 192);
    FastLED.show();
}

//wypełnia segmenty kolorami tęczy
void rainbow() {
  unsigned char message[4] = {0, 0, hue, 3};
  CAN.sendMsgBuf(51, 0, 4, message);
  
  fill_rainbow( leds, NUM_LEDS, hue, 12);
  FastLED.show();
}

//funkcja wypełnienia każdego z segmentów zdefiniowanym gradientem
CRGBPalette16 myPal = OceanColors_p;
uint8_t heatindex = 0;
void palette(){
  resetMessage();
  FastLED.clear();
  for(int i=NUM_LEDS;i>=0;i--){
    leds[i] = ColorFromPalette( myPal, heatindex);
    heatindex++;
    delay(15);
    FastLED.show();
  }
//  FastLED.clear();
//  FastLED.show();
  unsigned char message[4] = {0, 0, heatindex, 5};
  CAN.sendMsgBuf(51, 0, 4, message);
  FastLED.delay(6000);
}

//funkcja generująca losowy kolor każdej z diod
void sparkles(){
    unsigned char message[4] = {0, 0, 0, 6};
    CAN.sendMsgBuf(51, 0, 4, message);
    int randomDiode = random8(NUM_LEDS);
    leds[randomDiode]=CHSV(random8(),255,255);
//    leds[randomDiode].nscale8(200);
    delay(30);
    FastLED.show();
}

//funkcja przejścia koloru pomiędzy segmentami
void moveDot(){
  resetMessage();
  FastLED.clear();
  for(int i=NUM_LEDS;i>=0;i--){
    leds[i,i-1,i-2] = CHSV(hue,255,200);
    FastLED.show();
    hue++;
    delay(12);
    leds[i]= CRGB::Black;
//    fadeToBlackBy(leds,NUM_LEDS,20);
    FastLED.show();
  }

  unsigned char message[4] = {0, 0, hue, 7};
  CAN.sendMsgBuf(51, 0, 4, message);
  delay(100);
}

//efekt ognia
CRGBPalette16 firePal = heatmap_gp;
void fireEffect(){
  uint8_t heatindex = random(100,230);
  int i = random(0,100);
  leds[i]=ColorFromPalette(firePal,heatindex);
  FastLED.show();
  unsigned char message[4] = {0, 0, heatindex, 8};
  CAN.sendMsgBuf(51, 0, 4, message);
}

//funkcja resetująca stan każdego z segmentów
void resetMessage(){
  FastLED.clear();
  FastLED.show();
  delay(10);
  unsigned char reset[4] = {99, 0, 0, 0};
  CAN.sendMsgBuf(51, 0, 4, reset);
}
