#include <set>
#include <map>
#include <algorithm>

#include "location.h"

/* Section constructor vy two points indexes.
 * ARGUMENTS:
 *   - points indexes:
 *       size_t St, size_t End;
 */
segment::segment( size_t St, size_t End ) :
  St(St), End(End)
{
} /* End of 'segment::segment' function */

/* Get Point-Line distance function.
 * ARGUMENTS:
 *   - point:
 *       const vec2 &Pnt;
 * RETURNS:
 *   (FLT) distance between point and line.
 */
FLT segment::GetPointDistance( const vec2& Pnt ) const
{
  FLT dist = location::PointsPool[St].Distance(location::PointsPool[End]);

  if (dist == 0)
    return Pnt.Distance(location::PointsPool[St]);

  return abs((location::PointsPool[End][0] - location::PointsPool[St][0]) * (location::PointsPool[St][1] - Pnt[1]) -
             (location::PointsPool[St][0] - Pnt[0]) * (location::PointsPool[End][1] - location::PointsPool[St][1])) / dist;
} /* End of 'segment::GetPointLineDistance' function */

/* Get location of a point in a plane relative to a straight line function.
 * ARGUMENTS:
 *   - point:
 *       const vec2 &Pnt;
 * RETURNS:
 *   (INT) -1 - point is on the right half-plane,
 *          0 - point is on the line,
 *          1 - point in on the left half-plane.
 */
INT segment::GetPointHalfPlaneLocation( const vec2& Pnt ) const
{
  DBL cross_product = ((DBL)location::PointsPool[End][0] - (DBL)location::PointsPool[St][0]) * ((DBL)Pnt[1] - (DBL)location::PointsPool[St][1]) -
                      ((DBL)location::PointsPool[End][1] - (DBL)location::PointsPool[St][1]) * ((DBL)Pnt[0] - (DBL)location::PointsPool[St][0]);

  return  cross_product > 0.00001 ? 1  :
          cross_product < 0.00001 ? -1 : 0;
} /* End of 'segment::GetPointHalfPlane' function */

/* Intersect two lines function.
 * ARGUMENTS:
 *   - two points, setting line to intersect with:
 *       const vec2 &P0, const vec2 &P1;
 *   - variable to set result in:
 *       vec2 *Result;
 * RETURNS:
 *   (BOOL) Whether lines intersected or not.
 */
BOOL segment::Intersect( const vec2& P0, const vec2& P1, vec2* Result ) const
{
  DBL divider_t = (P0[0] - P1[0]) * (location::PointsPool[St][1] - location::PointsPool[End][1]) - (P0[1] - P1[1]) * (location::PointsPool[St][0] - location::PointsPool[End][0]);
  DBL divider_s = (P0[0] - P1[0]) * (location::PointsPool[St][1] - location::PointsPool[End][1]) - (P0[1] - P1[1]) * (location::PointsPool[St][0] - location::PointsPool[End][0]);

  // Lines are equal or parallel
  if (divider_t == 0 || divider_s == 0)
    // Then if at least one pair point of points are equal all onther are equal too
    if (location::PointsPool[St] == P0 || location::PointsPool[St] == P1)
    {
      if (Result != nullptr)
        *Result = P1;
      return TRUE;
    }
    else
      return FALSE;

  FLT
    t =
      ((P0[0] - location::PointsPool[St][0]) * (location::PointsPool[St][1] - location::PointsPool[End][1]) - (P0[1] - location::PointsPool[St][1]) * (location::PointsPool[St][0] - location::PointsPool[End][0])) /
      divider_t,
    s =
      ((P0[0] - location::PointsPool[St][0]) * (P0[1] - P1[1]) - (P0[1] - location::PointsPool[St][1]) * (P0[0] - P1[0])) /
      divider_s;

  // Check if intersection lie between two points
  if ((t < 0 || t > 1 || s < 0 || s > 1))
    return FALSE;

  if (Result != nullptr)
    *Result = P0 + (P1 - P0) * t;
  return TRUE;
} /* End of 'segment::Intersect' function */