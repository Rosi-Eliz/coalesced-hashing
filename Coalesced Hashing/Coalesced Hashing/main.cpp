//
//  main.cpp
//  Coalesced Hashing
//
//  Created by Rosi-Eliz Dzhurkova on 22.10.20.
//

#include <iostream>
#include <unordered_map>
#include "ADS_Set.h"
#include <set>
#include <vector>
using namespace std;

//template <typename T>
//struct Key{
//
//};

struct Person {
    string first, last;
    Person() {}
    Person(string f, string l)
    {
        first = f;
        last = l;
    }
//
    bool operator==(const Person& p) const
    {
        return first == p.first && last == p.last;
    }
};
  
class MyHashFunction {
public:
  
    // We use predfined hash functions of strings
    // and define our hash function as XOR of the
    // hash values.
    size_t operator()(const Person& p) const
    {
        return (hash<string>()(p.first)) ^ (hash<string>()(p.last));
    }
};

class MyCompare {
public:
  
     bool operator()(const Person &lhs, const Person &rhs) const
    {
        return true;
    }
};

int main(int argc, const char * argv[]) {
//    ADS_set<int> set {1,2,3,4};
//    vector<int> vec {1,2,3};
//    vector<int> vec2(vec.end(), vec.end());
//
//    ADS_set<int> setd;
    ADS_set<int> set1 {5,6,7,8,9};
    
    
    set1.insert({1, 2, 3, 5, 6, 7, 9, 11});
    
    cout<<set1.count(2);
    
//    auto itS = set1.begin();
//    auto itE = set1.end();
//
//    for(auto &a : set1) {
//        cout<< a<<endl;
//    }
    
//    while (itS != itE) {
//        cout<<(*itS)<<endl;
//        itS++;
//    }
    

//    swap(set, set1);
//    auto b = set == set1;
//
//    auto f = set.end();
//    HashMap<string, int> h;
//    h.insert("a", 1);
//    h.insert("b", 2);
//    h.insert("c", 3);
//    h.insert("d", 4);
//    auto z = h.begin();
//    ++z;
//    ++z;
//    z++;
//    auto op = *z;
//    bool a1 = h.contains("a");
//    bool a2 = h.contains("cz");
//    int r = h["b"];
//    int r1 = h["bz"];
//    h["bz"] += 4;
//    r1 = h["bz"];
//    size_t c = h.size();
//    h.erase("b");
//    h.erase("sds");
//    c = h.size();
    
    return 0;
}
