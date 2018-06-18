#include <algorithm>

#include "Circle.h"

void Circle::addEdge(size_t node0, size_t node1, bool _isResidual) {
    edges.push_back(std::make_pair(node0, node1));
    isResidual.push_back(_isResidual);
    length = edges.size();
}

//see paper for details and proof (probably)
void Circle::update(Circle& c) {
    std::pair<size_t, size_t> edgeSame = c.getEdges()[0];
    std::pair<size_t, size_t> edgeReverse = std::make_pair(edgeSame.second, edgeSame.first);

    bool reversed = false;
    std::vector<std::pair<size_t, size_t>>::iterator it = std::find(edges.begin(), edges.end(), edgeSame);
    //if edgeSame is in this->edges, reverse c, do second if-case
    //and reverse back
    if (it != edges.end()) {
        c.rotateBy(0, true);
        reversed = true;
    }
    else {it = std::find(edges.begin(), edges.end(), edgeReverse);}

    //redirect this circle over c
    if (reversed or  it != edges.end()) {
        size_t index = std::distance(edges.begin(), it);
        std::vector<bool> toCopy (c.length, true);
        size_t indexLeft = index - 1, indexRight = index + 1;

        //take out all edges existent in both circles
        toCopy[0] = false; //first edge not even in tree
        //left from first edge
        for (; indexLeft > 0; indexLeft--) {
            //circles are in opposite directions, so reverse the edge for existence check
            std::pair<size_t, size_t> checkEdge =
                std::make_pair(c.getEdges()[index - indexLeft].second, c.getEdges()[index - indexLeft].first);
            if (edges[indexLeft] == checkEdge) {toCopy[index - indexLeft] = false;}
            else {break;}
        }
        //right from first edge
        for (; indexRight < this->length; indexRight++) {
            //circles are in opposite directions, so reverse the edge for existence check
            std::pair<size_t, size_t> checkEdge =
                std::make_pair(c.getEdges()[c.length + index - indexRight].second, c.getEdges()[c.length + index - indexRight].first);
            if (edges[indexRight] == checkEdge) {toCopy[c.length + index - indexRight] = false;}
            else {break;}
        }

        //construct new circle
        std::vector<std::pair<size_t, size_t>> edgesNew;
        std::vector<bool> isResidualNew;

        //first part of old circle
        for (size_t i = 0; i <= indexLeft; i++) {
            edgesNew.push_back(this->edges[i]);
            isResidualNew.push_back(this->isResidual[i]);
        }
        //bypass
        for (size_t i = 0; i < c.length; i++) {
            if (toCopy[i]) {
                edgesNew.push_back(c.getEdges()[i]);
                isResidualNew.push_back(c.getIsResidual()[i]);
            }
        }
        //last part of old circle
        for (size_t i = indexRight; i < this->length; i++) {
            edgesNew.push_back(this->edges[i]);
            isResidualNew.push_back(this->isResidual[i]);
        }

        this->edges = edgesNew;
        this->isResidual = isResidualNew;
    }

    //reverse back
    if(reversed) {c.rotateBy(0, true);}
    length = edges.size();
}

void Circle::rotateBy (size_t index, bool toReverse) {
    //rotate in either case, such that
    //the correct edge is on front
    std::rotate(edges.begin(), edges.begin() + index, edges.end());
    std::rotate(isResidual.begin(), isResidual.begin() + index, isResidual.end());
    //all but the first entry have to be reversed in position
    if (toReverse) {
        //change direction of all edges
        for (size_t i = 0; i < this->length; i++) {
            std::swap(edges[i].first, edges[i].second);
            isResidual[i] = not isResidual[i];
        }
        std::reverse(edges.begin() + 1, edges.end());
        std::reverse(isResidual.begin() + 1, isResidual.end());
    }
}

const std::vector<std::pair<size_t, size_t>>& Circle::getEdges() {
    return edges;
}

const std::vector<bool>& Circle::getIsResidual() {
    return isResidual;
}

size_t Circle::size() {
    return length;
}
