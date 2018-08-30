#include <iostream>
#include <random>
#include <time.h>

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
    /*
    std::mt19937 rng;
    rng.seed(static_cast<long unsigned int>(time(0)));
    std::uniform_real_distribution<> rand(0., 1.);

    bool cont = true;
    while (cont) {
        double sum = 0;
        std::vector<double> distribution;
        //create
        for (int i = 0; i < 5; i++) {
            distribution.push_back(rand(rng));
            sum += distribution.back();
        }
        //normalize
        for (int i = 0; i < 5; i++) {
            distribution[i] /= sum;
        }

        Network n = Network(1);
        while (n.getNoOfNodes() < 20) {
            n.addNode(0);
            size_t i = n.getNoOfNodes();

            RandomGraph rg = RandomGraph(n);
            rg.smartEvolve(1000*i, distribution);
            n = rg.getNetwork();
            Algorithm solver = Algorithm(n, pivotMaxRev);
            if (i > 4 ) {
                solver.solution(true);
                double temp = solver.getNoOfIter()/ (double) i;
                if (temp < 2) {std::cout << "."; break;}
            }
            if (i == 20) {
                for (int i = 0; i < 5; i++) {std::cout << distribution[i] << " | ";}
                std::cout << std::endl << "20: " <<  solver.getNoOfIter()/ (double) i << std::endl << std::endl;
            }
        }
        //cont = false;
    }
    */
    /*Algorithm solver = Algorithm(n, pivotMaxRev);
    solver.solution(true);
    n.print();*/

    /*for (int i = 0; i < 10; i++) {
        RandomGraph test = RandomGraph(Network(6));
        test.evolve(10000);
        Network n = test.getNetwork();
        Algorithm alg = Algorithm (n, pivotMaxRev);
        alg.solution();
        if (n.getFlow() != 0) {n.print();}
        std::cout << alg.iterations / (double) n.getNoOfNodes() << std::endl;
    }*/
    for (size_t k = 0; k < 50; k++) {

    Network test2 = Network(0);
    std::vector<intmax_t> b_value = {9, -9, 0, 0, 0, 0, 0, 0};
    //basic construct
    //"infite" capacity
    intmax_t u = 10000;
    std::vector<size_t> from = {0,0,0,2,2,2,3,4,4,5,6,6,7,2,4,6,3,1,1,1};
    std::vector<size_t> to   = {2,4,6,3,5,7,1,3,7,1,3,5,1,0,0,0,2,3,5,7};
    std::vector<intmax_t> co = {0,0,0,0,1,3,0,1,3,0,3,3,0,0,0,0,0,0,0,0};
    std::vector<intmax_t> ca = {1,3,5,u,u,u,2,u,u,2,u,u,5,1,3,5,u,2,2,5};

    for (intmax_t b : b_value) {
        test2.addNode(b);
    }

    for (size_t i = 0; i < from.size(); i++) {
        if (not test2.addEdge(Edge(from[i], to[i], co[i], ca[i]))) {
            std::cout << "ya failed";
        }
    }

    //enlargen i times
    for (size_t i = 0; i < k; i++) {
        test2.addNode(0); //n_i*2
        test2.addNode(0); //n_i*2 + 1
        size_t n_i = (test2.getNoOfNodes() - 2)/2;
        size_t newLeft = n_i*2, newRight = n_i*2 + 1;
        intmax_t capSideEdges = std::pow(2,n_i - 1) + std::pow(2,n_i - 3);
        intmax_t costInnerEdges = std::pow(2,n_i - 1) - 1;
        test2.addEdge(Edge(0, newLeft, 0, capSideEdges));
        test2.addEdge(Edge(newLeft, 0, 0, capSideEdges));
        test2.addEdge(Edge(newRight, 1, 0, capSideEdges));
        test2.addEdge(Edge(1, newRight, 0, capSideEdges));
        for (size_t j = 1; j < n_i; j++) {
            test2.addEdge(Edge(2*j, newRight, costInnerEdges, u));
            test2.addEdge(Edge(newLeft, 2*j, costInnerEdges, u));
        }

        test2.changeBvalue(0, capSideEdges);
        test2.changeBvalue(1, -capSideEdges);
    }

    Algorithm rand = Algorithm (test2, pivotMaxRev);

    //rand.solution(false);
    //test.print();

    test2.clean();
    //test2.print();
    rand.solution(false);
    //test2.print();
    std::cout << rand.getNoOfIter()/*/ (double) test2.getNoOfNodes() */<< std::endl;
    }

}
