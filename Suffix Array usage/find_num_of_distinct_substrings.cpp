#include <iostream>
#include <string>
#include <utility>
#include <vector>

typedef long long LL;


template<size_t ALPHABET_SIZE, char END>
std::pair<std::vector<std::vector<size_t>>, size_t> find_zero_equivalence_class(const std::string &s,
                                                                                std::vector<size_t> &cnt,
                                                                                std::vector<LL> &perm) {
    for (char symb : s)
        ++cnt[symb - END];

    for (size_t i = 1; i < ALPHABET_SIZE; ++i)
        cnt[i] += cnt[i - 1];

    for (size_t i = 0; i < s.length(); ++i)
        perm[--cnt[s[i] - END]] = i;

    std::vector<std::vector<size_t >> eq_class(1, std::vector<size_t>(s.length()));
    eq_class[0][perm[0]] = 0;
    size_t classes = 1;
    for (size_t i = 1; i < s.length(); ++i) {
        classes += (s[perm[i]] != s[perm[i - 1]]) ? 1 : 0;
        eq_class[0][perm[i]] = classes - 1;
    }
    return std::make_pair(eq_class, classes);
}

size_t find_nonzero_equivalence_classes(const std::string &s,
                                        std::vector<std::vector<size_t>> &eq_class,
                                        std::vector<size_t> &cnt,
                                        std::vector<LL> &perm,
                                        size_t &classes) {
    std::vector<LL> new_perm(s.length());
    size_t ind;
    for (ind = 0; (1ll << ind) < s.length(); ++ind) {
        for (size_t i = 0; i < s.length(); ++i)
            new_perm[i] = (s.length() + perm[i] - (1 << ind)) % s.length();

        cnt.assign(classes, 0);
        for (size_t i = 0; i < s.length(); ++i)
            ++cnt[eq_class[ind][new_perm[i]]];

        for (size_t i = 1; i < classes; ++i)
            cnt[i] += cnt[i - 1];

        for (LL i = s.length() - 1; i > -1; --i)
            perm[--cnt[eq_class[ind][new_perm[i]]]] = new_perm[i];

        eq_class.emplace_back(s.length());
        eq_class[ind + 1][perm[0]] = 0;
        classes = 1;
        for (size_t i = 1; i < s.length(); ++i) {
            auto[cur, prev] = std::pair<LL, LL>{((perm[i] + (1 << ind)) % s.length()),
                                                ((perm[i - 1] + (1 << ind)) % s.length())};
            if (eq_class[ind][perm[i]] != eq_class[ind][perm[i - 1]] or eq_class[ind][cur] != eq_class[ind][prev])
                ++classes;
            eq_class[ind + 1][perm[i]] = classes - 1;
        }
    }
    return ind;
}

std::vector<size_t> find_lcp(const std::string &s,
                             const std::vector<std::vector<size_t>> &eq_class,
                             const std::vector<LL> &perm,
                             const size_t ind) {
    std::vector<size_t> lcp(s.length(), 0);
    for (size_t i = 1; i < s.length() - 1; ++i) {
        LL first = perm[i];
        LL second = perm[i + 1];
        for (LL j = ind - 1; j >= 0; --j)
            if (eq_class[j][first] == eq_class[j][second]) {
                lcp[i - 1] += (1 << j);
                first += (1 << j);
                second += (1 << j);
            }
    }
    return lcp;
}

//int(END) == int(c) - 1, where c is minimal alphabet symbol
template<size_t ALPHABET_SIZE = 27, char END = '`'>
long long find_distinct_substrings(std::string s) {
    s += END; //end of string

    std::vector<LL> perm(s.length());
    std::vector<size_t> c(ALPHABET_SIZE, 0);

    auto[eq_class, classes] = find_zero_equivalence_class<ALPHABET_SIZE, END>(s, c, perm);
    auto ind = find_nonzero_equivalence_classes(s, eq_class, c, perm, classes);
    auto lcp = find_lcp(s, eq_class, perm, ind);

    LL distinct_substr_qnt(0);

    for (size_t i = 1; i < s.length(); i++)
        distinct_substr_qnt += (s.length() - 1) - perm[i];

    for (size_t i = 0; i < s.length() - 2; i++)
        distinct_substr_qnt -= lcp[i];

    return distinct_substr_qnt;
}

int main() {
    std::string str;
    std::cin >> str;
    std::cout << find_distinct_substrings(str);
}
