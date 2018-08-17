#include <iostream>

#include "Network.h"
#include "Edge.h"
#include "Algorithm.h"
#include "PivotAlgorithms.h"
#include "RandomGraph.h"

//TODO private/public noch mal überdenken
//TODO make all(?) sets unordered

bool isConnected(const Network& n) {
    std::vector<bool> checked (n.largestNodeID+1, false);
    std::set<size_t> reached_nodes;
    std::vector<size_t> checkNext;

    checkNext.push_back(n.getNodes().begin()->second.id);
    reached_nodes.insert(checkNext[0]);

    while (not checkNext.empty()) {
        size_t temp = checkNext.back();
        checkNext.pop_back();
        if (checked[temp]) {continue;}

        for (size_t neighbour : n.getNodes().find(temp)->second.neighbours) {
            checkNext.push_back(neighbour);
            reached_nodes.insert(neighbour);
        }

        checked[temp] = true;
    }
    return reached_nodes.size() == n.getNoOfNodes();
}

int main() {

    std::vector<double> distribution = {0.15, 0.1, 0.05, 0.35, 0.35};
    RandomGraph test = RandomGraph(Network(4));
    test.smartEvolve(10000, distribution);

    Network n = test.getNetwork();
    Algorithm solver = Algorithm(n, pivotMaxRev);
    solver.solution(true);
    n.print();

    /*for (int i = 0; i < 10; i++) {
        RandomGraph test = RandomGraph(Network(6));
        test.evolve(10000);
        Network n = test.getNetwork();
        Algorithm alg = Algorithm (n, pivotMaxRev);
        alg.solution();
        if (n.getFlow() != 0) {n.print();}
        std::cout << alg.iterations / (double) n.getNoOfNodes() << std::endl;
    }*/

    /*Network test2 = Network(4);
    std::vector<intmax_t> b_value = {3, 10, 1, -2, -1, -5, -2, -4};

    std::vector<size_t> from = {0, 4, 5, 5, 6, 3, 2, 2, 1, 1, 0, 1, 0, 1, 0, 1, 1, 3, 10,9, 8};
    std::vector<size_t> to   = {4, 5, 2, 3, 3, 2, 1, 0, 0, 3, 7, 7, 8, 8, 9, 9, 10,11,9, 8, 10};
    std::vector<intmax_t> co = {1, 1, 3, 2, 1, 0, 1, 1, 1, 0, 2, 1, 0, 1, 3, 3, 20,2, 1, 1, 3};
    std::vector<intmax_t> ca = {5, 4, 15,5, 1, 4, 20,20,20,20,5, 1, 3, 3, 3, 3, 20,5, 5, 3, 3};
    //std::vector<intmax_t> ca = {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20};*/

    /*Network test2 = Network(0);
    std::vector<intmax_t> b_value = {-2, 1, 1};

    std::vector<size_t> from = {0, 0, 1, 2};
    std::vector<size_t> to   = {1, 2, 2, 0};
    std::vector<intmax_t> ca = {5, 7, 2, 7};
    std::vector<intmax_t> co = {2,32, 9, 6};

    for (intmax_t b : b_value) {
        test2.addNode(b);
    }

    for (size_t i = 0; i < from.size(); i++) {
        if (not test2.addEdge(Edge(from[i], to[i], co[i], ca[i]))) {
            std::cout << "ya failed";
        }
    }

    Algorithm rand = Algorithm (test2, pivotMaxRev);

    //rand.solution(false);
    //test.print();

    test2.clean();
    test2.print();
    rand.solution(true);
    //test2.print()
    std::cout << rand.iterations << std::endl;*/

}
