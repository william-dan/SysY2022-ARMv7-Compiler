#ifndef __TABLE_HPP
#define __TABLE_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <deque>


namespace TABLE {

template <typename T> class SymbolTable {
private:
    std::unordered_map<std::string, T> map;
    std::deque<std::pair<std::string, std::pair<T, bool>>> deq;

public:
    SymbolTable() {}
    ~SymbolTable() {}

    void enter(std::string, T);
    T find(std::string);
    bool exist(std::string);
    void beginScope(T useless);  // anything
    void endScope();
    std::pair<std::string, T> pop();
    auto begin() -> decltype(deq.begin()) { return deq.begin(); }
    auto end() -> decltype(deq.end()) { return deq.end(); }
};

}

template <typename T>
void TABLE::SymbolTable<T>::enter(std::string key, T value) {
    if (this->map.count(key))
        this->deq.push_back(std::make_pair(key, std::make_pair(map[key], true)));
    else
        this->deq.push_back(std::make_pair(key, std::make_pair(map[key], false)));
    map[key] = value;
}

template <typename T> bool TABLE::SymbolTable<T>::exist(std::string key) {
    return map.find(key) != map.end();
}

template <typename T> T TABLE::SymbolTable<T>::find(std::string key) {
    return map.find(key)->second;
}

template <typename T>
std::pair<std::string, T> TABLE::SymbolTable<T>::pop() {
    assert(deq.size());
    std::pair<std::string, T> res = *map.find(deq.back().first);
    if (deq.back().second.second)
        map[deq.back().first] = deq.back().second.first;
    else
        map.erase(deq.back().first);
    deq.pop_back();
    return res;
}

template <typename T> void TABLE::SymbolTable<T>::beginScope(T useless) {
    this->enter(std::string("_SCOPE_"), useless);
}

template <typename T> void TABLE::SymbolTable<T>::endScope() {
    while ((this->pop().first) != std::string("_SCOPE_"));
}

#endif // __TABLE_H
