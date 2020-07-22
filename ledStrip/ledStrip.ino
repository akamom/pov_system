#include <Adafruit_NeoPixel.h>




#define PIN            6
#define NUMPIXELS      10

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN);

int delayval = 500;

void setup() {
  
  pixels.begin();
  pixels.show();
  Serial.begin(9600);
}

void loop() {
  for(int i=0;i<NUMPIXELS;i++){
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(pixels.getPixelColor(i));
    pixels.setPixelColor(i, 255,255,255); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(delayval); // Delay for a period of time (in milliseconds).
  }
}
