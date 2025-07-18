#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <vector>
#include <string>
#include <optional>
#include <stdexcept>
#include <variant>

template <typename ValueType>
class HashTable {
protected:
    std::string collision_type;
    std::vector<int> params;
    int capacity;
    int size;
    double load_factor;
    std::vector<std::vector<std::pair<std::string, ValueType>>> chain_data;
    std::vector<std::optional<std::pair<std::string, ValueType>>> linear_double_data;

    void find_storage_according_to_collision_type() {
        if (collision_type == "Chain") {
            chain_data = std::vector<std::vector<std::pair<std::string, ValueType>>>(capacity);
            linear_double_data.clear();
        } else {
            linear_double_data = std::vector<std::optional<std::pair<std::string, ValueType>>>(capacity, std::nullopt);
            chain_data.clear();
        }
    }

    int hashing(const std::string& key) const {
        int z = params[0];
        int exp = 1;
        int hash_key = 0;
        for (char c : key) {
            int num = 0;
            if ('a' <= c && c <= 'z') {
                num = c - 'a';
            } else if ('A' <= c && c <= 'Z') {
                num = c - 'A' + 26;
            } else if ('0' <= c && c <= '9') {
                num = c - '0' + 52;
            }
            num *= exp;
            exp *= z;
            hash_key += num;
        }
        return hash_key % capacity;
    }

    int double_hash(const std::string& key) const {
        if (collision_type != "Double") return 1;
        int z = params[1];
        int c2 = params[2];
        int exp = 1;
        int hash_key = 0;
        for (char c : key) {
            int num = 0;
            if ('a' <= c && c <= 'z') {
                num = c - 'a';
            } else if ('A' <= c && c <= 'Z') {
                num = c - 'A' + 26;
            }
            num *= exp;
            exp *= z;
            hash_key += num;
        }
        hash_key = c2 - (hash_key % c2);
        return (hash_key == capacity) ? 1 : (hash_key != 0 ? hash_key : 1);
    }

public:
    HashTable(const std::string& collision_type_, const std::vector<int>& params_)
        : collision_type(collision_type_), params(params_), size(0), load_factor(0.5) {
        if (params.empty()) {
            throw std::invalid_argument("Params vector cannot be empty");
        }
        capacity = params.back();
        find_storage_according_to_collision_type();
    }

    virtual void insert(const std::pair<std::string, ValueType>& x) = 0;
    virtual std::optional<ValueType> find(const std::string& key) const = 0;
    virtual std::variant<int, std::pair<int, int>> get_slot(const std::string& key) const = 0;
    virtual std::string to_string() const = 0;

    double get_load() const {
        return static_cast<double>(size) / capacity;
    }

    int get_size() const {
        return size;
    }

    int get_capacity() const {
        return capacity;
    }
};

class HashSet : public HashTable<std::string> {
public:
    HashSet(const std::string& collision_type, const std::vector<int>& params)
        : HashTable<std::string>(collision_type, params) {}

    void insert(const std::pair<std::string, std::string>& x) override {
        if (collision_type == "Linear") {
            insert_util_1(x.first);
        } else if (collision_type == "Double") {
            insert_util_2(x.first);
        } else if (collision_type == "Chain") {
            insert_util_3(x.first);
        } else {
            throw std::invalid_argument("Invalid collision type");
        }
    }

    std::optional<std::string> find(const std::string& key) const override {
        if (collision_type == "Linear") {
            return search_util_1(key) ? std::optional<std::string>(key) : std::nullopt;
        } else if (collision_type == "Double") {
            return search_util_2(key) ? std::optional<std::string>(key) : std::nullopt;
        } else if (collision_type == "Chain") {
            return search_util_3(key) ? std::optional<std::string>(key) : std::nullopt;
        }
        return std::nullopt;
    }

    std::variant<int, std::pair<int, int>> get_slot(const std::string& key) const override {
        if (collision_type == "Chain") {
            int hash_key = hashing(key);
            for (size_t idx = 0; idx < chain_data[hash_key].size(); ++idx) {
                if (chain_data[hash_key][idx].first == key) {
                    return std::pair<int, int>{hash_key, static_cast<int>(idx)};
                }
            }
            return std::pair<int, int>{hash_key, -1};
        } else if (collision_type == "Linear") {
            int hash_key = hashing(key);
            while (linear_double_data[hash_key].has_value()) {
                if (linear_double_data[hash_key]->first == key) {
                    return hash_key;
                }
                hash_key = (hash_key + 1) % capacity;
            }
            return hash_key;
        } else if (collision_type == "Double") {
            int hash_key = hashing(key);
            int step = double_hash(key);
            while (linear_double_data[hash_key].has_value()) {
                if (linear_double_data[hash_key]->first == key) {
                    return hash_key;
                }
                hash_key = (hash_key + step) % capacity;
            }
            return hash_key;
        }
        throw std::invalid_argument("Invalid collision type");
    }

    std::string to_string() const override {
        std::vector<std::string> items;
        if (collision_type == "Linear" || collision_type == "Double") {
            for (const auto& item : linear_double_data) {
                items.push_back(item.has_value() ? item->first : "<EMPTY>");
            }
        } else if (collision_type == "Chain") {
            for (const auto& bucket : chain_data) {
                if (bucket.empty()) {
                    items.push_back("<EMPTY>");
                } else {
                    std::string aggregate;
                    for (size_t i = 0; i < bucket.size(); ++i) {
                        aggregate += bucket[i].first;
                        if (i < bucket.size() - 1) aggregate += " ; ";
                    }
                    items.push_back(aggregate);
                }
            }
        }
        return join(items, " | ");
    }

private:
    void insert_util_1(const std::string& key) {
        int hash_key = hashing(key);
        while (linear_double_data[hash_key].has_value()) {
            if (linear_double_data[hash_key]->first == key) {
                return;
            }
            hash_key = (hash_key + 1) % capacity;
        }
        linear_double_data[hash_key] = {key, key};
        size++;
    }

    void insert_util_2(const std::string& key) {
        int hash_key = hashing(key);
        int double_hash_value = double_hash(key);
        while (linear_double_data[hash_key].has_value()) {
            if (linear_double_data[hash_key]->first == key) {
                return;
            }
            hash_key = (hash_key + double_hash_value) % capacity;
        }
        linear_double_data[hash_key] = {key, key};
        size++;
    }

    void insert_util_3(const std::string& key) {
        int hash_key = hashing(key);
        for (const auto& kv : chain_data[hash_key]) {
            if (kv.first == key) return;
        }
        chain_data[hash_key].push_back({key, key});
        size++;
    }

    bool search_util_1(const std::string& key) const {
        int hash_key = hashing(key);
        while (linear_double_data[hash_key].has_value()) {
            if (linear_double_data[hash_key]->first == key) {
                return true;
            }
            hash_key = (hash_key + 1) % capacity;
        }
        return false;
    }

    bool search_util_2(const std::string& key) const {
        int hash_key = hashing(key);
        int step = double_hash(key);
        while (linear_double_data[hash_key].has_value()) {
            if (linear_double_data[hash_key]->first == key) {
                return true;
            }
            hash_key = (hash_key + step) % capacity;
        }
        return false;
    }

    bool search_util_3(const std::string& key) const {
        int hash_key = hashing(key);
        for (const auto& kv : chain_data[hash_key]) {
            if (kv.first == key) return true;
        }
        return false;
    }

    std::string join(const std::vector<std::string>& items, const std::string& delimiter) const {
        std::string result;
        for (size_t i = 0; i < items.size(); ++i) {
            result += items[i];
            if (i < items.size() - 1) result += delimiter;
        }
        return result;
    }
};

template <typename ValueType>
class HashMap : public HashTable<ValueType> {
public:
    HashMap(const std::string& collision_type, const std::vector<int>& params)
        : HashTable<ValueType>(collision_type, params) {}

    void insert(const std::pair<std::string, ValueType>& x) override {
        if (this->collision_type == "Linear") {
            insert_util_1(x);
        } else if (this->collision_type == "Double") {
            insert_util_2(x);
        } else if (this->collision_type == "Chain") {
            insert_util_3(x);
        } else {
            throw std::invalid_argument("Invalid collision type");
        }
    }

    std::optional<ValueType> find(const std::string& key) const override {
        if (this->collision_type == "Linear") {
            return search_util_1(key);
        } else if (this->collision_type == "Double") {
            return search_util_2(key);
        } else if (this->collision_type == "Chain") {
            return search_util_3(key);
        }
        return std::nullopt;
    }

    std::variant<int, std::pair<int, int>> get_slot(const std::string& key) const override {
        if (this->collision_type == "Chain") {
            int hash_key = this->hashing(key);
            for (size_t idx = 0; idx < this->chain_data[hash_key].size(); ++idx) {
                if (this->chain_data[hash_key][idx].first == key) {
                    return std::pair<int, int>{hash_key, static_cast<int>(idx)};
                }
            }
            return std::pair<int, int>{hash_key, -1};
        } else if (this->collision_type == "Linear") {
            int hash_key = this->hashing(key);
            while (this->linear_double_data[hash_key].has_value()) {
                if (this->linear_double_data[hash_key]->first == key) {
                    return hash_key;
                }
                hash_key = (hash_key + 1) % this->capacity;
            }
            return hash_key;
        } else if (this->collision_type == "Double") {
            int hash_key = this->hashing(key);
            int step = this->double_hash(key);
            while (this->linear_double_data[hash_key].has_value()) {
                if (this->linear_double_data[hash_key]->first == key) {
                    return hash_key;
                }
                hash_key = (hash_key + step) % this->capacity;
            }
            return hash_key;
        }
        throw std::invalid_argument("Invalid collision type");
    }

    std::string to_string() const override {
        std::vector<std::string> items;
        if (this->collision_type == "Linear" || this->collision_type == "Double") {
            for (const auto& item : this->linear_double_data) {
                items.push_back(item.has_value() ? "(" + item->first + "," + item->second.to_string() + ")" : "<EMPTY>");
            }
        } else if (this->collision_type == "Chain") {
            for (const auto& bucket : this->chain_data) {
                if (bucket.empty()) {
                    items.push_back("<EMPTY>");
                } else {
                    std::string aggregate;
                    for (size_t i = 0; i < bucket.size(); ++i) {
                        aggregate += "(" + bucket[i].first + "," + bucket[i].second.to_string() + ")";
                        if (i < bucket.size() - 1) aggregate += " ; ";
                    }
                    items.push_back(aggregate);
                }
            }
        }
        return join(items, " | ");
    }

private:
    void insert_util_1(const std::pair<std::string, ValueType>& x) {
        int hash_key = this->hashing(x.first);
        while (this->linear_double_data[hash_key].has_value()) {
            if (this->linear_double_data[hash_key]->first == x.first) {
                this->linear_double_data[hash_key] = x;
                return;
            }
            hash_key = (hash_key + 1) % this->capacity;
        }
        this->linear_double_data[hash_key] = x;
        this->size++;
    }

    void insert_util_2(const std::pair<std::string, ValueType>& x) {
        int hash_key = this->hashing(x.first);
        int step = this->double_hash(x.first);
        while (this->linear_double_data[hash_key].has_value()) {
            if (this->linear_double_data[hash_key]->first == x.first) {
                this->linear_double_data[hash_key] = x;
                return;
            }
            hash_key = (hash_key + step) % this->capacity;
        }
        this->linear_double_data[hash_key] = x;
        this->size++;
    }

    void insert_util_3(const std::pair<std::string, ValueType>& x) {
        int hash_key = this->hashing(x.first);
        for (size_t i = 0; i < this->chain_data[hash_key].size(); ++i) {
            if (this->chain_data[hash_key][i].first == x.first) {
                this->chain_data[hash_key][i] = x;
                return;
            }
        }
        this->chain_data[hash_key].push_back(x);
        this->size++;
    }

    std::optional<ValueType> search_util_1(const std::string& key) const {
        int hash_key = this->hashing(key);
        while (this->linear_double_data[hash_key].has_value()) {
            if (this->linear_double_data[hash_key]->first == key) {
                return this->linear_double_data[hash_key]->second;
            }
            hash_key = (hash_key + 1) % this->capacity;
        }
        return std::nullopt;
    }

    std::optional<ValueType> search_util_2(const std::string& key) const {
        int hash_key = this->hashing(key);
        int step = this->double_hash(key);
        while (this->linear_double_data[hash_key].has_value()) {
            if (this->linear_double_data[hash_key]->first == key) {
                return this->linear_double_data[hash_key]->second;
            }
            hash_key = (hash_key + step) % this->capacity;
        }
        return std::nullopt;
    }

    std::optional<ValueType> search_util_3(const std::string& key) const {
        int hash_key = this->hashing(key);
        for (const auto& kv : this->chain_data[hash_key]) {
            if (kv.first == key) {
                return kv.second;
            }
        }
        return std::nullopt;
    }

    std::string join(const std::vector<std::string>& items, const std::string& delimiter) const {
        std::string result;
        for (size_t i = 0; i < items.size(); ++i) {
            result += items[i];
            if (i < items.size() - 1) result += delimiter;
        }
        return result;
    }
};

#endif