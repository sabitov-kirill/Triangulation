#include <set>
#include <map>
#include <algorithm>

#include "location.h"

points_pool location::PointsPool = points_pool();

/* Find point function.
 * ARGUMENTS:
 *   - point to find:
 *       const vec2 &Pnt;
 *   - should try to find nearest point on segments:
 *       BOOL FindOnLines;
 *   - variable to set index of found point in:
 *       size_t *Result; 
 * RETURNS:
 *   (BOOL) wheather point found or not.
 */
BOOL location::FindPoint( const vec2 &Pnt, BOOL FindOnLines, vec2 *Result ) const
{
  // Try to find existing point
  size_t index;
  BOOL is_found = PointsPool.Get(Pnt, &index, PlaceingRadius);
  if (is_found)
  {
    if (Result != nullptr)
      *Result = PointsPool[index];
    return TRUE;
  }

  // If dont need to find point on segments return
  if (!FindOnLines)
    return is_found;

  // Try to fin point on segments
  vec2 inter;
  BOOL is_inter = FALSE;
  DBL min_dist = PlaceingRadius;

  // Check if point lies on one of polygon's segments function.
  const auto PointOnPollyogn = [&]( const pollygon &Polly )
  {
    for (size_t i = 0, cnt = Polly.Lines.size(); i < cnt; i++)
    {
      if (Polly.Lines[i].End == CurrPolyLastLineEndIndex())
        continue;

      vec2 L1 = PointsPool[Polly.Lines[i].St], L2 = PointsPool[Polly.Lines[i].End];
      vec2 Line = L2 - L1;

      // Calculate cormal to the segment
      vec2 N = -vec2(Line[1], -Line[0]).Normalize() * (SHORT)Polly.Lines[i].GetPointHalfPlaneLocation(Pnt);

      // Check if point line in placing radius to the segment
      DBL dist = N & (Pnt - L1);
      if (dist < min_dist)
      {
        vec2 inter_test = Pnt - N * (FLT)dist;
        if ((L2 - L1).Length() > (L2 - inter_test).Length())
          inter = inter_test, min_dist = dist, is_inter = TRUE;
      }
    }
  };

  for (const auto &polly: Walls)
    PointOnPollyogn(polly);
  PointOnPollyogn(CurrPoly);

  if (!is_inter)
    return FALSE;
  if (Result != nullptr)
    *Result = inter;

  return TRUE;
} /* End of 'location::FindPoint' function */

/* Find point function.
 * ARGUMENTS:
 *   - point to find:
 *       const vec2 &Pnt;
 *   - should try to find nearest point on segments:
 *       BOOL FindOnLines;
 *   - variable to set index of found point in:
 *       size_t *Result; 
 * RETURNS:
 *   (BOOL) wheather point found or not.
 */
BOOL location::FindPoint( const vec2 &Pnt, BOOL FindOnLines, size_t *Result ) const
{
  vec2 found_pnt;
  if (FindPoint(Pnt, FindOnLines, &found_pnt))
  {
    if (Result != nullptr)
      *Result = PointsPool.Add(found_pnt);
    return TRUE;
  }
  return FALSE;
} /* End of 'location::FindPoint' function */

/* Add point to the pool or return existing.
 * ARGUMENTS:
 *   - point to add:
 *       const vec2 &Pnt;
 *   - should try to find nearest point on segments:
 *       BOOL ConnectToLines;
 * RETURNS:
 *   (size_t) index of added or found point in pool.
 */
size_t location::PlacePoint( const vec2 &Pnt, BOOL ConnectToLines )
{
  if (!ConnectToLines)
    return PointsPool.Add(Pnt, PlaceingRadius);
  else
  {
    size_t found_point;
    if (FindPoint(Pnt, TRUE, &found_point))
      return found_point;
    return PointsPool.Add(Pnt, PlaceingRadius);
  }
} /* End of 'location::PlacePoint' function */

/* Add segment to walls sections container.
 * ARGUMENTS:
 *   - point of line end:
 *       const vec2 &PntEnd;
 * RETURNS:
 *   (BOOL) whether added segment closed current pollygon.
 */
BOOL location::CurrPolyPlaceSegment( const vec2 &PntEnd, const vec2 &PntSt )
{
  size_t SegmentStart, SegmentEnd;
  BOOL IsClosed = FALSE;

  // Adding segments to exitsting pollygon
  if (CurrPoly.IsEditing)
  {
    SegmentStart = CurrPoly.Lines[CurrPoly.Lines.size() - 1].End;

    // End point of segment is found
    if (PointsPool.Get(PntEnd, &SegmentEnd, PlaceingRadius))
    {
      CurrPoly.Lines.push_back(segment(SegmentStart, SegmentEnd));

      // If segment ends on its start point closing them.
      if (SegmentEnd == CurrPoly.Start)
      {
        // Mergin current pollygon with existing
        if (Walls.size() == 0)
          Walls.push_back(CurrPoly);
        else if (CurrPoly.ShouldMerge)
          CurrPoly.Merge(Walls[0], &Walls[0]);

        // Reseting current pollygon, but save close mode.
        BOOL should_merge = CurrPoly.ShouldMerge;
        CurrPoly = current_pollygon();
        CurrPoly.ShouldMerge = should_merge;
        return TRUE;
      }
    }
    // End point of segment is not found
    else
    {
      SegmentEnd = PlacePoint(PntEnd, TRUE);
      CurrPoly.Lines.push_back(segment(SegmentStart, SegmentEnd));
    }
  }
  // Adding first segment to pollygon
  else
  {
    SegmentStart = PlacePoint(PntSt, TRUE);
    SegmentEnd = PlacePoint(PntEnd, TRUE);

    CurrPoly.IsEditing = TRUE;
    CurrPoly.Start = SegmentStart;

    CurrPoly.Lines.push_back(segment(SegmentStart, SegmentEnd));
  }

  return FALSE;
} /* End of 'location::CurrPolyPlaceSegment' function */

/* Set current pollygon close mode.
 * ARGUMENTS:
 *   - should pollygon be merged with previous while closing:
 *       BOOL ShouldMerge;
 * RETURNS: None.
 */
VOID location::CurrPolySetCloseMode( BOOL ShouldMerge )
{
  CurrPoly.ShouldMerge = ShouldMerge;
} /* End of 'location::SetCurrPolygonCloseMode' function */

/* Draw circle function.
 * ARGUMETNS:
 *  - possition:
 *      GLfloat X, GLfloat Y;
 *  - radius:
 *      GLfloat Radius;
 * RETURNS: None.
 */
VOID DrawCircle( GLfloat X, GLfloat Y, GLfloat Radius )
{
  INT i;
  INT lineAmount = 15;

  FLT twicePi = 2.0f * (FLT)mth::PI;

  glBegin(GL_LINE_LOOP);
  for(i = 0; i <= lineAmount; i++)
    glVertex2f(X + (Radius * cos(i * twicePi / lineAmount)), Y + (Radius * sin(i * twicePi / lineAmount)));
  glEnd();
} /* End of 'DrawCircle' function */

/* Draw wall sections function.
 * ARGUMENTS: None.
 * RETURNS: None.
 */
VOID location::Draw( VOID ) const
{
  const auto DrawPolly = [&]( const pollygon &Wall, BOOL IsCurrent )
  {
    size_t cnt = Wall.Lines.size();
    size_t i;
    for (i = 0; i < cnt; i++)
    {
      glBegin(GL_LINES);
      glColor3f(!IsCurrent * 0.4f, !IsCurrent * 0.8f, !IsCurrent * 0.3f);
      glVertex2d(PointsPool[Wall.Lines[i].St][0], PointsPool[Wall.Lines[i].St][1]);
      glVertex2d(PointsPool[Wall.Lines[i].End][0], PointsPool[Wall.Lines[i].End][1]);
      DrawCircle(PointsPool[Wall.Lines[i].St][0], PointsPool[Wall.Lines[i].St][1], (FLT)PlaceingRadius);
      DrawCircle(PointsPool[Wall.Lines[i].End][0], PointsPool[Wall.Lines[i].End][1], (FLT)PlaceingRadius);
      glEnd();

      std::string index;
      index = std::to_string(Wall.Lines[i].St);
      glRasterPos2d(PointsPool[Wall.Lines[i].St][0], PointsPool[Wall.Lines[i].St][1] + 0.03);
      glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)index.c_str());
      index = std::to_string(Wall.Lines[i].End);
      glRasterPos2d(PointsPool[Wall.Lines[i].End][0], PointsPool[Wall.Lines[i].End][1] + 0.03);
      glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)index.c_str());
    }
  };

  for (const auto &wall: Walls)
    DrawPolly(wall, FALSE);
  DrawPolly(CurrPoly, TRUE);
} /* End pf 'location::Draw' function */