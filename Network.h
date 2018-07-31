#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include <cstddef>
#include <set>
#include <map>
#include <utility>
#include <vector>
#include <tuple>

#include "Edge.h"
#include "Circle.h"

//custom comparator for (nodeId, nodeId, isResidual)
//needed for edge storage
struct custComp {
    bool operator() (const std::tuple<size_t, size_t, bool>& edge0, const std::tuple<size_t, size_t, bool>& edge1) const {
        if (std::get<0>(edge0) != std::get<0>(edge1)) {
            return std::get<0>(edge0) < std::get<0>(edge1);
        }
        if (std::get<1>(edge0) != std::get<1>(edge1)) {
            return std::get<1>(edge0) < std::get<1>(edge1);
        }
        return std::get<2>(edge0) < std::get<2>(edge1);
    }

};

struct Node {
        size_t id;
        intmax_t b_value;
        //defined by (the other) nodeID
        //always incoming and outgoing due to residual edges
        std::set<size_t> neighbours;

        Node (size_t _id, intmax_t _b_value) : id(_id), b_value(_b_value) {};
    };

//no parallel (nonresidual) edges are allowed
class Network {

public:
    Network(size_t noOfNodes);
    void print();

    void randomNoise(double phi);

    //return 0 if there’s a parallel edge
    //adds residual edge
    bool addEdge(Edge e);
    //returns nodeID
    //used to add sources and sinks
    size_t addNode(intmax_t b_value = 0);

    size_t getNoOfNodes () const;
    const std::map<std::tuple<size_t, size_t, bool>, Edge, custComp>& getEdges() const;
    const std::map<size_t, Node, std::less<size_t>>& getNodes() const;

    //fails and returns 0 if there’s flow left
    bool deleteEdge(size_t node0, size_t node1);
    //fails and returns 0 if there’s flow on an edge to this node left
    bool deleteNode(size_t nodeID);

    //both functions fail and return 0 if not possible

    //takes a path from a source to a sink
    //doesn't use residual edges
    bool addFlow(std::vector<size_t>& path, intmax_t flow);
    //takes a circle
    bool changeFlow(Circle& c, intmax_t flow);

    //toggles all edges
    void toggleCost();
    //clears network from all flow
    void clean();

    intmax_t getFlow() {return flow;}
    intmax_t getCost() {return cost;}
    //TODO all of these probably should be private
    intmax_t sumSource = 0, sumSink = 0;
    size_t largestNodeID;
    std::vector<size_t> sources, sinks, transit;

private:
    intmax_t flow = 0, cost = 0;

    std::map<std::tuple<size_t, size_t, bool>, Edge, custComp> edges;
    std::map<size_t, Node, std::less<size_t>> nodes;

    void deleteEdge(std::tuple<size_t, size_t, bool>& e);
    std::tuple<size_t, size_t, bool> invertKey (const std::tuple<size_t, size_t, bool>& key);
};

#endif // NETWORK_H_INCLUDED
