#ifndef B_MyTypes_h

#define B_MyTypes_h

#include <WString.h>

typedef struct PWMSetpoint{
 byte PWM = 0;
 byte Minutes = 0;
};

typedef struct point {
  int x = 0 ;
  int y = -1;
 } point;

typedef struct graphhistory {
  int LineID  ;
  int SkipCount;
  int SkipLimit;
  int PixelsP;
  int ArraySize;
  point *Pixels;
 } graphhistory;



typedef struct labelID {
  int MenuID = -1 ;
  int ButtonID = -1 ;
 } lableID;


typedef struct rect {
  int x;
  int y;
  int xmax;
  int ymax;
} rect;

typedef struct indexed10strings {
    char s_0 [12];
    char s_1 [12];
    char s_2 [12];
    char s_3 [12];
    char s_4 [12];
    char s_5 [12];
    char s_6 [12];
    char s_7 [12];
    char s_8 [12];
    char s_9 [12];
} indexedstrings;


typedef struct buttontext {
    byte key;
    char label[6] ;
    char tip1[12] ;
    char tip2[12] ;
    char tip3[12] ;
    uint16_t color;
} buttontext;

typedef struct buttondef {
  //int h;
  //int w;
  boolean ToolTipShowing = false;
  rect Rect;
  lableID AlternateLableID;
} buttondef;

typedef int (*ClickHandler) (const int MenuID);

typedef struct buttonsetdef {
  rect bounding = {999,999,-1,-1};
  buttondef buttondefs[9];
  int menuID = -1;
  int ButtonCount = 0;
  int W = 0;
  int H = 0;
  int colstart = 0;
  int rowstart = 0;
  boolean vertical;
  boolean visable;
  ClickHandler ClickHandler;
  int nextMenu;
  int backMenu;
  int inputbutton;
  int priormenuID;
  
} buttonsetdef;



typedef struct  setpoint {
  
  int Minutes;
  int TemperatureDefault ;
  int Temperature  ;
  int SpanMinutes  ;
  int TemperatureNew ;
  
} roastdef;



#endif
