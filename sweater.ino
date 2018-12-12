#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_NeoPixel.h>

#define PIN 9
#define PIXELCOUNT 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are good but we are using 9)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products. product we are using)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs. product we are using)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELCOUNT, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

// Adjust the R, G, B from 0-255 and
// add new {nnn, nnn, nnn}, for more colors
//                                  R   G   B
uint8_t myFavoriteColors[][3] = {{255, 255, 255},   // white
                                 {255,   0,   0},   // red 
                                 {  0, 255,   0},   // green
                               };

// don't edit the line below
#define FAVCOLORS sizeof(myFavoriteColors) / 3

// this number adjusts the motion sensitivity
// lower number = more sensitive
#define MOVE_THRESHOLD 5    //super sensitive currently

void setup() 
{
  Serial.begin(9600);
  
  // Try to initialise and warn if we couldn't detect the chip
  // Use Serial Monitor to view the print out
  if (!accel.begin())
  {
    Serial.println("We got a problem. It's you, not me... unable to initialize the LSM303. I'd start with a quick check on the wiring");
    while (1);
  }
  strip.begin();
  strip.show(); // Set pixels to 'off'
}

void loop() 
{
  /* Get a new sensor event */ 
  sensors_event_t event; 
  accel.getEvent(&event);
//  Serial.print("Accel X: "); Serial.print(event.acceleration.x); Serial.print(" ");
//  Serial.print("Y: "); Serial.print(event.acceleration.y);       Serial.print(" ");
//  Serial.print("Z: "); Serial.print(event.acceleration.z);     Serial.print(" ");

  // Get the magnitude (length) of the 3 axis vector
  // http://en.wikipedia.org/wiki/Euclidean_vector#Length
  double storedVector = event.acceleration.x*event.acceleration.x;
  storedVector += event.acceleration.y*event.acceleration.y;
  storedVector += event.acceleration.z*event.acceleration.z;
  storedVector = sqrt(storedVector);
//  Serial.print("Len: "); Serial.println(storedVector);
  
  // wait a bit
  delay(250);
  
  // get new data!
  accel.getEvent(&event);
  double newVector = event.acceleration.x*event.acceleration.x;
  newVector += event.acceleration.y*event.acceleration.y;
  newVector += event.acceleration.z*event.acceleration.z;
  newVector = sqrt(newVector);
//  Serial.print("New Len: "); Serial.println(newVector);
  
  // are we moving yet?
  if (abs(newVector - storedVector) > MOVE_THRESHOLD) {
    Serial.println("Flashy! Flash! Flash! McFlash!");
    flashRandom(10, 2);  // first number is 'wait' delay, shorter num == shorter twinkle
    flashRandom(10, 4);  // second number is how many neopixels to simultaneously light up
    flashRandom(10, 6);
  }
}

void flashRandom(int wait, uint8_t howmany) {

  for(uint16_t i=0; i<howmany; i++) {
    // randomly chosen from your favorite colors
    int c = random(FAVCOLORS);
    int white = myFavoriteColors[c][0];
    int red = myFavoriteColors[c][1];
    int green = myFavoriteColors[c][2]; 

    //the pixels to turn on in an order
    for (int i=0; i < 6; i++)
      int j = strip.numPixels();
      Serial.print("Pixel on"); Serial.println(i);
  
    // now we will 'fade' it in 3 steps
    for (int x=0; x < 3; x++) {
      int w = white * (x+1); w /= 3;
      int r = red * (x+1); r /= 3;
      int g = green * (x+1); g /= 3;
      
      strip.setPixelColor(i, strip.Color(w, r, g));
      strip.show();
      delay(wait);
    }
    // & fade out in 3 steps
    for (int x=3; x >= 0; x--) {
      int w = white * x; w /= 3;
      int r = red * x; r /= 3;
      int g = green * x; g /= 3;
      
      strip.setPixelColor(i, strip.Color(w, r, g));
      strip.show();
      delay(wait);
    }
  }
  // LEDs will be off when done (they are faded to 0)
}
