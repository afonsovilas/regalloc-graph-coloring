#ifndef DA_PROJECT_INTERFERENCE_GRAPH_H
#define DA_PROJECT_INTERFERENCE_GRAPH_H

#include <map>
#include <set>
#include <vector>

/**
 * @struct InterferenceNode
 * @brief Nó do grafo de interferência.
 */
struct InterferenceNode {
    int webId;
    std::set<int> interferesWith;
    int color;
    bool spilled;
    
    /**
     * @brief Construtor do nó.
     * @param id Identificador da web.
     */
    InterferenceNode(int id) : webId(id), color(-1), spilled(false) {}
    
    /**
     * @brief Grau do nó.
     * @return Número de vizinhos.
     * \complexity O(1).
     */
    int getDegree() const { return interferesWith.size(); }
};

/**
 * @class InterferenceGraph
 * @brief Grafo de referência para as interferências entre as teias de variáveis (Webs).
 */
class InterferenceGraph {
private:
    std::map<int, InterferenceNode> nodes;
    
public:
    /**
     * @brief Adiciona um nó ao grafo.
     * @param webId ID da web.
     * \complexity O(log V) onde V é o número de nós presentes no grafo.
     */
    void addNode(int webId) {
        nodes.emplace(webId, InterferenceNode(webId));
    }
    
    /**
     * @brief Adiciona uma aresta de interferência não-dirigida entre dois nós.
     * @param web1 Web de origem.
     * @param web2 Web de destino.
     * \complexity O(log V) derivado à pesquisa dos dois IDs dentro do mapa estrutural.
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
     * @brief Verifica a existência de uma interferência ativa entre dois nós.
     * @param web1 Web de origem.
     * @param web2 Web de destino.
     * @return True se existir a aresta de colisão.
     * \complexity O(log V) pela necessidade de procurar a chave no mapa de nós.
     */
    bool interferes(int web1, int web2) const {
        auto it = nodes.find(web1);
        if (it == nodes.end()) return false;
        return it->second.interferesWith.count(web2) > 0;
    }
    
    /**
     * @brief Consulta o grau de conexões de um nó específico.
     * @param webId ID da web.
     * @return Número de vizinhos adjacentes.
     * \complexity O(log V) necessário para aceder ao elemento correto dentro do mapa.
     */
    int getDegree(int webId) const {
        auto it = nodes.find(webId);
        if (it == nodes.end()) return 0;
        return it->second.interferesWith.size();
    }
    
    /**
     * @brief Acesso de leitura (const) a todos os nós guardados no grafo.
     * @return Referência constante do mapa de nós.
     */
    const std::map<int, InterferenceNode>& getNodes() const { return nodes; }
    
    /**
     * @brief Acesso de modificação (mutável) aos nós do grafo.
     * @return Referência direta ao mapa de nós.
     */
    std::map<int, InterferenceNode>& getNodes() { return nodes; }
    
    /**
     * @brief Devolve a quantidade de nós registados no sistema.
     * @return Número total de nós.
     * \complexity O(1).
     */
    int getNumNodes() const { return nodes.size(); }
    
    /**
     * @brief Calcula e devolve o número total de arestas de interferência.
     * @return Número total de arestas únicas.
     * \complexity O(V) onde V representa o número total de nós que constituem o grafo.
     */
    int getNumEdges() const {
        int edges = 0;
        for (const auto& pair : nodes)
            edges += pair.second.interferesWith.size();
        return edges / 2;
    }
    
    /**
     * @brief Elimina por completo um nó do grafo, limpando as suas ocorrências nos vizinhos.
     * @param webId ID da web a remover.
     * \complexity O(V log V) para garantir a remoção segura do ID em todos os subconjuntos de vizinhos.
     */
    void removeNode(int webId) {
        if (nodes.find(webId) != nodes.end()) {
            for (auto& pair : nodes)
                pair.second.interferesWith.erase(webId);
            nodes.erase(webId);
        }
    }
};

#endif // DA_PROJECT_INTERFERENCE_GRAPH_H