#include "render.h"
#include "input.h"
#include "location.h"

class triangulation_representation : public render, private input, private location
{
private:
  BOOL
    IsRightButton = FALSE,
    IsLeftButton = FALSE,
    IsMiddleButton = FALSE,
    IsPerpendicular = FALSE,
    IsSegmentStarted = FALSE,
    IsPrimitiveStarted = FALSE;
  INT MouseWheel;
  std::pair<vec2, vec2> SegmentPreview;

  vec2 GetScreenCoord( INT X, INT Y )
  {
    return vec2((FLT)X / W * 2 - 1, -(FLT)Y / H * 2 + 1);
  }

  VOID Render( VOID ) final
  {
    //
    // Response
    //
    input::Response();

    if (KeysClick['Q'])
      IsPerpendicular = !IsPerpendicular;

    if (Keys[VK_RBUTTON] && !KeysOld[VK_RBUTTON])
    {
      if (IsPrimitiveStarted)
        SegmentPreview.first = CurrPolyLastLineEnd();
      else
        SegmentPreview.first = GetScreenCoord(Mx, My);
      FindPoint(SegmentPreview.first, TRUE, &SegmentPreview.first);

      IsSegmentStarted = TRUE;
    }

    else if (Keys[VK_RBUTTON] && IsSegmentStarted)
    {
      SegmentPreview.second = GetScreenCoord(Mx, My);
      if (IsPrimitiveStarted && IsPerpendicular)
        SegmentPreview.second = CurrPolyGetPerpPoint(SegmentPreview.second);
      FindPoint(SegmentPreview.second, TRUE, &SegmentPreview.second);
    }

    else if (!Keys[VK_RBUTTON] && KeysOld[VK_RBUTTON])
    {
      IsPrimitiveStarted = !CurrPolyPlaceSegment(SegmentPreview.second, SegmentPreview.first);
      IsSegmentStarted = FALSE;
    }

    //
    // Render
    //

    // Draw current segment preview
    if (IsSegmentStarted)
    {
      glBegin(GL_LINES);
      glColor3f(0.1, 0.2, 0.8);
      glVertex2d(SegmentPreview.first[0], SegmentPreview.first[1]);
      glVertex2d(SegmentPreview.second[0], SegmentPreview.second[1]);
      glEnd();
    }
    location::Draw();
  }

public:
  triangulation_representation( INT argc, char *argv[] ) :
    render(argc, argv, "TRIANGULATION. CGSG FOREVER.", 800, 800), input(FindWindow(NULL, "TRIANGULATION. CGSG FOREVER."), MouseWheel)
  {
  }
};

INT main( INT argc, char *argv[] )
{
  triangulation_representation TriWin(argc, argv);
  TriWin.Run();

  return 0;
}