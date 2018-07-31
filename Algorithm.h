#ifndef ALGORITHM_H_INCLUDED
#define ALGORITHM_H_INCLUDED

#include <vector>

#include "Network.h"
#include "Circle.h"

class Algorithm {
public:
    Algorithm (Network& _n, size_t (*_pivot)(const std::vector<Circle>&)) : n(_n), pivot(_pivot) {};

    //returns 0 if n cannot be solved
    //false for high cost edge initialization
    //TODO: not implemented yet …
    bool solution (bool modus = true);
    intmax_t noOfIter ();

//private:
    intmax_t iterations = 0;
private:
    //collect circles instead of calculating all the time
    std::vector<Circle> circles;

    size_t artificialNode;

    Network& n;
    size_t (*pivot)(const std::vector<Circle>&);

    //false if no optimization was possible
    bool optimize();

    //takes (maybe empty) tree, maximizes it and creates circles;
    void createCircles(std::vector<Node> partialTree);

    //it is assumed that for no edge (i,j) in the tree
    //there is an edge (j,i)
    Circle findCircle(size_t node0, size_t node1, bool isResidual, const std::vector<Node>& tree, const std::set<std::pair<size_t, size_t>>& treeEdges);
};

#endif // ALGORITHM_H_INCLUDED
