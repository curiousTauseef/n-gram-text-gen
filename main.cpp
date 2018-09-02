#include "dictionary.h"


using namespace std;

//typedef hypercube<2, uint16_t> HCube2D;
//typedef hypercube<4, uint16_t> HCube4D;

typedef size_t index_type;
typedef size_t data_type;

typedef index2d<index_type> Index2D;
typedef index4d<index_type> Index4D;

typedef hcube_t<data_type, Index2D> HCube2D;
typedef hcube_t<data_type, Index4D> HCube4D;


const int FIRST_WORD = 1, LAST_WORD = 0;

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


#if 0

void add_first_word(Matrix4D & matrix, const Words_array &words_array, Words_probability &first_word_prob, const string & word) {
    size_t index = static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), word) - words_array.begin());
    // matrix(index, FIRST_WORD) = matrix(index, FIRST_WORD) + 1;
    first_word_prob[index] = first_word_prob[index] + 1;
    cout << "add FIRST word: " << word << endl;
}

void add_last_word(Matrix4D & matrix, const Words_array &words_array, Words_probability &last_word_prob, const string & word) {
    size_t index = static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), word) - words_array.begin());
    // matrix(index, LAST_WORD) = matrix(index, LAST_WORD) + 1;
    last_word_prob[index] = last_word_prob[index] + 1;
    cout << "add LAST word: " << word << endl;
}


void process_2d(Matrix4D &matrix, const Words_array &words_array, const Words_array & sentence) {
    std::deque<size_t> words(2);
    size_t index1 = static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), sentence[0]) - words_array.begin());
    size_t index2 = static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), sentence[1]) - words_array.begin());
    words.push_back(index1);
    words.push_back(index2);
    auto asd = matrix.ref(words[0]).ref(words[1]).ref(1).ref(1);
    auto asd1 = asd.ref();
    matrix(words[0])(words[1]) = matrix[words[0]] [words[1]] + 1;

    for ( auto i = 2; i < sentence.size(); ++i) {
        words.pop_front();
        size_t index = static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), sentence[i]) - words_array.begin());
        words.push_back(index);
        matrix(words[0], words[1]) = matrix(words[0], words[1]) + 1;
    };
}

void process_3d(Matrix4D &matrix, const Words_array &words_array, const Words_array & sentence) {
    std::deque<size_t> words(3);
    size_t index1 = static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), sentence[0]) - words_array.begin());
    size_t index2 = static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), sentence[1]) - words_array.begin());
    size_t index3 = static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), sentence[2]) - words_array.begin());

    words.push_back(index1);
    words.push_back(index2);
    words.push_back(index3);

    matrix(words[0], words[1]) = matrix(words[0], words[1]) + 1;

    for ( auto i = 3; i < sentence.size(); ++i) {
        words.pop_front();
        size_t index = static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), sentence[i]) - words_array.begin());
        words.push_back(index);
        matrix(words[0], words[1]) = matrix(words[0], words[1]) + 1;
    };
}



void fill_matrix4D(Matrix4D &matrix, const Sentences_array &sentences, const Words_array &words_array,
                   Words_probability &first_word_prob, Words_probability &last_word_prob) {

    for (const auto &sentence : sentences) {

        // if (sentence.size() < 2) { continue; }

        // get index of the first word in the sentence
        // auto iterator = std::lower_bound(words_array.begin(), words_array.end(), sentence.front());

//        if (iterator == words_array.end()) {
//            cerr << "The word: " << sentence.front() << " was not found in words_array!" << endl;
//        }
//        auto index = iterator - words_array.begin();
//        first_word_prob[index] = first_word_prob[index] + 1;
//
//        // get index of the last word in the sentence
//        iterator = std::lower_bound(words_array.begin(), words_array.end(), sentence.back());
//        if (iterator == words_array.end()) {
//            cerr << "The word: " << sentence.front() << " was not found in words_array!" << endl;
//        }
//        index = iterator - words_array.begin();
//        last_word_prob[index] = last_word_prob[index] + 1;

        add_first_word(matrix, words_array, first_word_prob, sentence.front());
        add_last_word(matrix, words_array, last_word_prob, sentence.back());

        if (sentence.size() < 2) { continue; }

        size_t d1 = 0, d2 = 0, d3 = 0, d4 = 0;

        std::vector<size_t> words(4);

        // for (const auto &word : sentence) {
        for ( auto i = 0; i < sentence.size(); ++i) {

            if (words.size() < words.capacity()) {
                // "+ 2" needs for pass first two lines which contains numbers of the first and the last words in the sentence
                words.push_back(static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), sentence[i]) - words_array.begin()) + 2);
                cout << "add word: " << sentence[i] << endl;
            }
            else if (words.size() == words.capacity()) {
                matrix(words[0], words[1]) = matrix(words[0], words[1]) + 1;
                words.clear();
            }

            /*
            if (d1 == 0) {
                d1 = std::lower_bound(words_array.begin(), words_array.end(), word) - words_array.begin();
//                cout << "add word to d1: " << word << endl;
            } else if (d2 == 0) {
                d2 = std::lower_bound(words_array.begin(), words_array.end(), word) - words_array.begin();
//                cout << "add word to d2: " << word << endl;
            } else if (d3 == 0) {
                d3 = std::lower_bound(words_array.begin(), words_array.end(), word) - words_array.begin();
//                cout << "add word to d3: " << word << endl;
            } else if (d4 == 0) {
                d4 = std::lower_bound(words_array.begin(), words_array.end(), word) - words_array.begin();
//                cout << "add word to d4: " << word << endl;
            }
            */
        }
    }
}

#endif

void fill_hcube2D(HCube2D & hcube, Dictionary & dict) {
    const Sentences_array & sentences = dict.get_sentences();

    for (const auto &sentence : sentences) {

        std::deque<index_type> words(2, 0);

        auto iterations = sentence.size() + words.size() - 1;

        for ( auto i = 0; i < iterations; ++i) {
            words.pop_front();
            if(i < sentence.size()) {
                size_t  index = dict.get_word_index(sentence[i]);
                words.push_back(index);
            }
            else {
                words.push_back(0);
            }
            if ( (words[0] == 0) && (words[1] == 0)  ) {
                cout << "ERROR!!!!" << endl;
            }
            hcube[ { words[0], words[1] } ] ++;
        }
    }
}


void fill_hcube4D(HCube4D & hcube, Dictionary & dict) {

    const Sentences_array & sentences = dict.get_sentences();

    for (const auto &sentence : sentences) {

        std::deque<index_type> words(4, 0);

        auto iterations = sentence.size() + words.size() - 1;

        for ( auto i = 0; i < iterations; ++i) {
            words.pop_front();
            if(i < sentence.size()) {
                size_t  index = dict.get_word_index(sentence[i]);
                words.push_back(index);
            }
            else {
                words.push_back(0);
            }
            if ( (words[0] == 0) && (words[1] == 0) && (words[2] == 0) && (words[3] == 0) ) {
                cout << "ERROR!!!!" << endl;
            }
            (hcube[ { words[0], words[1], words[2], words[3] } ]) ++;
        }
    }
}

#if 0

void fill_matrix2D(Matrix2D &matrix, const Sentences_array &sentences, const Words_array &words_array,
                   Words_probability &first_word_prob, Words_probability &last_word_prob) {

    // std::queue<int> prev_words_index;
//    prev_words_index.reserve(options.dim);

    for (const auto &sentence : sentences) {

        // Ignore one word sentences. It must contains at least two words to be processed.
        // if(sentence.size() < 2) { continue; }
/*
        cout << "process sentence: ";
        for (const auto& i: sentence) {
            std::cout << i << ' ';
        }
        cout << endl;


        // get index of the first word in the sentence
        auto iterator = std::lower_bound(words_array.begin(), words_array.end(), sentence.front());

//        cout << "sentence.front() = " << sentence.front() << endl;

        if(iterator == words_array.end()) {
            cerr << "The word: " << sentence.front() << " was not found in words_array!" << endl;
        }

        auto index = iterator - words_array.begin();
//        cout << "index = " << index << endl;

        first_word_prob[index] = first_word_prob[index] + 1;

//        cout << "first_word_prob[index] = " << first_word_prob[index] << endl;

//        cout << "words_array[index] = " << words_array[index] << endl;


        // get index of the last word in the sentence
        iterator = std::lower_bound(words_array.begin(), words_array.end(), sentence.back());
        if(iterator == words_array.end()) {
            cerr << "The word: " << sentence.front() << " was not found in words_array!" << endl;
        }
        index =  iterator - words_array.begin();
        last_word_prob[index] = last_word_prob[index] + 1;

        if(sentence.size() == 1) {
            auto col = static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), sentence.back()) - words_array.begin());
            matrix(0, col+1) = matrix(0, col+1) + 1;
            continue;
        }
*/
        std::vector<size_t> words(2);

         for ( auto i = 0; i < sentence.size(); ++i) {
        //for (const auto &word : sentence) {

            // col = static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), sentence[i]) - words_array.begin());
//                cout << "add word to col: " << word << endl;

             if(i == 0) {
                 // if this is the first word
                 size_t index = static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), sentence[i]) - words_array.begin());
                 matrix(index, FIRST_WORD) = matrix(index, FIRST_WORD) + 1;
                 cout << "add FIRST word: " << sentence[i] << endl;
             }

             if (words.size() < words.capacity()) {
                // "+ 2" needs for pass first two lines which contains numbers of the first and the last words in the sentence
                 words.push_back(static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), sentence[i]) - words_array.begin()));
                 cout << "add word: " << sentence[i] << endl;
             }
             else if (words.size() == words.capacity()) {
                matrix(words[0], words[1]) = matrix(words[0], words[1]) + 1;
                words.clear();
             }

//            row = static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), word) - words_array.begin());
//            if (row == 0) {
//                row = static_cast<size_t>(std::lower_bound(words_array.begin(), words_array.end(), word) - words_array.begin());
//                cout << "add word to row: " << word << endl;
//            }
//            if ((col != 0) && (row != 0)) {
//                matrix(row, col) = matrix(row, col) + 1;
//                cout << "add to matrix col=" << words_array[col] << " row=" << words_array[row] << endl;
//                col = row;
//                row = 0;
         }

         if( ! words.empty()) {
             if(words.size() != 1) {
                 cout << "Error! line 330" << endl;
             }
             // matrix(row, col);
             matrix(words[0], LAST_WORD) = matrix(words[0], LAST_WORD) + 1;
         }
    }
}

#endif

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


void show_array(Matrix2D::value_array_type & a)  {

    for(const auto &element : a ) {
        std::cout << element << ' ';
    }
    cout << endl;
}
*/

void print_probs(const Words_probability & probs, const Words_array & words_array) {
    std::cout << "Probability array: \n";
    for( auto i = 0; i < probs.size(); ++i ) {
        std::cout << " probs[" << i << "] = " << probs[i] << " words_array[" << i << "] = " << words_array[i] << std::endl;
    }
}


void get_col(HCube2D & matrix, Words_probability & col, size_t col_index) {
    auto col_size = matrix.size();
    col.clear();
    col.resize(col_size, 0);
    for (const auto & item : matrix) {
        Index2D index = item.first;
        size_t data = item.second;
        if(index.x == col_index) {
            col[index.y] = data;
        }
    }
}


void get_line(HCube4D & m, Words_probability & line, size_t line_index) {
    auto line_size = m.size();
    line.clear();
    line.resize(line_size, 0);
    for (const auto & item : m) {
        Index4D index = item.first;
        size_t data = item.second;
        if(index.d1 == line_index) {
            line[index.d2] = data;
        }
    }
}


std::string & make_sentence(std::string & s, HCube2D & hCube2D, Dictionary & dictionary) {

    std::vector<string> new_sentence;

    int length_of_sentence = dictionary.get_len_of_sentence();

    Words_probability row;
    get_col(hCube2D, row, 0);

//    print_probs(row, dictionary.get_words_array());

    size_t first_word_index = 0;
    std::string first_word = dictionary.get_word(first_word, &first_word_index, row);
    new_sentence.push_back(first_word);

    size_t index = first_word_index;

    std::string word;

    // i == 1 because the first word is already generated in first_word_index
    for(int i = 1; i < length_of_sentence; ++i) {

        Words_probability line;
        // по номеру колонки в строке получаем колонку
        get_col(hCube2D, line, index);
        //print_probs(line, dictionary.get_words_array());
        // line хранит колонку. index хранит номер этой колонки в строке
        // получаем слово из колонки
        dictionary.get_word(word, &index, line);
        new_sentence.push_back(word);
        // line хранит уже не нужную колонку. index хранит номер строки полученного слова
        // сохраняем слово
    }

    for (const auto & word : new_sentence) {
        s.append(word);
        s.push_back(' ');
    }
    s.back() = '.';

    return s;
}


std::string & make_sentence(std::string & s, HCube4D & hCube4D, Dictionary & dictionary) {

    std::vector<string> new_sentence;

    int length_of_sentence = dictionary.get_len_of_sentence();

    Words_probability row;
    get_line(hCube4D, row, 0);

//    print_probs(row, dictionary.get_words_array());

    size_t first_word_index = 0;
    std::string first_word = dictionary.get_word(first_word, &first_word_index, row);
    new_sentence.push_back(first_word);

    size_t index = first_word_index;

    std::string word;

    // i == 1 because the first word is already generated in first_word_index
    for(int i = 1; i < length_of_sentence; ++i) {

        Words_probability line;
        // по номеру колонки в строке получаем колонку
        get_line(hCube4D, line, index);
        //print_probs(line, dictionary.get_words_array());
        // line хранит колонку. index хранит номер этой колонки в строке
        // получаем слово из колонки
        dictionary.get_word(word, &index, line);
        new_sentence.push_back(word);
        // line хранит уже не нужную колонку. index хранит номер строки полученного слова
        // сохраняем слово
    }

    for (const auto & word : new_sentence) {
        s.append(word);
        s.push_back(' ');
    }
    s.back() = '.';

    return s;
}


#if 0
std::string & make_sentence(std::string & s, Matrix2D & matrix, const Words_array & words_array, Words_probability & first_word_prob, Words_probability & last_word_prob, Words_probability & words_prob_array, std::mt19937 & gen) {

    std::vector<string> new_sentence;


    int first_word_index = get_prob(first_word_prob.begin(), first_word_prob.end(),  gen);

//    int last_word_index = get_prob(last_word_prob.begin(), last_word_prob.end(), last_word_prob.size(), gen);

    std::vector<std::pair<int,int>> num_len_of_sentences;
    std::vector<int> nums;

    for (const auto & t : stats.sentence_len) {
        num_len_of_sentences.push_back({t.second, t.first});
        nums.push_back(t.second);
    }

    int index_in_nums = get_prob(nums.begin(), nums.end(), gen);

    int sentence_length = num_len_of_sentences[index_in_nums].second;

    if(sentence_length < 2) {
        sentence_length = 2;
    }
    // Generate of words in the middle of the sentence

    // Add first word to the sentence
    new_sentence.push_back(words_array[first_word_index]);

//    string current_word(words_array[first_word_index]);

    int col_index = first_word_index;

    // i == 1 because the first word is already generated in first_word_index
    for(int i = 1; i < sentence_length; ++i) {

        Words_probability col;
        get_col(matrix, col, col_index);

        int row_index = get_prob(col.begin(), col.end(), gen);

        col_index = row_index;

        new_sentence.push_back(words_array[col_index]);

    }

    for (const auto & word : new_sentence) {
        s.append(word);
        s.push_back(' ');
    }
    s.back() = '.';

    return s;
}
#endif


void print(HCube4D & m, Dictionary & dict) {
    std::cout << "Print matrix and words_array:" << std::endl;
    for (const auto & item : m) {
        Index4D index = item.first;
        size_t data = item.second;
        std::cout  <<
                   dict.get_word_by_index(index.d1) << " d1 = " << index.d1 << " "  <<
                   dict.get_word_by_index(index.d2) << " d2 = " << index.d2  << " " <<
                   dict.get_word_by_index(index.d3) << " d3 = " << index.d3 << " " <<
                   dict.get_word_by_index(index.d2) << " d4 = " << index.d4 << " " <<
                   " [" << data << "] " << std::endl;
    }
}



void print(HCube2D & m, Dictionary & dict) {
    std::cout << "Print matrix and words_array:" << std::endl;
    for (const auto & item : m) {
        Index2D index = item.first;
        size_t data = item.second;
        std::cout << dict.get_word_by_index(index.x) << " [" << data << "] " << dict.get_word_by_index(index.y) << " index.x = " << index.x << " index.y = " << index.y << std::endl;
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
    // Sentences with the only word are not count.
//    HCube2D hCube2D(stats.unique_words_number + 1);
//    HCube4D hCube4D(stats.unique_words_number + 1);

    HCube2D hCube2D(stats.unique_words_number + 1);

//    HCube4D hCube4D(stats.unique_words_number + 1);

    fill_hcube2D(hCube2D, dictionary);

//    dictionary.print();

//    print(hCube2D, dictionary);

//    fill_hcube4D(hCube4D, dictionary);

//    print(hCube4D, dictionary);

    stats.matrix_size = hCube2D.size() * HCube2D::get_dim();
    stats.matrix_cols = hCube2D.size();
    stats.matrix_rows = hCube2D.size();

    // fill 2D matrix by words probabilities and return probabilities of first and last words in the sentences
    // fill_matrix2D(matrix, sentences, words_array, first_word_prob, last_word_prob);

    stats.print_stats();

    // ----- generation --------

    // std::mt19937 gen { std::random_device()() };

    std::string sentence;
    cout << "Generated text: " << endl;

    for( int i = 0; i < 10; ++i) {

        make_sentence(sentence, hCube2D, dictionary);

        std::cout <<  sentence << endl;
        sentence.clear();
    }

    // ------ print sentence -------




/*

    clean_text(text);

    //convert the text to lower case
    std::transform(text.begin(), text.end(), text.begin(), ::tolower);

    cout << text;


    Matrix2D m (10, 10);
    Matrix2D m2;

    m(0, 5) = 1; // underlying array is {1, 0, 0, 0, ...}
    show_array(m.value_data());
    m(0, 6) = 2; // underlying array is {1, 2, 0, 0, ...}
    show_array(m.value_data());
    m(0, 4) = 3;  // underlying array is {3, 1, 2, 0, ...}
    show_array(m.value_data());
    m(0, 4) = 7;  // underlying array is {7, 1, 2, 0, ...}
    show_array(m.value_data());

    std::cout << m << std::endl;

    std::cout << "Save matrix" << std::endl;

    save_matrix(m, "matrix.bin");

    std::cout << "Load matrix" << std::endl;

    load_matrix(m2, "matrix.bin");

    std::cout << m2 << std::endl;
*/
    return 0;
}
