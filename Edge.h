#ifndef EDGE_H_INCLUDED
#define EDGE_H_INCLUDED

#include <cstdint>
#include <cstddef>

//residual edges have negative cost
class Edge {

public:
    //edges are initialized with a flow of zero
    Edge (size_t node0, size_t node1, intmax_t cost, intmax_t capacity, bool isResidual = false);

    //returns true when flow change is possible
    bool changeFlowPossible (intmax_t value);
    //returns true when flow change was successful
    bool changeFlow (intmax_t value);
    //toggles cost betweeen itself and 0
    void toggleCost ();

    //turn edge into residual edge and vice versa
    void invert ();

//private:
    size_t node0, node1;
    intmax_t cost, capacity, flow, toggledCost;
    bool isResidual, isToggled = false;

};

#endif // EDGE_H_INCLUDED
