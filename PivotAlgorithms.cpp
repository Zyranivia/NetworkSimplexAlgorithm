#include <random>
#include <chrono>

#include "PivotAlgorithms.h"

size_t pivotRandom(const std::vector<Circle>& circles) {
    std::vector<size_t> negCircles;
    for (size_t i = 0; i < circles.size(); i++) {
        if (circles[i].costPerFlow < 0) {negCircles.push_back(i);}
    }
    if (negCircles.empty()) {return circles.size();}

    std::mt19937 rng;
    rng.seed(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<size_t> rand(0, negCircles.size() - 1);

    return negCircles[rand(rng)];
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

