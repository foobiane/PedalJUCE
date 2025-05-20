// Basic implementation of an unweighted directed graph ADT using an adjacency list.

#pragma once

#include <unordered_map>
#include <unordered_set>

template <typename T>
class Graph {
    public:
        Graph();
        ~Graph();

        std::vector<T> vertices();
        bool adjacent(T x, T y);
        std::unordered_set<T> neighbors(T x);
        bool addVertex(T x);
        bool removeVertex(T x);
        bool addEdge(T x, T y);
        bool removeEdge(T x, T y);

    private:
        std::unordered_map<T, std::unordered_set<T>> adjList;
};

#include "Graph.tpp"