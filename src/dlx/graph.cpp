#include "graph.h"
#include <queue>
#include <iostream>

using namespace std;

size_t Graph::size() const {
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
    /*for (auto it = _edge.begin(); it != _edge.end(); ++it)
        cout << *(*it) << ' ';
    cout << endl;*/
    for (auto it = _edge.begin(); it != _edge.end(); ++it) {
        for (auto ti = (it->second).begin(); ti != (it->second).end(); ++ti) {
            cout << *(ti->second) << endl;
        }
    }
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
        for (unsigned int i = 0; i < _adjList[currentNode->ID].size(); ++i) {
            if (_vertex[_adjList[currentNode->ID][i]]->isGlobalRef()) continue;
            Q.push(_vertex[_adjList[currentNode->ID][i]]);
        }
    }
    _bfsDone = true;
}

void Graph::RetrieveVertexes(vector<Vertex*>& vec) const {
    vec = _vertex;
}

void Graph::RetrieveEdges(map<int, map<int, Edge*> >& vec) const {
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
    _adjList.resize(vertexNum+1);
    for (int id = 1; id <= vertexNum; ++id) _vertex[id] = new Vertex(id);
}

void Graph::construct_edge(string& token, istringstream& iss) {
    string buffer;
    iss >> buffer;
    int id_1 = stoi(token);
    int id_2 = stoi(buffer);

    // record the edge just once, make key1 > key2
    if (id_1 > id_2) ::swap(id_1, id_2);
    if (_edge.find(id_1) == _edge.end()) {
        _edge[id_1][id_2] = new Edge(id_1, id_2);
    }
    else if (_edge[id_1].find(id_2) == _edge[id_1].end()) {
        _edge[id_1][id_2] = new Edge(id_1, id_2);
    }

    _adjList[id_1].push_back(id_2);
    _adjList[id_2].push_back(id_1);
}

int Graph::get_order(const Vertex* v) const {
    return _adjList[v->ID].size();
}

void Graph::RemoveEdge(int id_1, int id_2) {
    if (id_1 > id_2) ::swap(id_1, id_2);

    // check if key exists
    auto it   = _edge.find(id_1);
    if (it == _edge.end()) { cerr << "edge (" << id_1 << ", " << id_2 << ") does not exist!" << endl; return; }
    auto iter = _edge[id_1].find(id_2);
    if (iter == _edge[id_1].end()) { cerr << "edge (" << id_1 << ", " << id_2 << ") does not exist!" << endl; return; }

#ifdef DEBUG_MODE
    cerr << "removing edge (" << id_1 << ", " << id_2 << ")!" << endl;
#endif

    // remove edge and revise adjacency list
    _edge[id_1].erase(_edge[id_1].find(id_2));
    for (auto it = _adjList[id_1].begin(); it != _adjList[id_1].end(); ++it) {
        if (*it == id_2) { _adjList[id_1].erase(it); break; }
    }
    for (auto it = _adjList[id_2].begin(); it != _adjList[id_2].end(); ++it) {
        if (*it == id_1) { _adjList[id_2].erase(it); break; }
    }
    _removed_edges.push(_edge[id_1][id_2]);
    _bfsDone = false;
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
    for (auto it = _adjList[v->ID].begin(); it != _adjList[v->ID].end(); ++it) {
        for (unsigned int i = 0; i < _adjList[*it].size(); ++i) {
            if (_adjList[*it].at(i) == v->ID) {
                ::swap(_adjList[*it].at(i), _adjList[*it].back());
                _adjList[*it].pop_back();
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
    for (auto it = _adjList[v->ID].begin(); it != _adjList[v->ID].end(); ++it) {
        _adjList[*it].push_back(v->ID);
    }
}

void Graph::print() {
    if (!_bfsDone) bfs();
    cout << "==============================" << endl;
    cout << "= Graph printed in BFS order =" << endl;
    cout << "==============================" << endl;
    for (auto it = _bfsList.begin(); it != _bfsList.end(); ++it) {
        cout << "Vertex ID:" << *(*it) << endl;
    }
    cout << endl;
}
