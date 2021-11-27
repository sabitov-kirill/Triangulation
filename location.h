#ifndef __location_h_
#define __location_h_

#include "def.h"

/* Points pool class. */
class points_pool
{
}; /* end of 'points_pool' class */

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
   *   - two points, setting line:
   *       const vec2 &LineP1, &LineP2;
   * RETURNS:
   *   (FLT) distance between point and line.
   */
  static FLT GetPointLineDistance( const vec2 &Pnt, const vec2 &LineP1, const vec2 &LineP2 );

  /* Get location of a point in a plane relative to a straight line function.
   * ARGUMENTS:
   *   - point:
   *       const vec2 &Pnt;
   *   - two points, setting line:
   *       const vec2 &LineP1, &LineP2;
   * RETURNS:
   *   (INT) -1 - point is on the right half-plane,
   *          0 - point is on the line,
   *          1 - point in on the left half-plane.
   */
  static INT GetPointHalfPlaneLocation( const vec2 &Pnt, const vec2 &LineP1, const vec2 &LineP2 );

  /* Intersect two lines function.
   * ARGUMENTS:
   *   - two points, setting first line:
   *       const vec2 &P0, const vec2 &P1;
   *   - two points, setting second line:
   *       const vec2 &P2, const vec2 &P3;
   *   - variable to set result in:
   *       vec2 *Result;
   * RETURNS:
   *   (BOOL) Whether lines intersected or not.
   */
  static BOOL LinesIntersect( const vec2 &P0, const vec2 &P1, const vec2 &P2, const vec2 &P3, vec2 *Result = nullptr );
}; /* end of 'segment' class */

/* Pollygon class. */
struct pollygon
{
  std::vector<segment> Lines; // Pollygon side lines container
}; /* end of 'pollygon' struct */

/* Location representation class. */
class location
{
private:
  mutable std::vector<vec2> PointsPool; // All location points pool

  /* State of currently editing pollygon */
  struct current_pollygon : pollygon
  {
    BOOL IsEditing = 0;   // Displays if first segment of primitive added
    BOOL ShouldMerge = 1; // Displays should pollygon be merged after closing

    size_t Start = 0;
  } CurrentPolly {}; /* end of 'current_pollygon' struct */
  
  const DBL Epsilon = 0.01;               // Epsilon of point finding
  const DBL Epsilon2 = Epsilon * Epsilon; // Epsilon sqared

  public:
  // For test public, remove later
  std::vector<pollygon> Walls; // Location walls

  //
  // Points pool functions
  //

  /* Get point index in points pool.
   * ARGUMENTS:
   *   - point to get index of:
   *       const vec2 &Pnt;
   * - variable to set index of point in:
   *       seze_t *Result;
   * RETURNS:
   *   (size_t) point index;
   */
  BOOL GetPointIndex( const vec2 &Pnt, size_t *Result ) const;

  /* Find aproximate point in pool function.
   * ARGUMETNS:
   *   - point to find:
   *       const vec2 &Pnt
   *   - variable to set found point in:
   *       vec2 *Result;
   * RETURNS:
   *   (BOOL) whether point found or not.
   */
  BOOL FindNearestPointToPoints( const vec2 &Point, vec2 *Result = nullptr ) const;

  /* Find aproximate point in wall segments.
   * ARGUMETNS:
   *   - point to find:
   *       const vec2 &Pnt
   *   - variable to set found point in:
   *       vec2 *Result;
   * RETURNS:
   *   (BOOL) whether point found or not.
   */
  BOOL FindNearestPointToSegments( const vec2 &Point, vec2 *Result = nullptr ) const;

  /* Find aproximate point on neares segments or points.
   * ARGUMETNS:
   *   - point to find:
   *       const vec2 &Pnt
   *   - variable to set index of found point in:
   *       vec2 *Result;
   * RETURNS:
   *   (BOOL) whether point found or not.
   */
  BOOL FindNearestPoint( const vec2 &Point, vec2 *Result = nullptr ) const;

  /* Add point to the pool or return existing.
   * ARGUMENTS:
   *   - point to add:
   *       const vec2 &Pnt;
   * RETURNS:
   *   (size_t) index of added or found point in pool.
   */
  size_t PlacePoint( const vec2 &Pnt ) const;

  /* Place or find existing point.
   * ARGUMENTS:
   *   - point to add or find:
   *       const vec2 &Pnt;
   * RETURNS:
   *   (size_t) index of added or found point in pool.
   */
  size_t PlaceOrFindPoint( const vec2 &Pnt ) const;

  /* Place or find existing point.
   * ARGUMENTS:
   *   - point to add or find:
   *       const vec2 &Pnt;
   * RETURNS:
   *   (size_t) index of added or found point in pool.
   */
  size_t PlaceOrFindPointToPoint( const vec2 &Pnt ) const;

  //
  // Pollygons functions
  //

  /* Add segment to walls location container.
   * ARGUMENTS:
   *   - point of line end:
   *       const vec2 &PntEnd;
   * RETURNS:
   *   (BOOL) whether added segment closed current pollygon.
   */
  BOOL PlaceSegment( const vec2 &PntEnd, const vec2 &PntSt = {0} );

  /* Set current pollygon close mode.
   * ARGUMENTS:
   *   - should pollygon be merged with previous while closing:
   *       BOOL ShouldMerge;
   * RETURNS: None.
   */
  VOID SetCurrentPollygonCloseMode( BOOL ShouldMerge );

  /* Merge pollygons function.
   * ARGUMENTS:
   *   - pollygon to merge with:
   *       const pollygon &Polly1, const pollygon &Polly2;
   *   - pollygon to set merged one in:
   *       pollygon *Merged;
   * RETURNS: None.
   */
  VOID MergePolly( const pollygon &Polly1, const pollygon &Polly2, pollygon *Merged ) const;

  /* Check if point inside pollygon.
   * ARGUMENTS:
   *   - point to check:
   *       const vec3 &Pnt;
   * RETURNS:
   *   (BOOL) whether point in pollygon or not.
   */
  BOOL IsPointInsidePolly( const vec2 &Pnt, const pollygon &Polly ) const;

  //
  // Render functions
  //

  /* Draw wall location function.
   * ARGUMENTS: None.
   * RETURNS: None.
   */
  VOID Draw( VOID ) const;
}; /* end of 'location' class */

#endif /* __location_h_ */