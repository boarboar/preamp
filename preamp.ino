
#include <IRremote.h>

const int RECV_PIN = P2_2;
// REP FFFFFFFF
// VUP FFA857
// VDN FFE01F
// CHDN FFA25D
// CH FF629D
// CHUP FFE21D
// PAU 286101BD

//TMO 100ms




IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  Serial.begin(9600);
  delay(5000);
  Serial.println("IR TEST++++");
  irrecv.enableIRIn(); // Start the receiver
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.print(millis());
    Serial.print(" : ");
    Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  }
}
