#include <iostream>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/storage.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>

namespace ublas = boost::numeric::ublas;
namespace po = boost::program_options;
using namespace std;

struct Options {
    int dim = 2;
    string input;
    string matrix;
    bool print_chains = false;
} options;

struct Stats {
    int words_number = 0;           // Nubmer of all words in the analysed text
    int unique_words_number = 0;    // Nubmer of unique words in the analysed text
    int max_numver = 0;             // Max number of one unique word found in the analysed text

};

typedef std::map<string, int> Dictionary;



void show_array(const ublas::unbounded_array<double>& a)  {
    for(const int &element : a ) {
        std::cout << element << ' ';
    }
    cout << endl;
}

int parse_command_line(int ac, char* av[], Options & options) {

    try {

        po::options_description desc("Allowed options");
        desc.add_options()
                ("help", "produce help message")
                ("dim", po::value<int>(), "set dimension of the matrix")
                ("input", po::value<string>(), "set input file for learning")
                ("matrix", po::value<string>(), "set output file for saving of the matrix")
                ("print_matrix", "print the matrix (for dimension = 2 only)")
                ("print_chains", "print the chains")
                ;

        po::variables_map vm;
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);

        if ((ac == 1) || vm.count("help")) {
            cout << desc << endl;
            exit(0);
        }

        if (vm.count("print_chains")) {
            options.print_chains = true;
        }


        if (vm.count("input")) {
            options.input = vm["input"].as<string>();
        }

        if (vm.count("dim")) {
            options.dim = vm["dim"].as<int>();
        }

        cout << "Dimension of the matrix = " << options.dim << endl;
    }
    catch(exception& e) {
        cerr << "error: " << e.what() << endl;
        exit(1);
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }
}

const std::string & read_file(const std::string & file_name, std::string & out) {
    std::ifstream ifs(file_name);
    out.assign((std::istreambuf_iterator<char>(ifs)),
               std::istreambuf_iterator<char>());
    return out;
}

bool remove_predicate(const unsigned char c) {
    // needed to avoid undefined behavior
    // static_cast<unsigned char>(ch);

    return ! (isalpha(c) && isblank(c) && (c == '.') );

}

const std::string & clean_text(std::string & str) {
    str.erase(std::remove_if(str.begin(), str.end(), remove_predicate),
              str.end());
    return str;
}

void print_dictionary(const Dictionary & dictionary) {
    int word_num = 0;
    for(const auto & p : dictionary) {
        cout << p.first << " " << p.second << endl;
        word_num += p.second;
    }
    cout << "Unique words: " << dictionary.size() << endl;
    cout << "Total words: " << word_num << endl;
}

void fill_dictionary(const std::string & str, Dictionary & dictionary) {

    typedef boost::tokenizer<> tokenizer;
    tokenizer tok{str};
    for (const auto &t : tok) {
//        std::cout << t << '\n';
        // add a new pair <word, number of this word in the text>
        auto ret = dictionary.insert({t,1});
        // if the word is already in the dictionary increase its number
        if(ret.second == false) {
            ret.first->second++;
        }
    }
}

int main(int ac, char* av[]) {

//    setlocale(LC_CTYPE, "Russian_Russia.1251");

    Stats stats;
    Options options;
    Dictionary dictionary;

    parse_command_line(ac, av, options);

    if(options.input.empty()) {
        cout << "set --input file" << endl;
    }

    std::string the_text;
    read_file(options.input, the_text);

    typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
    boost::char_separator<char> sep{"."};
    tokenizer tok{the_text, sep};
    for (const auto &t : tok) {
//        std::cout << t << '\n';
        fill_dictionary(t, dictionary);
    }

//    print_dictionary(dictionary);



/*

    clean_text(text);

    //convert the text to lower case
    std::transform(text.begin(), text.end(), text.begin(), ::tolower);

    cout << text;
*/

    ublas::compressed_matrix<double> m (10, 10, 3 * 10);

    m(0, 5) = 1; // underlying array is {1, 0, 0, 0, ...}
    show_array(m.value_data());
    m(0, 6) = 2; // underlying array is {1, 2, 0, 0, ...}
    show_array(m.value_data());
    m(0, 4) = 3;  // underlying array is {3, 1, 2, 0, ...}
    show_array(m.value_data());
    m(0, 4) = 7;  // underlying array is {7, 1, 2, 0, ...}
    show_array(m.value_data());
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
