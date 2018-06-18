#include "PivotAlgorithms.h"

size_t pivotRandom(const std::vector<Circle>& circles) {
    return circles.size();
}

size_t pivotMax(const std::vector<Circle>& circles) {
    intmax_t mini = 0;
    size_t index = circles.size();
    for (size_t i = 0; i < circles.size(); i++) {
        intmax_t value = circles[i].costPerFlow * circles[i].flow;
        if (value < mini) {mini = value; index = i;}
    }
    return index;
}

