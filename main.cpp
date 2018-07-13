#include <iostream>

#include "Network.h"
#include "Edge.h"
#include "Algorithm.h"
#include "PivotAlgorithms.h"

//TODO private/public noch mal überdenken
//TODO make all(?) sets unordered

int main() {

    /*Network test = Network(4);
    std::vector<intmax_t> b_value = {3, 10, 1, -2, -1, -5, -2, -4};

    std::vector<size_t> from = {0, 4, 5, 5, 6, 3, 2, 2, 1, 1, 0, 1, 0, 1, 0, 1, 1, 3, 10,9, 8};
    std::vector<size_t> to   = {4, 5, 2, 3, 3, 2, 1, 0, 0, 3, 7, 7, 8, 8, 9, 9, 10,11,9, 8, 10};
    std::vector<intmax_t> co = {1, 1, 3, 2, 1, 0, 1, 1, 1, 0, 2, 1, 0, 1, 3, 3, 20,2, 1, 1, 3};
    std::vector<intmax_t> ca = {5, 4, 15,5, 1, 4, 20,20,20,20,5, 1, 3, 3, 3, 3, 20,5, 5, 3, 3};
    //std::vector<intmax_t> ca = {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20};*/


    Network test = Network(0);
    std::vector<intmax_t> b_value = {2, 2, 0, 0, 0,-1,-3};

    //HERE Testgraph reinschreiben, entgegengesetzte Kanten implementieren
    std::vector<size_t> from = {0, 1, 1, 2, 2, 2, 3, 4, 4, 4, 5, 6};
    std::vector<size_t> to   = {2, 3, 2, 1, 4, 5, 4, 3, 2, 6, 4, 4};
    std::vector<intmax_t> co = {2, 2, 0, 1, 6, 0, 0,10, 0, 0,10, 0};
    std::vector<intmax_t> ca = {5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};

    for (intmax_t b : b_value) {
        test.addNode(b);
    }

    for (size_t i = 0; i < from.size(); i++) {
        if (not test.addEdge(Edge(from[i], to[i], co[i], ca[i]))) {
            std::cout << "ya failed";
        }
    }

    Algorithm a = Algorithm(test, pivotMaxRev);
    a.solution();
    test.print();
    std::cout << "\nIterationen: " << a.noOfIter();
    std::cout << "\nKosten: " << test.getCost() << " Flow: " << test.getFlow() << std::endl;
}
/*
//=== Circle.update() Test
    Circle left, right;
    left.addEdge(0,1, false);
    left.addEdge(1,2, false);
    left.addEdge(2,3, false);
    left.addEdge(3,4, true);
    left.addEdge(4,5, true);
    left.addEdge(5,0, true);

    right.addEdge(1,0, false);
    right.addEdge(0,1, false);

    //left.rotateBy(0, true);
    //left.update(right);
    right.update(left);

    for (size_t i = 0; i < left.size(); i++) {
        std::cout << left.getEdges()[i].first << "-" << left.getEdges()[i].second
                << " (" << left.getIsResidual()[i] << ") | ";
    }
    std::cout << std::endl;
    for (size_t i = 0; i < right.size(); i++) {
        std::cout << right.getEdges()[i].first << "-" << right.getEdges()[i].second
                << " (" << right.getIsResidual()[i] << ") | ";
    }
*/
