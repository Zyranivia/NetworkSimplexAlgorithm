#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>

#include "Network.h"

//adds noOfNodes nodes with b_value = 0
Network::Network(size_t noOfNodes) {
    largestNodeID = noOfNodes - 1;
    for (size_t i = 0; i < noOfNodes; i++) {
        nodes.insert(std::make_pair(i, Node (i, 0)));
        transit.push_back(i);
    }
}

void Network::print() {
    std::cout << std::endl;
    for (auto const x : edges) {
        if (not x.second.isResidual) {
            std::cout << x.second.node0 << "-" << x.second.node1 << " f: " << x.second.flow <<
                " ca: " << x.second.capacity << " co: " << x.second.cost << std::endl;
        }
    }
    for (auto const x : nodes) {
        std::cout << x.second.id << " b: " << x.second.b_value << " | ";
    }
    std::cout << std::endl << "cost: " << cost << " | flow: " << flow << std::endl;
}

void Network::randomNoise(double phi) {
    if (phi <= 0) {return;}
    std::mt19937 rng;
    rng.seed(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_real_distribution<double> rand(0, phi);

    //find max capacity
    intmax_t maxCap = 1;
    for (std::pair<const std::tuple<size_t, size_t, bool>, Edge>& key : edges) {
        if (key.second.isResidual) {continue;}
        if (key.second.capacity > maxCap) {maxCap = key.second.capacity;}
    }

    //change both b-values and edge capacity
    //but additive for positive values to not restrict solution space
    for (std::pair<const std::tuple<size_t, size_t, bool>, Edge>& key : edges) {
        if (key.second.isResidual) {continue;}
        double eps0 = rand(rng), eps1 = rand(rng);
        intmax_t addToNode = (intmax_t) (eps0*((double) maxCap));
        intmax_t addToEdge = addToNode + (intmax_t) (eps1*((double) maxCap));

        changeBvalue(key.second.node0, addToNode);
        changeBvalue(key.second.node1, -addToNode);

        key.second.capacity += addToEdge;
        std::tuple<size_t, size_t, bool> keyInvert = invertKey(key.first);
        Edge& edgeInvert = edges.find(keyInvert)->second;
        edgeInvert.capacity += addToEdge;
        edgeInvert.flow = edgeInvert.capacity;
    }
}

//returns false if there’s a parallel edge
bool Network::addEdge(Edge e) {
    size_t node0 = e.node0;
    size_t node1 = e.node1;
    bool iR = e.isResidual;

    if (node0 == node1) {return false;}
    if (nodes.count(node0) == 0 or nodes.count(node1) == 0) {return false;}

    std::tuple<size_t, size_t, bool> key = std::make_tuple(node0, node1, iR);

    //only true if no parallel edge exists
    if (edges.count(key) == 0) {

        //insert both the edge and the residual edge
        edges.insert(std::make_pair(key, e));
        e.invert();
        edges.insert(std::make_pair(invertKey(key), e));

        nodes.find(node0)->second.neighbours.insert(node1);
        nodes.find(node1)->second.neighbours.insert(node0);

        return true;
    }
    return false;
}

size_t Network::addNode(intmax_t b_value) {
    largestNodeID++;
    nodes.insert(std::make_pair(largestNodeID, Node(largestNodeID, b_value)));

    if (b_value > 0) {
        sources.push_back(largestNodeID);
        sumSource += b_value;
    }
    if (b_value == 0) {
        transit.push_back(largestNodeID);
    }
    if (b_value < 0) {
        sinks.push_back(largestNodeID);
        sumSink -= b_value;
    }

    return largestNodeID;
}

size_t Network::getNoOfNodes () const {
    return nodes.size();
}

size_t Network::getNoOfEdges () const {
    //without residual edges
    return edges.size()/2;
}

const std::map<std::tuple<size_t, size_t, bool>, Edge, custComp>& Network::getEdges() const {
    return edges;
}

const std::map<size_t, Node, std::less<size_t>>& Network::getNodes() const {
    return nodes;
}

size_t Network::getNode(size_t index) {
    size_t i = 0;
    for (const std::pair<const size_t, Node>& keyPair : nodes) {
        if (i == index) {return keyPair.first;}
        i++;
    }
    //should never occur
    std::cout << "ERROR Network::getNode" << std::endl;
    return 0;
}

Edge Network::getEdge(size_t index) {
    size_t i = 0;
    for (const std::pair<const std::tuple<size_t, size_t, bool>, Edge>& keyPair : edges) {
        //skip residual edges
        if (keyPair.second.isResidual) {continue;}
        if (i == index) {return keyPair.second;}
        i++;
    }
    //should never occur
    std::cout << "ERROR Network::getEdge" << std::endl;
    return edges.begin()->second;
}

//fails and returns 0 if there’s flow left
bool Network::deleteEdge(size_t node0, size_t node1) {
    std::tuple<size_t, size_t, bool> key = std::make_tuple(node0, node1, false);

    if (edges.count(key) == 1 and edges.find(key)->second.flow == 0) {
        deleteEdge(key);
        return true;
    }
    return false;
}

//fails and returns 0 if there’s flow on an edge to this node left
bool Network::deleteNode(size_t n) {

    //does the node exist?
    if (nodes.count(n) == 0) {return false;}

    //check for flow left
    std::vector<std::tuple<size_t, size_t, bool>> edgesToDelete;
    for (size_t neigh : nodes.find(n)->second.neighbours) {
        std::tuple<size_t, size_t, bool> out = std::make_tuple(n, neigh, false),
                                        inc = std::make_tuple(neigh, n, false);
        if (edges.count(out) != 0) {
            if (edges.find(out)->second.flow != 0) {return false;}
            edgesToDelete.push_back(out);
        }
        if (edges.count(inc) != 0) {
            if (edges.find(inc)->second.flow != 0) {return false;}
            edgesToDelete.push_back(inc);
        }
    }

    //delete edges
    for (std::tuple<size_t, size_t, bool>& key : edgesToDelete) {
        deleteEdge(key);
    }

    intmax_t b = nodes.find(n)->second.b_value;

    //delete node
    if (b > 0) {sources.erase(std::remove(sources.begin(), sources.end(), n), sources.end()); sumSource -= b;}
    if (b == 0) {transit.erase(std::remove(transit.begin(), transit.end(), n), transit.end());}
    if (b < 0) {sinks.erase(std::remove(sinks.begin(), sinks.end(), n), sinks.end()); sumSink += b;}

    nodes.erase(n);
    return true;
}

void Network::deleteEdge(std::tuple<size_t, size_t, bool>& key) {
    edges.erase(key);
    edges.erase(invertKey(key));
    size_t node0 = std::get<0>(key), node1 = std::get<1>(key);

    //check whether the nodes are still neighbours
    if (edges.count(std::forward_as_tuple(node1, node0, std::get<2>(key))) == 0) {
        nodes.find(node0)->second.neighbours.erase(node1);
        nodes.find(node1)->second.neighbours.erase(node0);
    }
}

std::tuple<size_t, size_t, bool> Network::invertKey (const std::tuple<size_t, size_t, bool>& key) {
    return std::forward_as_tuple(std::get<1>(key), std::get<0>(key), not std::get<2>(key));
}

//decreases b_value
bool Network::addFlow(std::vector<size_t>& path, intmax_t f) {
    std::map<std::tuple<size_t, size_t, bool>, Edge, custComp>::iterator it;
    //existence of nodes gets checked indirectly

    for (size_t i = 1; i < path.size(); i++) {
        it = edges.find(std::forward_as_tuple(path[i-1], path[i], false));
        if (it == edges.end() or (not (it->second.changeFlowPossible(f)))) {return 0;}
    }

    //check whether first and last node are source resp. sink
    if (nodes.find(path.front())->second.b_value < f or
        nodes.find(path.back())->second.b_value > -f) {return false;}

    //every edge exists and can be flown
    for (size_t i = 1; i < path.size(); i++) {
        it = edges.find(std::forward_as_tuple(path[i-1], path[i], false));
        it->second.changeFlow(f);
        this->cost += it->second.cost*f;

        //change residual edge
        it = edges.find(std::forward_as_tuple(path[i], path[i-1], true));
        it->second.changeFlow(-f);
    }

    //change b_values of source and sink
    changeBvalue(path.front(), -f);
    changeBvalue(path.back(), f);

    this->flow += f;
    return true;
}

bool Network::changeFlow(Circle& c, intmax_t f) {
    //circles have a minimal length of 2
    if (c.size() <= 1) {return false;}
    if (0 == f) {return true;}

    std::map<std::tuple<size_t, size_t, bool>, Edge, custComp>::iterator it;

    //check whether flow change is possible
    for (size_t i = 0; i < c.size(); i++) {
        it = edges.find(std::forward_as_tuple(c.getEdges()[i].first, c.getEdges()[i].second, c.getIsResidual()[i]));
        if (it == edges.end() or (not it->second.changeFlowPossible(f))) {return false;}
    }

    //all edges exist and can be flown
    for (size_t i = 0; i < c.size(); i++) {
        it = edges.find(std::forward_as_tuple(c.getEdges()[i].first, c.getEdges()[i].second, c.getIsResidual()[i]));
        it->second.changeFlow(f);
        this->cost += it->second.cost*f;

        //change residual edge
        it = edges.find(std::forward_as_tuple(c.getEdges()[i].second, c.getEdges()[i].first, not c.getIsResidual()[i]));
        it->second.changeFlow(-f);
    }
    return true;
}

//probably not optimized, but not relevant in this context
void Network::clean() {
    for (std::pair<const std::tuple<size_t, size_t, bool>, Edge>& keypair : edges) {
        //if edges is residual, only change flow (if necessary)
        if (keypair.second.isResidual) {
            intmax_t f = keypair.second.flow;
            keypair.second.changeFlow(keypair.second.capacity - f);
            continue;
        }

        intmax_t f = keypair.second.flow;
        //nothing to do here
        if (f == 0) {continue;}

        //reset flow, change b_values
        keypair.second.changeFlow(-f);
        changeBvalue(keypair.second.node0, f);
        changeBvalue(keypair.second.node1, -f);
    }
    this->flow = 0;
    this->cost = 0;
}

//also updates cost, just in case
void Network::toggleCost() {
    intmax_t newCost = 0;
    for (std::pair<const std::tuple<size_t, size_t, bool>, Edge>& keypair : edges) {
        Edge& e = keypair.second;
        e.toggleCost();
        if (not e.isResidual) {
            newCost += e.flow*e.cost;
        }
    }
    cost = newCost;
}

bool Network::changeBvalue(size_t a, intmax_t b) {
    std::map<size_t, Node, std::less<size_t>>::iterator it = nodes.find(a);
    if (it == nodes.end()) {return false;}

    intmax_t oldB = it->second.b_value;
    intmax_t newB = oldB + b;
    it->second.b_value += b;
    //update sums and nodesets
    //not most efficient, but clear
    size_t id = it->second.id;

    //"delete" old node
    if (oldB > 0) {sources.erase(std::remove(sources.begin(), sources.end(), id), sources.end()); sumSource -= oldB;}
    if (oldB == 0) {transit.erase(std::remove(transit.begin(), transit.end(), id), transit.end());}
    if (oldB < 0) {sinks.erase(std::remove(sinks.begin(), sinks.end(), id), sinks.end()); sumSink += oldB;}

    //insert "new" node
    if (newB > 0) {sources.push_back(id); sumSource += newB;}
    if (newB == 0) {transit.push_back(id);}
    if (newB < 0) {sinks.push_back(id); sumSink -= newB;}

    return true;
}
