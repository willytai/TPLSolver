#include "graph.h"
#include <iostream>

using namespace std;

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

void Graph::RetrieveVertexes(vector<Vertex*>& vec) {
    vec = _vertex;
}

void Graph::RetrieveEdges(vector<Edge*>& vec) {
    vec = _edge;
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
    (_vertex[id_1]->VertexList).push_back(_vertex[id_2]);

}
