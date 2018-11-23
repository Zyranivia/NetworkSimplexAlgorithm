#include <iostream>
#include <random>
#include <chrono>
#include <cstdio>

#include "Network.h"
#include "Edge.h"
#include "Algorithm.h"
#include "PivotAlgorithms.h"
#include "RandomGraph.h"

void zadeh() {
    std::cout << "MaxVal, HC:" << std::endl;

    //basic construct
    Network n = Network(0);
    std::vector<intmax_t> b_value = {1, -2, 3, -2, 5, -5};

    //"infinite" capacity
    intmax_t u = std::pow(2, 30);
    std::vector<size_t> from = {0,1,0,0,2,2,4,4};
    std::vector<size_t> to   = {1,0,3,5,1,5,1,3};
    std::vector<intmax_t> co = {0,0,1,3,1,3,3,3};
    std::vector<intmax_t> ca = {u,u,u,u,u,u,u,u};

    for (intmax_t b : b_value) {
        n.addNode(b);
    }

    for (size_t i = 0; i < from.size(); i++) {
        if (not n.addEdge(Edge(from[i], to[i], co[i], ca[i]))) {
            std::cout << "ya failed";
        }
    }

    Algorithm alg = Algorithm (n, pivotMaxVal);

    //intmax_t sum = 0;
    //for (size_t x = 0; x < test2.getNoOfNodes(); x++) {
        alg.solution(false);

        //artificialNode id …
        n.largestNodeID--;
        //sum += rand.getNoOfIter();
        n.clean();
    //}

    std::cout << alg.getNoOfIter() << std::endl;
    //std::cout << sum / (double) n.getNoOfNodes()<< std::endl;

    for (size_t k = 0; k < 7; k++) {

        //enlargen i times
        //n_i equals j in paper
        size_t n_i = n.getNoOfNodes()/2 + 1;
        intmax_t b_value = std::pow(2,n_i - 1) + std::pow(2,n_i - 3);
        size_t newLeft = n.addNode(b_value), //n_i*2 - 2
               newRight = n.addNode(-b_value); //n_i*2 - 1

        intmax_t costInnerEdges = std::pow(2,n_i - 1) - 1;
        for (size_t j = 0; j < n_i - 1; j++) {
            n.addEdge(Edge(newLeft, 2*j + 1, costInnerEdges, u));
            n.addEdge(Edge(2*j, newRight, costInnerEdges, u));
        }

        //Network save = n;
        Algorithm alg = Algorithm (n, pivotMaxVal);
        //n.randomNoise(0.0001);

        //sum = 0;
        //for (size_t x = 0; x < n.getNoOfNodes(); x++) {
            alg.solution(false);
            //sum += rand.getNoOfIter();
            //artificialNode id …
            n.largestNodeID--;
            n.clean();
        //}
        std::cout << alg.getNoOfIter() << std::endl;
        //std::cout << sum / (double) test2.getNoOfNodes()<< std::endl;

        //n = save;
    }
    std::cout << std::endl;
}

void experimentalSearch() {
    std::mt19937 rng;
    rng.seed(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
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
            if (i > 4) {
                solver.solution(true);
                double temp = solver.getNoOfIter()/ (double) i;
                if (temp < 2) {std::cout << "."; break;}
            }
            if (i == 20) {
                for (int i = 0; i < 5; i++) {std::cout << distribution[i] << " | ";}
                std::cout << std::endl << "20: " <<  solver.getNoOfIter()/ (double) i << std::endl << std::endl;
            }
        }
    }
}

int main() {
    zadeh();
    //experimentalSearch();

    std::getchar();
}
