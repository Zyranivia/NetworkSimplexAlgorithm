#include <algorithm>
#include <cmath>
#include <iostream>

#include "Algorithm.h"

//anonymous namespace
namespace {

//return position of node or vec.size() if not a member
size_t findNode (size_t node, const std::vector<Node>& vec) {
    for (size_t i = 0; i < vec.size(); i++) {
        if (vec[i].id == node) {return i;}
    }
    return vec.size();
}

}

intmax_t Algorithm::noOfIter () {
    return iterations;
}

bool Algorithm::solution (bool modus) {
    //all variables to initial setting
    circles.clear();
    iterations = 0;

    //high cost edge
    intmax_t maxCost = 0;
    for (const std::pair<const std::tuple<size_t, size_t, bool>, Edge>& edge : n.getEdges()) {
        if (edge.second.cost > maxCost) {
            maxCost = edge.second.cost;
         }
    }

    maxCost = maxCost*n.getNoOfNodes() + 1;

    if (n.sumSource != n.sumSink) {return false;}

    //add edges from sources to artNode
    //and from artNode to sinks
    //they are part of the underlying tree
    std::vector<Node> tree;
    tree.reserve(n.getNoOfNodes());
    artificialNode = n.addNode(0);
    Node artNode_Tree = Node(artificialNode, 0);

    for (size_t source : n.sources) {
        n.addEdge(Edge(source, artificialNode, maxCost, n.sumSource + 1));

        artNode_Tree.neighbours.insert(source);
        Node source_Tree = Node(source, 0);
        source_Tree.neighbours.insert(artificialNode);
        tree.push_back(source_Tree);
    }
    for (size_t sink : n.sinks) {
        n.addEdge(Edge(artificialNode, sink, maxCost, n.sumSink + 1));

        artNode_Tree.neighbours.insert(sink);
        Node sink_Tree = Node(sink, 0);
        sink_Tree.neighbours.insert(artificialNode);
        tree.push_back(sink_Tree);
    }

    //get that flow started
    //sumSources == sumSinks
    std::vector<size_t>::iterator itSource = n.sources.begin(), itSink = n.sinks.begin();
    while (itSource != n.sources.end()) {
        if (std::abs(n.getNodes().find(*itSource)->second.b_value) <= std::abs(n.getNodes().find(*itSink)->second.b_value)) {
            std::vector<size_t> temp = {*itSource, artificialNode, *itSink};
            n.addFlow(temp, std::abs(n.getNodes().find(*itSource)->second.b_value));
            itSource++;
        }
        //NOTE: could be also if, but then check for != .end() again
        else {
            std::vector<size_t> temp = {*itSource, artificialNode, *itSink};
            n.addFlow(temp, std::abs(n.getNodes().find(*itSink)->second.b_value));
            itSink++;
        }
    }

    tree.push_back(artNode_Tree);

    std::vector<std::pair<size_t, size_t>> problematicEdges;
    //complete spanning tree
    //number of edges that have to be added: |transit|
    //algorithm used: Dijkstra
    size_t added = 0;
    std::vector<size_t> nodeStack = n.sources;

    //not optimized yet, nodes which are several times
    //in the stack will be checked out several times
    while (not nodeStack.empty()) {
        size_t node = nodeStack.back();
        nodeStack.pop_back();

        //iterate through all outgoing edges of node
        for (size_t neigh : n.getNodes().find(node)->second.neighbours) {
            if (n.getEdges().count(std::forward_as_tuple(node, neigh, false)) == 0) {continue;}
            if (findNode(neigh, tree) != tree.size()) {continue;}

            //edge node-neighbour gets added to tree
            tree.push_back(Node(neigh, 0));
            tree[findNode(node, tree)].neighbours.insert(neigh);
            tree.back().neighbours.insert(node);

            nodeStack.push_back(neigh);
            added++;

            //std::cout << node << "-" << neigh << " added\n";

            //if there are invert edges of those of the tree, delete them later on
            //as findCircle won't work otherwise
            if (n.getEdges().count(std::forward_as_tuple(neigh, node, false)) != 0) {
                problematicEdges.push_back(std::make_pair(neigh, node));
            }

            if (added == n.transit.size()) {break;}
        }
        if (added == n.transit.size()) {break;}
    }

    //delete invert edges
    std::vector<Edge> toBeInsertedLaterOn;
    toBeInsertedLaterOn.reserve(problematicEdges.size());
    for (std::pair<size_t, size_t>& probEdge : problematicEdges) {
        toBeInsertedLaterOn.push_back(n.getEdges().find(std::forward_as_tuple(probEdge.first, probEdge.second, false))->second);
        n.deleteEdge(probEdge.first, probEdge.second);
    }

    //create std::vector<Circle> circles
    //TODO check whether e in tree really means c.size() == 2
    for (const std::pair<const std::tuple<size_t, size_t, bool>, Edge>& edge : n.getEdges()) {
        //don’t create circle for residual edge
        if (edge.second.isResidual) {continue;}

        Circle temp = findCircle(edge.second.node0, edge.second.node1, edge.second.isResidual, tree);
        if (temp.size() > 2) {circles.push_back(temp);}
    }

    //add invert edges again
    //also add circles (trivial ones) for them
    for (Edge edge : toBeInsertedLaterOn) {
        n.addEdge(edge);
        Circle temp = Circle();
        temp.addEdge(edge.node0, edge.node1, edge.isResidual);
        temp.addEdge(edge.node1, edge.node0, edge.isResidual);
        circles.push_back(temp);
    }

    //std::cout << "Kreise: " << circles.size() << std::endl;

    //solve with artificial node
    while (optimize());

    //if there’s flow on the artifical node left
    //the network is infeasible
    bool feasible = n.deleteNode(artificialNode);
    if (not feasible) {
        n.clean();
        n.deleteNode(artificialNode);
    }
    return feasible;
}

//returns a (real) circle, if tree + edge is not a tree anymore
Circle Algorithm::findCircle(size_t node0, size_t node1, bool isResidual, const std::vector<Node>& tree) {

    //create map nodeId->tree for constant access
    size_t mini = n.largestNodeID, maxi = 0;
    for (const Node& node : tree) {
        if (node.id > maxi) {maxi = node.id;}
        if (node.id < mini) {mini = node.id;}
    }

    std::vector<intmax_t> mapping (maxi + 1 - mini, -1);
    for (size_t i = 0; i < tree.size(); i++) {
        mapping[tree[i].id - mini] = i;
    }

    //check whether node0 and node1 are in the tree
    if (node0 < mini or node1 < mini or node0 > maxi or node0 > maxi
        or mapping[node0 - mini] == -1 or mapping[node1 - mini] == -1) {
            return Circle();
    }

    std::vector<bool> visited (tree.size(), false);
    std::vector<size_t> path;
    path.reserve(tree.size() + 1);
    path.push_back(node0);
    path.push_back(node1);
    //find circle via DFS
    //if there is no circle, path = {node0}
    while (path.back() != node0) {
        visited[mapping[path.back() - mini]] = true;
        bool isLeaf = true;
        for (size_t neigh : tree[mapping[path.back() - mini]].neighbours) {
            if (not visited[mapping[neigh - mini]]) {
                path.push_back(neigh);
                isLeaf = false;
                break;
            }
        }
        //return point
        if (isLeaf) {
            path.pop_back();
        }
    }

    Circle c;

    //TODO je nach konstruktor kommt unten
    //das richtige raus, daher redundant
    if (path.size() == 1) {
        return c;
    }

    //add first edge manually, because reverse edge could be existent
    c.addEdge(node0, node1, isResidual);
    //construct circle to return from tree edges
    for (size_t i = 2; i < path.size(); i++) {
        c.addEdge(path[i-1], path[i], n.getEdges().count(std::forward_as_tuple(path[i-1], path[i], true)));
    }
    return c;
}

//TODO optimize calculation of flow/cost for circle somehow
//probably in update
//NOOO circles gotta know their underlying graph

//chooses a circle via pivot function and iterates flow through it
bool Algorithm::optimize() {
    //size_t temp = 0;
    for (Circle& c : circles) {
        c.costPerFlow = 0;
        Edge e = n.getEdges().find(std::forward_as_tuple(c.getEdges()[0].first, c.getEdges()[0].second, c.getIsResidual()[0]))->second;
        intmax_t minFlow = e.capacity - e.flow;

        for (size_t i = 0; i < c.size(); i++) {
            e = n.getEdges().find(std::forward_as_tuple(c.getEdges()[i].first, c.getEdges()[i].second, c.getIsResidual()[i]))->second;
            if (e.capacity - e.flow < minFlow) {minFlow = e.capacity - e.flow;}
            c.costPerFlow += e.cost;
        }
        c.flow = minFlow;
        //temp++;
    }

    /*std::cout << std::endl;
    for (int i = 0; i < circles[4].size(); i++) {
        std::cout << circles[4].getEdges()[i].first << "-" << circles[4].getEdges()[i].second << " (" <<
            circles[4].getIsResidual()[i] << ")| ";
    }
    std::cout << std::endl;*/

    size_t chosenOneId = pivot(circles);
    //if there is no negative circle
    if (chosenOneId == circles.size()) {
        return false;
        //TODO below should not be necessary
        //degenerate iterations change the tree
        /*degenerated = true;
            for (uintmax_t i = 0; i < degenerateIteration.size(); i++) {
                if (circles[i].flow != 0 or degenerateIteration[i]) {continue;}
                chosenOneId = i;
                degenerateIteration[i] = true;
                break;
            }
            if (chosenOneId == circles.size()) {return false;}
    }
    //reset degenerateIteration (could be done less often …)
    else {
        if (degenerated) {
            //std::cout << "Das hier sollte nicht passieren";
            for (uintmax_t i = 0; i < degenerateIteration.size(); i++) {degenerateIteration[i] = false;}
            degenerated = false;
        }*/
    }

    //std::cout << "(" << chosenOneId << ") " << circles[chosenOneId].flow << " --> ";
    //std::cout << circles[chosenOneId].getEdges()[0].first << "-" << circles[chosenOneId].getEdges()[0].second;

    Circle& chosenOne = circles[chosenOneId];

    /*for (int i = 0; i < chosenOne.size(); i++) {
        std::cout << chosenOne.getEdges()[i].first << "-" << chosenOne.getEdges()[i].second << " (" << chosenOne.getIsResidual()[0] << ") | ";
    }
    std::cout << std::endl;*/

    n.changeFlow(chosenOne, chosenOne.flow);

    //reorder the circle such that the first edge beginning from the end
    //which has full or zero flow is now the first edge and therefore not
    //part of the tree anymore. In the first case the circle is reversed
    for (size_t i = chosenOne.size() - 1; i < chosenOne.size(); i--) {
        Edge e = n.getEdges().find(std::forward_as_tuple(chosenOne.getEdges()[i].first, chosenOne.getEdges()[i].second,
                                                         chosenOne.getIsResidual()[i]))->second;

        //new first edge, same direction
        if (e.flow == 0) {
            chosenOne.rotateBy(i, false);
            break;
        }
        //new first edge, but reversed direction
        if (e.flow == e.capacity) {
            chosenOne.rotateBy(i, true);
            break;
        }
    }

    //change all other circles which include the new first edge
    for (size_t i = 0; i < circles.size(); i++) {
        if (i == chosenOneId) {continue;}
        circles[i].update(chosenOne);
    }

    //std::cout << " wird zu " << circles[chosenOneId].getEdges()[0].first << "-" << circles[chosenOneId].getEdges()[0].second << std::endl;

    this->iterations += 1;
    //if (iterations % 100 == 0) {std::cout << "!";}
    return true;
}
