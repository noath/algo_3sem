#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>



class SuffixTree {
    typedef long long TNodeld;
private: 
    struct Node {
        Node(TNodeld l, TNodeld r, TNodeld p) : left(l), right(r), parent(p), link(-1) {}

        TNodeld left, right, parent, link;
        std::unordered_map<char, TNodeld> next;
    };

    //location of imaginary pointer after read any string begining from root
    struct Position {
        Position(TNodeld v, TNodeld l) : node(v), len(l) {}

        TNodeld node, len;
    };

    std::vector<Node> tree_;
    std::string s_;


    bool dfs(TNodeld v, TNodeld &current, TNodeld order, Position &final_pos, std::vector<char> &alphabet,
             std::vector<bool> &intersection) {
        for (short i = 0; i < alphabet.size(); ++i) {
            if (tree_[v].next.count(alphabet[i]) == 0)continue;
            if (!intersection[tree_[v].next[alphabet[i]]])
                continue;
            TNodeld up = current - order + length(tree_[v].next[alphabet[i]]);
            if (up + 1 > 0) {
                final_pos = Position(tree_[v].next[alphabet[i]], up);
                return true;
            }
            current += length(tree_[v].next[alphabet[i]]);
            if (dfs(tree_[v].next[alphabet[i]], current, order, final_pos, alphabet, intersection))
                return true;
        }
        return false;
    }

public:
    TNodeld length(TNodeld v) {
        return tree_[v].right - tree_[v].left;
    }

    TNodeld addEdgeToParent(TNodeld l, TNodeld r, TNodeld parent) {
        tree_.emplace_back(l, r, parent);
        tree_[parent].next[s_[l]] = tree_.size() - 1;
        return tree_.size() - 1;
    }

    TNodeld splitEdge(Position pos) {
        auto[v, up, down] = std::array<TNodeld, 3>{pos.node, pos.len, length(pos.node) - pos.len};
        if (!up)
            return v;
        if (!down)
            return tree_[v].parent;

        TNodeld divider = addEdgeToParent(tree_[v].left, tree_[v].left + down, tree_[v].parent);
        tree_[v].left += down;
        tree_[v].parent = divider;
        tree_[divider].next[s_[tree_[v].left]] = v;

        return divider;
    }

    Position readSymbol(Position pos, char c) {
        auto[v, up] = std::array<TNodeld, 2>{pos.node, pos.len};
        if (up > 0) {
            return (s_[tree_[v].right - up] == c) ? Position(v, up - 1) : Position(-1, -1);
        }
        TNodeld next_node = tree_[v].next.count(c) ? tree_[v].next[c] : -1;
        return (next_node != -1) ? Position(next_node, length(next_node) - 1) : Position(-1, -1);
    }

    Position descent(TNodeld v, TNodeld l, TNodeld r) {
        if (l == r)
            return Position(v, 0);
        while (true) {
            v = tree_[v].next[s_[l]];
            if (length(v) > r - l - 1)
                return Position(v, length(v) - r + l);
            l += length(v);
        }
    }

    TNodeld link(TNodeld v) {
        if (tree_[v].link == -1)
            tree_[v].link = splitEdge(
                    descent(link(tree_[v].parent), tree_[v].left + (tree_[v].parent ? 0 : 1), tree_[v].right));
        return tree_[v].link;
    }

    Position addSymbolToTree(Position pos, TNodeld i) {
        while (true) {
            Position npos = readSymbol(pos, s_[i]);
            if (npos.node != -1)
                return npos;

            TNodeld divider = splitEdge(pos);
            addEdgeToParent(i, s_.length(), divider);
            pos = Position(link(divider), 0);
            if (!divider)
                return pos;
        }
    }

    void makeTree(std::string str) {
        tree_.clear();
        s_ = std::move(str);
        tree_.emplace_back(-1, -1, -1);
        tree_[0].link = 0;

        Position pos(0, 0);
        for (int i = 0; i < s_.length(); ++i) {
            pos = addSymbolToTree(pos, i);
        }
    }

    void makeTreeNaively(std::string str) {
        tree_.clear();
        s_ = std::move(str);
        tree_.emplace_back(-1, -1, -1);

        for (int i = 0; i < s_.length(); ++i) {
            Position pos(0, 0);
            for (int j = i; j < s_.length(); ++j) {
                Position npos = readSymbol(pos, s_[j]);
                if (npos.node != -1) {
                    pos = npos;
                } else {
                    TNodeld mid = splitEdge(pos);
                    addEdgeToParent(j, s_.length(), mid);
                    break;
                }
            }
        }
    }

    void printRealTreeEdges() {
        using std::cout;
        using std::endl;
        cout << tree_.size() << endl;
        for (TNodeld i = 1; i < tree_.size(); ++i) {
            cout << tree_[i].parent << " " << i << " " << tree_[i].left << " " << tree_[i].right << endl;
        }
    }

    std::array<bool, 2>
    findIntersection(TNodeld v, std::vector<char> &alphabet, int end1, int end2, std::vector<bool> &intersection) {
        auto curr = std::array<bool, 2>{false, false};
        for (int i = 0; i < alphabet.size(); ++i) {
            if (!tree_[v].next.count(alphabet[i]))
                continue;
            TNodeld next_node = tree_[v].next[alphabet[i]];
            auto next = findIntersection(next_node, alphabet, end1, end2, intersection);
            if (tree_[next_node].left <= end1 and tree_[next_node].right > end1) curr[0] = true;
            if (tree_[next_node].left > end1 and tree_[next_node].right == end2) curr[1] = true;
            curr[0] = next[0] ? next[0] : curr[0];
            curr[1] = next[1] ? next[1] : curr[1];
        }
        intersection[v] = (curr[0] and curr[1]);
        return curr;
    }

    std::string findCommonSubstrByOrder(TNodeld order, int end1, int end2) {
        TNodeld current(0LL);
        Position answer_pos(-1, -1);
        std::vector<char> alphabet;
        std::vector<bool> intersection(tree_.size());
        alphabet.push_back('#');
        alphabet.push_back('$');
        for (char c = 'a'; c < 'z' + 1; ++c)
            alphabet.push_back(c);
        findIntersection(0, alphabet, end1, end2, intersection);
        bool is_empty = !dfs(0, current, order, answer_pos, alphabet, intersection);
        if (is_empty)
            return "-1";
        std::string substr;
        auto[node, up] = std::array<TNodeld, 2>{answer_pos.node, answer_pos.len};
        while (node > 0) {
            if (up - length(node) == 0) {
                node = tree_[node].parent;
                up = 0;
            }
            if (node < 1)
                break;
            ++up;
            substr += s_[tree_[node].right - up];
        }
        std::reverse(substr.begin(), substr.end());
        return substr;
    }
};

int main() {
    SuffixTree T;
    std::string s, t;
    long long k;
    std::cin >> s >> t >> k;
    T.makeTree(s + '$' + t + '#');
    std::cout << T.findCommonSubstrByOrder(k, s.length(), s.length() + t.length() + 2);
}
