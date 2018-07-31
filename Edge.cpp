#include "Edge.h"

//edges are initialized with a flow of zero
//isResidual has default value false
Edge::Edge (size_t _node0, size_t _node1, intmax_t _cost, intmax_t _capacity, bool _isResidual) :
    node0(_node0), node1(_node1), cost(_cost), capacity(_capacity), flow(0), isResidual(_isResidual) {}

//overflow (huehuehe) is handled, I think
bool Edge::changeFlowPossible(intmax_t f) {
    intmax_t temp = flow + f;

    if (temp < 0 or temp > capacity) {
        return false;
    }
    return true;
}

bool Edge::changeFlow (intmax_t f) {

    if (changeFlowPossible(f)) {
        flow += f;
        return true;
    }
    return false;
}

void Edge::invert () {
    size_t temp = node0;
    node0 = node1;
    node1 = temp;

    cost = -cost;
    flow = capacity - flow;
    isResidual = not isResidual;
}

void Edge::toggleCost() {
    if (isToggled) {
        cost = toggledCost;
        toggledCost = 0;
    }
    else {
        toggledCost = cost;
        cost = 0;
    }
    isToggled = not isToggled;
}
