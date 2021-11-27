#include "render.h"
#include "location.h"

class triangulation_representation : public render, private location
{
private:
  BOOL IsRightButton = false;
  BOOL IsLeftButton = false;

  VOID Render( VOID ) final
  {
    location::Draw();
  }

  VOID KeyboardClick( unsigned char Key, INT x, INT y ) override final
  {
  }

  VOID MouseClick( INT Button, INT State, INT X, INT Y ) override  final
  {
    static vec2 st, end;
    static bool IsSegmentStarted, IsPrimitiveStarted;

    if (Button == GLUT_RIGHT_BUTTON && State == GLUT_DOWN)
    {
      st = vec2((FLT)X / W * 2 - 1, -(FLT)Y / H * 2 + 1);
      IsSegmentStarted = !IsPrimitiveStarted ||
                         FindNearestPointToPoints(st, nullptr);
      IsPrimitiveStarted = TRUE;
    }
    if (Button == GLUT_RIGHT_BUTTON && State == GLUT_UP && IsSegmentStarted)
    {
      end = vec2((FLT)X / W * 2 - 1, -(FLT)Y / H * 2 + 1);
      IsPrimitiveStarted = !PlaceSegment(end, st);
    }

    if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN)
      IsLeftButton = true;
    if (Button == GLUT_LEFT_BUTTON && State == GLUT_UP)
      IsLeftButton = false;
  }

  VOID MouseMotion( INT X, INT Y ) override final
  {
    if (IsLeftButton)
    {
      static vec2 PrevPnt;
      static BOOL IsFirstSet = false;
      vec2 new_pnt = vec2((FLT)X / W * 2 - 1, -(FLT)Y / H * 2 + 1);

      if (IsFirstSet)
        PlaceSegment(PrevPnt, new_pnt);
      else
        IsFirstSet = true;

      PrevPnt = new_pnt;
    }
  }

public:
  triangulation_representation( INT argc, char *argv[] ) :
    render(argc, argv, "TRIANGULATION. CGSG FOREVER.")
  {
  }
};

INT main( INT argc, char *argv[] )
{
  // No intersection merge test
  // location Loc;
  // vec2 a(10, 10), b(5, 10), c(5, 5), d(10, 5);
  // Loc.PlaceSegment(b, a);
  // Loc.PlaceSegment(c);
  // Loc.PlaceSegment(d);
  // Loc.PlaceSegment(a);
  // 
  // vec2 a1(2,2), b1(4, 2), c1(3, 4);
  // Loc.PlaceSegment(b1, a1);
  // Loc.PlaceSegment(c1);
  // Loc.PlaceSegment(a1);
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
  // Loc.PlaceSegment(b, a);
  // Loc.PlaceSegment(c);
  // Loc.PlaceSegment(d);
  // Loc.PlaceSegment(a);
  // 
  // vec2 a1(12, 2), b1(12, 5), c1(0, 5.9), d1(0, 0);
  // Loc.PlaceSegment(b1, a1);
  // Loc.PlaceSegment(c1);
  // Loc.PlaceSegment(d1);
  // Loc.PlaceSegment(a1);
  // 
  // INT r = segment::GetPointHalfPlaneLocation(c, a, b);

  triangulation_representation TriWin(argc, argv);
  TriWin.Run();

  return 0;
}