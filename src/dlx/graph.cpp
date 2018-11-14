#include "graph.h"
#include <queue>
#include <iostream>

using namespace std;

int Graph::size() const {
    assert(_bfsDone);
    return _bfsList.size();
}

void Graph::ContstructByFile(fstream& file) {
    string buffer;
    if (!file.is_open()) return;

    while (getline(file, buffer)) {
        if (buffer.empty()) break;

        istringstream iss(buffer);

        string token;
        while (iss >> token) {
            // skip comments
            if (!token.compare("c")) break;
            if (!token.compare("vertex")) add_vertex(iss);
            else construct_edge(token, iss);
        }
    }
    bfs();
#ifdef DEBUG_MODE
    cout << "vertexes: ";
    auto it = _vertex.begin();
    ++it;
    for (; it != _vertex.end(); ++it)
        cout << *(*it) << ' ';
    cout << endl;
    cout << "edges: ";
    for (auto it = _edge.begin(); it != _edge.end(); ++it)
        cout << *(*it) << ' ';
    cout << endl;
#endif
}

unsigned Vertex::_globalRef = 0;

void Graph::bfs() {
    Vertex* root = NULL;
    _bfsList.clear();
    for (unsigned int i = 1; i < _vertex.size(); ++i) {
        if (!_vertex[i]) continue;
        if (!root) { root = _vertex[i]; continue; }
        if (get_order(root) < get_order(_vertex[i])) {
            root = _vertex[i];
        }
    }
    if (!root) { _bfsDone = true; return; }       // graph is empty
    Vertex::setGlobalRef();
    queue<Vertex*> Q;
    Q.push(root);
    while (!Q.empty()) {
        Vertex* currentNode = Q.front(); Q.pop();
        if (currentNode->isGlobalRef()) continue;
        _bfsList.push_back(currentNode);
        currentNode->setToGlobalRef();
        for (unsigned int i = 0; i < currentNode->VertexList.size(); ++i) {
            if (currentNode->VertexList[i]->isGlobalRef()) continue;
            Q.push(currentNode->VertexList[i]);
        }
    }
    _bfsDone = true;
}

void Graph::RetrieveVertexes(vector<Vertex*>& vec) const {
    vec = _vertex;
}

void Graph::RetrieveEdges(vector<Edge*>& vec) const {
    vec = _edge;
}

void Graph::GetBFSList(vector<Vertex*>& vec) const {
    assert(_bfsDone);
    vec = _bfsList;
}

void Graph::add_vertex(istringstream& iss) {
    string buffer;
    iss >> buffer;
    int vertexNum = stoi(buffer);
    _vertex.resize(vertexNum+1);
    for (int id = 1; id <= vertexNum; ++id) _vertex[id] = new Vertex(id);
}

void Graph::construct_edge(string& token, istringstream& iss) {
    string buffer;
    iss >> buffer;
    int id_1 = stoi(token);
    int id_2 = stoi(buffer);
    _edge.push_back(new Edge(_vertex[id_1], _vertex[id_2]));

    (_vertex[id_1]->VertexList).push_back(_vertex[id_2]);
    (_vertex[id_2]->VertexList).push_back(_vertex[id_1]);

}

int Graph::get_order(const Vertex* v) const {
    return v->VertexList.size();
}

void Graph::RemoveEdge(Edge* e) {

}

void Graph::RemoveVertex(Vertex* v) {
    if (!_vertex[v->ID]) {
        cerr << "vertex ID:" << *v << " is already removed!"  << endl;
        return;
    }
#ifdef DEBUG_MODE
    cout << "Removing vertex ID:" << *v << endl;
#endif
    _bfsDone       = false;
    _vertex[v->ID] = NULL;
    _removed_vertexes.push(v);
    for (auto it = v->VertexList.begin(); it != v->VertexList.end(); ++it) {
        for (unsigned int i = 0; i < (*it)->VertexList.size(); ++i) {
            if ((*it)->VertexList.at(i)->ID == v->ID) {
                ::swap((*it)->VertexList.at(i), (*it)->VertexList.back());
                (*it)->VertexList.pop_back();
                break;
            }
        }
    }
}

void Graph::RemoveVertex(int idx) {
    RemoveVertex(_vertex[idx]);
}

void Graph::RecoverVertex(Vertex* v) {
    RecoverVertex(v->ID);
}

void Graph::RecoverVertex(int idx) {
    if (_vertex[idx]) {
        cerr << "vertex ID:" << *_vertex[idx] << " is already in the graph!" << endl;
        return;
    }
    while (!_removed_vertexes.empty()) {
        restore(_removed_vertexes.top());
        _vertex[_removed_vertexes.top()->ID] = _removed_vertexes.top();
        if (_removed_vertexes.top()->ID == idx) {
            _removed_vertexes.pop();
            break;
        }
        _removed_vertexes.pop();
    }
    _bfsDone = false;
}

void Graph::restore(Vertex* v) {
#ifdef DEBUG_MODE
    cout << "Restoring vertex ID:" << *v << endl;
#endif
    for (auto it = v->VertexList.begin(); it != v->VertexList.end(); ++it) {
        (*it)->VertexList.push_back(v);
    }
}

void Graph::print() const {
    assert(_bfsDone);
    cout << "==============================" << endl;
    cout << "= Graph printed in BFS order =" << endl;
    cout << "==============================" << endl;
    for (auto it = _bfsList.begin(); it != _bfsList.end(); ++it) {
        cout << "Vertex ID:" << *(*it) << endl;
    }
    cout << endl;
}
