//
//  lines_intersects.hpp
//  1C_task_
//
//  Created by Александр on 13.04.2021.
//

#ifndef lines_intersects_h
#define lines_intersects_h

using point = std::pair<int, int>;

inline int area(point a, point b, point c) {
    return (b.first  - a.first)  * (c.second - a.second) -
           (b.second - a.second) * (c.first  - a.first);
}
 
inline bool intersect_(int a, int b, int c, int d) {
    if (a > b)
        std::swap(a, b);
    if (c > d)
        std::swap(c, d);
    return std::max(a, c) <= std::min(b, d);
}
 
bool intersect(point a, point b, point c, point d) {
    return intersect_(a.first,  b.first,  c.first,  d.first)
        && intersect_(a.second, b.second, c.second, d.second)
        && area(a, b, c) * area(a, b, d) <= 0
        && area(c, d, a) * area(c, d, b) <= 0;
}


#endif /* lines_intersects_h */
