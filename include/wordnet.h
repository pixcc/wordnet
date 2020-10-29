#include <set>
#include <string>
#include <vector>
#include <unordered_map>
#include <list>

class Digraph
{
public:

    void add_edge(int from, int to);

    std::vector<int> bfs(const std::set<int> & subset);

private:
    std::unordered_map<int, std::list<int>> m_adjacency_list;
};

class ShortestCommonAncestor
{
public:
   ShortestCommonAncestor(const Digraph & dg);

   // calculates length of shortest common ancestor path from node with id 'v' to node with id 'w'
   int length(int v, int w);

   // returns node id of shortest common ancestor of nodes v and w
   int ancestor(int v, int w);

   // calculates length of shortest common ancestor path from node subset 'subset_a' to node subset 'subset_b'
   int length_subset(const std::set<int> & subset_a, const std::set<int> & subset_b);

   // returns node id of shortest common ancestor of node subset 'subset_a' and node subset 'subset_b'
   int ancestor_subset(const std::set<int> & subset_a, const std::set<int> & subset_b);

private:
    Digraph m_graph;

    std::pair<int, int> ancestor_length_subset(const std::set<int> & subset_a, const std::set<int> & subset_b);
};

class WordNet
{
public:
    WordNet(const std::string & synsets_fn, const std::string & hypernyms_fn);

    using Iterator = std::unordered_map<std::string, std::set<int>>::iterator;

    // get iterator to list all nouns stored in WordNet
    Iterator nouns();
    Iterator end();

    // returns 'true' iff 'word' is stored in WordNet
    bool is_noun(const std::string & word) const;

    // returns gloss of "shortest common ancestor" of noun1 and noun2
    std::string sca(const std::string & noun1, const std::string & noun2);

    // calculates distance between noun1 and noun2
    int distance(const std::string & noun1, const std::string & noun2);

private:
    std::unordered_map<std::string, std::set<int>> m_noun_synsets;
    std::unordered_map<int, std::string> m_synset_gloss;
    ShortestCommonAncestor m_sca;

    static Digraph make_graph(const std::string & hypernyms_fn);
};

class Outcast
{
public:
   Outcast(WordNet & wordnet);

   // returns outcast word
   std::string outcast(const std::vector<std::string> & nouns);
private:
    WordNet m_wordnet;
};
