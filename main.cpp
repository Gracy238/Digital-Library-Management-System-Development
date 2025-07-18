#include "library.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <map>

void check_lib(DigitalLibrary* lib, const std::vector<std::vector<std::string>>& unique_words,
               const std::map<std::string, std::vector<std::string>>& word_to_books) {
    if (lib->distinct_words("book1") == unique_words[0] && lib->distinct_words("book2") == unique_words[1]) {
        std::cout << "DISTINCT WORDS CORRECT!" << std::endl;
    } else {
        std::cout << "DISTINCT WORDS FAILED!" << std::endl;
    }

    if (lib->count_distinct_words("book1") == static_cast<int>(unique_words[0].size()) &&
        lib->count_distinct_words("book2") == static_cast<int>(unique_words[1].size())) {
        std::cout << "COUNT DISTINCT WORDS CORRECT!" << std::endl;
    } else {
        std::cout << "COUNT DINSTINCT WORDS FAILED!" << std::endl;
    }

    std::string word = "book";
    if (lib->search_keyword(word) == word_to_books.at(word)) {
        std::cout << "SEARCH KEYWORD CORRECT!" << std::endl;
    } else {
        std::cout << "SEARCH KEYWORD FAILED!" << std::endl;
    }
    std::cout << "\n\n";
}

int main() {
    std::vector<std::string> book_titles = {"book1", "book2"};
    std::vector<std::vector<std::string>> texts = {
        {"The", "name", "of", "this", "book", "contains", "a", "number"},
        {"You", "can", "name", "this", "book", "anything"}
    };

    std::vector<std::vector<std::string>> unique_words;
    for (const auto& text : texts) {
        std::vector<std::string> unique;
        for (const auto& word : text) {
            if (std::find(unique.begin(), unique.end(), word) == unique.end()) {
                unique.push_back(word);
            }
        }
        std::sort(unique.begin(), unique.end());
        unique_words.push_back(unique);
    }

    std::map<std::string, std::vector<std::string>> word_to_books;
    for (size_t i = 0; i < book_titles.size(); ++i) {
        for (const auto& word : texts[i]) {
            word_to_books[word].push_back(book_titles[i]);
        }
    }

    PrimeGenerator::set_primes(get_primes());

    // Check Musk
    auto start = std::chrono::high_resolution_clock::now();
    MuskLibrary musk_lib(book_titles, texts);
    auto end = std::chrono::high_resolution_clock::now();
    auto musk_time = std::chrono::duration<double>(end - start).count();
    std::cout << "Musk Library sorting took " << musk_time << "s" << std::endl;
    std::cout << "Checking Library functions for Musk:" << std::endl;
    check_lib(&musk_lib, unique_words, word_to_books);

    JGBLibrary jobs_lib("Jobs", {10, 29});
    JGBLibrary gates_lib("Gates", {10, 37});
    JGBLibrary bezos_lib("Bezos", {10, 37, 7, 13});

    for (auto& [lib, name] : std::vector<std::pair<JGBLibrary*, std::string>>{
             {&jobs_lib, "Jobs"}, {&gates_lib, "Gates"}, {&bezos_lib, "Bezos"}}) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < book_titles.size(); ++i) {
            lib->add_book(book_titles[i], texts[i]);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto time_taken = std::chrono::duration<double>(end - start).count();
        std::cout << name << " Library took " << time_taken << "s" << std::endl;
        std::cout << "Checking Library Functions for " << name << ": " << std::endl;
        check_lib(lib, unique_words, word_to_books);
    }

    return 0;
}