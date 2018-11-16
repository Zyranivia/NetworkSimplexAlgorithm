#include <algorithm>
#include <cmath>
#include <iostream>

#include "Algorithm.h"

//anonymous namespace for helper functions
namespace {

//return position of node or vec.size() if not a member
size_t findNode (size_t node, const std::vector<Node>& vec) {
    for (size_t i = 0; i < vec.size(); i++) {
        if (vec[i].id == node) {return i;}
    }
    return vec.size();
}

bool feasible (Network& n, size_t artificialNode) {
    //if there’s flow on the artifical node left
    //the network is infeasible
    bool feasible = n.deleteNode(artificialNode);
    if (not feasible) {
        n.clean();
        n.deleteNode(artificialNode);
    }
    return feasible;
}

}

intmax_t Algorithm::getNoOfIter () {
    return iterations;
}

//used with the tree constructed around artificialNode
void Algorithm::createCircles(std::vector<Node> tree) {
    circles.clear();

    //create std::vector<Circle> circles
    //e or e_invert in tree means c.size() <= 2
    //trivial circles have already been taken care of
    for (const std::pair<const std::tuple<size_t, size_t, bool>, Edge>& edge : n.getEdges()) {
        //don’t create circle for residual edge
        if (edge.second.isResidual) {continue;}

        Circle temp = findCircle(edge.second.node0, edge.second.node1, edge.second.isResidual, tree);
        if (temp.size() > 2) {circles.push_back(temp);}
    }
}

//true for low cost, false for high cost
bool Algorithm::solution (bool modus) {
    if (n.sumSource != n.sumSink) {return false;}
    //initialize iterations
    iterations = 0;

    //high cost edge
    intmax_t maxCost = 0;
    if (modus == false) {
        for (const std::pair<const std::tuple<size_t, size_t, bool>, Edge>& edge : n.getEdges()) {
            if (edge.second.cost > maxCost) {
                maxCost = edge.second.cost;
             }
        }
    }
    //toggle all edges
    else {n.toggleCost();}

    maxCost = maxCost*n.getNoOfNodes() + 1;

    //add edges from sources to artNode
    //and from artNode to sinks/transit
    //those are the edges of the underlying tree
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
    for (size_t transit : n.transit) {
        n.addEdge(Edge(artificialNode, transit, maxCost, n.sumSink + 1));

        artNode_Tree.neighbours.insert(transit);
        Node sink_Tree = Node(transit, 0);
        sink_Tree.neighbours.insert(artificialNode);
        tree.push_back(sink_Tree);
    }

    //get that flow started
    //sumSources == sumSinks
    //networks updates n.sources after flow change
    while (not n.sources.empty()) {
        size_t source = n.sources.back(), sink = n.sinks.back();
        if (std::abs(n.getNodes().find(source)->second.b_value) <= std::abs(n.getNodes().find(sink)->second.b_value)) {
            std::vector<size_t> temp = {source, artificialNode, sink};
            n.addFlow(temp, std::abs(n.getNodes().find(source)->second.b_value));
        }
        //NOTE: could be also if, but then check for not .empty() again
        else {
            std::vector<size_t> temp = {source, artificialNode, sink};
            n.addFlow(temp, std::abs(n.getNodes().find(sink)->second.b_value));
        }
    }

    tree.push_back(artNode_Tree);

    //complete tree and create circles
    createCircles(tree);
    //create strongFeasibleTree
    //use that all nodes are in n.transit now
    strongFeasibleTree.clear();
    for (size_t id : n.transit) {
        strongFeasibleTree.insert(std::make_pair(id, artificialNode));
    }

    //solve with artificial node
    while (optimize());

    //in this case a solution only proves feasibility
    if (modus == true) {
        //first of all, toggle back
        n.toggleCost();
        if (feasible(n, artificialNode)) {
            //iterate < n times until artificialNode is a leaf
            for (std::vector<Circle>::iterator it = circles.begin(); it != circles.end(); it++) {
                //find edge over artificial node if existent
                size_t i = 1;
                for (; i < it->size(); i++) {
                    if (it->getEdges()[i].first == artificialNode or it->getEdges()[i].second == artificialNode) {break;}
                }
                if (i < it->size()) {
                    updateStrongFeasibleTree(*it, i, findApex(*it));
                    //since n is feasible, one of the both cases occures

                    //not residual <==> flow == 0
                    //new first edge, same direction
                    if (not it->getIsResidual()[i]) {it->rotateBy(i, false);}
                    //new first edge, but reversed direction
                    else {it->rotateBy(i, true);}

                    //new first edge can’t be included by any relevant circle before it
                    //with relevant meaning not getting deleted a few lines later
                    for (std::vector<Circle>::iterator otherCircle = it + 1; otherCircle != circles.end(); otherCircle++) {
                        otherCircle->update(*it);
                    }

                    iterations++;
                }
            }

            //now remove all circles beginning at the artificial node
            circles.erase(std::remove_if(circles.begin(), circles.end(), [this](const Circle& c)
                                                                        {return c.getEdges()[0].first == artificialNode or
                                                                                c.getEdges()[0].second == artificialNode;}
                                        ), circles.end());

            while (optimize());

            return true;
        }
        else {return false;}
    }
    else {return feasible(n, artificialNode);}
}

//returns a (real) circle, if tree + edge is not a tree anymore
Circle Algorithm::findCircle(size_t node0, size_t node1, bool isResidual, const std::vector<Node>& tree) {
    if (tree.empty()) {return Circle();}

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
    if (node0 < mini or node1 < mini or node0 > maxi or node1 > maxi
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

    //return empty circle if there was found none
    if (path.size() == 1) {
        return c;
    }

    //add first edge manually, because reverse edge could be existent
    c.addEdge(node0, node1, isResidual);
    //construct circle to return from tree edges
    for (size_t i = 2; i < path.size(); i++) {
        //direction is unique, because it’s an artifical edge
        bool direction = n.getEdges().count(std::forward_as_tuple(path[i-1], path[i], true));
        c.addEdge(path[i-1], path[i], direction);
    }
    return c;
}

//chooses a circle via pivot function and iterates flow through it
bool Algorithm::optimize() {
    //this is not trivial to do smarter, since even circles unchanged by update can have
    //their value for flow changed
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
    }

    size_t chosenOneId = pivot(circles);
    //if there is no negative circle
    if (chosenOneId == circles.size()) {return false;}

    Circle& chosenOne = circles[chosenOneId];
    n.changeFlow(chosenOne, chosenOne.flow);

    size_t apex = findApex(chosenOne);

    //traverse backwards from apex and reorder the circle such that
    //the first edge which has full flow is now the first edge and therefore not
    //part of the tree anymore. The circle is reversed
    for (size_t i = apex; ; i--) {
        const Edge& e = n.getEdges().find(std::forward_as_tuple(chosenOne.getEdges()[i].first, chosenOne.getEdges()[i].second,
                                                         chosenOne.getIsResidual()[i]))->second;

        if (e.flow == e.capacity) {
            updateStrongFeasibleTree(chosenOne, i, apex);

            //new first edge, but reversed direction
            chosenOne.rotateBy(i, true);
            break;
        }
        //go from first to last edge if necessary
        if (0 == i) {i = chosenOne.size();}
    }

    //change all other circles which include the new first edge
    for (size_t i = 0; i < circles.size(); i++) {
        if (i == chosenOneId) {continue;}
        circles[i].update(chosenOne);
    }

    this->iterations += 1;
    return true;
}

size_t Algorithm::findApex (Circle& c) {
    //find apex of circle
    size_t apex = 0;
    for (; apex < c.size() - 1; apex++) {
        size_t node = c.getEdges()[apex].second,
               neighbourInCircle = c.getEdges()[apex+1].second;
        //if the way to the root is leaving the circle, we’re done
        if (strongFeasibleTree.find(node)->second != neighbourInCircle) {break;}
    }
    //just to be sure
    if (apex == c.size() - 1 and strongFeasibleTree.find(c.getEdges().back().second) ==
                                 strongFeasibleTree.find(c.getEdges()[0].second)) {
        std::cout << "ERROR Algorithm::findApex" << std::endl;
        exit(1);
    }
    return apex;
}

void Algorithm::updateStrongFeasibleTree(Circle& c, size_t i, size_t apex) {
    //change direction of all edges from new first edge i to old one
    if (i <= apex) {
        for (size_t pos = 0; pos < i; pos++) {
            size_t id = c.getEdges()[pos].second;
            strongFeasibleTree.find(id)->second = c.getEdges()[pos].first;
        }
    }
    //i > apex
    else {
        for (size_t pos = i + 1; pos < c.size(); pos++) {
            size_t id = c.getEdges()[pos].first;
            strongFeasibleTree.find(id)->second = c.getEdges()[pos].second;
        }
        size_t id = c.getEdges()[0].first;
        strongFeasibleTree.find(id)->second = c.getEdges()[0].second;
    }
}

