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

namespace ublas = boost::numeric::ublas;
namespace po = boost::program_options;

typedef std::map<std::string, int> Dict;
typedef std::vector<std::string> Words_array;
typedef std::vector<int> Words_probability;
typedef ublas::compressed_vector<int> CVector;
typedef ublas::compressed_vector<CVector> Matrix2D;
typedef ublas::compressed_vector<Matrix2D> Matrix3D;
typedef ublas::compressed_vector<Matrix3D> Matrix4D;
typedef std::vector<Words_array> Sentences_array;



struct Stats {
    int words_number = 0;           // Number of all words in the analysed text
    int unique_words_number = 0;    // Number of unique words in the analysed text
    int max_number = 0;             // Max number of one unique word found in the analysed text
    std::string max_word;
    std::map<int, int> sentence_len;       // number of words in sentences depending of sentence length
    int matrix_size = 0;
    int matrix_rows = 0;
    int matrix_cols = 0;

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
    Sentences_array sentences;
    std::string text;

    Words_probability first_word_prob, last_word_prob, words_prob_array;

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


    void fill_dictionary() {

        int number_of_words = 0;
        typedef boost::tokenizer<> tokenizer;
        tokenizer tok{text};

        Words_array words_array;

        for (const auto &t : tok) {
            number_of_words++;
            words_array.push_back(t);
//        std::cout << t << '\n';
            // add a new pair <word, number of this word in the text>
            auto ret = dictionary.insert({t,1});
            // if the word is already in the dictionary increase its number
            if(ret.second == false) {
                ret.first->second++;
            }
        }

        sentences.push_back(words_array);

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

    void print_dictionary() {
        int word_num = 0;
        for(const auto & p : dictionary) {
            cout << p.first << " " << p.second << endl;
            word_num += p.second;
        }
        cout << "Unique words: " << dictionary.size() << endl;
        cout << "Total words: " << word_num << endl;
    }

    Dictionary(const std::string & file_name) {

        read_file(file_name);

        typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
        boost::char_separator<char> sep(".?!;");
        tokenizer tok{text, sep};
        for (auto &t : tok) {
            // std::cout << t << '\n';
            fill_dictionary();
        }

        // free memory of the unneeded text
        text.clear();
        text.shrink_to_fit();

        stats.unique_words_number = dictionary.size();

        words_array.reserve(stats.unique_words_number);
        first_word_prob.resize(stats.unique_words_number, 0);
        last_word_prob.resize(stats.unique_words_number, 0);

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
