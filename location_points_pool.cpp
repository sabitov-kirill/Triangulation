#include "location_points_pool.h"

const DBL points_pool::Epsilon = 0.01;               // Epsilon of point finding
const DBL points_pool::Epsilon2 = Epsilon * Epsilon; // Epsilon sqared

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
BOOL points_pool::Get( const vec2 &Pnt, size_t *Result, DBL CheckRadius ) const
{
  size_t min_dist_pnt = -1;
  DBL min_dist2 = CheckRadius == Epsilon ? Epsilon2 : CheckRadius * CheckRadius;

  // Check if point exectly matches with existing (with Epsilon)
  for (size_t i = 0, cnt = Points.size(); i < cnt; i++)
  {
    // Point-Circle intersection
    DBL dist2 = (Pnt - Points[i]).Length2();
    if (dist2 < min_dist2)
      min_dist2 = dist2, min_dist_pnt = i;
  }

  if (min_dist_pnt == -1)
    return FALSE;

  if (Result != nullptr)
    *Result = min_dist_pnt;

  return TRUE;
} /* End of 'points_pool::Get' function */

/* Get point index in points pool.
 * ARGUMENTS:
 *   - point to get index of:
 *       const vec2 &Pnt;
 * - variable to set index of point in:
 *       seze_t *Result;
 * RETURNS:
 *   (size_t) point index;
 */
BOOL points_pool::operator()( const vec2 &Pnt, size_t *Result ) const
{
  Get(Pnt, Result);
} /* End of 'points_pool::operator()' function */

/* Get point rvalue by index.
 * ARGUMETNS:
 *   - index:
 *       size_t Index;
 * RETURNS:
 *   (const vec2 &) point.
 */
const vec2 &points_pool::Get( size_t Index ) const
{
  return Points[mth::Clamp(Index, (size_t)0, Points.size() - 1)];
} /* End of 'points_pool::Get' function */

/* Get point rvalue by index.
 * ARGUMETNS:
 *   - index:
 *       size_t Index;
 * RETURNS:
 *   (const vec2 &) point.
 */
const vec2 &points_pool::operator[]( size_t Index ) const
{
  return Get(Index);
} /* End of 'points_pool::operator[]' function */

/* Get point lvalue by index.
 * ARGUMETNS:
 *   - index:
 *       size_t Index;
 * RETURNS:
 *   (vec2 &) point.
 */
vec2 &points_pool::Get( size_t Index )
{
  return Points[mth::Clamp(Index, (size_t)0, Points.size() - 1)];
} /* End of 'points_pool::Get' function */

/* Get point lvalue by index.
 * ARGUMETNS:
 *   - index:
 *       size_t Index;
 * RETURNS:
 *   (vec2 &) point.
 */
vec2 &points_pool::operator[]( size_t Index )
{
  return Get(Index);
} /* End of 'points_pool::operator[]' function */

/* Add point to pool function.
 * ARGUMENTS:
 *   - point to add:
 *       const vec2 &Pnt;
 *   - radius of area in which try to find points:
 *       DBL CheckRadius;
 * RETURNS:
 *   (size_t) point index in pool;
 */
size_t points_pool::Add( const vec2 &Pnt, DBL CheckRadius )
{
  size_t new_pnt;

  if (Get(Pnt, &new_pnt, CheckRadius))
    return new_pnt;

  Points.push_back(Pnt);
  return Points.size() - 1;
} /* End of 'points_pool::AddPoint' function */

/* Add point to pool function.
 * ARGUMENTS:
 *   - point to add:
 *       const vec2 &Pnt;
 * RETURNS:
 *   (points_pool &) self-ref;
 */
points_pool &points_pool::operator<<( const vec2 &Pnt )
{
  Add(Pnt);
  return *this;
} /* End of 'points_pool::operator<<' function */