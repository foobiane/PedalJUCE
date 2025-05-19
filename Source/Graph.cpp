#include "Graph.h"

template <typename T>
bool Graph<T>::adjacent(T x, T y) {
    if (!adj_list.contains(x))
        return false;
    
    return adj_list[x].contains(y);
}

template <typename T>
std::unordered_set<T> Graph<T>::neighbors(T x) {
    return adj_list.get(x);
}

template <typename T>
bool Graph<T>::add_vertex(T x) {
    if (adj_list.contains(x))
        return false;
    
    adj_list.insert({x, std::unordered_set<T>()});
    
    return true;
}

template <typename T>
bool Graph<T>::remove_vertex(T x) {
    if (!adj_list.contains(x))
        return false;
    
    adj_list.erase(x);

    for (auto& (k, v) : adj_list) {
        if (v.contains(x))
            v.erase(x);
    }

    return true;
}

template <typename T>
bool Graph<T>::add_edge(T x, T y) {
    if (!adj_list.contains(x) || !adj_list.contains(y))
        return false;
    
    adj_list[x].insert(y);

    return true;
}

template <typename T>
bool Graph<T>::remove_edge(T x, T y) {
    if (!adj_list.contains(x) || !adj_list.contains(y) || !adj_list[x].contains(y))
        return false;

    adj_list[x].erase(y);

    return true;
}