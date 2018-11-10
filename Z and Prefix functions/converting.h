#include <algorithm>
#include <iostream>
#include <string>
#include <set>
#include <vector>

std::string build_str_from_zfunc(const std::vector<int> &z){
    if (z.empty())
        return "";
    std::string s = "a";
    int rem_zblock_len(0), zblock_ind(0), gen_ind(1);
    bool is_after_zblock(true);
    std::set<char> used;
    while(gen_ind < z.size()){
        if (z[gen_ind]){
            is_after_zblock = true;
            rem_zblock_len = z[gen_ind];
            zblock_ind = 0;

            while (rem_zblock_len){
                if (z[gen_ind] > rem_zblock_len){
                    used.insert(s[z[gen_ind]]);
                    zblock_ind = 0;
                    rem_zblock_len = z[gen_ind];
                }

                if (z[gen_ind] == rem_zblock_len)
                    used.insert(s[z[gen_ind]]);

                ++gen_ind;
                --rem_zblock_len;
                s += s[zblock_ind++];
            }
        }
        else {
            char current_symb = 'b';
            if (is_after_zblock) {
                is_after_zblock ^= is_after_zblock;
                while (used.find(current_symb) != used.end())
                    ++current_symb;
                used.clear();
            }
            ++gen_ind;
            s += current_symb;
        }
    }
    return s;
}

std::vector<int> build_pfunc_from_str(const std::string &s) {
    std::vector<int> p(s.length());
    p[0] = 0;
    for (size_t i = 1; i < s.length(); ++i){
        int last = p[i - 1];
        while (last > 0 && s[last] != s[i])
            last = p[last - 1];
        p[i] = last + (s[last] == s[i] ? 1 : 0);
    }
    return p;
}

std::vector<int> build_zfunc_from_pfunc(const std::vector<int> &p){
    std::vector<int> z(p.size());
    z[0] = z.size();
    for (size_t i = 1; i < z.size(); ++i){
        if (p[i])
            z[i + 1 - p[i]] = p[i];
    }
    size_t i(1);
    while (i < z.size()){
        size_t shift(i);
        if (z[i]){
            for (size_t j = 1; j < z[i]; ++j){
                if (z[i + j] > z[j])
                    break;
                else {
                    z[i + j] = std::min(z[j], int(z[i] - j));
                    shift = i + j;
                }
            }
        }
        i = shift + 1;
    }
    return z;
}

std::vector<int> build_zfunc_from_str(const std::string &s){
    return build_zfunc_from_pfunc(build_pfunc_from_str(s));
}

std::string build_str_from_pfunc(const std::vector<int> &p){
    return build_str_from_zfunc(build_zfunc_from_pfunc(p));
}

std::vector<int> build_pfunc_from_zfunc(const std::vector<int> &z){
    return build_pfunc_from_str(build_str_from_zfunc(z));
}
