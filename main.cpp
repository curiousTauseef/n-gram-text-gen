#include "dictionary.h"


using namespace std;

typedef size_t index_type;
typedef size_t data_type;

typedef hcube_t<data_type, index_type, 6> HCube4D;


struct Options {
    int dim = 2;
    string input;
    string matrix;
    bool print_chains = false;
} options;

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

        cout << "Dimension of the matrix (2 or 4) = " << options.dim << endl;
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



void shift_left(HCube4D::index_type & arr, int n = 1) {
    std::move(arr.begin()+n, arr.end(), arr.begin());
    std::fill(arr.end()-n, arr.end(), 0);
}

void shift_right(HCube4D::index_type & arr, int n = 1) {
    std::move_backward(arr.begin(), arr.end()-n, arr.end());
    std::fill(arr.begin(), arr.begin()+n, 0);
}



void fill_hcube4D(HCube4D & hcube, Dictionary & dict) {

    const Sentences_array & sentences = dict.get_sentences();

    for (const auto &sentence : sentences) {

        // HCube4D::index_deque words(4, 0);
        HCube4D::index_type words {};
        HCube4D::index_type empty {};

        auto iterations = sentence.size() + words.size() - 1;

        for ( auto i = 0; i < iterations; ++i) {
            //words.pop_front();
            // words.pop_back();
            shift_left(words);
            if(i < sentence.size()) {
                size_t  index = dict.get_word_index(sentence[i]);
                words.back() = index;
                // words.push_front(index);
            }
            else {
                //words.push_back(0);
                //words.push_front(0);
            }
            assert( words != empty && "ERROR!!!!");
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

size_t get_line(HCube4D & m, Words_probability & line, HCube4D::index_type line_index, size_t dim) {
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

std::string & make_sentence(std::string & s, HCube4D & hCube4D, Dictionary & dictionary) {

    std::vector<string> new_sentence;

    int length_of_sentence = dictionary.get_len_of_sentence();

    auto dim = HCube4D::get_dim();

    HCube4D::index_type line_index {};

    Words_probability row;
    get_line(hCube4D, row, line_index, dim);

//    print_probs(row, dictionary.get_words_array());

    size_t first_word_index = 0;
    std::string first_word;
    dictionary.get_word(first_word, &first_word_index, row);
    new_sentence.push_back(first_word);

    line_index[dim-1] =  first_word_index;
    size_t index = first_word_index;

    std::string word;

    shift_left(line_index);

//    --dim;

    auto total_length = length_of_sentence + HCube4D::get_dim();
    // i == 1 because the first word is already generated in first_word_index
//    for(auto i = 1; i < total_length; ++i) {
    for(auto i = 1; i < length_of_sentence; ++i) {

        Words_probability line;

        if(get_line(hCube4D, line, line_index, dim) == 0) {
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



void print(HCube4D & m, Dictionary & dict) {
    std::cout << "Print matrix and words_array:" << std::endl;
    for (const auto & item : m) {
        HCube4D::index_type index = item.first;
        size_t data = item.second;
        std::cout  <<
                   dict.get_word_by_index(index[0]) << " d1 = " << index[0] << " "  <<
                   dict.get_word_by_index(index[1]) << " d2 = " << index[1] << " " <<
                   dict.get_word_by_index(index[2]) << " d3 = " << index[2] << " " <<
                   dict.get_word_by_index(index[3]) << " d4 = " << index[3] << " " <<
                   dict.get_word_by_index(index[4]) << " d3 = " << index[4] << " " <<
                   dict.get_word_by_index(index[5]) << " d4 = " << index[5] << " " <<
                   " [" << data << "] " << std::endl;
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

    HCube4D hCube4D(stats.unique_words_number + 1);

    //dictionary.print();

    fill_hcube4D(hCube4D, dictionary);

//    print(hCube4D, dictionary);

    stats.matrix_size = hCube4D.size() * HCube4D::get_dim();
    stats.matrix_cols = hCube4D.size();
    stats.matrix_rows = hCube4D.size();

    // fill 2D matrix by words probabilities and return probabilities of first and last words in the sentences
    // fill_matrix2D(matrix, sentences, words_array, first_word_prob, last_word_prob);

    stats.print_stats();

    // ----- generation --------

    // std::mt19937 gen { std::random_device()() };

    std::string sentence;
    cout << "Generated text: " << endl;

    for( int i = 0; i < 50; ++i) {

        make_sentence(sentence, hCube4D, dictionary);

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
