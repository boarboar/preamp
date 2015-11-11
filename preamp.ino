
#include <IRremote.h>

#define _AMP_DBG_  

const unsigned int NSRC=2; 

const unsigned int RECV_PIN = P2_2;

const unsigned long K_REP=0xFFFFFFFF;
const unsigned long K_VOL_UP=0xFFA857;
const unsigned long K_VOL_DN=0xFFE01F;
const unsigned long K_SOURCE=0xFF629D;
const unsigned long K_SRC_UP=0xFFE21D;
const unsigned long K_SRC_DN=0xFFA25D;
const unsigned long K_PAUSE =0x286101BD; // ???

const unsigned int REP_RTMO=120; //ms


IRrecv irrecv(RECV_PIN);

decode_results results;

uint32_t ms;
uint32_t lastkey=0;
uint32_t lastms=0;
uint8_t  invol=0;
uint8_t  src=0;

void setup()
{
#ifdef _AMP_DBG_  
  Serial.begin(9600);
  delay(5000);
  Serial.println("IR TEST++++");
#endif    
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  uint32_t ms = millis();
  if(invol) {
    if(ms-lastms>REP_RTMO || ms<lastms) {
      // stop vol HERE
      invol=0;
#ifdef _AMP_DBG_          
    Serial.println("VOL STOP");
#endif          
    } 
  }
  if (irrecv.decode(&results)) {
#ifdef _AMP_DBG_      
    Serial.print(millis());
    Serial.print(" : ");
    Serial.print(results.value, HEX);
    Serial.print(" : ");
#endif    
    switch(results.value) {
      case K_VOL_UP: cmd_vol(2); break;
      case K_VOL_DN: cmd_vol(1); break;
      case K_SRC_UP: cmd_src(2); break;
      case K_SRC_DN: cmd_src(1); break;
      case K_REP: 
        if(lastkey==K_VOL_UP) cmd_vol(2);
        else if(lastkey==K_VOL_DN) cmd_vol(1);
        break;
      default:;
    }
    if(results.value != K_REP) lastkey=results.value;
#ifdef _AMP_DBG_          
    Serial.println();
#endif    
    irrecv.resume(); // Receive the next value
  }
}

void cmd_vol(uint16_t d) {
  if(invol && invol!=d) {
    // vol stop HERE
    delay(100);    
#ifdef _AMP_DBG_          
    Serial.print("VOL STOP ");
#endif          
  }
  invol = d;
  lastms=millis();
#ifdef _AMP_DBG_            
  Serial.print("VOL ");
  Serial.print(d ? "UP" : "DN");  
  Serial.print(" START");
#endif        
}

void cmd_src(uint16_t d) {
  // src stop HERE
  src++;
  if(src==NSRC) src=0;
  // do switch HERE  
#ifdef _AMP_DBG_            
  Serial.print("SRC ");
  Serial.print(src);  
#endif      
}

