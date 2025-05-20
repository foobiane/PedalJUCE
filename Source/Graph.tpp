template <typename T>
Graph<T>::Graph() { /* Nothing for now */ }

template <typename T>
Graph<T>::~Graph() { /* Nothing for now */ }

template <typename T>
std::vector<T> Graph<T>::vertices() {
    std::vector<T> vec;

    for (auto& kv : adjList)
        vec.push_back(kv.first);

    return vec;
}

template <typename T>
bool Graph<T>::adjacent(T x, T y) {
    if (!adjList.contains(x))
        return false;
    
    return adjList[x].contains(y);
}

template <typename T>
std::unordered_set<T> Graph<T>::neighbors(T x) {
    return adjList.get(x);
}

template <typename T>
bool Graph<T>::addVertex(T x) {
    if (adjList.contains(x))
        return false;
    
    adjList.insert({x, std::unordered_set<T>()});
    
    return true;
}

template <typename T>
bool Graph<T>::removeVertex(T x) {
    if (!adjList.contains(x))
        return false;
    
    adjList.erase(x);

    for (auto& kv : adjList) {
        if (kv.second.contains(x))
            kv.second.erase(x);
    }

    return true;
}

template <typename T>
bool Graph<T>::addEdge(T x, T y) {
    if (!adjList.contains(x) || !adjList.contains(y))
        return false;
    
    adjList[x].insert(y);

    return true;
}

template <typename T>
bool Graph<T>::removeEdge(T x, T y) {
    if (!adjList.contains(x) || !adjList.contains(y) || !adjList[x].contains(y))
        return false;

    adjList[x].erase(y);

    return true;
}