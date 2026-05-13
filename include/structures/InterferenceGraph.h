#ifndef DA_PROJECT_INTERFERENCE_GRAPH_H
#define DA_PROJECT_INTERFERENCE_GRAPH_H

#include <map>
#include <set>
#include <vector>

struct InterferenceNode {
    int webId;
    std::set<int> interferesWith;
    int color;
    bool spilled;
    
    InterferenceNode(int id) : webId(id), color(-1), spilled(false) {}
    
    int getDegree() const { return interferesWith.size(); }
};

class InterferenceGraph {
private:
    std::map<int, InterferenceNode> nodes;
    
public:
    void addNode(int webId) {
        if (nodes.find(webId) == nodes.end())
            nodes[webId] = InterferenceNode(webId);
    }
    
    void addInterference(int web1, int web2) {
        if (web1 == web2) return;
        if (nodes.find(web1) != nodes.end() && nodes.find(web2) != nodes.end()) {
            nodes[web1].interferesWith.insert(web2);
            nodes[web2].interferesWith.insert(web1);
        }
    }
    
    bool interferes(int web1, int web2) const {
        auto it = nodes.find(web1);
        if (it == nodes.end()) return false;
        return it->second.interferesWith.count(web2) > 0;
    }
    
    int getDegree(int webId) const {
        auto it = nodes.find(webId);
        if (it == nodes.end()) return 0;
        return it->second.interferesWith.size();
    }
    
    const std::map<int, InterferenceNode>& getNodes() const { return nodes; }
    std::map<int, InterferenceNode>& getNodes() { return nodes; }
    
    int getNumNodes() const { return nodes.size(); }
    
    int getNumEdges() const {
        int edges = 0;
        for (const auto& pair : nodes)
            edges += pair.second.interferesWith.size();
        return edges / 2;
    }
    
    void removeNode(int webId) {
        if (nodes.find(webId) != nodes.end()) {
            for (auto& pair : nodes)
                pair.second.interferesWith.erase(webId);
            nodes.erase(webId);
        }
    }
};

#endif
