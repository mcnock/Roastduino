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


//typedef struct rect {
//  int xmin = 999;
//  int ymin = 999;
//  int xmax = -1;
//  int ymax = -1;
//} rect;

typedef struct rect {
  int xmin;
  int ymin;
  int xmax;
  int ymax;
} rect;

typedef struct buttondef {
  int x;
  int y;
  int h;
  int w;
  uint16_t color;
  char label[8];
  int fontsize;
  rect Rect;

} buttondef;

typedef struct buttonsetdef {
  rect bounding = {999,999,-1,-1};
  buttondef* buttondefs = 0;
  int Count = 0;
} buttonsetdef;


typedef struct  roastdef {
  int MySpanMinutesLength[6];
  int MySetpointTempuratures[6];
  int TempRoastDone;
} roastdef;




#endif
