//
// Created by sanya on 8/16/18.
//

#ifndef TEXTGEN_DICTIONARY_H
#define TEXTGEN_DICTIONARY_H

#include <iostream>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/storage.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <fstream>
#include <queue>
#include <codecvt>
#include <unordered_map>
#include "hypercube.h"

namespace ublas = boost::numeric::ublas;
namespace po = boost::program_options;

typedef std::map<std::string, int> Dict;
typedef std::vector<std::string> Words_array;
typedef std::vector<int> Words_probability;
// typedef ublas::compressed_vector<int> CVector;
// typedef ublas::compressed_vector<CVector> Matrix2D;
// typedef ublas::compressed_vector<Matrix2D> Matrix3D;
// typedef ublas::compressed_vector<Matrix3D> Matrix4D;
typedef std::vector<Words_array> Sentences_array;


struct Stats {
    size_t words_number = 0;           // Number of all words in the analysed text
    size_t unique_words_number = 0;    // Number of unique words in the analysed text
    size_t max_number = 0;             // Max number of one unique word found in the analysed text
    std::string max_word;
    std::map<int, int> sentence_len;       // number of words in sentences depending of sentence length
    size_t matrix_size = 0;
    size_t matrix_rows = 0;
    size_t matrix_cols = 0;

    void print_stats() {
        /*
         *     int words_number = 0;           // Number of all words in the analysed text
        int unique_words_number = 0;    // Number of all unique words in the analysed text
        int max_number = 0;             // Max number of one unique word found in the analysed text
        std::map<int, int> sentence_len;       // number of words in sentences depending of sentence length
         */
        std::cout << "Statistics:" << std::endl
             << words_number << ": Number of all words in the text" << std::endl
             << unique_words_number << ": Number of unique words in the text" << std::endl
             << max_number << ": Max number of one unique word found in the text. The word is: " << max_word << std::endl
             << matrix_size << ": matrix size" << std::endl
             << matrix_rows << ": matrix rows" << std::endl
             << matrix_cols << ": matrix cols" << std::endl << std::endl
             << "Number of words in sentences depending of sentence length: <words : sentences>" << std::endl;
        for (const auto & t : sentence_len) {
            std::cout << t.first << " : "
                      << t.second << std::endl;
        }
    }
} stats;


class Dictionary {
    Dict dictionary;
    Words_array words_array;
    Words_probability words_prob_array;
    Sentences_array sentences;
    std::string text;
    std::mt19937 gen { std::random_device()() };

    bool read_file(const std::string & file_name) {
        std::ifstream ifs(file_name);
        if( ! ifs.is_open()) {
            std::cerr << "can't open input file: " << file_name << std::endl;
            return false;
        }
        text.assign((std::istreambuf_iterator<char>(ifs)),
                   std::istreambuf_iterator<char>());
        return true;
    }


    void fill_dictionary(const std::string & s) {

        int number_of_words = 0;
        typedef boost::tokenizer<> tokenizer;
        tokenizer tok{s};

        Words_array words;

        for (const auto &t : tok) {
            number_of_words++;
            words.push_back(t);
//        std::cout << t << '\n';
            // add a new pair <word, number of this word in the text>
            auto ret = dictionary.insert({t,1});
            // if the word is already in the dictionary increase its number
            if(ret.second == false) {
                ret.first->second++;
            }
        }

        if( ! words.empty()) {
            sentences.push_back(words);
        }

        if(number_of_words > 0) {
            stats.words_number += number_of_words;
            // add a new pair <number of words in sentence, number of sentences with this number of words>
            auto ret = stats.sentence_len.insert({number_of_words, 1});
            // if this number of words is already in the dictionary increase the number sentences
            if (ret.second == false) {
                ret.first->second++;
            }
        }
    }

public:

    template< class InputIt>
    size_t get_prob(InputIt first, InputIt last) {
        std::discrete_distribution<> dd(first, last); // инициализируем контейнер для генерации числа на основе распределения вероятности
        size_t res = static_cast<size_t>(dd(gen));
        return res;
    };


    // Get most probable length of sentence based on collected info about all lengths of sentences in the analyzed text
    int get_len_of_sentence() {
        std::vector<std::pair<int,int>> num_len_of_sentences;
        std::vector<int> nums;

        for (const auto & t : stats.sentence_len) {
            num_len_of_sentences.push_back({t.second, t.first});
            nums.push_back(t.second);
        }

        size_t index_in_nums = get_prob(nums.begin(), nums.end());
        //    cout << endl << "index_in_nums=" << index_in_nums << endl;

        int sentence_length = num_len_of_sentences[index_in_nums].second;
        //    cout << endl << "sentence_length=" << sentence_length << endl;

        if(sentence_length < 2) {
            sentence_length = 2;
        }

        return sentence_length;
    }

    const std::string & get_word_by_index(size_t index) const {
        return words_array[index];
    }

    std::string & get_word(std::string & word, size_t * word_index, Words_probability & word_prob) {
        auto index = get_prob(word_prob.begin() + 1, word_prob.end()) + 1;
        *word_index = index;
        word = get_word_by_index(*word_index);
        return word;
    }

    const Words_array & get_words_array() const {
        return words_array;
    }
    const Sentences_array & get_sentences() const {
        return sentences;
    }

    size_t get_word_index(const std::string & word) {
        return static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), word) - words_array.begin());
    }

    void print() {
        int word_num = 0, i = 1;
        std::cout << " words_array [0] = " << words_array[0] << " words_prob_array [0] = " << words_prob_array[0] << std::endl;
        for(const auto & p : dictionary) {
            std::cout << p.first << " " << p.second << " words_array [" << i << "] = " << words_array[i] << " words_prob_array [" << i << "] = " << words_prob_array[i] << std::endl;
            word_num += p.second;
            ++i;
        }
        std::cout << "Unique words: " << dictionary.size() << std::endl;
        std::cout << "Total words: " << word_num << std::endl;

//        std::cout << "Print words array" << std::endl;
//        for(auto i = 0; i < words_array.size(); ++i) {
//            std::cout << i << " " << words_array[i] << std::endl;
//        }

    }

    Dictionary(const std::string & file_name) {

        read_file(file_name);

        typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
        boost::char_separator<char> sep(".?!;");
        tokenizer tok{text, sep};
        for (auto &t : tok) {
            // std::cout << t << '\n';
            fill_dictionary(t);
        }

        // free memory of the unneeded text
        text.clear();
        text.shrink_to_fit();

        stats.unique_words_number = dictionary.size();


        // Add one item to start count from 1
        // it needs to synchronize with the hypercube data
        // where zero lines contain first and last words so actual probability arrays start from 1.
        words_array.reserve(stats.unique_words_number + 1);
        words_prob_array.reserve(stats.unique_words_number + 1);
        words_array.push_back(std::string());
        words_prob_array.push_back(0);


        // find the most frequent word in the text and fill the stats
        for(const auto &pair : dictionary ) {
            if(stats.max_number < pair.second) {
                stats.max_number = pair.second;
                stats.max_word = pair.first;
            }
            // fill separated sorted arrays of words and corresponding numbers of their appears in the text
            words_array.push_back(pair.first);
            words_prob_array.push_back(pair.second);
        }
    }


};


#endif //TEXTGEN_DICTIONARY_H
