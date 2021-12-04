#include <set>
#include <map>
#include <algorithm>

#include "location.h"

points_pool location::PointsPool = points_pool();

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
FLT segment::GetPointDistance( const vec2 &Pnt ) const
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
INT segment::GetPointHalfPlaneLocation( const vec2 &Pnt ) const
{
  DBL cross_product = ((DBL)location::PointsPool[End][0] - (DBL)location::PointsPool[St][0]) * ((DBL)Pnt[1] - (DBL)location::PointsPool[St][1]) -
                      ((DBL)location::PointsPool[End][1] - (DBL)location::PointsPool[St][1]) * ((DBL)Pnt[0] - (DBL)location::PointsPool[St][0]);
  return  cross_product > 0.00001 ?  1 :
          cross_product < 0.00001 ? -1 : 0;
} /* End of 'segment::GetPointHalfPlane' function */

/* Get location of a point in a plane relative to a straight line function.
 * ARGUMENTS:
 *   - point:
 *       const vec2 &Pnt;
 * RETURNS:
 *   (INT) -1 - point is on the right half-plane,
 *          0 - point is on the line,
 *          1 - point in on the left half-plane.
 */
INT segment::GetPointHalfPlaneLocation( const vec2 &Pnt, const vec2 &L1, const vec2 &L2 )
{
  DBL cross_product = ((DBL)L2[0] - (DBL)L1[0]) * ((DBL)Pnt[1] - (DBL)L1[1]) -
                      ((DBL)L2[1] - (DBL)L1[1]) * ((DBL)Pnt[0] - (DBL)L1[0]);
  return  cross_product > 0.00001 ?  1 :
          cross_product < 0.00001 ? -1 : 0;
} /* End of 'segment::GetPointHalfPlane' function */

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
DBL segment::IsPointOnSegment( const vec2 &Pnt, DBL CheckRadius, vec2 *Result ) const
{
  vec2 min_dist_pnt;
  DBL dist = 0;
  BOOL is_found = FALSE;

  if (CheckRadius <= 0)
  {
    DBL t = (Pnt - location::PointsPool[St]) / (location::PointsPool[End] - location::PointsPool[St]);
    if (t >= 0 && t <= 1)
    {
      vec2 N = vec2(-location::PointsPool[St][1], location::PointsPool[St][0]).Normalizing() * GetPointHalfPlaneLocation(Pnt);
      min_dist_pnt = Pnt;
      is_found = TRUE;
    }
  }
  else
  {
    // Check if distance to line less then epsilon
    dist = GetPointDistance(Pnt);
    if (dist < CheckRadius)
    {
      // Check if point lies between two points
      DBL t = (Pnt - location::PointsPool[St]) / (location::PointsPool[End] - location::PointsPool[St]);
      if (t >= 0 && t <= 1)
      {
        vec2 N = vec2(-location::PointsPool[St][1], location::PointsPool[St][0]).Normalizing() * GetPointHalfPlaneLocation(Pnt);
        min_dist_pnt = Pnt - N * (FLT)dist;
        is_found = TRUE;
      }
    }
  }

  if (Result != nullptr)
    *Result = min_dist_pnt;

  return is_found ? CheckRadius == 0 ? 0.0001 : dist : 0;
} /* End of 'segment::IsPointOnSegment' function */

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
BOOL segment::LinesIntersect( const vec2 &P0, const vec2 &P1, vec2 *Result, BOOL CheckLength ) const
{
  FLT
    t =
      ((P0[0] - location::PointsPool[St][0]) * (location::PointsPool[St][1] - location::PointsPool[End][1]) - (P0[1] - location::PointsPool[St][1]) * (location::PointsPool[St][0] - location::PointsPool[End][0])) /
      ((P0[0] - P1[0]) * (location::PointsPool[St][1] - location::PointsPool[End][1]) - (P0[1] - P1[1]) * (location::PointsPool[St][0] - location::PointsPool[End][0])),
    s =
      ((P0[0] - location::PointsPool[St][0]) * (P0[1] - P1[1]) - (P0[1] - location::PointsPool[St][1]) * (P0[0] - P1[0])) /
      ((P0[0] - P1[0]) * (location::PointsPool[St][1] - location::PointsPool[End][1]) - (P0[1] - P1[1]) * (location::PointsPool[St][0] - location::PointsPool[End][0]));

  if ((t < 0 || t > 1 || s < 0 || s > 1) && CheckLength)
    return FALSE;

  if (Result != nullptr) 
    *Result = P0 + (P1 - P0) * t;
  return TRUE;
} /* End of 'segment::LinesIntersect' function */

/*
 * Polygon functions
 */

/* Merge pollygons function.
 * ARGUMENTS:
 *   - pollygon to merge with:
 *       const pollygon &Polly1, const pollygon &Polly2;
 *   - pollygon to set merged one in:
 *       pollygon *Merged;
 * RETURNS: None.
 */
VOID pollygon::Merge( const pollygon &Polly, pollygon *Merged ) const
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
    
          if (PollyFirst.Lines[i].LinesIntersect(location::location::PointsPool[seg_to_inter.St], location::PointsPool[seg_to_inter.End], &res))
          {
            size_t index = location::PointsPool.Add(res);
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
              CurrA = location::PointsPool[A] - location::PointsPool[curr],
              CurrB = location::PointsPool[B] - location::PointsPool[curr];
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
  IntersectPollygons(*this, Polly);
  IntersectPollygons(Polly, *this);

  // Check if no intersections. There is only 3 cases:
  //   - first contains second; - return first
  //   - second contains first; - return second
  //   - first near second;     - return both
  if (IntersectionsCount == 0)
  {
    // Check if first polly inside second
    BOOL IsInside = TRUE;
    for (const segment &seg: Lines)  
      if (!Polly.IsPointInside(location::PointsPool[seg.St]))
      {
        IsInside = FALSE;
        break;
      }
    if (IsInside)
    {
      *Merged = Polly;
      return;
    }

    // Check if second contains first
    IsInside = TRUE;
    for (const segment &seg: Polly.Lines)  
      if (!IsPointInside(location::PointsPool[seg.St]))
      {
        IsInside = FALSE;
        break;
      }
    if (IsInside)
    {
      *Merged = *this;
      return;
    }

    // First is near second. Create merged pollygon just by adding segments from second to first.
    pollygon merged(*this);
    for (const segment &seg: Polly.Lines)
      merged.Lines.push_back(seg);
    *Merged = merged;
    return;
  }

  // Find minimum point to start merging from
  vec2 Min(INFINITY);
  size_t MinInd = 0;

  for (const segment &seg: Lines)
    if (Min > location::PointsPool[seg.St])
      Min = location::PointsPool[seg.St], MinInd = seg.St;

  for (const segment &seg: Polly.Lines)
    if (Min > location::PointsPool[seg.St])
      Min = location::PointsPool[seg.St], MinInd = seg.St;

  // Find the most left point and add new segment to merged pollygon with end in that point
  pollygon merged;
  size_t curr = MinInd, prev = -1;
  do
  {
    auto curr_vertex = InterEdjes.find(curr);
    size_t next = curr_vertex->second[0];
    vec2
      curr_pnt = location::PointsPool[curr],
      next_pnt = location::PointsPool[next];

    for (size_t i = 1, cnt = curr_vertex->second.size(); i < cnt; i++)
      if ((curr_vertex->second[i] != prev && curr_vertex->second[i] != next && curr_vertex->second[i] != curr &&
           segment(curr, next).GetPointHalfPlaneLocation(location::PointsPool[curr_vertex->second[i]]) == 1) || next == prev)
      {
        next_pnt = location::PointsPool[curr_vertex->second[i]];
        next = curr_vertex->second[i];
        i = 0;
      }

    merged.Lines.push_back(segment(curr, next));
    prev = curr;
    curr = next;
  } while (curr != MinInd);

  *Merged = merged;
} /* End of 'pollygon::Merge' function */

/* Check if point inside pollygon.
 * ARGUMENTS:
 *   - point to check:
 *       const vec3 &Pnt;
 * RETURNS:
 *   (BOOL) whether point in pollygon or not.
 */
BOOL pollygon::IsPointInside( const vec2 &Pnt ) const
{
  INT cnt = 0;
  vec2 PntInfLine = Pnt + vec2(10000000, 0);

  for (const segment &seg: Lines)
    cnt += seg.LinesIntersect(Pnt, PntInfLine);

  return cnt % 2 != 0;
} /* End of 'pollygon::IsPointInside' function */

/*
 * Location functions
 */

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
  if (!FindOnLines)
  {
    size_t index;
    BOOL is_found = PointsPool.Get(Pnt, &index, PlaceingRadius);
    if (is_found)
    {
      if (Result != nullptr)
        *Result = PointsPool[index];
      return TRUE;
    }
    return is_found;
  }
  else
  {
    size_t index;
    BOOL is_found = PointsPool.Get(Pnt, &index, PlaceingRadius);
    if (is_found)
    {
      if (Result != nullptr)
        *Result = PointsPool[index];
      return TRUE;
    }

    vec2 inter;
    BOOL IsInters = FALSE;

    // Check if point lies on one of existing segment
    const auto CheckIfLieOnSegment = [&]( const pollygon &Polly )
    {
      DBL min_dist = PlaceingRadius;
      for (size_t i = 0, cnt = Polly.Lines.size(); i < cnt; i++)
      {
        vec2 L1 = PointsPool[Polly.Lines[i].St], L2 = PointsPool[Polly.Lines[i].End];

        vec2 Line = L2 - L1;
        vec2 N = -vec2(Line[1], -Line[0]).Normalize() * Polly.Lines[i].GetPointHalfPlaneLocation(Pnt);
        DBL dist = N & (Pnt - L1);

        if (Polly.Lines[i].LinesIntersect(Pnt, CurrPolyLastLineEnd(), &inter, FALSE) && dist < min_dist)
          min_dist = dist, IsInters = TRUE;
      }
    };

    for (const auto &polly: Walls)
      CheckIfLieOnSegment(polly);
    CheckIfLieOnSegment(CurrPoly);

    if (!IsInters)
      return FALSE;
    if (Result != nullptr)
      *Result = inter;

    return TRUE;
  }
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

/*
 * Function that handles the drawing of a circle using the line loop
 * method. This will create a hollow circle.
 *
 * Params:
 *	x (GLFloat) - the x position of the center point of the circle
 *	y (GLFloat) - the y position of the center point of the circle
 *	radius (GLFloat) - the radius that the painted circle will have
 */
void drawHollowCircle(GLfloat x, GLfloat y, GLfloat radius)
{
  int i;
  int lineAmount = 30;

  GLfloat twicePi = 2.0f * mth::PI;

  glBegin(GL_LINE_LOOP);
    for(i = 0; i <= lineAmount;i++)
      glVertex2f(x + (radius * cos(i *  twicePi / lineAmount)), 
                 y + (radius* sin(i * twicePi / lineAmount)));
  glEnd();
}

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
      drawHollowCircle(PointsPool[Wall.Lines[i].St][0], PointsPool[Wall.Lines[i].St][1], PlaceingRadius);
      drawHollowCircle(PointsPool[Wall.Lines[i].End][0], PointsPool[Wall.Lines[i].End][1], PlaceingRadius);
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