#ifndef __location_h_
#define __location_h_

#include "location_points_pool.h"

/* Pollygon segment class. */
class segment
{
public:
  // Indexes of segment start and end points in location points pool.
  size_t St, End;

  /* Section default constructor. */
  segment( VOID ) = default;

  /* Section constructor vy two points indexes.
   * ARGUMENTS:
   *   - points indexes:
   *       size_t St, size_t End;
   */
  segment( size_t St, size_t End );

  //
  // Useful geometry functions
  //

  /* Get Point-Line distance function.
   * ARGUMENTS:
   *   - point:
   *       const vec2 &Pnt;
   * RETURNS:
   *   (FLT) distance between point and line.
   */
  FLT GetPointDistance( const vec2 &Pnt ) const;

  /* Check if point lie on segment. If set Check radius check in that area.
   * ARGUMENTS:
   *   - point to check:
   *       const vec2 &Pnt;
   *   - checking area radius:
   *       DBL CheckRadius;
   *   - variable to set point on segment if cheking radius was given:
   *       vec2 *Result;
   * RETURNS:
   *   (DBL) distance to the line. If no lines nearby - 0.
   */
  DBL IsPointOnSegment( const vec2 &Pnt, DBL CheckRadius = 0, vec2 *Result = nullptr ) const;

  /* Get location of a point in a plane relative to a straight line function.
   * ARGUMENTS:
   *   - point:
   *       const vec2 &Pnt;
   * RETURNS:
   *   (INT) -1 - point is on the right half-plane,
   *          0 - point is on the line,
   *          1 - point in on the left half-plane.
   */
  INT GetPointHalfPlaneLocation( const vec2 &Pnt ) const;

  /* Get location of a point in a plane relative to a straight line function.
   * ARGUMENTS:
   *   - point:
   *       const vec2 &Pnt;
   * RETURNS:
   *   (INT) -1 - point is on the right half-plane,
   *          0 - point is on the line,
   *          1 - point in on the left half-plane.
   */
  static INT GetPointHalfPlaneLocation( const vec2 &Pnt, const vec2 &L1, const vec2 &L2 );

  /* Intersect two lines function.
   * ARGUMENTS:
   *   - two points, setting line to intersect with:
   *       const vec2 &P0, const vec2 &P1;
   *   - variable to set result in:
   *       vec2 *Result;
   *   - should check length of intersecting lines or not:
   *       BOOL CheckLength;
   * RETURNS:
   *   (BOOL) Whether lines intersected or not.
   */
  BOOL LinesIntersect( const vec2 &P0, const vec2 &P1, vec2 *Result = nullptr, BOOL CheckLength = TRUE ) const;
}; /* end of 'segment' class */

/* Pollygon class. */
class pollygon
{
public:
  std::vector<segment> Lines; // Pollygon side lines container

  /* Merge pollygons function.
   * ARGUMENTS:
   *   - pollygon to merge with:
   *       const pollygon &Polly1, const pollygon &Polly2;
   *   - pollygon to set merged one in:
   *       pollygon *Merged;
   * RETURNS: None.
   */
  VOID Merge( const pollygon &Polly, pollygon *Merged ) const;

  /* Check if point inside pollygon.
   * ARGUMENTS:
   *   - point to check:
   *       const vec3 &Pnt;
   * RETURNS:
   *   (BOOL) whether point in pollygon or not.
   */
  BOOL IsPointInside( const vec2 &Pnt ) const;
}; /* end of 'pollygon' struct */

/* Location representation class. */
class location
{
private:
  friend class segment;
  friend class pollygon;

  /* Currently editing pollygon structure. */
  struct current_pollygon : pollygon
  {
    BOOL IsEditing = 0;   // Displays if first segment of primitive added
    size_t Start = 0;     // Index of first point in pollygon
    BOOL ShouldMerge = 1; // Displays should pollygon be merged after closing
  }; /* end of 'current_pollygon' struct */

  static points_pool PointsPool;   // All location points pool
  const DBL PlaceingRadius = 0.1; // Points  for segments placment radius
  current_pollygon CurrPoly {};    // Currently editing pollygon

public:
  // For test public, remove later
  std::vector<pollygon> Walls; // Location walls

  //
  // Points pool functions
  //

  /* Find point function.
   * ARGUMENTS:
   *   - point to find:
   *       const vec2 &Pnt;
   *   - should try to find nearest point on segments:
   *       BOOL FindOnLines;
   *   - variable to set index of found point in:
   *       vec2 *Result; 
   * RETURNS:
   *   (BOOL) wheather point found or not.
   */
  BOOL FindPoint( const vec2 &Pnt, BOOL FindOnLines, vec2 *Result = nullptr ) const;

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
  BOOL FindPoint( const vec2 &Pnt, BOOL FindOnLines, size_t *Result ) const;

  /* Add point to the pool or return existing.
   * ARGUMENTS:
   *   - point to add:
   *       const vec2 &Pnt;
   *   - should try to find nearest point on segments:
   *       BOOL ConnectToLines;
   * RETURNS:
   *   (size_t) index of added or found point in pool.
   */
  size_t PlacePoint( const vec2 &Pnt, BOOL ConnectToLines );

  //
  // Current pollygon functions
  //

  /* Get index of end point of last segment in current pollygon function.
   * ARGUMENTS: None.
   * RETURNS:
   *   (vec2) end point of last segment in current pollygon.
   */
  size_t CurrPolyLastLineEndIndex(VOID) const
  {
    size_t count = CurrPoly.Lines.size();
    if (count > 0)
      return CurrPoly.Lines[CurrPoly.Lines.size() - 1].End;
    return 0;
  } /* End of 'GetLastLineEndIndex' function */

  /* Get end point of last segment in current pollygon function.
   * ARGUMENTS: None.
   * RETURNS:
   *   (vec2) end point of last segment in current pollygon.
   */
  vec2 CurrPolyLastLineEnd( VOID ) const
  {
    return PointsPool[CurrPolyLastLineEndIndex()];
  } /* End of 'GetLastLineEnd' function */

  /* Get start point of last segment in current pollygon function.
   * ARGUMENTS: None.
   * RETURNS:
   *   (vec2) end point of last segment in current pollygon.
   */
  size_t CurrPolyLastLineStartIndex( VOID ) const
  {
    return CurrPoly.Lines[CurrPoly.Lines.size() - 1].St;
  } /* End of 'GetLastLineStart' function */

  /* Get start point of last segment in current pollygon function.
   * ARGUMENTS: None.
   * RETURNS:
   *   (vec2) end point of last segment in current pollygon.
   */
  vec2 CurrPolyLastLineStart( VOID ) const
  {
    return PointsPool[CurrPolyLastLineStartIndex()];
  } /* End of 'GetLastLineStart' function */

  /* Get count of lines in current pollygon.
   * ARGUMENTS: None.
   * RETURNS:
   *   (size_t) count of lines in current pollygon.
   */
  size_t CurrPolyLinesSize( VOID ) const
  {
    return CurrPoly.Lines.size();
  } /* End of 'GetLinesSize' function */

  /* Add segment to current pollygon function.
   * ARGUMENTS:
   *   - point of line end:
   *       const vec2 &PntEnd;
   * RETURNS:
   *   (BOOL) whether added segment closed current pollygon.
   */
  BOOL CurrPolyPlaceSegment( const vec2 &PntEnd, const vec2 &PntSt = {0} );

  /* Get perpendicular point to last segment function.
   * ARGUMENTS:
   *   - point to create perpendicular in direction of:
   *       vec2 Point;
   * RETURNS:
   *   (vec2) end of perpendicular segment.
   */
  vec2 CurrPolyGetPerpPoint( vec2 Point ) const
  {
    segment last_seg = CurrPoly.Lines[CurrPoly.Lines.size() - 1];
    vec2
      norm,
      last_line = vec2(PointsPool[last_seg.End][0] - PointsPool[last_seg.St][0],
                       PointsPool[last_seg.End][1] - PointsPool[last_seg.St][1]);

    if (last_seg.GetPointHalfPlaneLocation(Point) == 1)
      norm = vec2(-last_line[1], last_line[0]);
    else
      norm = vec2(last_line[1], -last_line[0]);

    norm.Normalize();

    // Set length of normal
    norm *= (Point - PointsPool[last_seg.End]) & norm;
    norm += PointsPool[last_seg.End];
   
    return norm;
  } /* End of 'CurrPolyGetPerpPoint' function */

  /* Destroy last segment in current pollygon function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  VOID CurrPolyDestroyLastSegment( VOID )
  {
    size_t l = CurrPoly.Lines.size(), p = PointsPool.Points.size();

    // If end point of last segment is used only in last line delete it point
    if (p > 2 && p > l + 2 && CurrPolyLastLineEndIndex() == p - 1)
      PointsPool.Points.pop_back();

    // Delete segment
    if (l > 0)
      CurrPoly.Lines.pop_back();
  } /* End of 'DestroyLastSegment' function */

  /* Set current pollygon close mode.
   * ARGUMENTS:
   *   - should pollygon be merged with previous while closing:
   *       BOOL ShouldMerge;
   * RETURNS: None.
   */
  VOID CurrPolySetCloseMode( BOOL ShouldMerge );

  //
  // Location functions
  //

  /* Optimize point pool function. Deletes all unused points.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  VOID OptimizePointsPool( VOID );

  /* Draw wall location function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  VOID Draw( VOID ) const;
}; /* end of 'location' class */

#endif /* __location_h_ */