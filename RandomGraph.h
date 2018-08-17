#ifndef RANDOMGRAPH_H_INCLUDED
#define RANDOMGRAPH_H_INCLUDED

#include "Network.h"

//saves the change done to a network
//either an edge was changed or two b_values
struct Action {
    Action (Edge _oldE, Edge _newE) : oldE(_oldE), newE(_newE), edgeCase(true) {};
    Action (std::tuple<size_t, size_t, intmax_t> _b_change) : b_change(_b_change), edgeCase(false) {};

    Edge oldE, newE;
    std::tuple<size_t, size_t, intmax_t> b_change;
    bool edgeCase;
};

class RandomGraph {

public:
    RandomGraph (Network _n) : n(_n) {};
    //create an somehow evenly distributed network
    RandomGraph (size_t maxNoNodes, intmax_t maxFlow, intmax_t maxCost);
    Network getNetwork();

    //just random moves and the optimum under all networks
    void evolve (size_t steps);
    //somewhat more sophisticated
    void smartEvolve (size_t steps, std::vector<double> distribution);

private:
    Network n = Network(0);
    std::vector<Action> takenActions;
};

#endif // RANDOMGRAPH_H_INCLUDED
