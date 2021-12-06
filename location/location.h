#ifndef __location_h_
#define __location_h_

#include "location_points_pool.h"

/* Polygon segment class. */
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

  /* Intersect two lines function.
   * ARGUMENTS:
   *   - two points, setting line to intersect with:
   *       const vec2 &P0, const vec2 &P1;
   *   - variable to set result in:
   *       vec2 *Result;
   * RETURNS:
   *   (BOOL) Whether lines intersected or not.
   */
  BOOL Intersect( const vec2 &P0, const vec2 &P1, vec2 *Result = nullptr ) const;
}; /* end of 'segment' class */

/* Polygon class. */
class polygon
{
public:
  std::vector<segment> Lines; // Polygon side lines container

  /* Merge polygons function.
   * ARGUMENTS:
   *   - polygon to merge with:
   *       const polygon &Polly1, const polygon &Polly2;
   *   - polygon to set merged one in:
   *       polygon *Merged;
   * RETURNS: None.
   */
  VOID Merge( const polygon &Polly, polygon *Merged ) const;

  /* Check if point inside polygon.
   * ARGUMENTS:
   *   - point to check:
   *       const vec3 &Pnt;
   * RETURNS:
   *   (BOOL) whether point in polygon or not.
   */
  BOOL IsPointInside( const vec2 &Pnt ) const;
}; /* end of 'polygon' struct */

/* Location representation class. */
class location
{
private:
  friend class segment;
  friend class polygon;

  /* Currently editing polygon structure. */
  struct current_polygon : polygon
  {
    BOOL IsEditing = 0;   // Displays if first segment of primitive added
    size_t Start = 0;     // Index of first point in polygon
    BOOL ShouldMerge = 1; // Displays should polygon be merged after closing
  }; /* end of 'current_polygon' struct */

  static points_pool PointsPool;   // All location points pool
  const DBL PlaceingRadius = 0.03; // Points  for segments placment radius
  current_polygon CurrPoly {};    // Currently editing polygon

public:
  // For test public, remove later
  std::vector<polygon> Walls; // Location walls

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

  /* Point moving function.
   * ARGUMENTS:
   *   - point possition (start possition at first call):
   *       const vec2 &Point;
   *   - end flag, if equals 0 end moving.
   * RETURNS: None.
   */
  VOID MovePoint( const vec2 &Point, BOOL IsEnd = FALSE)
  {
    static size_t Index;
    static BOOL IsMoveStarted;

    if (IsEnd)
    {
      IsMoveStarted = FALSE;
      Index = -1;
      return;
    }

    if (!IsMoveStarted)
    {
      if (!FindPoint(Point, FALSE, &Index))
        return;
      IsMoveStarted = TRUE;
    }

    if (Index != -1)
      PointsPool[Index] = Point;
  } /* End of 'MovePoint' function */

  //
  // Current polygon functions
  //

  /* Get index of end point of last segment in current polygon function.
   * ARGUMENTS: None.
   * RETURNS:
   *   (vec2) end point of last segment in current polygon.
   */
  size_t CurrPolyLastLineEndIndex(VOID) const
  {
    size_t count = CurrPoly.Lines.size();
    if (count > 0)
      return CurrPoly.Lines[CurrPoly.Lines.size() - 1].End;
    return 0;
  } /* End of 'GetLastLineEndIndex' function */

  /* Get end point of last segment in current polygon function.
   * ARGUMENTS: None.
   * RETURNS:
   *   (vec2) end point of last segment in current polygon.
   */
  vec2 CurrPolyLastLineEnd( VOID ) const
  {
    return PointsPool[CurrPolyLastLineEndIndex()];
  } /* End of 'GetLastLineEnd' function */

  /* Get start point of last segment in current polygon function.
   * ARGUMENTS: None.
   * RETURNS:
   *   (vec2) end point of last segment in current polygon.
   */
  size_t CurrPolyLastLineStartIndex( VOID ) const
  {
    return CurrPoly.Lines[CurrPoly.Lines.size() - 1].St;
  } /* End of 'GetLastLineStart' function */

  /* Get start point of last segment in current polygon function.
   * ARGUMENTS: None.
   * RETURNS:
   *   (vec2) end point of last segment in current polygon.
   */
  vec2 CurrPolyLastLineStart( VOID ) const
  {
    return PointsPool[CurrPolyLastLineStartIndex()];
  } /* End of 'GetLastLineStart' function */

  /* Get count of lines in current polygon.
   * ARGUMENTS: None.
   * RETURNS:
   *   (size_t) count of lines in current polygon.
   */
  size_t CurrPolyLinesSize( VOID ) const
  {
    return CurrPoly.Lines.size();
  } /* End of 'GetLinesSize' function */

  /* Add segment to current polygon function.
   * ARGUMENTS:
   *   - point of line end:
   *       const vec2 &PntEnd;
   * RETURNS:
   *   (BOOL) whether added segment closed current polygon.
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

    // Build normal by rotating segment to 90 or 270 degrees
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

  /* Destroy last segment in current polygon function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  VOID CurrPolyDestroyLastSegment( VOID )
  {
    size_t l = CurrPoly.Lines.size(), p = PointsPool.Points.size();

    // If end point of last segment is used only in last line delete it point
    if (p > 2 && p > l + 2 && CurrPolyLastLineEndIndex() == p - 1)
      PointsPool.Points.pop_back();

    if (l > 0)
    {
      // Delete segment
      CurrPoly.Lines.pop_back();

      // If last existing segment deleted checnge editing mode
      if (CurrPoly.Lines.size() == 0)
        CurrPoly.IsEditing = FALSE;
    }
  } /* End of 'DestroyLastSegment' function */

  /* Set current polygon close mode.
   * ARGUMENTS:
   *   - should polygon be merged with previous while closing:
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

  /* Clear location function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  VOID Clear(VOID)
  {
    PointsPool.Points.clear();
    Walls.clear();
    CurrPoly.Lines.clear();
    CurrPoly.Start = 0;
    CurrPoly.IsEditing = 0;
    CurrPoly.ShouldMerge = 1;
  } /* End of 'Clear' function */

  /* Draw wall location function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  VOID Draw( VOID ) const;
}; /* end of 'location' class */

#endif /* __location_h_ */