#include <iostream>
#include <random>
#include <time.h>

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <dos.h>
#include <windows.h>

#include "Network.h"
#include "Edge.h"
#include "Algorithm.h"
#include "PivotAlgorithms.h"
#include "RandomGraph.h"

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

    /*for (int i = 100; i < 110; i++) {
        RandomGraph test = RandomGraph(i, 50, 50);
        //test.evolve(i*1000);
        Network n = test.getNetwork();
        Algorithm alg = Algorithm (n, pivotMaxRev);
        alg.solution(i%2==1 ? true : false);
        //alg.solution(true);
        //if (n.getFlow() != 0) {n.print();}
        std::cout << n.getCost() << " | " << n.getFlow() << std::endl;
    }*/

    std::cout << "Random, HC:" << std::endl;

    Network test2 = Network(0);

        std::vector<intmax_t> b_value = {1, -2, 3, -2, 5, -5};
        //basic construct
        //"infinite" capacity
        intmax_t u = std::pow(2, 30);
        std::vector<size_t> from = {0,1,0,0,2,2,4,4};
        std::vector<size_t> to   = {1,0,3,5,1,5,1,3};
        std::vector<intmax_t> co = {0,0,1,3,1,3,3,3};
        std::vector<intmax_t> ca = {u,u,u,u,u,u,u,u};

        for (intmax_t b : b_value) {
            test2.addNode(b);
        }

        for (size_t i = 0; i < from.size(); i++) {
            if (not test2.addEdge(Edge(from[i], to[i], co[i], ca[i]))) {
                std::cout << "ya failed";
            }
        }

        Algorithm rand = Algorithm (test2, pivotRandom);

        intmax_t sum = 0;
        for (size_t x = 0; x < test2.getNoOfNodes(); x++) {
        rand.solution(true);
        //artificialNode id …
        test2.largestNodeID--;
        sum += rand.getNoOfIter();
        test2.clean();
        }

        std::cout << sum / (double) test2.getNoOfNodes()<< std::endl;
    for (size_t k = 0; k < 7; k++) {

        //enlargen i times
        //n_i equals j in paper
            size_t n_i = test2.getNoOfNodes()/2 + 1;
            intmax_t b_value = std::pow(2,n_i - 1) + std::pow(2,n_i - 3);
            size_t newLeft = test2.addNode(b_value), //n_i*2 - 2
                  newRight = test2.addNode(-b_value); //n_i*2 - 1

            intmax_t costInnerEdges = std::pow(2,n_i - 1) - 1;
            for (size_t j = 0; j < n_i - 1; j++) {
                test2.addEdge(Edge(newLeft, 2*j + 1, costInnerEdges, u));
                test2.addEdge(Edge(2*j, newRight, costInnerEdges, u));
            }

        Algorithm rand = Algorithm (test2, pivotRandom);

        sum = 0;
        for (size_t x = 0; x < test2.getNoOfNodes(); x++) {
        rand.solution(true);
        sum += rand.getNoOfIter();
        //artificialNode id …
        test2.largestNodeID--;
        test2.clean();
        }
        std::cout << sum / (double) test2.getNoOfNodes()<< std::endl;
    }
    std::cout << std::endl;
}
