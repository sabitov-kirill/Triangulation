#include <set>
#include <map>
#include <algorithm>

#include "location.h"

/* Merge polygons function.
 * ARGUMENTS:
 *   - polygon to merge with:
 *       const polygon &Polly1, const polygon &Polly2;
 *   - polygon to set merged one in:
 *       polygon *Merged;
 * RETURNS: None.
 */
VOID polygon::Merge(const polygon &Polly, polygon *Merged) const
{
  BOOL IsIntersection = FALSE;                      // Show wheather any segments in polygons intersected or not.
  std::map<size_t, std::vector<size_t>> InterEdjes; // Adjacnency list of edjes with start and end in polygons segments intersection points

  // Fill adjacnency list of edjes list function
  const auto IntersectPolygons = [&](const polygon &PollyFirst, const polygon &PollySeccond)
  {
    std::vector<size_t> prev_pnt_inters;

    for (size_t i = 0, cnt = PollyFirst.Lines.size(); i < cnt; i++)
    {
      size_t next_pnt_ind = PollyFirst.Lines[i].End;
      size_t curr_pnt_ind = PollyFirst.Lines[i].St;

      // Get all intersection with another polly
      std::vector<size_t> current_pnt_inters;
      current_pnt_inters.push_back(next_pnt_ind);
      current_pnt_inters.push_back(curr_pnt_ind);
      for (const segment &seg_to_inter : PollySeccond.Lines)
      {
        vec2 res;

        // Check is segments intersects
        if (PollyFirst.Lines[i].Intersect(location::PointsPool[seg_to_inter.St], location::PointsPool[seg_to_inter.End], &res))
        {
          size_t index = location::PointsPool.Add(res);
          IsIntersection = TRUE;

          // Add intersection point only if its not added already
          if (std::find(InterEdjes[curr_pnt_ind].begin(), InterEdjes[curr_pnt_ind].end(), index) == InterEdjes[curr_pnt_ind].end() &&
              std::find(current_pnt_inters.begin(), current_pnt_inters.end(), index) == current_pnt_inters.end())
            current_pnt_inters.push_back(index);
        }
      }

      // Sort all intersections by length
      std::sort(current_pnt_inters.begin(), current_pnt_inters.end(),
                [&](size_t A, size_t B) -> BOOL
                {
                  vec2
                    CurrA = location::PointsPool[A] - location::PointsPool[curr_pnt_ind],
                    CurrB = location::PointsPool[B] - location::PointsPool[curr_pnt_ind];
                  return CurrA.Length() < CurrB.Length();
                });

      // Add all segmetns as possible edjes of new polygon
      for (size_t i = 0, cnt = current_pnt_inters.size(); i < cnt; i++)
      {
        if (i != cnt - 1)
        {
          InterEdjes.emplace(current_pnt_inters[i], std::vector<size_t>());

          if (std::find(InterEdjes[current_pnt_inters[i]].begin(),
                        InterEdjes[current_pnt_inters[i]].end(),
                        current_pnt_inters[i + 1]) == InterEdjes[current_pnt_inters[i]].end())
            InterEdjes[current_pnt_inters[i]].push_back(current_pnt_inters[i + 1]);
        }

        if (i != 0 && std::find(InterEdjes[current_pnt_inters[i]].begin(),
                                InterEdjes[current_pnt_inters[i]].end(),
                                current_pnt_inters[i - 1]) == InterEdjes[current_pnt_inters[i]].end())
          InterEdjes[current_pnt_inters[i]].push_back(current_pnt_inters[i - 1]);
      }

      prev_pnt_inters = current_pnt_inters;
    }
  };

  // Fillign possible edjes list for new polygon
  IntersectPolygons(*this, Polly);
  IntersectPolygons(Polly, *this);

  // Check if no intersections. Check all 3 cases:
  //   - first contains second; - return first
  //   - second contains first; - return second
  //   - first near second;     - return both
  if (!IsIntersection)
  {
    // Check if first polly inside second
    BOOL IsInside = TRUE;
    for (const segment &seg : Lines)
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
    for (const segment &seg : Polly.Lines)
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

    // First is near second. Create merged polygon just by adding segments from second to first.
    polygon merged(*this);
    for (const segment &seg : Polly.Lines)
      merged.Lines.push_back(seg);
    *Merged = merged;
    return;
  }

  // Find minimum point in two polygons to start merging from
  vec2 first_pnt(INFINITY);
  size_t first_pnt_ind = 0;

  for (const segment &seg : Lines)
    if (first_pnt > location::PointsPool[seg.St])
      first_pnt = location::PointsPool[seg.St], first_pnt_ind = seg.St;

  for (const segment &seg : Polly.Lines)
    if (first_pnt > location::PointsPool[seg.St])
      first_pnt = location::PointsPool[seg.St], first_pnt_ind = seg.St;

  // Find the most left point and add new segment to merged polygon with end in that point
  // polygon merged;
  // 
  // std::map<size_t, BOOL> visits;
  // 
  // for (const auto &edje: InterEdjes)
  //   visits[edje.first] = FALSE;
  // 
  // size_t curr_pnt_ind = first_pnt_ind;
  // do
  // {
  //   auto curr_vertex = InterEdjes.find(curr_pnt_ind);
  //   vec2 curr_pnt = location::PointsPool[curr_pnt_ind];
  // 
  //   DBL max_angle = 0;
  //   size_t max_angle_ind = curr_vertex->second[0];
  // 
  //   for (size_t i = 0, cnt = curr_vertex->second.size(); i < cnt; i++)
  //   {
  //     if (visits[curr_vertex->second[i]])
  //       continue;
  // 
  //     vec2 next_pnt = location::PointsPool[curr_vertex->second[i]];
  //     DBL angle = acos(curr_pnt.Normalizing() & next_pnt.Normalizing());
  // 
  //     vec3 cross = (vec3(curr_pnt, 0) % vec3(next_pnt, 0));
  //     if (cross[2] < 0)
  //       angle = 2 * mth::PI - angle;
  // 
  //     if (angle > max_angle)
  //       max_angle = angle, max_angle_ind = curr_vertex->second[i];
  //   }
  // 
  //   merged.Lines.push_back(segment(curr_pnt_ind, max_angle_ind));
  //   visits[curr_pnt_ind] = TRUE;
  //   curr_pnt_ind = max_angle_ind;
  // } while (curr_pnt_ind != first_pnt_ind);

  // Find the most left point and add new segment to merged polygon with end in that point
  polygon merged;
  size_t curr_pnt_ind = first_pnt_ind, prev = -1;
  do
  {
    auto curr_vertex = InterEdjes.find(curr_pnt_ind);
    size_t next_pnt_ind = curr_vertex->second[0];
    vec2
      curr_pnt = location::PointsPool[curr_pnt_ind],
      next_pnt = location::PointsPool[next_pnt_ind];

    for (size_t i = 1, cnt = curr_vertex->second.size(); i < cnt; i++)
      if ((curr_vertex->second[i] != prev && curr_vertex->second[i] != next_pnt_ind && curr_vertex->second[i] != curr_pnt_ind &&
          segment(curr_pnt_ind, next_pnt_ind).GetPointHalfPlaneLocation(location::PointsPool[curr_vertex->second[i]]) == 1) || next_pnt_ind == prev)
      {
        next_pnt = location::PointsPool[curr_vertex->second[i]];
        next_pnt_ind = curr_vertex->second[i];
        i = 0;
      }

    merged.Lines.push_back(segment(curr_pnt_ind, next_pnt_ind));
    prev = curr_pnt_ind;
    curr_pnt_ind = next_pnt_ind;
  } while (curr_pnt_ind != first_pnt_ind);

  *Merged = merged;
} /* End of 'polygon::Merge' function */

/* Check if point inside polygon.
 * ARGUMENTS:
 *   - point to check:
 *       const vec3 &Pnt;
 * RETURNS:
 *   (BOOL) whether point in polygon or not.
 */
BOOL polygon::IsPointInside(const vec2 &Pnt) const
{
  INT cnt = 0;
  vec2 PntInfLine = Pnt + vec2(10000000, 0);

  for (const segment &seg : Lines)
    cnt += seg.Intersect(Pnt, PntInfLine);

  return cnt % 2 != 0;
} /* End of 'polygon::IsPointInside' function */