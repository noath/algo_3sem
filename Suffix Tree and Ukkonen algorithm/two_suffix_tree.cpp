#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>

class SuffixTree {
private:
    struct Node {
        Node(int l, int r, int p) : left(l), right(r), parent(p), link(-1) {}

        int left, right, parent, link;
        std::unordered_map<char, int> next;
    };

    //location of imaginary pointer after read any string begining from root
    struct Position {
        Position(int v, int l) : node(v), len(l) {}

        int node, len;
    };

    std::vector<Node> tree_;
    std::string s_;

    void dfs(int v, std::vector<bool> &used, std::vector<int> &new_numeration, std::vector<int> &new_parent_num) {
        used[v] = true;
        int new_ind = new_numeration.size();
        new_numeration.push_back(v);
        if (tree_[v].next.count('#')) {
            new_parent_num[tree_[v].next['#']] = new_ind;
            dfs(tree_[v].next['#'], used, new_numeration, new_parent_num);
        }
        if (tree_[v].next.count('$')) {
            new_parent_num[tree_[v].next['$']] = new_ind;
            dfs(tree_[v].next['$'], used, new_numeration, new_parent_num);
        }
        for (char c = 'a'; c < 'z' + 1; ++c) {
            if (tree_[v].next.count(c)) {
                new_parent_num[tree_[v].next[c]] = new_ind;
                dfs(tree_[v].next[c], used, new_numeration, new_parent_num);
            }
        }
    }

public:
    int length(int v) {
        return tree_[v].right - tree_[v].left;
    }

    int addEdgeToParent(int l, int r, int parent) {
        tree_.emplace_back(l, r, parent);
        tree_[parent].next[s_[l]] = tree_.size() - 1;
        return tree_.size() - 1;
    }

    int splitEdge(Position pos) {
        auto [v, up, down] = std::array<int, 3>{pos.node, pos.len, length(pos.node) - pos.len};
        if (!up)
            return v;
        if (!down)
            return tree_[v].parent;

        int divider = addEdgeToParent(tree_[v].left, tree_[v].left + down, tree_[v].parent);
        tree_[v].left += down;
        tree_[v].parent = divider;
        tree_[divider].next[s_[tree_[v].left]] = v;

        return divider;
    }

    Position readSymbol(Position pos, char c) {
        auto [v, up] = std::array<int, 2>{pos.node, pos.len};
        if (up > 0) {
            return (s_[tree_[v].right - up] == c) ? Position(v, up - 1) : Position(-1, -1);
        }
        int next_node = tree_[v].next.count(c) ? tree_[v].next[c] : -1;
        return (next_node != -1) ? Position(next_node, length(next_node) - 1) : Position(-1, -1);
    }

    Position descent(int v, int l, int r) {
        if (l == r)
            return Position(v, 0);
        while (true) {
            v = tree_[v].next[s_[l]];
            if (length(v) > r - l - 1)
                return Position(v, length(v) - r + l);
            l += length(v);
        }
    }

    int link(int v) {
        if (tree_[v].link == -1)
            tree_[v].link = splitEdge(
                    descent(link(tree_[v].parent), tree_[v].left + (tree_[v].parent ? 0 : 1), tree_[v].right));
        return tree_[v].link;
    }

    Position addSymbolToTree(Position pos, int i) {
        while (true) {
            Position npos = readSymbol(pos, s_[i]);
            if (npos.node != -1)
                return npos;

            int divider = splitEdge(pos);
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
                    int mid = splitEdge(pos);
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
        for (int i = 1; i < tree_.size(); ++i) {
            cout << tree_[i].parent << " " << i << " " << tree_[i].left << " " << tree_[i].right << endl;
        }
    }

    void printNumeratedTree(int septum) {
        std::vector<bool> used(tree_.size(), false);
        std::vector<int> new_numeration;
        std::vector<int> new_parent_num(tree_.size());
        dfs(0, used, new_numeration, new_parent_num);
        printf("%d\n", tree_.size());
        for (int i = 1; i < new_numeration.size(); ++i) {
            Node v = tree_[new_numeration[i]];
            bool after_septum = (v.left >= septum && v.right > septum);
            printf("%d %d %d ", new_parent_num[new_numeration[i]],
                   after_septum ? 1 : 0,
                   v.left - (v.left >= septum ? septum : 0));
            if (after_septum)
                printf("%d\n", v.right - septum);
            else
                printf("%d\n", v.right > septum ? septum : v.right);
        }
    }
};

int main() {
    SuffixTree T;
    std::string s, t;
    std::cin >> s >> t;
    T.makeTree(s + t);
    T.printNumeratedTree(s.length());
}
