#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include "dynamic_hash_table.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>

std::vector<int> get_primes(int start = 1000, int end = 100000) {
    std::vector<bool> is_prime(end + 1, true);
    is_prime[0] = is_prime[1] = false;
    for (int i = 2; i * i <= end; ++i) {
        if (is_prime[i]) {
            for (int j = i * i; j <= end; j += i) {
                is_prime[j] = false;
            }
        }
    }
    std::vector<int> prime_sizes;
    for (int i = end; i >= start; --i) {
        if (is_prime[i]) prime_sizes.push_back(i);
    }
    return prime_sizes;
}

class DigitalLibrary {
public:
    virtual std::vector<std::string> distinct_words(const std::string& book_title) = 0;
    virtual int count_distinct_words(const std::string& book_title) = 0;
    virtual std::vector<std::string> search_keyword(const std::string& keyword) = 0;
    virtual void print_books() = 0;
    virtual void add_book(const std::string& book_title, const std::vector<std::string>& text) = 0;
    virtual ~DigitalLibrary() = default;
};

class MuskLibrary : public DigitalLibrary {
private:
    std::vector<std::pair<std::string, std::vector<std::string>>> lib;

    static bool comp1(const std::string& a, const std::string& b) {
        return a < b;
    }

    static bool comp2(const std::pair<std::string, std::vector<std::string>>& a,
                      const std::pair<std::string, std::vector<std::string>>& b) {
        return a.first < b.first;
    }

    std::vector<std::string> merge_sort(std::vector<std::string> arr, bool (*compare)(const std::string&, const std::string&)) const {
        if (arr.size() <= 1) return arr;
        size_t mid = arr.size() / 2;
        std::vector<std::string> l1(arr.begin(), arr.begin() + mid);
        std::vector<std::string> l2(arr.begin() + mid, arr.end());
        l1 = merge_sort(l1, compare);
        l2 = merge_sort(l2, compare);
        return merge(l1, l2, compare);
    }

    std::vector<std::pair<std::string, std::vector<std::string>>> merge_sort(
        std::vector<std::pair<std::string, std::vector<std::string>>> arr,
        bool (*compare)(const std::pair<std::string, std::vector<std::string>>&, const std::pair<std::string, std::vector<std::string>>&)) const {
        if (arr.size() <= 1) return arr;
        size_t mid = arr.size() / 2;
        std::vector<std::pair<std::string, std::vector<std::string>>> l1(arr.begin(), arr.begin() + mid);
        std::vector<std::pair<std::string, std::vector<std::string>>> l2(arr.begin() + mid, arr.end());
        l1 = merge_sort(l1, compare);
        l2 = merge_sort(l2, compare);
        return merge(l1, l2, compare);
    }

    template<typename T>
    std::vector<T> merge(const std::vector<T>& l1, const std::vector<T>& l2, bool (*compare)(const T&, const T&)) const {
        std::vector<T> result;
        size_t i = 0, j = 0;
        while (i < l1.size() && j < l2.size()) {
            if (compare(l1[i], l2[j])) {
                result.push_back(l1[i++]);
            } else {
                result.push_back(l2[j++]);
            }
        }
        result.insert(result.end(), l1.begin() + i, l1.end());
        result.insert(result.end(), l2.begin() + j, l2.end());
        return result;
    }

    std::vector<std::string> remove_duplicates(std::vector<std::string> arr) const {
        if (arr.empty()) return arr;
        std::vector<std::string> ans;
        ans.push_back(arr[0]);
        for (size_t i = 1; i < arr.size(); ++i) {
            if (ans.back() != arr[i]) {
                ans.push_back(arr[i]);
            }
        }
        return ans;
    }

public:
    MuskLibrary(const std::vector<std::string>& book_titles, const std::vector<std::vector<std::string>>& texts) {
        std::vector<std::vector<std::string>> sorted_texts = texts;
        for (auto& text : sorted_texts) {
            text = merge_sort(text, comp1);
            text = remove_duplicates(text);
        }
        for (size_t i = 0; i < book_titles.size(); ++i) {
            lib.emplace_back(book_titles[i], sorted_texts[i]);
        }
        lib = merge_sort(lib, comp2);
    }

    void add_book(const std::string&, const std::vector<std::string>&) override {}

    std::vector<std::string> distinct_words(const std::string& book_title) override {
        int i = 0, j = lib.size() - 1;
        while (i <= j) {
            int mid = i + (j - i) / 2;
            if (lib[mid].first == book_title) {
                return lib[mid].second;
            } else if (lib[mid].first < book_title) {
                i = mid + 1;
            } else {
                j = mid - 1;
            }
        }
        return {};
    }

    int count_distinct_words(const std::string& book_title) override {
        auto words = distinct_words(book_title);
        return words.size();
    }

    std::vector<std::string> search_keyword(const std::string& keyword) override {
        std::vector<std::string> ans;
        for (const auto& [book, text] : lib) {
            if (std::binary_search(text.begin(), text.end(), keyword)) {
                ans.push_back(book);
            }
        }
        return ans;
    }

    void print_books() override {
        for (const auto& [book, text] : lib) {
            std::ostringstream oss;
            for (size_t i = 0; i < text.size(); ++i) {
                oss << text[i];
                if (i < text.size() - 1) oss << " | ";
            }
            std::cout << book << ": " << oss.str() << std::endl;
        }
    }
};

#endif
