#ifndef DA_PROJECT_INTERFERENCE_GRAPH_H
#define DA_PROJECT_INTERFERENCE_GRAPH_H

#include <map>
#include <set>
#include <vector>

/**
 * @struct InterferenceNode
 * @brief No do grafo de interferencia.
 */
struct InterferenceNode {
    int webId;
    std::set<int> interferesWith;
    int color;
    bool spilled;
    
    /**
     * @brief Construtor do no.
     * @param id Identificador do web.
     */
    InterferenceNode(int id) : webId(id), color(-1), spilled(false) {}
    
    /**
     * @brief Grau do no.
     * @return Numero de vizinhos.
     * @complexity O(1).
     */
    int getDegree() const { return interferesWith.size(); }
};

/**
 * @class InterferenceGraph
 * @brief Grafo de interferencia entre webs.
 */
class InterferenceGraph {
private:
    std::map<int, InterferenceNode> nodes;
    
public:
    /**
     * @brief Adiciona um no ao grafo.
     * @param webId ID do web.
     * @complexity O(log n).
     */
    void addNode(int webId) {
        nodes.emplace(webId, InterferenceNode(webId));
    }
    
    /**
     * @brief Adiciona uma aresta entre dois webs.
     * @param web1 Web origem.
     * @param web2 Web destino.
     * @complexity O(log n) por insercao em set.
     */
    void addInterference(int web1, int web2) {
        if (web1 == web2) return;
        auto it1 = nodes.find(web1);
        auto it2 = nodes.find(web2);
        if (it1 != nodes.end() && it2 != nodes.end()) {
            it1->second.interferesWith.insert(web2);
            it2->second.interferesWith.insert(web1);
        }
    }
    
    /**
     * @brief Verifica se dois webs interferem.
     * @param web1 Web origem.
     * @param web2 Web destino.
     * @return True se existe aresta.
     * @complexity O(log n).
     */
    bool interferes(int web1, int web2) const {
        auto it = nodes.find(web1);
        if (it == nodes.end()) return false;
        return it->second.interferesWith.count(web2) > 0;
    }
    
    /**
     * @brief Grau de um no.
     * @param webId ID do web.
     * @return Numero de vizinhos.
     * @complexity O(1).
     */
    int getDegree(int webId) const {
        auto it = nodes.find(webId);
        if (it == nodes.end()) return 0;
        return it->second.interferesWith.size();
    }
    
    /**
     * @brief Acesso a todos os nos (const).
     */
    const std::map<int, InterferenceNode>& getNodes() const { return nodes; }
    
    /**
     * @brief Acesso a todos os nos (mutavel).
     */
    std::map<int, InterferenceNode>& getNodes() { return nodes; }
    
    /**
     * @brief Numero de nos no grafo.
     * @complexity O(1).
     */
    int getNumNodes() const { return nodes.size(); }
    
    /**
     * @brief Numero de arestas no grafo.
     * @complexity O(n) no numero de nos.
     */
    int getNumEdges() const {
        int edges = 0;
        for (const auto& pair : nodes)
            edges += pair.second.interferesWith.size();
        return edges / 2;
    }
    
    /**
     * @brief Remove um no e as suas arestas.
     * @param webId ID do web.
     * @complexity O(n log n) para remover o web de todos os sets.
     */
    void removeNode(int webId) {
        if (nodes.find(webId) != nodes.end()) {
            for (auto& pair : nodes)
                pair.second.interferesWith.erase(webId);
            nodes.erase(webId);
        }
    }
};

#endif
