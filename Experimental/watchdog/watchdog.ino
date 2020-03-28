
#include <avr/wdt.h>// librairie pour le wathdogg

int count=0;
void setup() {
  Serial.begin(9600);
  Serial.println("Hello World");
   wdt_enable(WDTO_2S); // Max 8s, ici setté à 2s
}

void loop() {
    count++;
    Serial.println(count);
    delay(count*500);
    wdt_reset(); // On reset pour ne pas que le watchdog s'exécute
}
