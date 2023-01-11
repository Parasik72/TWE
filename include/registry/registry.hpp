#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>

namespace TWE {
    template<typename T>
    class Registry {
    public:
        template<typename Instance = T>
        T* add(const std::string& key);
        T* add(const std::string& key, T* value);
        T* get(const std::string& key);
        bool has(const std::string& key);
        void erase(const std::string& key);
        void clean();
        [[nodiscard]] std::vector<std::string> getKeys();
        [[nodiscard]] std::vector<T*> getValues();
        [[nodiscard]] std::map<std::string, T*>& getSource();
    private:
        std::map<std::string, T*> _registry;
    };

    template<typename T>
    template<typename Instance>
    T* Registry<T>::add(const std::string& key) {
        auto item = _registry.find(key);
        if(item != _registry.end())
            return nullptr;
        Instance* instance = new Instance;
        _registry.insert({key, instance});
        return instance;
    }

    template<typename T>
    T* Registry<T>::add(const std::string& key, T* value) {
        auto item = _registry.find(key);
        if(item != _registry.end())
            return nullptr;
        _registry.insert({key, value});
        return value;
    }

    template<typename T>
    T* Registry<T>::get(const std::string& key) {
        if(_registry.empty())
            return nullptr;
        auto item = _registry.find(key);
        if(item == _registry.end())
            return nullptr;
        return item->second;
    }

    template<typename T>
    bool Registry<T>::has(const std::string& key) {
        if(_registry.empty())
            return false;
        auto item = _registry.find(key);
        if(item == _registry.end())
            return false;
        return true;
    }

    template<typename T>
    void Registry<T>::erase(const std::string& key) {
        auto item = _registry.find(key);
        if(item == _registry.end())
            return;
        _registry.erase(key);
    }

    template<typename T>
    void Registry<T>::clean() {
        _registry.clear();
    }

    template<typename T>
    std::vector<std::string> Registry<T>::getKeys() {
        std::vector<std::string> res;
        for(auto& [key, value] : _registry)
            res.push_back(key);
        return res;
    }

    template<typename T>
    std::vector<T*> Registry<T>::getValues() {
        std::vector<T*> res;
        for(auto& [key, value] : _registry)
            res.push_back(value);
        return res;
    }

    template<typename T>
    std::map<std::string, T*>& Registry<T>::getSource() { return _registry; }
}

#endif