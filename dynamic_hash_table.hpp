#ifndef DYNAMIC_HASH_TABLE_HPP
#define DYNAMIC_HASH_TABLE_HPP

#include "hash_table.hpp"
#include "prime_generator.hpp"
#include <vector>
#include <string>
#include <optional>
class DynamicHashSet : public HashSet {
public:
    DynamicHashSet(const std::string& collision_type, const std::vector<int>& params)
        : HashSet(collision_type, params) {}

    void insert(const std::string& key) {
        HashSet::insert({key, key});
        if (get_load() >= 0.5) {
            rehash();
        }
    }

private:
   void rehash() {
    capacity = PrimeGenerator::get_next_size();
    size = 0;
    auto old_chain_data = chain_data;
    auto old_linear_double_data = linear_double_data;
    find_storage_according_to_collision_type();

    if (collision_type == "Chain") {
        for (const auto& bucket : old_chain_data) {
            for (const auto& key : bucket) {
                HashSet::insert({key.first, key.second});
            }
        }
    } else {
        for (const auto& key : old_linear_double_data) {
            if (key.has_value()) {
                HashSet::insert({key->first, key->second});
            }
        }
    }
}

};

class DynamicHashMap : public HashMap<DynamicHashSet> {
public:
    DynamicHashMap(const std::string& collision_type, const std::vector<int>& params)
        : HashMap<DynamicHashSet>(collision_type, params) {}

    void insert(const std::pair<std::string, DynamicHashSet>& x) {
        HashMap<DynamicHashSet>::insert(x);
        if (get_load() >= 0.5) {
            rehash();
        }
    }

private:
    void rehash() {
        capacity = PrimeGenerator::get_next_size();
        size = 0;
        auto old_chain_data = chain_data;
        auto old_linear_double_data = linear_double_data;
        find_storage_according_to_collision_type();

        if (collision_type == "Chain") {
            for (const auto& bucket : old_chain_data) {
                for (const auto& kv : bucket) {
                    insert(kv);
                }
            }
        } else {
            for (const auto& kv : old_linear_double_data) {
                if (kv.has_value()) {
                    insert(*kv);
                }
            }
        }
    }
};

#endif