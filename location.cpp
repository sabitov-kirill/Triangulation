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

/*
 * Useful geometry functions
 */

/* Get Point-Line distance function.
 * ARGUMENTS:
 *   - point:
 *       const vec2 &Pnt;
 *   - two points, setting line:
 *       const vec2 &LineP1, &LineP2;
 * RETURNS:
 *   (FLT) distance between point and line.
 */
FLT segment::GetPointLineDistance( const vec2 &Pnt, const vec2 &LineP1, const vec2 &LineP2 )
{
  FLT dist = LineP1.Distance(LineP2);

  if (dist == 0)
    return Pnt.Distance(LineP1);
  return abs((LineP2[0] - LineP1[0]) * (LineP1[1] - Pnt[1]) - 
             (LineP1[0] - Pnt[0]) * (LineP2[1] - LineP1[1])) / dist;
} /* End of 'segment::GetPointLineDistance' function */

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
INT segment::GetPointHalfPlaneLocation( const vec2 &Pnt, const vec2 &LineP1, const vec2 &LineP2 )
{
  DBL cross_product = ((DBL)LineP2[0] - (DBL)LineP1[0]) * ((DBL)Pnt[1] - (DBL)LineP1[1]) -
                      ((DBL)LineP2[1] - (DBL)LineP1[1]) * ((DBL)Pnt[0] - (DBL)LineP1[0]);
  return  cross_product > 0.00001 ?  1 :
          cross_product < 0.00001 ? -1 : 0;
} /* End of 'segment::GetPointHalfPlane' function */

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
BOOL segment::LinesIntersect( const vec2 &P0, const vec2 &P1, const vec2 &P2, const vec2 &P3, vec2 *Result )
{
  FLT
    t =
      ((P0[0] - P2[0]) * (P2[1] - P3[1]) - (P0[1] - P2[1]) * (P2[0] - P3[0])) /
      ((P0[0] - P1[0]) * (P2[1] - P3[1]) - (P0[1] - P1[1]) * (P2[0] - P3[0])),
    s =
      ((P0[0] - P2[0]) * (P0[1] - P1[1]) - (P0[1] - P2[1]) * (P0[0] - P1[0])) /
      ((P0[0] - P1[0]) * (P2[1] - P3[1]) - (P0[1] - P1[1]) * (P2[0] - P3[0]));

  if (t < 0 || t > 1 || s < 0 || s > 1)
    return FALSE;

  if (Result != nullptr) 
    *Result = P0 + (P1 - P0) * t;
  return TRUE;
} /* End of 'segment::LinesIntersect' function */

/*
 * Location functions
 */

/* Get point index in points pool.
 * ARGUMENTS:
 *   - point to get index of:
 *       const vec2 &Pnt;
 * - variable to set index of point in:
 *       seze_t *Result;
 * RETURNS:
 *   (size_t) point index;
 */
BOOL location::GetPointIndex( const vec2 &Pnt, size_t *Result ) const
{
  size_t min_dist_pnt = -1;
  DBL min_dist2 = Epsilon2;

  // Check if point exectly matches with existing (with Epsilon)
  for (size_t i = 0, cnt = PointsPool.size(); i < cnt; i++)
  {
    // Point-Circle intersection
    DBL dist2 = (Pnt - PointsPool[i]).Length2();
    if (dist2 < min_dist2)
      min_dist2 = dist2, min_dist_pnt = i;
  }

  if (min_dist_pnt == -1)
    return FALSE;

  if (Result != nullptr)
    *Result = min_dist_pnt;

  return TRUE;
} /* End of 'GetPointIndex' function */

/* Find aproximate point in pool function.
 * ARGUMETNS:
 *   - point to find:
 *       const vec2 &Pnt;
 *   - variable to set found point in:
 *       vec2 *Result;
 * RETURNS:
 *   (BOOL) whether point found or not.
 */
BOOL location::FindNearestPointToPoints( const vec2 &Point, vec2 *Result ) const
{
  size_t min_dist_pnt = -1;
  DBL min_dist2 = Epsilon2;

  // Check if point exectly matches with existing (with Epsilon)
  for (size_t i = 0, cnt = PointsPool.size(); i < cnt; i++)
  {
    // Point-Circle intersection
    DBL dist2 = (Point - PointsPool[i]).Length2();
    if (dist2 < min_dist2)
      min_dist2 = dist2, min_dist_pnt = i;
  }

  if (min_dist_pnt == -1)
    return FALSE;

  if (Result != nullptr)
    *Result = PointsPool[min_dist_pnt];

  return TRUE;
} /* End of 'location::FindNearestPointToPoints' function */

/* Find aproximate point in wall segments.
 * ARGUMETNS:
 *   - point to find:
 *       const vec2 &Pnt
 *   - variable to set found point in:
 *       vec2 *Result;
 * RETURNS:
 *   (BOOL) whether point found or not.
 */
BOOL location::FindNearestPointToSegments( const vec2 &Point, vec2 *Result ) const
{
  vec2 min_dist_pnt;
  DBL min_dist = Epsilon;

  // Check if point lies on one of existing segment
  const auto CheckIfLieOnSegment = [&]( const pollygon &Polly )
  {
    for (size_t i = 0, cnt = CurrentPolly.Lines.size(); i < cnt; i++)
    {
      vec2 L1 = PointsPool[CurrentPolly.Lines[i].St], L2 = PointsPool[CurrentPolly.Lines[i].End];

      // Check if distance to line less then epsilon
      DBL dist = segment::GetPointLineDistance(Point, L1, L2);
      if (dist < min_dist)
      {
        // Check if point lies between two points
        DBL t = (Point - L1) / (L2 - L1);
        if (t >= 0 && t <= 1)
        {
          vec2 N = vec2(-L1[1], L1[0]).Normalizing() * segment::GetPointHalfPlaneLocation(Point, L1, L2);
          min_dist_pnt = Point - N * (FLT)dist;
          min_dist = dist;
        }
      }
    }
  };

  for (const auto &polly: Walls)
    CheckIfLieOnSegment(polly);
  CheckIfLieOnSegment(CurrentPolly);

  if (min_dist == Epsilon)
    return FALSE;

  if (Result != nullptr)
    *Result = min_dist_pnt;

  return TRUE;
} /* End of 'location::FindNearesPointToSegments' function */

/* Find aproximate point on neares segments or points.
 * ARGUMETNS:
 *   - point to find:
 *       const vec2 &Pnt
 *   - variable to set found point in:
 *       vec2 *Result;
 * RETURNS:
 *   (BOOL) whether point found or not.
 */
BOOL location::FindNearestPoint( const vec2 &Point, vec2 *Result ) const
{
  if (FindNearestPointToPoints(Point, Result))
    return TRUE;
  if (FindNearestPointToSegments(Point, Result))
    return TRUE;
  return FALSE;
} /* End of 'location::FindPointSegments' function */

/* Add point to the pool or return existing.
 * ARGUMENTS:
 *   - point to add:
 *       const vec2 &Pnt;
 * RETURNS:
 *   (size_t) index of added point in pool.
 */
size_t location::PlacePoint( const vec2 &Pnt ) const
{
  size_t FindRes = -1;

  PointsPool.push_back(Pnt);
  return PointsPool.size() - 1;
} /* End oof 'location::AddPoint' function */

/* Place or find existing point.
 * ARGUMENTS:
 *   - point to add or find:
 *       const vec2 &Pnt;
 * RETURNS:
 *   (size_t) index of added or found point in pool.
 */
size_t location::PlaceOrFindPoint( const vec2 &Pnt ) const
{
  size_t FindRes = -1;
  if (GetPointIndex(Pnt, &FindRes))
    return FindRes;

  vec2 LinePoint;
  if (FindNearestPointToSegments(Pnt, &LinePoint))
  {
    PointsPool.push_back(LinePoint);
    return PointsPool.size() - 1;
  }

  PointsPool.push_back(Pnt);
  return PointsPool.size() - 1;
} /* End of 'location::PlaceOrFindPoint' function */

/* Place or find existing point.
 * ARGUMENTS:
 *   - point to add or find:
 *       const vec2 &Pnt;
 * RETURNS:
 *   (size_t) index of added or found point in pool.
 */
size_t location::PlaceOrFindPointToPoint( const vec2 &Pnt ) const
{
  size_t FindRes = -1;
  if (GetPointIndex(Pnt, &FindRes))
    return FindRes;

  PointsPool.push_back(Pnt);
  return PointsPool.size() - 1;
} /* End of 'location::PlaceOrFindPoint' function */

/* Add segment to walls sections container.
 * ARGUMENTS:
 *   - point of line end:
 *       const vec2 &PntEnd;
 * RETURNS:
 *   (BOOL) whether added segment closed current pollygon.
 */
BOOL location::PlaceSegment( const vec2 &PntEnd, const vec2 &PntSt )
{
  size_t SegmentStart, SegmentEnd;
  BOOL IsClosed = FALSE;

  if (CurrentPolly.IsEditing)
  {
    SegmentStart = CurrentPolly.Lines[CurrentPolly.Lines.size() - 1].End;

    if (GetPointIndex(PntEnd, &SegmentEnd))
    {
      CurrentPolly.Lines.push_back(segment(SegmentStart, SegmentEnd));

      // If segment ends on its start point closing them.
      if (SegmentEnd == CurrentPolly.Start)
      {
        if (Walls.size() == 0)
          Walls.push_back(CurrentPolly);
        else
          MergePolly(CurrentPolly, Walls[0], &Walls[0]);

        CurrentPolly = current_pollygon();
        return TRUE;
      }
    }
    else
      SegmentEnd = PlaceOrFindPoint(PntEnd);
  }
  else
  {
    SegmentStart = PlaceOrFindPoint(PntSt);
    SegmentEnd = PlaceOrFindPoint(PntEnd);

    CurrentPolly.IsEditing = TRUE;
    CurrentPolly.Start = SegmentStart;
  }

  CurrentPolly.Lines.push_back(segment(SegmentStart, SegmentEnd));
  return FALSE;
} /* End of 'location::PlaceSegment' function */

/* Set current pollygon close mode.
 * ARGUMENTS:
 *   - should pollygon be merged with previous while closing:
 *       BOOL ShouldMerge;
 * RETURNS: None.
 */
VOID location::SetCurrentPollygonCloseMode( BOOL ShouldMerge )
{
  CurrentPolly.ShouldMerge = ShouldMerge;
} /* End of 'location::SetCurrentPollygonCloseMode' function */

/* Merge pollygons function.
 * ARGUMENTS:
 *   - pollygon to merge with:
 *       const pollygon &Polly1, const pollygon &Polly2;
 *   - pollygon to set merged one in:
 *       pollygon *Merged;
 * RETURNS: None.
 */
VOID location::MergePolly( const pollygon &Polly1, const pollygon &Polly2, pollygon *Merged ) const
{
  INT IntersectionsCount = 0;                       // Intersection point of first pollygon with second
  std::map<size_t, std::vector<size_t>> InterEdjes; // Edjes with start and in pollygons intersection points

  // Creating two pollygons edjes intersections adjacency list function
  const auto IntersectPollygons = [&]( const pollygon &PollyFirst, const pollygon &PollySeccond )
    {
      std::vector<size_t> prev_pnt_inters;

      for (size_t i = 0, cnt = PollyFirst.Lines.size(); i < cnt; i++)
      {
        size_t next = PollyFirst.Lines[i].End;
        size_t curr = PollyFirst.Lines[i].St;
    
        // Get all intersection with another polly
        std::vector<size_t> current_pnt_inters;
        current_pnt_inters.push_back(next);
        current_pnt_inters.push_back(curr);
        for (const segment &seg_to_inter: PollySeccond.Lines)
        {
          vec2 res;
    
          if (segment::LinesIntersect(PointsPool[curr], PointsPool[next], PointsPool[seg_to_inter.St], PointsPool[seg_to_inter.End], &res))
          {
            size_t index = PlaceOrFindPoint(res);
            BOOL is_added = FALSE;
            for (size_t inter: InterEdjes[curr])
              if (inter == index)
              {
                is_added = TRUE;
                break;
              }
            for (size_t inter: current_pnt_inters)
              if (inter == index)
              {
                is_added = TRUE;
                break;
              }
            if (!is_added)
              current_pnt_inters.push_back(index), IntersectionsCount++;
          }
        }
    
        // Sort all intersections by length
        std::sort(current_pnt_inters.begin(), current_pnt_inters.end(),
          [&]( size_t A, size_t B ) -> BOOL
          {
            vec2
              CurrA = PointsPool[A] - PointsPool[curr],
              CurrB = PointsPool[B] - PointsPool[curr];
            return CurrA.Length() < CurrB.Length();
          });
    
        // Add all segmetns as possible edjes of new pollygon
        for (size_t i = 0, cnt = current_pnt_inters.size(); i < cnt; i++)
        {
          if (i != cnt - 1)
          {
            InterEdjes.emplace(current_pnt_inters[i], std::vector<size_t>());
            InterEdjes[current_pnt_inters[i]].push_back(current_pnt_inters[i + 1]);
          }

          if (i != 0)
            InterEdjes[current_pnt_inters[i]].push_back(current_pnt_inters[i - 1]);
        }

        prev_pnt_inters = current_pnt_inters;
      }
    };

  // Fillign possible edjes list for new pollygon
  IntersectPollygons(Polly1, Polly2);
  IntersectPollygons(Polly2, Polly1);

  // Check if no intersections. There is only 3 cases:
  //   - first contains second; - return first
  //   - second contains first; - return second
  //   - first near second;     - return both
  if (IntersectionsCount == 0)
  {
    // Check if first polly inside second
    BOOL IsInside = TRUE;
    for (const segment &seg: Polly1.Lines)  
      if (!IsPointInsidePolly(PointsPool[seg.St], Polly2))
      {
        IsInside = FALSE;
        break;
      }
    if (IsInside)
    {
      *Merged = Polly2;
      return;
    }

    // Check if second contains first
    IsInside = TRUE;
    for (const segment &seg: Polly2.Lines)  
      if (!IsPointInsidePolly(PointsPool[seg.St], Polly1))
      {
        IsInside = FALSE;
        break;
      }
    if (IsInside)
    {
      *Merged = Polly1;
      return;
    }

    // First is near second. Create merged pollygon just by adding segments from second to first.
    pollygon merged(Polly1);
    for (const segment &seg: Polly2.Lines)
      merged.Lines.push_back(seg);
    *Merged = merged;
    return;
  }

  // Find minimum point to start merging from
  vec2 Min(INFINITY);
  size_t MinInd = 0;

  for (const segment &seg: Polly1.Lines)
    if (Min > PointsPool[seg.St])
      Min = PointsPool[seg.St], MinInd = seg.St;

  for (const segment &seg: Polly2.Lines)
    if (Min > PointsPool[seg.St])
      Min = PointsPool[seg.St], MinInd = seg.St;

  // Find the most left point and add new segment to merged pollygon with end in that point
  pollygon merged;
  size_t curr = MinInd, prev = -1;
  do
  {
    auto curr_vertex = InterEdjes.find(curr);
    size_t next = curr_vertex->second[0];
    vec2
      curr_pnt = PointsPool[curr],
      next_pnt = PointsPool[next];

    for (size_t i = 1, cnt = curr_vertex->second.size(); i < cnt; i++)
      if ((curr_vertex->second[i] != prev && curr_vertex->second[i] != next && curr_vertex->second[i] != curr &&
           segment::GetPointHalfPlaneLocation(PointsPool[curr_vertex->second[i]], curr_pnt, next_pnt) == 1) ||
          next == prev)
      {
        next_pnt = PointsPool[curr_vertex->second[i]];
        next = curr_vertex->second[i];
        i = 0;
      }

    merged.Lines.push_back(segment(curr, next));
    prev = curr;
    curr = next;
  } while (curr != MinInd);

  *Merged = merged;
} /* End of 'location::MergePollys' function */

/* Check if point inside pollygon.
 * ARGUMENTS:
 *   - point to check:
 *       const vec3 &Pnt;
 * RETURNS:
 *   (BOOL) whether point in pollygon or not.
 */
BOOL location::IsPointInsidePolly( const vec2 &Pnt, const pollygon &Polly ) const
{
  INT cnt = 0;
  vec2 PntInfLine = Pnt + vec2(10000000, 0);

  for (const segment &seg: Polly.Lines)
    cnt += segment::LinesIntersect(Pnt, PntInfLine, PointsPool[seg.St], PointsPool[seg.End]);

  return cnt % 2 != 0;
} /* End of 'pollygon::IsInside' function */

/* Draw wall sections function.
 * ARGUMENTS: None.
 * RETURNS: None.
 */
VOID location::Draw( VOID ) const
{
  const auto DrawPolly = [&]( const pollygon &Wall, BOOL IsCurrent )
  {
    size_t cnt = Wall.Lines.size();
    for (size_t i = 0; i < cnt; i++)
    {
      glBegin(GL_LINES);
      glColor3f(!IsCurrent * 0.4, !IsCurrent * 0.8, !IsCurrent * 0.3);
      glVertex2d(PointsPool[Wall.Lines[i].St][0], PointsPool[Wall.Lines[i].St][1]);
      glVertex2d(PointsPool[Wall.Lines[i].End][0], PointsPool[Wall.Lines[i].End][1]);
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
  DrawPolly(CurrentPolly, TRUE);
} /* End pf 'location::Draw' function */