#ifndef __input_h_
#define __input_h_

#include "def.h"

#pragma comment(lib, "winmm")

/* Mouse handle class. */
class mouse 
{
private:
  INT &Wheel; // mouse wheel
  HWND hWnd;  // handle window discribtor

public:
  INT Mx, My, Mz, Mdx, Mdy, Mdz; // coordinates for mouse

  /* Mouse constructor function.
  * ARGUMENTS:
  *   - window handle:
  *       HWND hWnd;
  *   - mouse wheel:
  *       int &WheelRef;
  * RETURNS: None.
  */
  mouse( HWND hWnd, INT &WheelRef ) : Wheel(WheelRef), hWnd(hWnd), 
    Mx(0), My(0), Mz(0), Mdx(0), Mdy(0), Mdz(0)
  {
    POINT pt;

    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);

    Mx = pt.x, My = pt.y;
  } /* End of 'mouse' function */

  /* Mouse interframe response function.
  * ARGUMENTS: None.
  * RETURNS: None.
  */
  VOID Response( VOID )
  {
    POINT pt;

    /* Obtain position */
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);

    /* Delta (relative) values */
    Mdx = pt.x - Mx;
    Mdy = pt.y - My;

    /* Absolute values */
    Mx = pt.x;
    My = pt.y;

    /* Wheel (z) */
    Mdz = Wheel;
    Mz += Mdz;
    Wheel = 0;
  } /* End of 'Response' function */
}; /* End of 'mouse' class */

/* Keyboard handle class. */
class keyboard 
{  
public:
  BYTE Keys[256];      // array of keys
  BYTE KeysOld[256];   // array of old clicked keys
  BYTE KeysClick[256]; // array of clicked keys

  /* Keyboard interframe init function.
  * ARGUMENTS: None.
  * RETURNS: None.
  */ 
  keyboard( VOID )
  {
    memset(KeysOld, 0, 256);
    memset(Keys, 0, 256);
  } /* End of 'keyboard' function */

  VOID Response( VOID )
  {
    memcpy(KeysOld, Keys, 256);
    GetKeyboardState(Keys);
    for (INT i = 0; i < 256; i++)
    {
      Keys[i] >>= 7;
      KeysClick[i] = Keys[i] && !KeysOld[i];
    }
  } /* End of 'Response' function */
}; /* End of 'keyboard' class */

// class input
class input : public keyboard, public mouse
{
public:
  /* Input( constructor ) init function.
  * ARGUMENTS: None.
  * RETURNS: None.
  */ 
  input( HWND hWnd, INT &WheelRef ) : mouse(hWnd, WheelRef), keyboard()
  {
  } /* End of 'input' function */
    
  /* Input interframe response function.
  * ARGUMENTS: None.
  * RETURNS: None.
  */ 
  VOID Response( VOID )
  {
    keyboard::Response();
    mouse::Response();
  } /* End of 'Response' function */
}; /* end of 'input' class */

#endif /* __input_h_ */