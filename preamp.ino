
#include "IRremote1.h"

//#define _AMP_DBG_  

const unsigned int RECV_PIN = P2_2;

const unsigned int MOTOR_OUT_1 = P2_4;
const unsigned int MOTOR_OUT_2 = P2_3;
const unsigned int MOTOR_EN    = P2_5; // analog write
const unsigned int MOTOR_LED =   P1_6; 

const unsigned int SSW_HIBERNATE = P2_1;
const unsigned int SSW_SELECT = P1_5;  // sw A, B=LOW
const unsigned int SSW_LED1 = P1_0;
const unsigned int SSW_LED2 = P1_3;

const unsigned int NSRC=2; 

const unsigned long K_REP=0xFFFFFFFF;
const unsigned long K_VOL_UP=0xFFA857;
const unsigned long K_VOL_DN=0xFFE01F;
const unsigned long K_SOURCE=0xFF629D;
const unsigned long K_SRC_UP=0xFFE21D;
const unsigned long K_SRC_DN=0xFFA25D;
const unsigned long K_PAUSE =0xFFC23D; 

const unsigned int REP_RTMO=120; //ms

#define NPORTS 8
#define NPORTS2 6
#define PORTS_M 3
int ports[NPORTS]={MOTOR_OUT_1,MOTOR_OUT_2, MOTOR_EN, MOTOR_LED, SSW_LED1, SSW_LED2, SSW_SELECT, SSW_HIBERNATE};

IRrecv1 irrecv(RECV_PIN);

decode_results results;

uint32_t lastkey=0;
uint32_t lastms=0;
uint8_t  invol=0;
uint8_t  src=0;
uint8_t  powoff=0;

void setup()
{
  for(uint8_t i=0;i<NPORTS;i++) {
    digitalWrite(ports[i], LOW); 
    pinMode(ports[i], OUTPUT);
  }
  delay(200);
    
#ifdef _AMP_DBG_  
  Serial.begin(9600);
  delay(2000);
  Serial.println("IR TEST++++");
#endif    

  cmd_powoff(0); // will automatially switch to src 0 
  delay(200);

  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  uint32_t ms = millis();
  if(invol) {
    if(ms-lastms>REP_RTMO || ms<lastms) {
      for(uint8_t i=0;i<PORTS_M;i++) digitalWrite(ports[i], LOW); // stop vol
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
    Serial.println(" : ");
#endif    
    switch(results.value) {
      case K_VOL_UP: cmd_vol(2); break;
      case K_VOL_DN: cmd_vol(1); break;
      case K_SRC_UP:
      case K_SRC_DN:
      case K_SOURCE: cmd_src(0xff); break;
      case K_PAUSE: cmd_powoff(0xff); break;
      case K_REP: 
        if(lastkey==K_VOL_UP) cmd_vol(2);
        else if(lastkey==K_VOL_DN) cmd_vol(1);
        break;
      default:;
    }
    if(results.value != K_REP) lastkey=results.value;
    irrecv.resume(); // Receive the next value
  }
}

void cmd_vol(uint16_t d) {
  if(powoff) return;
  if(invol && invol!=d) {
    for(uint8_t i=0;i<PORTS_M;i++) digitalWrite(ports[i], LOW); // stop VOL
    delay(100);   
    invol=0; 
#ifdef _AMP_DBG_          
    Serial.println("VOL STOP ");
#endif          
  }
  if(!invol) {
#ifdef _AMP_DBG_            
    Serial.print("VOL ");
    Serial.print(d==2 ? "UP" : "DN");  
    Serial.println(" START");
#endif    
    invol = d;
    digitalWrite(MOTOR_OUT_1, invol==2 ? LOW : HIGH); digitalWrite(MOTOR_OUT_2, invol==2 ? HIGH : LOW); 
    digitalWrite(MOTOR_EN, HIGH); // or analog write - check the speed or add R (?)
  }
  lastms=millis();
}

void cmd_src(uint8_t s) {
  if(powoff) return;
  if(s != 0xff) src=s;
  else {
    src++;
    if(src==NSRC) src=0;
  }
  digitalWrite(SSW_LED1, src==0 ? HIGH : LOW);
  digitalWrite(SSW_LED2, src==1 ? HIGH : LOW);
  digitalWrite(SSW_SELECT, src==0 ? LOW : HIGH);
  // do switch HERE  
#ifdef _AMP_DBG_            
  Serial.print("SRC ");
  Serial.println(src);  
#endif      
}

void cmd_powoff(uint8_t s) {
  if(s==0 || powoff) { 
    // power up here
    digitalWrite(MOTOR_LED, HIGH);
    digitalWrite(SSW_HIBERNATE, LOW);
    powoff=0;
    cmd_src(src);
#ifdef _AMP_DBG_            
  Serial.println("POWER ON");
#endif          
  } else if(s==1 || !powoff) {
    // power down
    for(uint8_t i=0;i<NPORTS2;i++) digitalWrite(ports[i], LOW); 
    digitalWrite(SSW_HIBERNATE, HIGH);
#ifdef _AMP_DBG_            
  Serial.println("POWER OFF");
#endif              
    powoff=1;
  }
}

