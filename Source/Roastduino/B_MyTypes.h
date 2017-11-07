#ifndef B_MyTypes_h

#define B_MyTypes_h

#include <WString.h>

  
typedef struct rect{
      int xmin = 999;
      int ymin = 999;
      int xmax = -1;
      int ymax = -1;
  } rect;

typedef struct buttondef{
      int x;
      int y;
      int h;
      int w;
      uint16_t color;
      char label[8];
      int fontsize;
      rect Rect;
      
  } buttondef;


#endif
