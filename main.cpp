#include "render.h"
#include "location.h"

class triangulation_representation : public render, private location
{
private:
  BOOL
    IsRightButton = FALSE,
    IsLeftButton = FALSE,
    IsMiddleButton = FALSE,
    IsPerpendicular = FALSE,
    IsSegmentStarted = FALSE,
    IsPrimitiveStarted = FALSE;
  std::pair<vec2, vec2> SegmentPreview;

  vec2 GetScreenCoord( INT X, INT Y )
  {
    return vec2((FLT)X / W * 2 - 1, -(FLT)Y / H * 2 + 1);
  }

  VOID Render( VOID ) final
  {
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

  VOID KeyboardClick( unsigned char Key, INT X, INT Y ) override final
  {
    if ((Key == 'Q' || Key == 'q'))
      IsPerpendicular = !IsPerpendicular;
  }

  VOID MouseClick( INT Button, INT State, INT X, INT Y ) override  final
  {
    if (Button == GLUT_RIGHT_BUTTON && State == GLUT_DOWN)
    {
      SegmentPreview.first = GetScreenCoord(X, Y);
      FindPoint(GetScreenCoord(X, Y), TRUE, &SegmentPreview.first);
      IsSegmentStarted = !IsPrimitiveStarted ||
                         FindPoint(SegmentPreview.first, FALSE);
      IsPrimitiveStarted = TRUE;
    }
    if (Button == GLUT_RIGHT_BUTTON && State == GLUT_UP && IsSegmentStarted)
    {
      SegmentPreview.second = GetScreenCoord(X, Y);
      IsPrimitiveStarted = !CurrPolyPlaceSegment(SegmentPreview.second, SegmentPreview.first);
      IsSegmentStarted = FALSE;
    }

    if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN)
      IsLeftButton = true;
    if (Button == GLUT_LEFT_BUTTON && State == GLUT_UP)
      IsLeftButton = false;

    if (Button == GLUT_RIGHT_BUTTON && State == GLUT_DOWN)
      IsRightButton = true;
    else if (Button == GLUT_RIGHT_BUTTON && State == GLUT_UP)
      IsRightButton = false;

    if (Button == GLUT_MIDDLE_BUTTON && State == GLUT_DOWN)
      IsMiddleButton = true;
    else if (Button == GLUT_MIDDLE_BUTTON && State == GLUT_UP)
      IsMiddleButton = false;
  }

  VOID MouseMotion( INT X, INT Y ) override final
  {
    if (IsRightButton && IsPrimitiveStarted)
    {
      SegmentPreview.second = GetScreenCoord(X, Y);
      FindPoint(GetScreenCoord(X, Y), TRUE, &SegmentPreview.second);
    }
  }

public:
  triangulation_representation( INT argc, char *argv[] ) :
    render(argc, argv, "TRIANGULATION. CGSG FOREVER.", 800, 800)
  {
  }
};

INT main( INT argc, char *argv[] )
{
  // No intersection merge test
  // location Loc;
  // vec2 a(10, 10), b(5, 10), c(5, 5), d(10, 5);
  // Loc.CurrPolyPlaceSegment(b, a);
  // Loc.CurrPolyPlaceSegment(c);
  // Loc.CurrPolyPlaceSegment(d);
  // Loc.CurrPolyPlaceSegment(a);
  // 
  // vec2 a1(2,2), b1(4, 2), c1(3, 4);
  // Loc.CurrPolyPlaceSegment(b1, a1);
  // Loc.CurrPolyPlaceSegment(c1);
  // Loc.CurrPolyPlaceSegment(a1);
  // 
  // INT point_loc = segment::GetPointHalfPlaneLocation(vec2(-3, -3), vec2(2, 0), vec2(0, 2));
  // 
  // pollygon merged;
  // Loc.MergePolly(Loc.Walls[1], Loc.Walls[0], &merged);

  // Test geting half plane location
  // INT point_loc = segment::GetPointHalfPlaneLocation(vec2(-3, -3), vec2(2, 0), vec2(0, 2));

  // Full merge test
  // location Loc;
  // vec2 a(10, 10), b(1, 10), c(1, 1), d(10, 1);
  // Loc.CurrPolyPlaceSegment(b, a);
  // Loc.CurrPolyPlaceSegment(c);
  // Loc.CurrPolyPlaceSegment(d);
  // Loc.CurrPolyPlaceSegment(a);
  // 
  // vec2 a1(12, 2), b1(12, 5), c1(0, 5.9), d1(0, 0);
  // Loc.CurrPolyPlaceSegment(b1, a1);
  // Loc.CurrPolyPlaceSegment(c1);
  // Loc.CurrPolyPlaceSegment(d1);
  // Loc.CurrPolyPlaceSegment(a1);
  // 
  // INT r = segment::GetPointHalfPlaneLocation(c, a, b);

  triangulation_representation TriWin(argc, argv);
  TriWin.Run();

  return 0;
}