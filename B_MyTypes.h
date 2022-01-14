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


typedef struct rect {
  int x;
  int y;
  int xmax;
  int ymax;
} rect;

typedef struct buttondef {
  int h;
  int w;
  uint16_t color ;
  char label[8];
  uint8_t fontsize;
  rect Rect;
} buttondef;

typedef int (*ClickHandler) (const int MenuID);

typedef struct buttonsetdef {
  rect bounding = {999,999,-1,-1};
  buttondef* buttondefs = 0;
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
