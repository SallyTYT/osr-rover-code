// testcolors demo for Adafruit RGBmatrixPanel library.
// Renders 512 colors on our 16x32 RGB LED matrix:
// http://www.adafruit.com/products/420
// Library supports 4096 colors, but there aren't that many pixels!  :)

// Written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon
// for Adafruit Industries.
// BSD license, all text above must be included in any redistribution.

#include <RGBmatrixPanel.h>
#include <OsrScreen.h>
#include <Osr_Screen_Face.h>

Screen screen(1);
int* data = 0;

//int aswitch[] = {171, 205, 1, 15, 0, 0, 34, 34, 0, 0, 0, 0, 0, 1, 0, 85};
//int apage[] = {171, 205, 1, 15, 0, 0, 34, 34, 0, 0, 0, 0, 0, 3, 0, 87};
//int amode[] = {171, 205, 1, 15, 0, 0, 34, 34, 0, 0, 0, 0, 0, 7, 0, 91};
//int b[] = {171, 205, 1, 15, 0, 0, 34, 34, 0, 0, 0, 0, 0, 255, 1, 83};


void setup() 
{
  Serial.begin(9600);
  screen.init_display();

//  screen.RGBmatrixPanel::begin();
//  screen.update_screen(apage);
//  screen.update_screen(aswitch);
//  screen.update_screen(amode);
//  screen.update_screen(b);
}

void loop() 
{
  data = screen.get_data();
  if (data){
    screen.update_screen(data);
  }
  data = 0;
//  delay(1000);

//  screen.emoticon_print_white(1, 7, 4, "ORZ");
//  screen.emoticon_print_green(2, 6, 1, "NI");
//  screen.display_face(0xFF);
//  screen.update_screen(b);
  
}
