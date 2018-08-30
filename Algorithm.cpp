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

//used with the partial tree constructed around artificialNode
void Algorithm::createCircles(std::vector<Node> partialTree) {
    circles.clear();

    //old tree won’t have invert edges
    std::set<std::pair<size_t, size_t>> newTreeEdges;
    //complete spanning tree
    //number of edges that have to be added: nodes - partialTree.size()
    //equals |transit|
    //algorithm used: Randomgreedy
    size_t added = 0, toBeAdded = n.getNoOfNodes() - partialTree.size();

    for (const std::pair<const std::tuple<size_t, size_t, bool>, Edge>& keypair : n.getEdges()) {
        if (keypair.second.isResidual) {continue;}
        const Edge& e = keypair.second;
        size_t node = e.node0, neigh = e.node1;
        //if e + partialTree is still a tree
        //invert edges to those of a tree might be checked
        if (findCircle(node, neigh, e.isResidual, partialTree, newTreeEdges).size() < 2) {

            //add edge to tree, nodes too, if not already in it
            size_t nodePos = findNode(node, partialTree);
            if (nodePos == partialTree.size()) {partialTree.push_back(Node(node, 0));}
            //in either case nodePos is now exact
            partialTree[nodePos].neighbours.insert(neigh);

            size_t neighPos = findNode(neigh, partialTree);
            if (neighPos == partialTree.size()) {partialTree.push_back(Node(neigh, 0));}
            //in either case nodePos is now exact
            partialTree[neighPos].neighbours.insert(node);

            //keep track of added edges
            newTreeEdges.insert(std::make_pair(node, neigh));

            //if there are invert edges of those of the tree, create a trivial circle
            if (n.getEdges().count(std::forward_as_tuple(neigh, node, false)) != 0) {
                Circle temp = Circle();
                temp.addEdge(neigh, node, false);
                temp.addEdge(node, neigh, false);
                circles.push_back(temp);
            }
            //std::cout << node << "-" << neigh << " added" << std::endl;
            added++;
        }
        if (added == toBeAdded) {break;}
    }

    //create std::vector<Circle> circles
    //e or e_invert in tree means c.size() <= 2
    //trivial circles have already been taken care of
    for (const std::pair<const std::tuple<size_t, size_t, bool>, Edge>& edge : n.getEdges()) {
        //don’t create circle for residual edge
        if (edge.second.isResidual) {continue;}

        Circle temp = findCircle(edge.second.node0, edge.second.node1, edge.second.isResidual, partialTree, newTreeEdges);
        if (temp.size() > 2) {circles.push_back(temp);}
    }
}

//true for low cost, false for high cost
bool Algorithm::solution (bool modus) {
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

    //copy sources and sinks, since with changed flow b_values will change
    //TODO streamline that
    std::vector<size_t> sourcesCopy = n.sources, sinksCopy = n.sinks;

    //get that flow started
    //sumSources == sumSinks
    std::vector<size_t>::iterator itSource = sourcesCopy.begin(), itSink = sinksCopy.begin();
    while (itSource != sourcesCopy.end()) {
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

    //complete tree and create circles
    createCircles(tree);

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
                    //since n is feasible, one of the both cases occures

                    //not residual <==> flow == 0
                    //new first edge, same direction
                    if (not it->getIsResidual()[i]) {it->rotateBy(i, false);}
                    //new first edge, but reversed direction
                    else {it->rotateBy(i, true);}

                    //new first edge can’t be included by any circle before it
                    //TODO better prove this
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
Circle Algorithm::findCircle(size_t node0, size_t node1, bool isResidual, const std::vector<Node>& tree, const std::set<std::pair<size_t, size_t>>& treeEdges) {
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
        //if direction is unique, it is calculated
        bool direction = n.getEdges().count(std::forward_as_tuple(path[i-1], path[i], true));
        //otherwise it is always true and has to be changed in case
        if (treeEdges.count(std::make_pair(path[i-1], path[i]))) {direction = false;}
        c.addEdge(path[i-1], path[i], direction);
    }
    return c;
}

//TODO optimize calculation of flow/cost for circle somehow
//probably in update
//make a bool whether a circle was changed

//chooses a circle via pivot function and iterates flow through it
bool Algorithm::optimize() {
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

    //std::cout << "(" << chosenOneId << ") " << circles[chosenOneId].flow << " --> ";
    //std::cout << circles[chosenOneId].getEdges()[0].first << "-" << circles[chosenOneId].getEdges()[0].second;

    Circle& chosenOne = circles[chosenOneId];

    n.changeFlow(chosenOne, chosenOne.flow);

    //reorder the circle such that the first edge beginning from the end
    //which has full or zero flow is now the first edge and therefore not
    //part of the tree anymore. In the first case the circle is reversed
    for (size_t i = chosenOne.size() - 1; i < chosenOne.size(); i--) {
        const Edge& e = n.getEdges().find(std::forward_as_tuple(chosenOne.getEdges()[i].first, chosenOne.getEdges()[i].second,
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
    return true;
}
