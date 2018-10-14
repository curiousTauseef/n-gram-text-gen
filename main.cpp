#include "dictionary.h"


using namespace std;

typedef size_t index_type;
typedef size_t data_type;

typedef hcube_t<data_type, index_type> HCube;


struct Options {
    int dim = 2;
    int generate = 10;
    string input;
    bool print_chains = false;
    bool print_dictionary = false;
    bool print_stats = false;
} options;

int parse_command_line(int ac, char* av[], Options & options) {

    try {

        po::options_description desc("Allowed options");
        desc.add_options()
                ("help", "show help message")
                ("N", po::value<int>(), "set N-gram order (2 by default)")
                ("generate", po::value<int>(), "set how many sentences to generate (10 by default)")
//                ("max_length", po::value<int>(), "set max length of sentence (by default )")
                ("input", po::value<string>(), "set input file for learning")
                ("matrix", po::value<string>(), "set output file for saving of the matrix")
                ("print_stats", "print the statictics data")
                ("print_chains", "print the N-gram chains")
                ("print_dictionary", "print the dictionary")
                ;

        po::variables_map vm;
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);

        if ((ac == 1) || vm.count("help")) {
            cout << desc << endl;
            exit(0);
        }

        if (vm.count("print_stats")) {
            options.print_stats = true;
        }

        if (vm.count("print_chains")) {
            options.print_chains = true;
        }

        if (vm.count("print_dictionary")) {
            options.print_dictionary = true;
        }

        if (vm.count("input")) {
            options.input = vm["input"].as<string>();
        }

        if (vm.count("N")) {
            options.dim = vm["N"].as<int>();
        }

        cout << "N-gram order = " << options.dim << endl;
    }
    catch(exception& e) {
        cerr << "error: " << e.what() << endl;
        exit(1);
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }
}


/*
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

wstring toLow(const wstring & s)
{
    wstring result;
    locale loc = wcin.getloc() ;
    for (wstring::const_iterator it = s.begin() ; it != s.end() ; ++it)
    {
        result += std::use_facet<ctype<wchar_t>>(loc).tolower( *it );
    }
    return result;
}

std::wstring & wstr_tolower(std::wstring & s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned wchar_t c) {
                       return std::tolower(c, std::locale());
                   }
    );
    return s;
}


std::string & str_tolower(std::string & s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) {
                       return std::tolower(c, std::locale());
                   }
    );
    return s;
}
*/



void shift_left(HCube::index_type & arr, int n = 1) {
    std::move(arr.begin()+n, arr.end(), arr.begin());
    std::fill(arr.end()-n, arr.end(), 0);
}

void shift_right(HCube::index_type & arr, int n = 1) {
    std::move_backward(arr.begin(), arr.end()-n, arr.end());
    std::fill(arr.begin(), arr.begin()+n, 0);
}



void fill_hcube(HCube &hcube, Dictionary &dict) {

    const Sentences_array & sentences = dict.get_sentences();

    HCube::index_type words(hcube.get_dimsN(), 0);

    for (const auto &sentence : sentences) {

        std::fill(words.begin(), words.end(), 0);

        auto iterations = sentence.size() + words.size() - 1;

        for ( auto i = 0; i < iterations; ++i) {
            shift_left(words);
            if(i < sentence.size()) {
                size_t  index = dict.get_word_index(sentence[i]);
                words.back() = index;
            }
            else {
            }
            bool zeros = std::all_of(words.begin(), words.end(), [](HCube::dims_num_type i) { return i==0; });
            assert( ( ! zeros ) && "ERROR!!!!");
            (hcube[words]) ++;
        }
    }
}


/*
void save_matrix(const Matrix2D & m, const string & file_name) {
    std::ofstream ofs(file_name);
    boost::archive::binary_oarchive oarch(ofs);
    oarch << m;
    // m.serialize(oarch, 1);
}

void load_matrix(Matrix2D & m, const string & file_name) {
    std::ifstream ifs(file_name);
    boost::archive::binary_iarchive iarch(ifs);
    iarch >> m;
}
*/

void print_probs(const Words_probability & probs, const Words_array & words_array) {
    std::cout << "Probability array: \n";
    for( auto i = 0; i < probs.size(); ++i ) {
        std::cout << " probs[" << i << "] = " << probs[i] << " words_array[" << i << "] = " << words_array[i] << std::endl;
    }
}

size_t get_line(HCube & m, Words_probability & line, HCube::index_type line_index, size_t dim) {
    size_t words_in_line = 0;
    auto line_size = m.size();
    line.clear();
    line.resize(line_size, 0);
    for(size_t i = 0; i < line_size; ++i) {
        line_index[dim-1] = i;
        auto it = m.find(line_index);
        if( it != m.end()) {
            if(i > 0) { // skip zero line because it contains first words
                line[i] = (*it).second;
                ++words_in_line;
            }
        }
    }
    return words_in_line;
}

std::string & make_sentence(std::string & s, HCube & hcube, Dictionary & dictionary) {

    std::vector<string> new_sentence;

    int length_of_sentence = dictionary.get_len_of_sentence();

    auto dim = hcube.get_dimsN();

    HCube::index_type line_index(hcube.get_dimsN(), 0);

    Words_probability row;
    get_line(hcube, row, line_index, dim);

//    print_probs(row, dictionary.get_words_array());

    size_t first_word_index = 0;
    std::string first_word;
    dictionary.get_word(first_word, &first_word_index, row);
    new_sentence.push_back(first_word);

    line_index[dim-1] =  first_word_index;
    size_t index = first_word_index;

    std::string word;

    shift_left(line_index);

    auto total_length = length_of_sentence + hcube.get_dimsN();
    // i == 1 because the first word is already generated in first_word_index
//    for(auto i = 1; i < total_length; ++i) {
    for(auto i = 1; i < length_of_sentence; ++i) {

        Words_probability line;

        if(get_line(hcube, line, line_index, dim) == 0) {
            // if the line is empty return one word back
            // --i;
            //shift_right(line_index);
            //continue;
            break;
        }

        //print_probs(line, dictionary.get_words_array());
        dictionary.get_word(word, &index, line);
        if(i < length_of_sentence) {
            line_index[dim - 1] = index;
        }

        shift_left(line_index);
        new_sentence.push_back(word);
        --i;
    }

    for (const auto & word : new_sentence) {
        s.append(word);
        s.push_back(' ');
    }
    s.back() = '.';

    return s;
}



void print(HCube & m, Dictionary & dict) {
    std::cout << "Print N=grams:" << std::endl;
    for (const auto & item : m) {
        HCube::index_type index = item.first;
        size_t data = item.second;
        for(auto i = 0; i < m.get_dimsN(); ++i) {
            std::cout  << dict.get_word_by_index(index[i]) << " (d" << i << "=" << index[i] << ") ";
        }
        std::cout  << " [" << data << "] " << std::endl;
    }
}



int main(int ac, char* av[]) {

//    std::locale::global( std::locale("ru_RU.utf-8") );
    // std::setlocale(LC_ALL, "ru_RU.utf-8");

    Options options;

    parse_command_line(ac, av, options);

    if(options.input.empty()) {
        cout << "set --input file" << endl;
    }

    Dictionary dictionary(options.input);

    // std::cout << "locale setting is " << std::locale().name().c_str() << endl;
//    printf ("Locale is: %s\n", setlocale(LC_ALL,NULL) );
    //convert the text to lower case
//    std::transform(the_text.begin(), the_text.end(), the_text.begin(), ::tolower);

//    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
//    std::wstring wide = converter.from_bytes(the_text);
/*
    wcout.imbue( locale("ru_RU.utf-8") );
    std::wcout << "-----" << endl;
    std::wcout << wide << endl;
    std::wcout << "-----" << endl;

    toLow(wide);
*/

    // Init matrices
    // "+ 1" needs for counting of first and last words in the sentence.

    HCube hcube(options.dim, stats.unique_words_number + 1);

    if(options.print_dictionary) {
        dictionary.print();
    }

    fill_hcube(hcube, dictionary);

    if(options.print_chains) {
        print(hcube, dictionary);
    }

    stats.matrix_size = hcube.size() * hcube.get_dimsN();
    stats.matrix_cols = hcube.size();
    stats.matrix_rows = hcube.size();

    if(options.print_stats) {
        stats.print_stats();
    }

    std::string sentence;
    cout << "Generated text: " << endl;

    for( int i = 0; i < options.generate; ++i) {

        make_sentence(sentence, hcube, dictionary);

        std::cout <<  sentence << endl;
        sentence.clear();
    }

    // ------ print sentence -------




/*
    //convert the text to lower case
    std::transform(text.begin(), text.end(), text.begin(), ::tolower);
*/
    return 0;
}
