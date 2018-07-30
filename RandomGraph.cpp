#include <random>
#include <time.h>
#include <algorithm>

#include "RandomGraph.h"

Network RandomGraph::getNetwork() {
    return n;
}

RandomGraph::RandomGraph(size_t maxNoNodes, intmax_t maxFlow, intmax_t maxCost) {
    //initialize random number generator
    std::mt19937 rng;
    rng.seed(static_cast<long unsigned int>(time(0)));

    std::uniform_int_distribution<size_t> noNodesRng(2, maxNoNodes);
    std::uniform_int_distribution<intmax_t> flowRng(1, maxFlow);
    std::uniform_int_distribution<intmax_t> costRng(0, maxCost);
    std::uniform_int_distribution<short> boolean(0,1);

    intmax_t flow = flowRng(rng), sumSources = 0, sumSinks = 0;
    size_t nodes = noNodesRng(rng);
    std::uniform_int_distribution<size_t> nodesRng(0, nodes - 1);

    //<= threshold potential sources, all other potential sinks
    size_t threshold = nodes/2;
    std::vector<intmax_t> b_values(nodes, 0);
    while (sumSources != flow or sumSinks != flow) {
        size_t temp = nodesRng(rng);
        if (temp < threshold and sumSources != flow) {
            b_values[temp] += 1;
            sumSources +=1;
        }
        if (temp >= threshold and sumSinks != flow) {
            b_values[temp] -= 1;
            sumSinks +=1;
        }
    }

    for (size_t i = 0; i < b_values.size(); i++) {
        n.addNode(b_values[i]);
    }

    intmax_t maxB = std::max(*(std::max_element(b_values.begin(), b_values.end())), - *(std::min_element(b_values.begin(), b_values.end())));
    std::uniform_int_distribution<intmax_t> maxBRng(1, maxB);
    //now for some edges
    //all nodes get outgoing edges (ingoing for sinks),
    //until at least the max b_value to the power of two is reached (in sum)
    //random in or outgoing for transit

    for (size_t i = 0; i < b_values.size(); i++) {
        intmax_t sum = 0;
        size_t iterations = 0;
        //source
        if (b_values[i] > 0) {
            while (sum < maxB*maxB and iterations < nodes) {
                size_t temp = nodesRng(rng);
                intmax_t cap = maxBRng(rng);
                if (n.addEdge(Edge(i, temp, costRng(rng), cap))) {
                    sum += cap;
                }
                iterations++;
            }
        }
        //sink
        if (b_values[i] < 0) {
            while (sum < maxB*maxB and iterations < nodes) {
                size_t temp = nodesRng(rng);
                intmax_t cap = maxBRng(rng);
                if (n.addEdge(Edge(temp, i, costRng(rng), cap))) {
                    sum += cap;
                }
                iterations++;
            }
        }
        //transit
        if (b_values[i] == 0) {
            while (sum < maxB*maxB and iterations < nodes) {
                size_t temp = nodesRng(rng);
                intmax_t cap = maxBRng(rng);
                bool direction = boolean(rng);
                if (direction and n.addEdge(Edge(i, temp, costRng(rng), cap))) {sum += cap;}
                if ((not direction) and n.addEdge(Edge(temp, i, costRng(rng), cap)))  {sum += cap;}
                iterations++;
            }
        }
    }
}
