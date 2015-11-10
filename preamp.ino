
#include <IRremote.h>

#define _AMP_DBG_  


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

unsigned long lastkey=0;

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
  if (irrecv.decode(&results)) {
#ifdef _AMP_DBG_      
    Serial.print(millis());
    Serial.print(" : ");
    Serial.print(results.value, HEX);
    Serial.print(" : ");
#endif    
    switch(results.value) {
      case K_VOL_UP: cmd_vol(1); break;
      case K_VOL_DN: cmd_vol(0); break;
      case K_SRC_UP: cmd_src(1); break;
      case K_SRC_DN: cmd_src(0); break;
      case K_REP: 
        if(lastkey==K_VOL_UP) cmd_vol(1);
        else if(lastkey==K_VOL_DN) cmd_vol(0);
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
#ifdef _AMP_DBG_            
  Serial.print("VOL ");
  Serial.print(d ? "UP" : "DN");  
#endif      
}

void cmd_src(uint16_t d) {
#ifdef _AMP_DBG_            
  Serial.print("SRC ");
  Serial.print(d ? "UP" : "DN");  
#endif      
}

