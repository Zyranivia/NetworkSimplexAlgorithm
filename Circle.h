#ifndef CIRCLE_H_INCLUDED
#define CIRCLE_H_INCLUDED

#include <vector>
#include <utility>
#include <cstddef>
#include <cstdint>

//first edge of edges is not part
//of the underlying tree
class Circle {
public:
    void print() const;
    //default constructor
    void addEdge(size_t node0, size_t node1, bool isResidual);

    const std::vector<std::pair<size_t, size_t>>& getEdges() const;
    const std::vector<char>& getIsResidual() const;
    size_t size() const;

    //function is needed for Network simplex
    void update(Circle& c);
    //true means reverse circle
    void rotateBy (size_t index, bool toReverse);

    //both values are to be set
    //circles don’t know about their graphs
    intmax_t flow = 0, costPerFlow = 0;

private:
    size_t length = 0;
    std::vector<std::pair<size_t, size_t>> edges;
    //std::vector<char>, because std::vector<bool> is broken
    std::vector<char> isResidual;
};

#endif // CIRCLE_H_INCLUDED
