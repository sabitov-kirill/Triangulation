#include "animation/render.h"
#include "animation/input.h"
#include "location/location.h"

/* Location test scene class. */
class location_test : public render, private input, private location
{
private:
  INT MouseWheel;
  BOOL
    IsPerpendicular = FALSE,
    IsSegmentStarted = FALSE,
    IsPrimitiveStarted = FALSE;
  std::pair<vec2, vec2> SegmentPreview;

  /* Translate pixel coordinates to 0-1.
   * ARGUMENTS:
   *   - coordinates in pixels:
   *       INT X, INT Y;
   * RETURNS:
   *   (vec2) 0-1 coordinates.
   */
  vec2 GetScreenCoord( INT X, INT Y )
  {
    return vec2((FLT)X / W * 2 - 1, -(FLT)Y / H * 2 + 1);
  } /* End of 'GetScreenCoord' function */

  /* Scene response function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  VOID Response(VOID) override final
  {
    input::Response();

    // Changin perpendicular segments building state
    if (KeysClick['Q'])
      IsPerpendicular = !IsPerpendicular;

    // Starting new segment
    if (Keys[VK_RBUTTON] && !KeysOld[VK_RBUTTON])
    {
      if (IsPrimitiveStarted)
        SegmentPreview.first = CurrPolyLastLineEnd();
      else
        SegmentPreview.first = GetScreenCoord(Mx, My);
      FindPoint(SegmentPreview.first, TRUE, &SegmentPreview.first);

      IsSegmentStarted = TRUE;
    }

    // Moving end of started segment
    else if (Keys[VK_RBUTTON] && IsSegmentStarted)
    {
      SegmentPreview.second = GetScreenCoord(Mx, My);
      if (IsPrimitiveStarted && IsPerpendicular)
        SegmentPreview.second = CurrPolyGetPerpPoint(SegmentPreview.second);
      FindPoint(SegmentPreview.second, TRUE, &SegmentPreview.second);
    }

    // End segment
    else if (!Keys[VK_RBUTTON] && KeysOld[VK_RBUTTON])
    {
      IsPrimitiveStarted = !CurrPolyPlaceSegment(SegmentPreview.second, SegmentPreview.first);
      IsSegmentStarted = FALSE;
    }
  } /* End of 'Response' fnuction */

  /* Scene response function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  VOID Render( VOID ) override final
  {
    location::Draw();

    // Draw started segment preview
    if (IsSegmentStarted)
    {
      glBegin(GL_LINES);
      glColor3f(0.1f, 0.2f, 0.8f);
      glVertex2d(SegmentPreview.first[0], SegmentPreview.first[1]);
      glVertex2d(SegmentPreview.second[0], SegmentPreview.second[1]);
      glEnd();
    }
  } /* End of 'Render' function */

public:
  /* Scene render and response function.
   * ARGUMENTS:
   *   - console params:
   *       INT argc, char *argv[];
   * RETURNS: None.
   */
  location_test( INT argc, char *argv[] ) :
    render(argc, argv, "TRIANGULATION. CGSG FOREVER.", 800, 800), input(FindWindow(NULL, "TRIANGULATION. CGSG FOREVER."), MouseWheel)
  {
  } /* End of 'location_test' constructor */
}; /* end of 'location_test' class */

/* Main programm window.
 * ARGUMENTS:
 *   - console params:
 *       INT argc, char *argv[];
 * RETURNS:
 *   (INT) prgoramm end code.
 */
INT main( INT argc, char *argv[] )
{
  location_test TriWin(argc, argv);
  TriWin.Run();

  return 0;
} /* End of 'main' function. */