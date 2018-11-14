#include <random>
#include <time.h>
#include <algorithm>
#include <numeric>

#include "RandomGraph.h"

#include <iostream>
#include "Algorithm.h"
#include "PivotAlgorithms.h"

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
    networkSave = n;
}

//allows for deletion and insertion of nodes
void RandomGraph::evolve (size_t steps) {
    std::mt19937 rng;
    rng.seed(static_cast<long unsigned int>(time(0)));

    std::uniform_real_distribution<> choiceOfOperation(0., 1.);

    Network opt = Network(0);
    double opt_value = 0;
    //operations and their likelyhood
    std::vector<double> likelyhood = {0.08, 0.02, 0, 0, 0.1, 0.4, 0.4};
    for (size_t i = 0; i < steps; i++) {
        double rand = choiceOfOperation(rng);
        size_t op = 0;
        for (size_t j = 0; j < likelyhood.size(); j++) {
            if (rand < likelyhood[j]) {op = j; break;}
            else {rand -= likelyhood[j];}
        }

        if (n.getNoOfEdges() == 0) {op = 0;}
        if (n.getNoOfNodes() < 2) {op = 2;}

        //modify n
        switch (op) {
            //add edge
            case 0 :
                {std::uniform_int_distribution<intmax_t> randNode (0, n.getNoOfNodes()-1);
                intmax_t nodeA = n.getNode(randNode(rng)), nodeB = n.getNode(randNode(rng));
                std::uniform_int_distribution<intmax_t> fakeRan (100, 1000);
                n.addEdge(Edge(nodeA, nodeB, fakeRan(rng), fakeRan(rng)));
                }break;
            //remove edge
            case 1 :
                {std::uniform_int_distribution<intmax_t> randEdge (0, n.getNoOfEdges()-1);
                Edge e = n.getEdge(randEdge(rng));
                n.deleteEdge(e.node0, e.node1);
                }break;
            //add node
            case 2 :
                n.addNode(0);
                break;
            //remove node
            case 3 :
                {std::uniform_int_distribution<intmax_t> randNode (0, n.getNoOfNodes()-1);
                n.deleteNode(n.getNode(randNode(rng)));
                }break;
            //change b_value
            case 4 :
                {std::uniform_int_distribution<intmax_t> randNode (0, n.getNoOfNodes()-1);
                intmax_t nodeA = n.getNode(randNode(rng)), nodeB = n.getNode(randNode(rng));
                std::uniform_int_distribution<intmax_t> fakeRan (50, 200);
                intmax_t b = fakeRan(rng);
                n.changeBvalue(nodeA, b);
                n.changeBvalue(nodeB, -b);
                }break;
            //change cost of edge
            case 5 :
                {std::uniform_int_distribution<intmax_t> randEdge (0, n.getNoOfEdges()-1);
                Edge e = n.getEdge(randEdge(rng));
                n.deleteEdge(e.node0, e.node1);
                std::uniform_int_distribution<intmax_t> newCost(100, 2*e.cost + 100);
                e.cost = newCost(rng);
                n.addEdge(e);
                }break;
            //change capacity of edge
            case 6 :
                {std::uniform_int_distribution<intmax_t> randEdge (0, n.getNoOfEdges()-1);
                Edge e = n.getEdge(randEdge(rng));
                n.deleteEdge(e.node0, e.node1);
                std::uniform_int_distribution<intmax_t> newCap(100, 2*e.capacity + 100);
                e.capacity = newCap(rng);
                n.addEdge(e);
                }break;
        }

        //evaluate n
        intmax_t itNum = 0;
        int phiIt = 0;
        for (; phiIt < 1; phiIt++) {
            Network changed = n;
            //changed.randomNoise(0.1);
            Algorithm alg = Algorithm (changed, pivotMaxRev);
            alg.solution();
            itNum += alg.getNoOfIter();
        }
        double temp = itNum / (double) (phiIt*n.getNoOfNodes());
        n.clean();
        if (temp > opt_value) {opt = n; opt_value = temp;}
    }

    n = opt;
    networkSave = n;
}

void RandomGraph::smartEvolve (size_t steps, std::vector<double> distribution) {
    std::mt19937 rng;
    rng.seed(static_cast<long unsigned int>(time(0)));
    std::uniform_int_distribution<intmax_t> randNode (0, n.getNoOfNodes()-1);

    size_t opt_pos = 0;
    double opt_value = 0;

    //last entry of distribution is case 5 — undoing steps
    distribution.push_back(0);
    for (size_t i = 0; i < steps; i++) {
        //evaluate n
        intmax_t itNum = 0;
        int phiIt = 0;
        for (; phiIt < 1; phiIt++) {
            Network changed = n;
            //changed.randomNoise(0.1);
            Algorithm alg = Algorithm (changed, pivotMaxRev);
            alg.solution(true);
            itNum += alg.getNoOfIter();
        }
        double temp = itNum / (double) (phiIt*n.getNoOfNodes());
        n.clean();
        if (temp > opt_value) {opt_pos = takenActions.size(); opt_value = temp;}

        distribution.back() = (opt_value == 0 ? 0 : (1 - temp/opt_value));

        //choice of operation
        double sum = std::accumulate(distribution.begin(), distribution.end(), 0.);
        std::uniform_real_distribution<> choiceOfOperation(0., sum);

        double rand = choiceOfOperation(rng);
        size_t op = 0;
        //choose operation in regards to distribution
        for (size_t j = 0; j < distribution.size(); j++) {
            if (rand < distribution[j]) {op = j; break;}
            else {rand -= distribution[j];}
        }

        //if there are no edges yet, just insert one
        if (n.getNoOfEdges() == 0) {op = 0;}

        //if an Edge goes from node a to node a, it won’t be inserted and
        //Network::addEdge() returns false
        switch (op) {
            //add edge
            case 0 :
                {//don’t do anything if n is already complete
                if (n.getNoOfEdges() == n.getNoOfNodes()*(n.getNoOfNodes() - 1)) {break;}
                std::pair<size_t, size_t> e = randomMissingEdge();
                std::uniform_int_distribution<intmax_t> fakeRan (1, 10);
                Edge newE = Edge(e.first, e.second, fakeRan(rng), fakeRan(rng));
                if (n.addEdge(newE)) {
                    takenActions.push_back(Action(Edge(), newE));
                }
                }break;
            //remove edge
            case 1 :
                {std::uniform_int_distribution<intmax_t> randEdge (0, n.getNoOfEdges()-1);
                Edge oldE = n.getEdge(randEdge(rng));
                if (n.deleteEdge(oldE.node0, oldE.node1)) {
                    takenActions.push_back(Action(oldE, Edge()));
                }
                }break;
            //change b_value
            case 2 :
                {intmax_t nodeA = n.getNode(randNode(rng)), nodeB = n.getNode(randNode(rng));
                //not much of a random here right now
                std::uniform_int_distribution<intmax_t> fakeRan (1,
                    2/* + std::abs(n.getNodes().find(n.getNode(randNode(rng)))->second.b_value)*/);
                intmax_t b = fakeRan(rng);
                if (n.changeBvalue(nodeA, b) and n.changeBvalue(nodeB, -b)) {
                    takenActions.push_back(Action(std::forward_as_tuple(nodeA, nodeB, b)));
                }
                }break;
            //change cost of edge
            case 3 :
                {std::uniform_int_distribution<intmax_t> randEdge (0, n.getNoOfEdges()-1);
                Edge oldE = n.getEdge(randEdge(rng));
                std::uniform_int_distribution<intmax_t> newCost(1, 10 + 2*n.getEdge(randEdge(rng)).cost);
                Edge newE = oldE;
                newE.cost = newCost(rng);
                if (n.deleteEdge(oldE.node0, oldE.node1) and n.addEdge(newE)) {
                    takenActions.push_back(Action(oldE, newE));
                }
                }break;
            //change capacity of edge
            case 4 :
                {std::uniform_int_distribution<intmax_t> randEdge (0, n.getNoOfEdges()-1);
                Edge oldE = n.getEdge(randEdge(rng));
                std::uniform_int_distribution<intmax_t> newCap(1, 10 + 2*n.getEdge(randEdge(rng)).capacity);
                Edge newE = oldE;
                newE.capacity = newCap(rng);
                if (n.deleteEdge(oldE.node0, oldE.node1) and n.addEdge(newE)) {
                    takenActions.push_back(Action(oldE, newE));
                }
                }break;
            //undo last steps
            case 5 :
                {std::uniform_int_distribution<intmax_t> stepsTakenBack(0, takenActions.size() - opt_pos);
                size_t k = stepsTakenBack(rng);
                for (size_t j = 0; j < k; j++) {
                    Action lastAction = takenActions.back();
                    takenActions.pop_back();
                    if (lastAction.edgeCase) {
                        n.deleteEdge(lastAction.newE.node0, lastAction.newE.node1);
                        n.addEdge(lastAction.oldE);
                    }
                    else {
                        const std::tuple<size_t, size_t, intmax_t> temp = lastAction.b_change;
                        n.changeBvalue(std::get<0>(temp), - std::get<2>(temp));
                        n.changeBvalue(std::get<1>(temp), std::get<2>(temp));
                    }
                }
            }break;
        }
    }

    //recreate optimal network
    n = networkSave;
    for (size_t i = 0; i < opt_pos; i++) {
        Action& a = takenActions[i];
        if (a.edgeCase) {
            n.deleteEdge(a.oldE.node0, a.oldE.node1);
            n.addEdge(a.newE);
        }
        else {
            const std::tuple<size_t, size_t, intmax_t> temp = a.b_change;
            n.changeBvalue(std::get<0>(temp), std::get<2>(temp));
            n.changeBvalue(std::get<1>(temp), - std::get<2>(temp));
        }
    }

    networkSave = n;
}

std::pair<size_t, size_t> RandomGraph::randomMissingEdge() {
    size_t nodes = n.getNoOfNodes();
    std::vector<size_t> nodes0, nodes1;
    for (size_t i = 0; i < nodes; i++) {
        nodes0.push_back(n.getNode(i));
    }
    nodes1 = nodes0;
    std::random_shuffle(nodes0.begin(), nodes0.end());
    std::random_shuffle(nodes1.begin(), nodes1.end());

    for (size_t node0 : nodes0) {
        for (size_t node1 : nodes1) {
            if (n.getEdges().count(std::forward_as_tuple(node0, node1, false)) == 0) {
                return std::make_pair(node0, node1);
            }
        }
    }
    //shouldn’t happen
    std::cout << "ERROR RandomGraph::randomMissingEdge" << std::endl;
    return std::make_pair(0, 0);
}
