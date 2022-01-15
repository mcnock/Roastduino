#ifndef B_MyTypes_h

#define B_MyTypes_h

#include <WString.h>


typedef struct pointdb {
  double x ;
  double y ;
 } pointdb;

typedef struct point {
  int x ;
  int y ;
 } point;

typedef struct labelID {
  int MenuID ;
  int ButtonID ;
 } lableID;


typedef struct rect {
  int x;
  int y;
  int xmax;
  int ymax;
} rect;

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
  lableID AlernatelableID;
} buttondef;

typedef int (*ClickHandler) (const int MenuID);

typedef struct buttonsetdef {
  rect bounding = {999,999,-1,-1};
  buttondef buttondefs[9];
  int menuID = -1;
  int Count = 0;
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
  
} buttonsetdef;



typedef struct  setpoint {
  
  int Minutes;
  int TemperatureDefault ;
  int Temperature  ;
  int SpanMinutes  ;
  int TemperatureNew ;
  
} roastdef;



#endif
