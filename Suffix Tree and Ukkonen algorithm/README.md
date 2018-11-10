Two Suffix Tree
---------------

Given the strings s and t. Build a compressed suffix tree that contains all the suffixes of string s and string t. 
Find a tree that contains the minimum number of vertices. 

Number the vertices of the tree from 0 to n-1 in depth traversal order, bypassing the subtrees in the lexicographic sorting order of the outgoing edges. 
Use ASCII character codes to determine their order.

K-th common substring
-------------------
Two strings s, t and an integer k are given. Consider the set of all such non-empty strings that occur as substrings in s and t at the same time. 
Find the k-th in lexicographical order string from this set.
Print the required string or -1 if such does not exist.

The both problems solved by using **Ukkonen`s Algorithm** of compressed suffix tree building.
