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

  /* Intersect two lines function.
   * ARGUMENTS:
   *   - two points, setting line to intersect with:
   *       const vec2 &P0, const vec2 &P1;
   *   - variable to set result in:
   *       vec2 *Result;
   * RETURNS:
   *   (BOOL) Whether lines intersected or not.
   */
  BOOL LinesIntersect( const vec2 &P0, const vec2 &P1, vec2 *Result = nullptr ) const;
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

  static points_pool PointsPool; // All location points pool

  /* State of currently editing pollygon */
  struct current_pollygon : pollygon
  {
    BOOL IsEditing = 0;   // Displays if first segment of primitive added
    BOOL ShouldMerge = 1; // Displays should pollygon be merged after closing

    size_t Start = 0;
  } CurrentPolly {}; /* end of 'current_pollygon' struct */
  
  const DBL PlaceingRadius = 0.05; // Points  for segments placment radius

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