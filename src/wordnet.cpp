#include "wordnet.h"
#include <fstream>
#include <sstream>
#include <queue>

WordNet::WordNet(const std::string &synsets_fn, const std::string &hypernyms_fn) : m_sca(make_graph(hypernyms_fn)) {
    std::ifstream synsets(synsets_fn);

    std::string line;
    while (std::getline(synsets, line)) {
        std::istringstream str(line);

        std::string row;
        std::getline(str, row, ',');
        int id = stoi(row);

        std::getline(str, row, ',');
        std::istringstream words(row);

        std::string word;
        while (words >> word) {
            m_noun_synsets[word].insert(id);
        }

        std::getline(str, row);
        m_synset_gloss[id] = row;
    }

}

Digraph WordNet::make_graph(const std::string &hypernyms_fn) {
    std::ifstream hypernyms(hypernyms_fn);

    Digraph graph;
    std::string line;
    while (std::getline(hypernyms, line)) {
        std::istringstream str(line);

        std::string row;
        std::getline(str, row, ',');
        int from = stoi(row);

        while (std::getline(str, row, ',')) {
            int to = stoi(row);
            graph.add_edge(from, to);
        }
    }
    return graph;
}


WordNet::Iterator WordNet::nouns() {
    return m_noun_synsets.begin();
}

WordNet::Iterator WordNet::end() {
    return m_noun_synsets.end();
}


bool WordNet::is_noun(const std::string &word) const {
    return m_noun_synsets.count(word) != 0;
}


std::string WordNet::sca(const std::string &noun1, const std::string &noun2) {
    return m_synset_gloss[m_sca.ancestor_subset(m_noun_synsets[noun1], m_noun_synsets[noun2])];
}


int WordNet::distance(const std::string &noun1, const std::string &noun2) {
    return m_sca.length_subset(m_noun_synsets[noun1], m_noun_synsets[noun2]);
}

ShortestCommonAncestor::ShortestCommonAncestor(const Digraph &dg) : m_graph(dg) {}


int ShortestCommonAncestor::length(int v, int w) {
    return ShortestCommonAncestor::length_subset({v}, {w});
}

int ShortestCommonAncestor::ancestor(int v, int w) {
    return ShortestCommonAncestor::ancestor_subset({v}, {w});
}

std::pair<int, int> ShortestCommonAncestor::ancestor_length_subset(const std::set<int> &subset_a,
                                                                   const std::set<int> &subset_b) {
    std::vector<int> distance_a = m_graph.bfs(subset_a);
    std::vector<int> distance_b = m_graph.bfs(subset_b);

    int min_length = std::numeric_limits<int>::max();
    int min_id = -1;
    for (size_t i = 0; i < distance_a.size(); i++) {
        if (distance_a[i] >= 0 && distance_b[i] >= 0 && distance_a[i] + distance_b[i] < min_length) {
            min_length = distance_a[i] + distance_b[i];
            min_id = i;
        }
    }
    return {min_id, min_length};
}

int ShortestCommonAncestor::length_subset(const std::set<int> &subset_a, const std::set<int> &subset_b) {
    return ancestor_length_subset(subset_a, subset_b).second;
}

int ShortestCommonAncestor::ancestor_subset(const std::set<int> &subset_a, const std::set<int> &subset_b) {
    return ancestor_length_subset(subset_a, subset_b).first;
}

std::vector<int> Digraph::bfs(const std::set<int> &subset) {
    std::vector<int> distance(m_adjacency_list.size(), -1);

    std::queue<int> queue;
    for (auto const &v: subset) {
        distance[v] = 0;
        queue.push(v);
    }

    while (!queue.empty()) {
        int u = queue.front();
        queue.pop();

        for (auto const &v: m_adjacency_list[u]) {
            if (distance[v] == -1) {
                distance[v] = distance[u] + 1;
                queue.push(v);
            }
        }
    }

    return distance;
}

void Digraph::add_edge(int from, int to) {
    m_adjacency_list[from].push_back(to);
}

Outcast::Outcast(WordNet &wordnet) : m_wordnet(wordnet) {}

std::string Outcast::outcast(const std::vector<std::string> &nouns) {
    int max_dist = 0;
    std::string max_dist_noun;

    if (nouns.size() > 2) {
        for (size_t i = 0; i < nouns.size(); i++) {
            int dist = 0;
            for (size_t j = 0; j < nouns.size(); j++) {
                if (i != j) {
                    dist += m_wordnet.distance(nouns[i], nouns[j]);
                }
            }

            if (dist > max_dist) {
                max_dist_noun = nouns[i];
                max_dist = dist;
            }
        }
    }

    return max_dist_noun;
}