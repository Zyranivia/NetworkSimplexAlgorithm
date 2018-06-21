#include <iostream>

#include "Network.h"
#include "Edge.h"
#include "Algorithm.h"
#include "PivotAlgorithms.h"

//TODO handle b_values
//also von wegens wieviel flow ist da
//in Netzwerkklasse
//TODO private/public noch mal überdenken
//TODO make all(?) sets unordered
//TODO mal drüber nachdenken, ob update isResidual der
//ersten Kante ignorieren darf -> eher nicht
//noch ist Graph aber frei von Gegenkanten
//TODO network.clean() mit b_value zurück

int main() {
    Network test = Network(4);
    std::vector<intmax_t> b_value = {3, 10, 1, -2, -1, -5, -2, -4};
    for (intmax_t b : b_value) {
        test.addNode(b);
    }
    std::vector<size_t> from = {0, 4, 5, 5, 6, 3, 2, 2, 1, 1, 0, 1, 0, 1, 0, 1, 1, 3, 10,9, 8};
    std::vector<size_t> to   = {4, 5, 2, 3, 3, 2, 1, 0, 0, 3, 7, 7, 8, 8, 9, 9, 10,11,9, 8, 10};
    std::vector<intmax_t> co = {1, 1, 3, 2, 1, 0, 1, 1, 1, 0, 2, 1, 0, 1, 3, 3, 20,2, 1, 1, 3};
    std::vector<intmax_t> ca = {5, 4, 15,5, 1, 4, 20,20,20,20,5, 1, 3, 3, 3, 3, 20,5, 5, 3, 3};
    //std::vector<intmax_t> ca = {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20};

    for (size_t i = 0; i < from.size(); i++) {
        if (not test.addEdge(Edge(from[i], to[i], co[i], ca[i]))) {
            std::cout << "ya failed";
        }
    }

    Algorithm a = Algorithm(test, pivotMax);
    a.solution();
    //test.print();
    std::cout << "\nIterationen: " << a.noOfIter();
    std::cout << "\nKosten: " << test.getCost() << " Flow: " << test.getFlow() << std::endl;
}

/*
//=== Circle.update() Test
    Circle left, right;
    left.addEdge(3,6, true);
    left.addEdge(6,12, false);
    left.addEdge(12,8, false);
    left.addEdge(8,0, true);
    left.addEdge(0,2, true);
    left.addEdge(2,3, true);

    right.addEdge(5,2, false);
    right.addEdge(2,3, true);
    right.addEdge(3,6, true);
    right.addEdge(6,12, false);
    right.addEdge(12,5, true);

    //left.rotateBy(0, true);
    //left.update(right);
    right.update(left);

    for (size_t i = 0; i < left.length; i++) {
        std::cout << left.getEdges()[i].first << "-" << left.getEdges()[i].second
                << " (" << left.getIsResidual()[i] << ") | ";
    }
    std::cout << std::endl;
    for (size_t i = 0; i < right.length; i++) {
        std::cout << right.getEdges()[i].first << "-" << right.getEdges()[i].second
                << " (" << right.getIsResidual()[i] << ") | ";
    }
    */

