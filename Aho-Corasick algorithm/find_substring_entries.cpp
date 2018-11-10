#include <iostream>
#include <string>
#include <vector>

template<size_t ALPHABET_SIZE = 26>
struct Automaton {
    struct Node {
        Node() {
            is_terminal = false;
            sons = std::vector<int>(ALPHABET_SIZE, -1);
            aut_next = std::vector<int>(ALPHABET_SIZE, -1);
            parent_edge = char();
            parent = suff_link = -1;
        }

        bool is_terminal;
        std::vector<int> sons;
        std::vector<int> aut_next;
        int parent;
        char parent_edge;
        int suff_link;
        std::vector<int> ends;

        void print() {
            std::cout << "\nSons:";
            for (int i = 0; i < ALPHABET_SIZE; ++i) {
                std::cout << " " << sons[i];
            }
            std::cout << "\nAut_next:";
            for (int i = 0; i < ALPHABET_SIZE; ++i) {
                std::cout << " " << aut_next[i];
            }
            std::cout << "\nParent: " << parent << " by " << parent_edge;
            std::cout << "\nSuffix link: " << suff_link << std::endl;
        }
    };

    std::vector<Node> nodes_;

    Automaton() {
        nodes_ = std::vector<Node>(1);
    }

    void add_string(const std::string &s, int begin) {
        int v = 0;
        for (size_t i = 0; i < s.length(); ++i) {
            int symb_ind = s[i] - 'a';
            if (nodes_[v].sons[symb_ind] == -1) {
                Node new_node = Node();
                nodes_[v].sons[symb_ind] = nodes_.size();
                new_node.parent = v;
                new_node.parent_edge = s[i];
                nodes_.push_back(new_node);
            }
            v = nodes_[v].sons[symb_ind];
        }
        nodes_[v].is_terminal = true;
        nodes_[v].ends.push_back(begin + s.size() - 1);
    }

    void print() {
        for (int i = 0; i < nodes_.size(); ++i) {
            nodes_[i].print();
        }
    }

    int push(int v, char c) {
        int symb = c - 'a';
        if (nodes_[v].aut_next[symb] == -1) {
            nodes_[v].aut_next[symb] = (nodes_[v].sons[symb] != -1) ?
                                       nodes_[v].sons[symb] : ((!v) ? 0 : push(get_suff_link(v), c));
        }
        return nodes_[v].aut_next[symb];
    }

    int get_suff_link(int v) {
        if (nodes_[v].suff_link == -1) {
            nodes_[v].suff_link = (v && nodes_[v].parent) ? push(get_suff_link(nodes_[v].parent), nodes_[v].parent_edge)
                                                          : 0;
        }
        return nodes_[v].suff_link;
    }

    bool is_terminal(int v) {
        return nodes_[v].is_terminal;
    }
};

std::vector<std::string> parse(const std::string &s, std::vector<size_t> &begins, const char divider) {
    std::vector<std::string> subs;
    begins.clear();
    size_t ind = 0;
    std::string current;
    while (ind < s.length()) {
        if (s[ind] == divider) {
            if (!current.empty()) {
                subs.push_back(current);
                current = "";
            }
        } else {
            if (current.empty()) {
                begins.push_back(ind);
            }
            current += s[ind];
        }
        ++ind;
    }
    if (!current.empty())
        subs.push_back(current);
    return subs;
}

void print_mask_entries(const std::string &mask, const std::string &s, const char divider = '?') {
    std::vector<size_t> begins;
    auto patterns = parse(mask, begins, divider);
    std::vector<int> cnt(s.length());
    Automaton m;
    for (size_t i = 0; i < patterns.size(); ++i) {
        m.add_string(patterns[i], begins[i]);
    }
    int cur_node = 0;
    for (int j = 0; j < s.length(); ++j) {
        cur_node = m.push(cur_node, s[j]);
        int v = cur_node;
        while (v) {
            if (m.is_terminal(v)) {
                for (size_t i = 0; i < m.nodes_[v].ends.size(); ++i) {
                    if (j - m.nodes_[v].ends[i] >= 0)
                        ++cnt[j - m.nodes_[v].ends[i]];
                }
            }
            v = m.get_suff_link(v);
        }
    }
    for (int i = 0; i < cnt.size(); ++i) {
        if (cnt[i] == patterns.size()) {
            if (i + mask.length() - 1 < s.length()) {
                std::cout << i << "\n";
            }
        }
    }
}

int main() {
    std::string mask, s;
    std::cin >> mask >> s;
    print_mask_entries(mask, s);
}
