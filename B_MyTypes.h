#ifndef B_MyTypes_h

#define B_MyTypes_h

#include <WString.h>

struct flowsetpoint {
  float flow = 0;
  byte Minutes = 0;
};

typedef struct adjustmentlabels {
  char label[7];
} adjustmentlabels;

typedef struct point_byte {
  byte x = 0;
  byte y = -1;
} point_byte;

typedef struct point {
  int x = 0;
  int y = -1;
} point;

typedef struct opticalflow {
  Bitcraze_PMW3901 sensor;
  Average<int,int16_t> Avg;
  int YflowReadingskipped;
  int error;
  int BeanReadingPerSensor;
  int BeanReadingPerSensorCalc;
  int YflowReadingstrimmed;
  int BeanYflowCurrent;
} opticalflow;

typedef struct thermocouple {
  MAX6675 sensor;
  int Readingskipped;
} thermocouple;



typedef struct graphhistory {
  int LineID;
  //int SkipCount;
  //int SkipLimit;
  int pixelperlogging;
  //int LastUnMappedXPixelLogged;
  int PixelsP;
  int ArraySize;
  point_byte* Pixels;
} graphhistory;


typedef struct labelID {
  int MenuID = -1;
  int ButtonID = -1;
} lableID;

typedef struct rect {
  int x;
  int y;
  int xmax;
  int ymax;
} rect;

typedef struct indexed10strings {
  char s_0[12];
  char s_1[12];
  char s_2[12];
  char s_3[12];
  char s_4[12];
  char s_5[12];
  char s_6[12];
  char s_7[12];
  char s_8[12];
  char s_9[12];
} indexedstrings;

typedef struct buttontext {
  int butID;
  char label[8];
  char tip1[12];
  char tip2[12];
  char tip3[12];
  uint16_t color;
  byte action;
  byte adjustmentvalueset;
} buttontext;

typedef struct activeadjustment {
  int VmenuWhenCalled = -1;
  int ButtonWhenCalled = -1;
  int name = -1;
  int adjustmentvalueset = -1;
  float moveamount = -1;
  int spSelected = -1;
  bool savemenurequired = false;
} activeadjustment;


typedef struct buttondef {
  //int h;
  //int w;
  int butID = -1;
  boolean ToolTipShowing = false;
  rect Rect;
  int action = -1;
  int adjustmentvalueset;
} buttondef;

typedef void (*ClickHandler)(const int MenuID);

typedef struct buttonsetdef {
  rect bounding = { 999, 999, -1, -1 };
  buttondef buttondefs[9];
  int menuID = -1;
  int ButtonCount = -1;
  int W = 0;
  int H = 0;
  int colstart = 0;
  int rowstart = 0;
  boolean vertical;
  boolean visable;

  ClickHandler pClickHandler;


} buttonsetdef;

typedef struct error_status {
  int error = -1;
  int lasterror = -1;
  bool errorcleared = true;
  boolean newerrmsg = false;
  
} errorstatus;

typedef struct error {
  byte errorID;
  char line1[75];
} error;

typedef struct displaybox {
byte DisplayboxID;
rect Rect;
byte X_EP;
byte Y_EP;
byte rowmax;
rect RectHeader;
bool calcsdone ;

int rows[20];
int cols[5];
byte colmax;
} displaybox;

typedef struct menustatus {
  int VmenuShowing = -1;
  int VmenuPrior = -1;
  //int VbuttonClicked;
  int VmenubuttonClicked;
  buttonsetdef* TouchButtonSet;
  int ButtonClicked;
  boolean IsVmenu;
} menustatus;

typedef struct setpoint {

  byte Minutes;
  int Temperature;
  int SpanMinutes;
  //int TemperatureNew;

} roastdef;

typedef struct touchstatus{
   byte objectpressID = 3;
   int Xdown;
   int Ydown;
   int dragx;
   int dragy;
   int dragxlastdrawn;
   int dragylastdrawn;
} touchstatus;

typedef struct ssrstatus{
  char status[5];
} ssrstatus;

#endif
