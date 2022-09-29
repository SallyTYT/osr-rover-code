// testcolors demo for Adafruit RGBmatrixPanel library.
// Renders 512 colors on our 16x32 RGB LED matrix:
// http://www.adafruit.com/products/420
// Library supports 4096 colors, but there aren't that many pixels!  :)

// Written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon
// for Adafruit Industries.
// BSD license, all text above must be included in any redistribution.

#include <RGBmatrixPanel.h>
#include <OsrScreen.h>

Screen screen(1);
int* data = 0;
void setup() 
{
  Serial.begin(9600);
  screen.init_display();

  // screen.RGBmatrixPanel::begin();
}

void loop() 
{
  data = screen.get_data();
  if (data){
    screen.update_screen(data);
  }
  data = 0;

  // screen.eight_bit_face();
  // delay(1000);
  // screen.happy_face();
  // delay(1000);
  // screen.sleepy_face();
  // delay(1000);
  
}  
