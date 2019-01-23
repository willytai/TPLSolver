#include "graph.h"
#include <queue>
#include <iostream>

using namespace std;

size_t Graph::size(const int& component_id) const {
    assert(_bfsDone);
    return _bfsList[component_id].size();
}

size_t Graph::numComponents() const {
    return _root_of_cc.size();
}
 
// this part is for graph visualization only
void Graph::write_adjlist(ostream& os, vector<Cell*>& sol) {
    this->RestoreEdges();
    for (auto it = sol.begin(); it != sol.end(); ++it) {
        Vertex* v = (*it)->GetCorrespondVertex();
        os << v->ID << ":";
        for (unsigned int i = 0; i < _adjList[v->ID].size(); ++i) {
            if (_vertex[_adjList[v->ID][i]]->color == UNDEF) continue;
            os << ' ' << _adjList[v->ID][i];
        } os << endl;
    }
}

void Graph::RestoreEdges() {
    while (!_removed_edges.empty()) {
        auto current = _removed_edges.top(); _removed_edges.pop();
        _adjList[current->v1_id].push_back(current->v2_id);
        _adjList[current->v2_id].push_back(current->v1_id);
    }
}

void Graph::ContstructByFile(fstream& file) {
    cerr << "Method ContstructByFile(fstream& file) removed from Graph!" << endl;
    assert(false);

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
    _root_of_cc.push_back(bfs(_root_of_cc.size(), 1));
#ifdef DEBUG_MODE
    cout << "vertexes: ";
    auto it = _vertex.begin();
    ++it;
    for (; it != _vertex.end(); ++it)
        cout << *(*it) << ' ';
    cout << endl;
    cout << "edges of component 0: ";
    /*for (auto it = _edge.begin(); it != _edge.end(); ++it)
        cout << *(*it) << ' ';
    cout << endl;*/
    for (auto it = _edge[0].begin(); it != _edge[0].end(); ++it) {
        for (auto ti = (it->second).begin(); ti != (it->second).end(); ++ti) {
            cout << *(ti->second) << endl;
        }
    }
#endif
}

void Graph::ContstructByAdjList(fstream& file) {
    cerr << "Constructing Graph...";
    unsigned int RowNum = 0;
    string buffer;
    if (!file.is_open()) return;

    while (getline(file, buffer)) {
        // adjust the size of the _adjList
        if (_adjList.size() < RowNum+1) {
            _adjList.resize(RowNum+1);
            _vertex.resize(RowNum+1);
        }

        istringstream iss(buffer);
        string token;
        int id_1 = RowNum;
        while (iss >> token) {
            int id_2 = stoi(token);
            _adjList[id_1].push_back(id_2);
        }
        _vertex[RowNum] = new Vertex(RowNum);
        ++RowNum;
    }
#ifdef DEBUG_MODE_GRAPH
    for (unsigned int i = 0; i < _adjList.size(); ++i) {
        // cout << i << ':';
        for (auto ti = _adjList[i].begin(); ti != _adjList[i].end(); ++ti) {
            cout << *ti << ' ';
        } cout << endl;
    }
#endif
    cerr << "Done" << endl;
    cerr << "Identifying CC...";
    this->identify_connected_component();
    cerr << "Done" << endl;
}

unsigned Vertex::_globalRef = 0;

int Graph::bfs(int component_id, int r_id) {
    if (r_id == -1) {
        // cerr << "not yet implemented in graph.cpp" << endl;
        // cerr << "need to run bfs, this CC could be split into two or more due to the removeall of the conflict edges" << endl;
        // identify all the connect component add replace the current component with the largest one
        // while adding the other CCs to the end of _bfsList
        // set the root of _root_of_cc 
        vector<Vertex*> tmp;
        Vertex::setGlobalRef();
        for (unsigned int i = 0; i < _bfsList[component_id].size(); ++i) {
            if (_bfsList[component_id][i]->isGlobalRef()) continue;
            queue<Vertex*> Q; Q.push(_bfsList[component_id][i]);
            while (!Q.empty()) {
                Vertex* currentNode = Q.front(); Q.pop();
                if (currentNode->isGlobalRef()) continue;
                tmp.push_back(currentNode);
                currentNode->setToGlobalRef();
                for (unsigned int j = 0; j < _adjList[currentNode->ID].size(); ++j) {
                    if (_vertex[_adjList[currentNode->ID][j]]->isGlobalRef()) continue;
                    Q.push(_vertex[_adjList[currentNode->ID][j]]);
                }
            }
        }
        Vertex* root = tmp[0];
        for (auto it = tmp.begin(); it != tmp.end(); ++it) {
            if (get_order(root) < get_order(*it)) {
                root = *it;
            }
        }
        // run bfs again with the vertex with the greatest order as root
        tmp.clear();
        Vertex::setGlobalRef();
        queue<Vertex*> Q; Q.push(root);
        while (!Q.empty()) {
            Vertex* currentNode = Q.front(); Q.pop();
            if (currentNode->isGlobalRef()) continue;
            tmp.push_back(currentNode);
            currentNode->setToGlobalRef();
            for (unsigned int j = 0; j < _adjList[currentNode->ID].size(); ++j) {
                if (_vertex[_adjList[currentNode->ID][j]]->isGlobalRef()) continue;
                Q.push(_vertex[_adjList[currentNode->ID][j]]);
            }
        }
        for (unsigned int i = 0; i < _bfsList[component_id].size(); ++i) {
            if (_bfsList[component_id][i]->isGlobalRef()) continue;
            tmp.push_back(_bfsList[component_id][i]);
        }
        _bfsList[component_id].swap(tmp);
        /*for (auto it = _bfsList[component_id].begin(); it != _bfsList[component_id].end(); ++it) {
            cout << *(*it) << ' ';
        } cout << endl;*/
        return -1;
    }

    if (_bfsList.size() < component_id+1) _bfsList.resize(component_id+1);
    if (_edge.size() != _bfsList.size()) _edge.resize(_bfsList.size());

    // collect all the connected components first
    Vertex::setGlobalRef();
    queue<Vertex*> Q;
    Q.push(_vertex[r_id]);
    _bfsList[component_id].clear();
    while (!Q.empty()) {
        Vertex* currentNode = Q.front(); Q.pop();
        if (currentNode->isGlobalRef()) continue;
        _bfsList[component_id].push_back(currentNode);
        currentNode->setToGlobalRef();
        for (unsigned int i = 0; i < _adjList[currentNode->ID].size(); ++i) {
            if (_vertex[_adjList[currentNode->ID][i]]->isGlobalRef()) continue;
            Q.push(_vertex[_adjList[currentNode->ID][i]]);
            _predecessor[_adjList[currentNode->ID][i]] = currentNode->ID;
        }
    }

    // find the vertex with the greatest order
    Vertex* root = _bfsList[component_id][0];
    for (unsigned int i = 1; i < _bfsList[component_id].size(); ++i) {
        if (get_order(root) < get_order(_bfsList[component_id][i])) {
            root = _bfsList[component_id][i];
        }
    }

    // and run bfs again and record edges
    Vertex::setGlobalRef();
    Q.push(root);
    _bfsList[component_id].clear();
    _edge[component_id].clear();
    while (!Q.empty()) {
        Vertex* currentNode = Q.front(); Q.pop();
        if (currentNode->isGlobalRef()) continue;
        _bfsList[component_id].push_back(currentNode);
        currentNode->setToGlobalRef();
        for (unsigned int i = 0; i < _adjList[currentNode->ID].size(); ++i) {

#ifdef DEBUG_MODE_EDGES
            int id_1 = currentNode->ID;
            int id_2 = _adjList[id_1][i];
            _edge[component_id][id_1][id_2] = new Edge(id_1, id_2);
#else
            // record the edge just once, make key1 > key2
            int id_1 = currentNode->ID;
            int id_2 = _adjList[id_1][i];
            if (id_1 > id_2) ::swap(id_1, id_2);
            if (_edge[component_id].find(id_1) == _edge[component_id].end()) {
                _edge[component_id][id_1][id_2] = new Edge(id_1, id_2);
            }
            else if (_edge[component_id][id_1].find(id_2) == _edge[component_id][id_1].end()) {
                _edge[component_id][id_1][id_2] = new Edge(id_1, id_2);
            }
#endif

            // bfs part
            if (_vertex[_adjList[currentNode->ID][i]]->isGlobalRef()) continue;
            Q.push(_vertex[_adjList[currentNode->ID][i]]);
            _predecessor[_adjList[currentNode->ID][i]] = currentNode->ID;
        }
    }
    _bfsDone = true;
#ifdef DEBUG_MODE_GRAPH
    cout << "_bfsList " << component_id << " size: " << _bfsList[component_id].size() << endl;
#endif
    return root->ID;
}

void Graph::RetrieveVertexes(vector<Vertex*>& vec, const int& component_id) const {
    // vec = _vertex;
    vec = _bfsList[component_id];
}

void Graph::RetrieveEdges(map<int, map<int, Edge*> >& vec, const int& component_id) const {
    vec = _edge[component_id];
}

void Graph::GetBFSList(vector<Vertex*>& vec, int component_id) const {
    assert(_bfsDone);
    vec = _bfsList[component_id];
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
    assert(0);
    /*string buffer;
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
    _adjList[id_2].push_back(id_1);*/
}

int Graph::get_order(const Vertex* v) const {
    return _adjList[v->ID].size();
}

void Graph::RemoveEdge(int id_1, int id_2, int component_id) {
    if (id_1 > id_2) ::swap(id_1, id_2);

    // check if key exists
    auto it   = _edge[component_id].find(id_1);
    if (it == _edge[component_id].end()) { cerr << "edge (" << id_1 << ", " << id_2 << ") does not exist!" << endl; return; }
    auto iter = _edge[component_id][id_1].find(id_2);
    if (iter == _edge[component_id][id_1].end()) { cerr << "edge (" << id_1 << ", " << id_2 << ") does not exist!" << endl; return; }

#ifdef DEBUG_MODE_GRAPH
    cerr << "removing edge (" << id_1 << ", " << id_2 << ")!" << endl;
#endif

    // remove edge and revise adjacency list
    _removed_edges.push(_edge[component_id][id_1][id_2]);
    _edge[component_id][id_1].erase(_edge[component_id][id_1].find(id_2));
    for (auto it = _adjList[id_1].begin(); it != _adjList[id_1].end(); ++it) {
        if (*it == id_2) { _adjList[id_1].erase(it); break; }
    }
    for (auto it = _adjList[id_2].begin(); it != _adjList[id_2].end(); ++it) {
        if (*it == id_1) { _adjList[id_2].erase(it); break; }
    }

    /*************************************************
    * it's OK not to change the order of the bfslist *
    *************************************************/
    // _bfsDone = false;
}

void Graph::RemoveVertex(Vertex* v) {
    if (!_vertex[v->ID]) {
        cerr << "vertex ID:" << *v << " is already removed!"  << endl;
        return;
    }
#ifdef DEBUG_MODE_GRAPH
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
#ifdef DEBUG_MODE_GRAPH
    cout << "Restoring vertex ID:" << *v << endl;
#endif
    for (auto it = _adjList[v->ID].begin(); it != _adjList[v->ID].end(); ++it) {
        _adjList[*it].push_back(v->ID);
    }
}

void Graph::print() {
    if (!_bfsDone) assert(0);
    cout << "==============================" << endl;
    cout << "= Graph printed in BFS order =" << endl;
    cout << "==============================" << endl;
    for (unsigned int i = 0; i < _bfsList.size(); ++i) {
        cout << "Component index " << i << endl;
        for (auto it = _bfsList[i].begin(); it != _bfsList[i].end(); ++it) {
            cout << "\tVertex ID: " << *(*it) << endl;
        } cout << endl;
    }
}

void Graph::identify_connected_component() {
    int CC_count = 0;
    _predecessor = new int[_vertex.size()];
    for (unsigned int i = 0; i < _vertex.size(); ++i) _predecessor[i] = -1;
    for (unsigned int v = 1; v < _vertex.size(); ++v) {
        if (_predecessor[v] == -1) {
            // cerr << "bfs...";
            _root_of_cc.push_back(bfs(CC_count++, v));
            // cerr << "Done" << endl;
        }
    }
    delete _predecessor;
    _predecessor = NULL;
    cout << "There are " << _root_of_cc.size() << " connected components" << endl;
#ifdef DEBUG_MODE_GRAPH
    cout << "CC 0" << endl;
    for (int i = 0; i < _bfsList[0].size(); ++i) {
        cout << *_bfsList[0][i] << endl;
    }
    for (auto it = _edge[0].begin(); it != _edge[0].end(); ++it) {
        for (auto ti = (*it).second.begin(); ti != (*it).second.end(); ++ti) {
            cout << *((*ti).second) << endl;
        }
    }
#endif

    // return;
    // sort the CCs is ascendig order in terms of the number of vertexes
    // very stupid
    assert (_bfsList.size() == _root_of_cc.size());
    assert (_edge.size() == _bfsList.size());
    for (unsigned int i = 0; i < _root_of_cc.size()-1; ++i) {
        for (unsigned int j = i + 1; j < _root_of_cc.size(); ++j) {
            if (_bfsList[i].size() > _bfsList[j].size()) {
                ::swap(_bfsList[i], _bfsList[j]);
                ::swap(_root_of_cc[i], _root_of_cc[j]);
                ::swap(_edge[i], _edge[j]);
            }
        }
    }
    for (unsigned int i = 0; i < _bfsList.size(); ++i)
        cout << "cc " << i << ": " << _bfsList[i].size() << endl;

#ifdef DEBUG_MODE_IDENT
    if (_bfsList.size() > 96) {
        ::swap(_bfsList[0], _bfsList[96]);
        ::swap(_root_of_cc[0], _root_of_cc[96]);
        ::swap(_edge[0], _edge[96]);
    }
#endif
}
