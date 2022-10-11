#ifndef OsrScreen
#define OsrScreen

#include "RGBmatrixPanel.h"


#define BIT_MAX  				15
#define RED 					BIT_MAX,0,0
#define GREEN 					0,BIT_MAX,0
#define BLUE					0,0,BIT_MAX
#define BLACK					0,0,0
#define WHITE					BIT_MAX,BIT_MAX,BIT_MAX

#define YELLOW_GREEN 			7,BIT_MAX,0
#define YELLOW					BIT_MAX,BIT_MAX,0
#define YELLOW_RED				BIT_MAX,7,0

#define PREAMBLE_HIGH 			0xAB
#define PREAMBLE_LOW  			0xCD

#define CONNECTED_POS			2
#define BATTERY_POS				3
#define STATUS_POS				4
#define TEMP_POS				5
#define DRIVE_POS				8
#define STEER_POS				11
#define FACE_POS				13
#define CHKSUM_POS				14
#define MAX_MILLIS_TO_WAIT		10000
#define RUNNING					0x00
#define IDLE					0x01

#define SWITCH_FACE_INC         1	// Button B to swtich face (increment)
#define SWITCH_FACE_DEC         2	// Button X to swtich face (decrement)
#define SWITCH_FACE_PAGE_INC    3	// Button Y to swtich face page (increment)
#define SWITCH_FACE_PAGE_DEC    0	// Button A to swtich face page (decrement)
#define SWITCH_FACE_MODE        7	// Button START to swtich face mode - debug / face only


class Screen: public RGBmatrixPanel {
	int timeout;
	int l_eye[2];
	int r_eye[2];
	int mouth[2];
	int l_cheek[2];
	int r_cheek[2];
	int l_brow[2];
	int r_brow[2];
	int* data = new int[16];

    int face_index = 0;
    int face_page_index = 0;
    int face_mode = 1;  // 0, face only mode / 1, debug mode

    int TEST_MODE			= 0;
    int state				= IDLE;
    int CONNECT[2] 			= {0,0};    // {0,1};
    int BATTERY[2] 			= {3,0};    // {0,0};
    int STATUS[2]  			= {9,0};    // {6,0};
    int TEMP[2]    			= {15,0};   // {12,0};
    int DRIVE_CURRENT[2] 	= {21,0};   // {19,0};
    int STEER_CURRENT[2] 	= {28,0};   // {26,0};
    int DEBUG[2]         	= {0,8};
    int STATE[2]          	= {1,0};    // {1,1};

public:
    
    unsigned long time;

    Screen(int);
    void init_display();
    void init_comm();
    void testpixel(int);
    int * get_data();
    void connected_status(int);
    int preamble_check(int[]);
    int chksum_check(int[]);
    void affirm_message();
    int get_state();

    void display_state();
    void display_battery(int);
    int* get_color(int);
    // void display_face(int);
    void display_temp(int[]);
    void display_status(int);
    void display_currents(int[], int[]);


/**********************************************************************/
// for FACE
/**********************************************************************/
    void clear_face();

    void display_face(int);
    void face_page_0_origin();
    void face_page_1_osr_screen_emoji();
    void face_page_2_emoticon();

    void update_screen(int[]);

/**********************************************************************/
// for face_page_0_origin()
/**********************************************************************/
    void happy_eye(int, int, int[]);
    void sleepy_eye(int, int, int[]);
    void happy_mouth(int, int, int[]);
    void circle_cheek(int,int,int[]);
    void cute_mouth(int,int,int[]);
    void cute_cheeks(int,int,int[]);
    void eight_bit_eye(int,int);
    void happy_face();
    void sleepy_face();
    void eight_bit_face();

/**********************************************************************/
// for face_page_2_emoticon()
/**********************************************************************/
    void emoticon_print_green(uint8_t, int16_t, int16_t, String);
    void emoticon_print_white(uint8_t, int16_t, int16_t, String);

};

#endif




