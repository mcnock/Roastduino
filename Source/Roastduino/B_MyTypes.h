#ifndef B_MyTypes_h

#define B_MyTypes_h

#include <WString.h>

typedef struct buttondef
  {
      int x;
      int y;
      int h;
      int w;
      uint16_t color;
      char label[8];
      int fontsize;
      
  } buttondef;
#endif
