#include "PivotAlgorithms.h"

size_t pivotRandom(const std::vector<Circle>& circles) {
    return circles.size();
}

size_t pivotMaxVal(const std::vector<Circle>& circles) {
    intmax_t mini = 0;
    size_t index = circles.size();
    for (size_t i = 0; i < circles.size(); i++) {
        intmax_t value = circles[i].costPerFlow;
        if (value < mini) {mini = value; index = i;}
    }
    return index;
}

size_t pivotMaxRev(const std::vector<Circle>& circles) {
    intmax_t mini = 0;
    size_t index = circles.size();
    for (size_t i = 0; i < circles.size(); i++) {
        intmax_t value = circles[i].costPerFlow * circles[i].flow;
        if (value < mini) {mini = value; index = i;}
    }
    //if revenue = 0, choose most negative circle
    if (index == circles.size()) {return pivotMaxVal(circles);}
    return index;
}

