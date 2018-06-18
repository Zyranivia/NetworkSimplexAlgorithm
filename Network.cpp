#include <algorithm>
#include <iostream>

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
            std::cout << x.second.node0 << "-" << x.second.node1 /*<< "|"
                << x.second.node0 << "-" << x.second.node1 */<< " f: " << x.second.flow <<
                " ca: " << x.second.capacity << std::endl;
        }
    }
    for (auto const x : nodes) {
        std::cout << x.second.id << " b: " << x.second.b_value << " | ";
    }
    std::cout << std::endl << "cost: " << cost << " | flow: " << flow << std::endl;
}

//returns 0 if there�s a parallel edge
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

size_t Network::getNoOfNodes () {
    return nodes.size();
}

const std::map<std::tuple<size_t, size_t, bool>, Edge, custComp>& Network::getEdges() const {
    return edges;
}

const std::map<size_t, Node, std::less<size_t>>& Network::getNodes() const {
    return nodes;
}

//fails and returns 0 if there�s flow left
bool Network::deleteEdge(size_t node0, size_t node1) {
    std::tuple<size_t, size_t, bool> key = std::make_tuple(node0, node1, false);

    if (edges.count(key) == 1 and edges.find(key)->second.flow == 0) {
        deleteEdge(key);
        return true;
    }

    return false;
}

//fails and returns 0 if there�s flow on an edge to this node left
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
    if (b == 0) {transit.erase(std::remove(sources.begin(), sources.end(), n), sources.end());}
    if (b < 0) {sinks.erase(std::remove(sources.begin(), sources.end(), n), sources.end()); sumSink += b;}

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
    nodes.find(path.front())->second.b_value -= f;
    nodes.find(path.back())->second.b_value += f;

    flow += f;
    return true;
}

bool Network::changeFlow(Circle& c, intmax_t f) {
    //circles have a minimal length of 2
    if (c.size() <= 1) {return false;}

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

void Network::unite (const Network& n) {
    size_t translativeFactor = this->largestNodeID + 1;

    for (const std::pair<size_t, Node>& key : n.getNodes()) {
        this->addNode(key.second.b_value);
    }
    for (const std::pair<const std::tuple<size_t, size_t, bool>, Edge>& e : n.getEdges()) {
        if (e.second.isResidual) {continue;}
        this->addEdge(Edge(e.second.node0 + translativeFactor, e.second.node1 + translativeFactor,
                          e.second.cost, e.second.capacity));
    }
}

