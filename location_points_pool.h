#ifndef __location_points_pool_h_
#define __location_points_pool_h_

#include "def.h"

/* Location representation class. */
class points_pool
{
private:
  std::vector<vec2> Points; // All location_points_pool points pool

  static const DBL Epsilon;  // Epsilon of point finding
  static const DBL Epsilon2; // Epsilon sqared

public:

  //
  // Points manage functions
  //

  /* Get point index in points pool.
   * ARGUMENTS:
   *   - point to get index of:
   *       const vec2 &Pnt;
   * - variable to set index of point in:
   *       seze_t *Result;
   *   - radius of area in which try to find points:
   *       DBL CheckRadius;
   * RETURNS:
   *   (size_t) point index;
   */
  BOOL Get( const vec2 &Pnt, size_t *Result, DBL CheckRadius = Epsilon ) const;

  /* Get point index in points pool.
   * ARGUMENTS:
   *   - point to get index of:
   *       const vec2 &Pnt;
   * - variable to set index of point in:
   *       seze_t *Result;
   * RETURNS:
   *   (size_t) point index;
   */
  BOOL operator()( const vec2 &Pnt, size_t *Result ) const;

  /* Get point rvalue by index.
   * ARGUMETNS:
   *   - index:
   *       size_t Index;
   * RETURNS:
   *   (const vec2 &) point.
   */
  const vec2 &Get( size_t Index ) const;

  /* Get point rvalue by index.
   * ARGUMETNS:
   *   - index:
   *       size_t Index;
   * RETURNS:
   *   (const vec2 &) point.
   */
  const vec2 &operator[]( size_t Index ) const;

  /* Get point lvalue by index.
   * ARGUMETNS:
   *   - index:
   *       size_t Index;
   * RETURNS:
   *   (vec2 &) point.
   */
  vec2 &Get( size_t Index );

  /* Get point lvalue by index.
   * ARGUMETNS:
   *   - index:
   *       size_t Index;
   * RETURNS:
   *   (vec2 &) point.
   */
  vec2 &operator[]( size_t Index );

  /* Add point to pool function.
   * ARGUMENTS:
   *   - point to add:
   *       const vec2 &Pnt;
   *   - radius of area in which try to find points:
   *       DBL CheckRadius;
   * RETURNS:
   *   (size_t) point index in pool;
   */
  size_t Add( const vec2 &Pnt, DBL CheckRadius = Epsilon );

  /* Add point to pool function.
   * ARGUMENTS:
   *   - point to add:
   *       const vec2 &Pnt;
   * RETURNS:
   *   (points_pool &) self-ref;
   */
  points_pool &operator<<( const vec2 &Pnt );

}; /* end of 'location_points_pool' class */

#endif /* __location_points_pool_h_ */