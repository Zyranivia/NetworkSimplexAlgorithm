#ifndef RANDOMGRAPH_H_INCLUDED
#define RANDOMGRAPH_H_INCLUDED

#include "Network.h"

class RandomGraph {

public:
    RandomGraph (size_t maxNoNodes, intmax_t maxFlow, intmax_t maxCost);
    Network getNetwork();


private:
    Network n = Network(0);

};

#endif // RANDOMGRAPH_H_INCLUDED
