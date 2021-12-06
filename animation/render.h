#ifndef __render_h_
#define __render_h_

#include "../def.h"

class render
{
private:
  static render &This;

  /*
   * Window callbacks.
   */

  /* Frame redraw callback function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  static VOID OnDisplay( VOID )
  {
    render* This = (render*)glutGetWindowData();
    This->Response();

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    This->Render();

    glFinish();
    glutSwapBuffers();
    glutPostRedisplay();
  } /* End of 'OnDisplay' function */

  /* OnKeyboard buttons down hanlde callback function.
   * ARGUMETNS:
   *   - keyboard button kode:
   *       unsigned char Key;
   *   - mouse pointer coordinates:
   *       INT X, INT Y;
   * RETURN: None;
   */
  static VOID OnKeyboard( unsigned char Key, INT X, INT Y )
  {
    render *This = (render *)glutGetWindowData();
    This->KeyboardClick(Key, X, Y);
  } /* End of 'OnKeyboard' function */

  /* Mouse buttons state change callback function.
   * ARGUMETNS:
   *   - mouse button kode:
   *       INT Button;
   *   - mouse button state code:
   *       INT State;
   *   - mouse pointer coordinates:
   *       INT X, INT Y;
   * RETURN: None;
   */
  static VOID OnMouse( INT Button, INT State, INT X, INT Y )
  {
    render *This = (render *)glutGetWindowData();
    This->MouseClick(Button, State, X, Y);
  } /* End of 'OnMouse' function */

  /* Mouse move action callback function.
   * ARGUMETNS:
   *   - mouse pointer coordinates:
   *       INT X, INT Y;
   * RETURN: None;
   */
  static VOID OnMotion( INT X, INT Y )
  {
    render *This = (render *)glutGetWindowData();
    This->MouseMotion(X, Y);
  } /* End of 'OnMotion' function */

  /* Window resize callback function.
   * ARGUMENTS:
   *   - new window size:
   *       INT NewW, INT NewH;
   * RETURNS: None.
   */
  static VOID OnReshape( INT NewW, INT NewH )
  {
    render *This = (render *)glutGetWindowData();

    This->W = NewW, This->H = NewH;
    This->Resize(NewW, NewH);
    glViewport(0, 0, This->W, This->H);
  } /* End of 'OnReshape' function */

protected:
  INT W, H; // Window size

  /*
   * Virtual sub-API callback.
   */

  /* Renser response function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  virtual VOID Response( VOID )
  {
  } /* End of 'Render' function */

  /* Frame redraw function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  virtual VOID Render( VOID )
  {
  } /* End of 'Render' function */

  /* Keyboard buttons down hanlde function.
   * ARGUMETNS:
   *   - keyboard button kode:
   *       unsigned char Key;
   *   - mouse pointer coordinates:
   *       INT X, INT Y;
   * RETURN: None;
   */
  virtual VOID KeyboardClick( unsigned char Key, INT x, INT y )
  {
  } /* End of 'KeyboardClick' function */

  /* Mouse buttons state change hanlde function.
   * ARGUMETNS:
   *   - mouse button kode:
   *       INT Button;
   *   - mouse button state code:
   *       INT State;
   *   - mouse pointer coordinates:
   *       INT X, INT Y;
   * RETURN: None;
   */
  virtual VOID MouseClick( INT Button, INT State, INT X, INT Y )
  {
  } /* End of 'MouseClick' function */

  /* Mouse move hanlde function.
   * ARGUMETNS:
   *   - mouse pointer coordinates:
   *       INT X, INT Y;
   * RETURN: None;
   */
  virtual VOID MouseMotion( INT X, INT Y )
  {
  } /* End of 'MouseMotion' function */

  /* Window resize reaction function.
   * ARGUMENTS:
   *   - new window size:
   *       INT NewW, INT NewH;
   * RETURNS: None.
   */
  virtual VOID Resize( INT NewW, INT NewH )
  {
  } /* End of 'Resize' function */

public:
  /* Render class default constructor.
   * ARGUMENTS:
   *   - application tun parametrs:
   *       INT argc, char *argv[];
   *   - window name:
   *       const std::string &WindowName;
   *   - window size:
   *       INT WindowW, INT WindowH;
   */
  render( INT argc, char *argv[], const std::string &WindowName, INT WindowW = 700, INT WindowH = 500 )
  {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    
    W = WindowW, H = WindowH;
    glutInitWindowSize(WindowW, WindowH);
    glutInitWindowPosition(0, 0);
    glutCreateWindow(WindowName.c_str());
    glutSetWindowData(this);

    glutDisplayFunc(OnDisplay);
    glutKeyboardFunc(OnKeyboard);
    glutReshapeFunc(OnReshape);
    glutMouseFunc(OnMouse);
    glutMotionFunc(OnMotion);
  } /* End of 'render' function */

  /* Run window loop function.
   * ARGUMETNS: None.
   * RETURNS: None.
   */
  VOID Run( VOID )
  {
    glutMainLoop();
  } /* End of 'Run' function */

}; /* end of 'render' class */

#endif /* __render_h_ */