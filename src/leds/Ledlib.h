//
//SMALL LED PACKAGE
//JWH - DECEMBER 2000
//
#ifndef _ledlib_h_
#define _ledlib_h_

typedef enum {LED_RED=0, LED_GREEN=2, LED_YELLOW=4, LED_BLUE=6, LED_GREY=8} LEDLIB_COLOR;
typedef enum {LED_ROUND=0, LED_RECT=9, LED_SQUARE=18, LED_VVUMETRE=27, LED_HVUMETRE=36} LEDLIB_SHAPE;
typedef enum {LED_ON=0, LED_OFF=1} LEDLIB_MODE;

typedef int HLED;
#define LEDLIB_WRONG -1         //Return value if something wrong
#define LEDLIB_STANDARD_GAP 12  //To space the leds
#define LEDLIB_SMALL_GAP 5      //To space the vumetres

/////////////////////////////////////////
char* ledlib_start(void);  //MANDATORY
void  ledlib_end  (void);  //DIDO

HLED ledlib_create     (void); //Create a led without properties ! ???
HLED ledlib_create_init(HWND, int, int, LEDLIB_SHAPE, LEDLIB_COLOR, LEDLIB_MODE); //Create a led ans init properties

//Each accessor returns the previous value for its property
HWND ledlib_set_hwnd         (HLED, HWND);          //Set property
int  ledlib_set_x            (HLED, int);           //Set property
int  ledlib_set_y            (HLED, int);           //Set property
LEDLIB_COLOR ledlib_set_color(HLED, LEDLIB_COLOR);  //Set property
LEDLIB_SHAPE ledlib_set_shape(HLED, LEDLIB_SHAPE);  //Set property
LEDLIB_MODE  ledlib_set_mode (HLED, LEDLIB_MODE);   //Set property

void ledlib_draw          (HLED);        //Draw a single led
void ledlib_draw_tab      (HLED *, int); //Draw a tab of leds
void ledlib_draw_swap_mode(HLED);        //Swap the led state and draw it

//NAMEWARE Concept: If you find this useful, please mail
//  once: your name, country and city to jep-nware@altern.org
//  That's all.

#endif

