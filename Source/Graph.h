// Basic implementation of an unweighted directed graph ADT using an adjacency list.

#pragma once

#include <unordered_map>
#include <unordered_set>

template <typename T>
class Graph {
    public:
        bool adjacent(T x, T y);
        std::unordered_set<T> neighbors(T x);
        bool add_vertex(T x);
        bool remove_vertex(T x);
        bool add_edge(T x, T y);
        bool remove_edge(T x, T y);

    private:
        std::unordered_map<T, std::unordered_set<T>> adj_list;
};