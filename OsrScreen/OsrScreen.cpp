#include "OsrScreen.h"

#include "Osr_Screen_Face.h"
#include "Arduino.h"
#include "RGBmatrixPanel.h"



Screen::Screen(int t) : RGBmatrixPanel(14,15,16,8,10,9,false)
{
	timeout = t;

  face_index = 0;
  face_page_index = 0;
  face_mode = 1;  // 0, face only mode / 1, debug mode
}

void Screen::init_display()
{
	RGBmatrixPanel::begin();

	int temp[]  = {0xFF,0xFF,0xFF};
	int drive[] = {0xFF,0xFF,0xFF};
	int steer[] = {0xFF,0xFF,0xFF};
	
	display_state();
	connected_status(0x0);
	display_battery(0xFF);
	display_status(0xFF);
	display_temp(temp);
	display_currents(drive,steer);
	sleepy_face();
	
	RGBmatrixPanel::begin();
  // RGBmatrixPanel::swapBuffers(true);
	Serial.flush();
}

void Screen::init_comm()
{
	while (1) {
	  if (Serial.read() == 0xA){
	    RGBmatrixPanel::drawPixel(DEBUG[0]+1,DEBUG[1], RGBmatrixPanel::Color444(BLUE));
	    Serial.print(205,HEX);
	    break;
	  }
	  RGBmatrixPanel::drawPixel(DEBUG[0]+1,DEBUG[1], RGBmatrixPanel::Color444(WHITE));
	}
}

void Screen::testpixel(int c)
{
	RGBmatrixPanel::drawPixel(1,1,RGBmatrixPanel::Color444(c,c,c));
}


int Screen::get_state()
{
	return state;
}

int* Screen::get_data()
{	
	if (state)
	{
		RGBmatrixPanel::drawPixel(30,5,RGBmatrixPanel::Color444(5,5,5));
		Screen::init_comm();
		state = RUNNING;
	}
	else 
	{
		RGBmatrixPanel::drawPixel(30,5,RGBmatrixPanel::Color444(BLACK));
		time = millis();
		//while ( (Serial.available()<16) && ((millis() - time) < MAX_MILLIS_TO_WAIT) )
		while ( Serial.available() < 16)
		{
			//do nothing  
		}
		if (Serial.available() > 0)
		{
			for(int n=0; n<16; n++)
			{
				data[n] = Serial.read();
			}
			if (data[CONNECTED_POS] == 0xFF)
			{
				state = IDLE;
			}
			return data;
		}
	}
	return 0;
}


void Screen::display_state(){
  if (state == RUNNING){
    RGBmatrixPanel::drawPixel(STATE[0], STATE[1], RGBmatrixPanel::Color444(GREEN));        
  }
  else if (state == IDLE){
    RGBmatrixPanel::drawPixel(STATE[0], STATE[1], RGBmatrixPanel::Color444(RED));
  } else if (state == 0x00){
    RGBmatrixPanel::drawPixel(STATE[0], STATE[1], RGBmatrixPanel::Color444(WHITE));
  }
}


void Screen::connected_status(int status){
  if (status) {
    RGBmatrixPanel::drawPixel(CONNECT[0], CONNECT[1], RGBmatrixPanel::Color444(GREEN));
  }
  else {
    RGBmatrixPanel::drawPixel(CONNECT[0], CONNECT[1], RGBmatrixPanel::Color444(RED));
  }
}

int Screen::preamble_check(int message[]){
  if (message[0] == PREAMBLE_HIGH && message[1] == PREAMBLE_LOW){
    RGBmatrixPanel::drawPixel(DEBUG[0],DEBUG[1], RGBmatrixPanel::Color444(BLACK));
    return 1;
  }
  else {
    RGBmatrixPanel::drawPixel(DEBUG[0],DEBUG[1], RGBmatrixPanel::Color444(BLUE));
    return 0;
  }
}


int Screen::chksum_check(int message[]){
  int chksum = 0;
  for (int i=2; i < 14; i++){
    chksum += message[i];
  }
  int low = chksum & 0x00FF;
  int high = (chksum & 0xFF00) >> 8;
  if ((high == message[CHKSUM_POS]) && (low == message[CHKSUM_POS +1])){
    RGBmatrixPanel::drawPixel(DEBUG[0],DEBUG[1], RGBmatrixPanel::Color444(BLACK));
    Screen::affirm_message();
    return 1;
  }
  RGBmatrixPanel::drawPixel(DEBUG[0],DEBUG[1], RGBmatrixPanel::Color444(RED));
  return 0;
}

void Screen::affirm_message()
{
	Serial.print(205,HEX);
}

void Screen::display_battery(int battery_level){
  RGBmatrixPanel::drawLine(BATTERY[0],BATTERY[1],BATTERY[0]+4,BATTERY[1],RGBmatrixPanel::Color444(BLACK));
  if (battery_level == 0xFF){
    RGBmatrixPanel::drawLine(BATTERY[0],BATTERY[1],BATTERY[0]+4,BATTERY[1],RGBmatrixPanel::Color444(WHITE));    
  }
  else{
    if (battery_level & 0x10){
      RGBmatrixPanel::drawPixel(BATTERY[0]+4,BATTERY[1], RGBmatrixPanel::Color444(GREEN));
    }
    if (battery_level & 0x08){
      RGBmatrixPanel::drawPixel(BATTERY[0]+3,BATTERY[1], RGBmatrixPanel::Color444(YELLOW_GREEN));
    }
    if (battery_level & 0x04){
      RGBmatrixPanel::drawPixel(BATTERY[0]+2,BATTERY[1], RGBmatrixPanel::Color444(YELLOW));
    }
    if (battery_level & 0x02){
      RGBmatrixPanel::drawPixel(BATTERY[0]+1,BATTERY[1], RGBmatrixPanel::Color444(YELLOW_RED));
    }
    if (battery_level & 0x01){
      RGBmatrixPanel::drawPixel(BATTERY[0],BATTERY[1], RGBmatrixPanel::Color444(RED));
    }   
  }
}


int* Screen::get_color(int num){
  int* color = new int[3];
  if (num == 0x04){
    color[0] = BIT_MAX;
    color[1] = 0;
    color[2] = 0;
  }
  else if (num == 0x03){
    color[0] = BIT_MAX;
    color[1] = 7;
    color[2] = 0;
  }
  else if (num == 0x02){
    color[0] = BIT_MAX;
    color[1] = BIT_MAX;
    color[2] = 0;
  }
  else if (num == 0x01){
    color[0] = 7;
    color[1] = BIT_MAX;
    color[2] = 0;
  }
  else if (num == 0x00){
    color[0] = 0;
    color[1] = BIT_MAX;
    color[2] = 0;
  }
  else {
    color[0] = BIT_MAX;
    color[1] = BIT_MAX;
    color[2] = BIT_MAX;
  }
  return color;
}

void Screen::display_temp(int temp[]){
  int* color;
  for (int i =0; i < 5; i ++){
    color = get_color(temp[i]);
    RGBmatrixPanel::drawPixel(TEMP[0]+i,TEMP[1],RGBmatrixPanel::Color444(color[0],color[1],color[2]));
  }
}

void Screen::display_status(int error_status){
  RGBmatrixPanel::drawLine(STATUS[0],STATUS[1],STATUS[0]+4,STATUS[1],RGBmatrixPanel::Color444(GREEN));
  if (error_status == 0xFF){
    RGBmatrixPanel::drawLine(STATUS[0],STATUS[1],STATUS[0]+4,STATUS[1],RGBmatrixPanel::Color444(WHITE));
  }
  else{
    if (error_status & 0x01){
      RGBmatrixPanel::drawPixel(STATUS[0],STATUS[1],RGBmatrixPanel::Color444(RED));
    }
    if (error_status & 0x02){
      RGBmatrixPanel::drawPixel(STATUS[0]+1,STATUS[1],RGBmatrixPanel::Color444(RED));
    }
    if (error_status & 0x04){
      RGBmatrixPanel::drawPixel(STATUS[0]+2,STATUS[1],RGBmatrixPanel::Color444(RED));
    }
    if (error_status & 0x08){
      RGBmatrixPanel::drawPixel(STATUS[0]+3,STATUS[1],RGBmatrixPanel::Color444(RED));
    }
    if (error_status & 0x10){
      RGBmatrixPanel::drawPixel(STATUS[0]+4,STATUS[1],RGBmatrixPanel::Color444(RED));
    }         
  }
}


void Screen::display_currents(int drive_current[],int steering_current[]){
  int* color;
  for (int i =0; i < 6; i ++){
    color = get_color(drive_current[i]);
    RGBmatrixPanel::drawPixel(DRIVE_CURRENT[0]+i,DRIVE_CURRENT[1],RGBmatrixPanel::Color444(color[0],color[1],color[2]));
  }

  for (int i =0; i < 4; i ++){
    color = get_color(steering_current[i]);
    RGBmatrixPanel::drawPixel(STEER_CURRENT[0]+i,STEER_CURRENT[1],RGBmatrixPanel::Color444(color[0],color[1],color[2]));
  }
}



/**********************************************************************/
// for FACE
/**********************************************************************/

void Screen::clear_face(){
  RGBmatrixPanel::fillRect(0,face_mode,32,16-face_mode,RGBmatrixPanel::Color444(BLACK));
  // RGBmatrixPanel::fillScreen(RGBmatrixPanel::Color444(BLACK));
}


void Screen::display_face(int switch_face_cmd){

  int update_face = 0;

  switch (switch_face_cmd)
  {
    case 0xFF:
      break;
    case SWITCH_FACE_INC:         // Button B to swtich face (increment)
      face_index += 1;
      update_face = 1;
      break;
    case SWITCH_FACE_DEC:         // Button X to swtich face (decrement)
      face_index -= 1;
      update_face = 1;
      break;
    case SWITCH_FACE_PAGE_INC:    // Button Y to swtich face page (increment)
      face_page_index += 1;
      update_face = 1;
      break;
    case SWITCH_FACE_PAGE_DEC:    // Button A to swtich face page (decrement)
      face_page_index -= 1;
      update_face = 1;
      break;
    case SWITCH_FACE_MODE:        // Button START to swtich face mode - 0, face only mode / 1, debug mode
      face_mode = 1 - face_mode;
      update_face = 1;
      break;
    default:
      break;
  }

  if (update_face) {
    clear_face();

    int number_of_page = 2;

    if (face_page_index < 0) face_page_index = number_of_page;
    if (face_page_index > number_of_page) face_page_index = 0;
    
    switch (face_page_index)
    {
      case 0:
        face_page_0_origin();
        break;
      case 1:
        face_page_1_osr_screen_emoji();
        break;
      case 2:
        face_page_2_emoticon();
        break;
      default:
        break;
    }
    
  }
}

void Screen::face_page_0_origin(){

  int number_of_face = 4;

  if (face_index < 0) face_index = number_of_face;
  if (face_index > number_of_face) face_index = 0;
  
	switch (face_index)
  {
    case 0:
      sleepy_face();
      break;
    case 1: 
      happy_face();
      break;
    case 2:
      eight_bit_face();
      break;
    case 3:
      NI_logo();
      break;
    case 4:
      break;
    default:
      break;
  }
}

void Screen::face_page_1_osr_screen_emoji(){
  
  int number_of_face = 7;

  if (face_index < 0) face_index = number_of_face;
  if (face_index > number_of_face) face_index = 0;
  
	switch (face_index)
  {
    // case 0:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_0, 32, 15);
    //   break;
    // case 1:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_1, 32, 15);
    //   break;
    case 0:     //2:
      RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_2, 32, 15);
      break;
    // case 3:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_3, 32, 15);
    //   break;
    // case 4:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_4, 32, 15);
    //   break;
    // case 5:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_5, 32, 15);
    //   break;
    // case 6:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_6, 32, 15);
    //   break;
    // case 7:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_7, 32, 15);
    //   break;
    // case 8:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_8, 32, 15);
    //   break;
    // case 9:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_9, 32, 15);
    //   break;
    // case 10:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_10, 32, 15);
    //   break;
    case 1:     //11:
      RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_11, 32, 15);
      break;
    // case 12:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_12, 32, 15);
    //   break;
    // case 13:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_13, 32, 15);
    //   break;
    case 2:     //14:
      RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_14, 32, 15);
      break;
    // case 15:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_15, 32, 15);
    //   break;
    case 3:     //16:
      RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_16, 32, 15);
      break;
    // case 17:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_17, 32, 15);
    //   break;
    // case 18:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_18, 32, 15);
    //   break;
    // case 19:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_19, 32, 15);
    //   break;
    // case 20:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_20, 32, 15);
    //   break;
    case 4:     //21:
      RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_21, 32, 15);
      break;
    // case 22:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_22, 32, 15);
    //   break;
    // case 23:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_23, 32, 15);
    //   break;
    // case 24:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_24, 32, 15);
    //   break;
    case 5:     //25:
      RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_25, 32, 15);
      break;
    case 6:     //26:
      RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_26, 32, 15);
      break;
    // case 27:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_27, 32, 15);
    //   break;
    // case 28:
    //   RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_28, 32, 15);
    //   break;
    case 7:     //29:
      RGBmatrixPanel::drawRGBBitmap(0, 1, (const uint16_t *)osr_face_emoji_29, 32, 15);
      break;
    default:
      break;
  }
}


void Screen::face_page_2_emoticon(){
  
  int number_of_face = 0;

  if (face_index < 0) face_index = number_of_face;
  if (face_index > number_of_face) face_index = 0;
  
  switch (face_index)
  {
    case 0:
      // emoticon_print_green(2, 6, 1, "NI");
      emoticon_1();
      break;
    // case 1:
    //   // emoticon_print_white(1, 7, 4, "ORZ");
    //   break;
    // case 2:
    //   // emoticon_print_white(1, 0, 4, "(╯°Д°)╯");
    //   break;
    default:
      break;
  }
}


void Screen::update_screen(int message[]){
  if ((preamble_check(message) && chksum_check(message)) || TEST_MODE){
    
    display_face(message[FACE_POS]);

    if (face_mode){                                 // 0, face only mode / 1, debug mode
      connected_status(message[CONNECTED_POS]);
      display_status(message[STATUS_POS]);
      display_battery(message[BATTERY_POS]);

      int temp[]  = {(message[5] & 0x0F),(message[6] & 0xF0) >> 4,(message[6] & 0x0F),(message[7] & 0xF0) >> 4,(message[7] & 0x0F)};
      int drive[] = {(message[8] & 0xF0) >> 4,(message[8] & 0x0F),(message[9] & 0xF0) >> 4,(message[9] & 0x0F),(message[10] & 0xF0) >> 4,(message[10] & 0x0F)};
      int steer[] = {(message[11] & 0xF0) >> 4,(message[11] & 0x0F),(message[12] & 0xF0) >> 4,(message[12] & 0x0F),(message[13] & 0xF0) >> 4,(message[13] & 0x0F)};

      display_temp(temp);
      display_currents(drive,steer);
    }

    // RGBmatrixPanel::swapBuffers(true);
  
  }
}

void Screen::NI_logo(){
  RGBmatrixPanel::drawLine(7,6,7,13,RGBmatrixPanel::Color444(GREEN));
  RGBmatrixPanel::drawLine(8,6,8,13,RGBmatrixPanel::Color444(GREEN));
  RGBmatrixPanel::drawLine(9,6,9,13,RGBmatrixPanel::Color444(GREEN));
  RGBmatrixPanel::drawLine(10,6,10,13,RGBmatrixPanel::Color444(GREEN));
  RGBmatrixPanel::drawLine(11,2,11,5,RGBmatrixPanel::Color444(GREEN));
  RGBmatrixPanel::drawLine(12,2,12,5,RGBmatrixPanel::Color444(GREEN));
  RGBmatrixPanel::drawLine(13,2,13,5,RGBmatrixPanel::Color444(GREEN));
  RGBmatrixPanel::drawLine(14,2,14,5,RGBmatrixPanel::Color444(GREEN));
  RGBmatrixPanel::drawLine(15,2,15,13,RGBmatrixPanel::Color444(GREEN));
  RGBmatrixPanel::drawLine(16,2,16,13,RGBmatrixPanel::Color444(GREEN));
  RGBmatrixPanel::drawLine(17,3,17,13,RGBmatrixPanel::Color444(GREEN));
  RGBmatrixPanel::drawLine(18,4,18,13,RGBmatrixPanel::Color444(GREEN));
  RGBmatrixPanel::drawLine(21,2,21,11,RGBmatrixPanel::Color444(GREEN));
  RGBmatrixPanel::drawLine(22,2,22,12,RGBmatrixPanel::Color444(GREEN));
  RGBmatrixPanel::drawLine(23,2,23,13,RGBmatrixPanel::Color444(GREEN));
  RGBmatrixPanel::drawLine(24,2,24,13,RGBmatrixPanel::Color444(GREEN));
}

/**********************************************************************/
// for face_page_0_origin()
/**********************************************************************/

void Screen::happy_eye(int x, int y, int rgb[]){
  RGBmatrixPanel::drawLine(x-2,y,x,y-2,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  RGBmatrixPanel::drawLine(x+1,y-2,x+3,y,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  
}

void Screen::sleepy_eye(int x, int y, int rgb[]){
  RGBmatrixPanel::drawLine(x-2,y,x+2,y,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
}

void Screen::happy_mouth(int x, int y, int rgb[]){
  RGBmatrixPanel::drawLine(x-4,y,x-2,y+2,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  RGBmatrixPanel::drawLine(x-1,y+2,x+2,y+2,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  RGBmatrixPanel::drawLine(x+3,y+2,x+5,y,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  RGBmatrixPanel::drawPixel(x-4,y-1,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  RGBmatrixPanel::drawPixel(x+5,y-1,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
}

void Screen::circle_cheek(int x, int y, int rgb[]){
  RGBmatrixPanel::drawLine(x-1,y,x-1,y+1,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  RGBmatrixPanel::drawLine(x+2,y,x+2,y+1,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  RGBmatrixPanel::drawLine(x,y-1,x+1,y-1,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  RGBmatrixPanel::drawLine(x,y+2,x+1,y+2,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
}

void Screen::cute_mouth(int x, int y, int rgb[]){
  RGBmatrixPanel::drawLine(x-4,y,x-2,y+2,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  RGBmatrixPanel::drawLine(x-1,y+2,x,y+1,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  RGBmatrixPanel::drawLine(x+1,y+2,x+2,y+2,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  RGBmatrixPanel::drawLine(x+3,y+1,x+4,y,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  RGBmatrixPanel::drawPixel(x-4,y-1,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  RGBmatrixPanel::drawPixel(x+4,y-1,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  RGBmatrixPanel::drawPixel(x,y,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
  
  //RGBmatrixPanel::drawLine(x+3,y+2,x+5,y,RGBmatrixPanel::Color444(r,g,b));
  //RGBmatrixPanel::drawPixel(x-4,y-1,RGBmatrixPanel::Color444(r,g,b));
  //RGBmatrixPanel::drawPixel(x+5,y-1,RGBmatrixPanel::Color444(r,g,b));
}
    
void Screen::cute_cheeks(int x, int y, int rgb[]){
   RGBmatrixPanel::drawLine(x-2,y-1,x+1,y-1,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
   RGBmatrixPanel::drawLine(x-2,y,x+1,y,RGBmatrixPanel::Color444(rgb[0],rgb[1],rgb[2]));
}
    
void Screen::eight_bit_eye(int x, int y){
  RGBmatrixPanel::fillCircle(x,y,2,RGBmatrixPanel::Color444(0,0,15));
  RGBmatrixPanel::drawLine(x-1,y-1,x-1,y+1,RGBmatrixPanel::Color444(3,0,15));
  RGBmatrixPanel::drawLine(x,y+1,x+1,y+1,RGBmatrixPanel::Color444(3,0,15));
  RGBmatrixPanel::drawLine(x,y-1,x+1,y-1,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawLine(x,y,x+1,y,RGBmatrixPanel::Color444(WHITE));
}
    
void Screen::happy_face(){      //0x01
	// clear_face();
	int eye_color[] = {3,0,15};
	int mouth_color[] = {3,0,15};
	int cheek_color[] = {12,0,15};
  
	// happy_eye(10,5,eye_color);
	// happy_eye(18,5,eye_color);
	// cute_mouth(14,12,mouth_color);
	// cute_cheeks(7,9,cheek_color);
	// cute_cheeks(22,9,cheek_color);

  // clear_face();
	happy_eye(11,4+face_mode,eye_color);
	happy_eye(19,4+face_mode,eye_color);
	cute_mouth(15,11+face_mode,mouth_color);
	cute_cheeks(8,8+face_mode,cheek_color);
	cute_cheeks(23,8+face_mode,cheek_color);
}
    
void Screen::sleepy_face(){     //0x02
  int eye_color[] = {3,0,15};
  int mouth_color[] = {3,0,15};
  int cheek_color[] = {12,0,15};
  // sleepy_eye(10,5,eye_color);
  // sleepy_eye(18,5,eye_color);
  // cute_mouth(14,12,mouth_color);
  // cute_cheeks(7,9,cheek_color);
  // cute_cheeks(22,9,cheek_color);

  // clear_face();
  sleepy_eye(11,4+face_mode,eye_color);
  sleepy_eye(19,4+face_mode,eye_color);
  cute_mouth(15,11+face_mode,mouth_color);
  cute_cheeks(8,8+face_mode,cheek_color);
  cute_cheeks(23,8+face_mode,cheek_color);
}

void Screen::eight_bit_face(){  //0x03
  // clear_face();
	int cheek_color[] = {15,0,0};
	int mouth_color[] = {12,0,15};
  
	// eight_bit_eye(10,5);
	// eight_bit_eye(18,5);
	// circle_cheek(5,9,cheek_color);
	// circle_cheek(23,9,cheek_color);
	// happy_mouth(14,11,mouth_color);

  // clear_face();
  eight_bit_eye(11,4+face_mode);
	eight_bit_eye(19,4+face_mode);
	circle_cheek(6,8+face_mode,cheek_color);
	circle_cheek(24,8+face_mode,cheek_color);
	happy_mouth(15,10+face_mode,mouth_color);
}



/**********************************************************************/
// for face_page_2_emoticon()
/**********************************************************************/
// void Screen::emoticon_print_green(uint8_t size, int16_t cursor_x, int16_t cursor_y, String emoticon){
//   RGBmatrixPanel::setTextSize(size);
//   RGBmatrixPanel::setTextColor(RGBmatrixPanel::Color444(GREEN));
//   RGBmatrixPanel::setCursor(cursor_x,cursor_y);
//   RGBmatrixPanel::print(emoticon);
// }

// void Screen::emoticon_print_white(uint8_t size, int16_t cursor_x, int16_t cursor_y, String emoticon){
//   RGBmatrixPanel::setTextSize(size);
//   RGBmatrixPanel::setTextColor(RGBmatrixPanel::Color444(WHITE));
//   RGBmatrixPanel::setCursor(cursor_x,cursor_y);
//   RGBmatrixPanel::print(emoticon);
// }

void Screen::emoticon_1(){ //( ╯▽╰)
  RGBmatrixPanel::drawLine(2,4,2,5,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawLine(1,6,1,9,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawLine(2,10,2,11,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawLine(5,6,7,6,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawPixel(8,5,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawLine(9,3,9,4,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawLine(5,6,7,6,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawLine(14,4,19,4,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawLine(14,5,16,8,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawLine(19,5,17,8,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawLine(24,3,24,4,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawPixel(25,5,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawLine(26,6,28,6,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawLine(29,4,29,5,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawLine(30,6,30,9,RGBmatrixPanel::Color444(WHITE));
  RGBmatrixPanel::drawLine(29,10,29,11,RGBmatrixPanel::Color444(WHITE));
}