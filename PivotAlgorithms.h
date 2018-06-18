#ifndef PIVOTALGORITHMS_H_INCLUDED
#define PIVOTALGORITHMS_H_INCLUDED

#include <vector>
#include <cstddef>

#include "Circle.h"

//return index of chosen circle or circles.size() if
//no circle can be chosen

//returns a random circle with negative cost
//TODO not yet implemented
size_t pivotRandom(const std::vector<Circle>& circles);
//returns most negative circle
size_t pivotMax(const std::vector<Circle>& circles);

#endif // PIVOTALGORITHMS_H_INCLUDED
