#include "dictionary.h"
#include "hypercube.h"

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

template< class InputIt>
int get_prob(InputIt first, InputIt last, std::mt19937 & gen) {
//    std::random_device rd;
//    std::mt19937 gen(rd());
    std::discrete_distribution<> dd(first, last); // инициализируем контейнер для генерации числа на основе распределения вероятности
    int res = dd(gen);
    return res;
};
/*
std::string & make_next_word(std::string & word, Matrix2D & matrix, const Words_array & words_array) {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> dd_first_word(first_word_prob.begin(), first_word_prob.end()); // инициализируем контейнер для генерации числа на основе распределения вероятности
    int first_word_index = dd_first_word(gen); // генерируем следующую вершину
    if (first_word_index == first_word_prob.size()) {// тонкости работы генератора, если распределение вероятностей нулевое, то он возвращает количество элементов
        cerr << "first_word_prob array is empty or filled by zeroes" << endl;
        exit(1);
    }
}
*/

Words_probability & get_row(Matrix2D & matrix, Words_probability & row, size_t row_index) {
    auto row_size = matrix.size1();
    row.reserve(row_size);
    for(auto i = 0; i < row_size; ++i) {
        Matrix2D::const_reference element = matrix(row_index, i);
        row.push_back(element);
    }
}

Words_probability & get_col(Matrix2D & matrix, Words_probability & col, size_t col_index) {
    auto col_size = matrix.size2();
    col.reserve(col_size);
    for(auto i = 0; i < col_size; ++i) {
        Matrix2D::const_reference element = matrix(i, col_index);
        col.push_back(element);
    }
}

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
/*
    cout << endl << "Print num_len_of_sentences <Number of sentences : length of sentences>" << endl;
    for (const auto & t : num_len_of_sentences) {
        cout << t.first << " : " << t.second << endl;
    }

    cout << endl << "Print nums:" << endl;
    for (const auto & t : nums) {
        cout << t << endl;
    }
*/
/*
    std::discrete_distribution<> dd_sentence_len(num_len_of_sentences.begin(), num_len_of_sentences.end());
    int number_of_sentences = dd_sentence_len(gen);
    if (number_of_sentences == num_len_of_sentences.size()) {
        cerr << "len_num_of_sentences array is empty or filled by zeroes" << endl;
        exit(1);
    }

    int sentence_length = num_len_of_sentences.at(number_of_sentences);
*/

    int index_in_nums = get_prob(nums.begin(), nums.end(), gen);

//    cout << endl << "index_in_nums=" << index_in_nums << endl;

    int sentence_length = num_len_of_sentences[index_in_nums].second;

//    cout << endl << "sentence_length=" << sentence_length << endl;

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

/*
        std::cout << "COL index: " << col_index << " word: " << words_array[col_index] << endl;

        for(int i = 0; i < col.size(); ++i) {
            if(col[i] != 0) {
                std::cout << "i=" << i << " d=" << col[i] << endl;
            }
        }
*/
        int row_index = get_prob(col.begin(), col.end(), gen);

        col_index = row_index;

        new_sentence.push_back(words_array[col_index]);

/*
//        std::cout << "ROW index: " << row_index << " word: " << words_array[row_index] << endl;
//        new_sentence.push_back(words_array[row_index]);
//      ++i;

        if(i < sentence_length) {

            ublas::matrix_row<Matrix2D> mr = ublas::row(matrix, row_index);

            vector<int> v1 (mr.begin(), mr.end());

            for(int i = 0; i < v1.size(); ++i) {
                if(v1[i] != 0) {
                    std::cout << "i=" << i << " d=" << v1[i] << endl;
                }
            }

            col_index = get_prob(v1.begin(), v1.end(), v1.size(), gen);

            new_sentence.push_back(words_array[col_index]);
        }
        else {
            break;
        }
*/
    }

    for (const auto & word : new_sentence) {
        s.append(word);
        s.push_back(' ');
    }
    s.back() = '.';

    return s;
}


void init_matrix4D(Matrix4D & m, int size) {

    for (int col = 0; col < size; ++col) {
        for (int row = 0; row < size; ++row) {
            m[col]
            Matrix2D item = m[col];
            CVector v = item[row];
            // resize matrices inside the matrix and don't preserve their existing elements
            v.resize(size, false);

        }
    }
}

void init_matrix2D(Matrix2D & m, int size) {
    for (int col = 0; col < size; ++col) {
        CVector item = m[col];
        // resize matrices inside the matrix and don't preserve their existing elements
        item.resize(size, false);
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
    // "+ 2" needs for counting of first and last words in the sentence.
    // Sentences with the only word are not count.
    Matrix2D matrix(stats.unique_words_number);
    Matrix4D matrix4D(stats.unique_words_number);

    // fill 2D matrix by words probabilities and return probabilities of first and last words in the sentences
    fill_matrix2D(matrix, sentences, words_array, first_word_prob, last_word_prob);

    stats.matrix_size = matrix.value_data().size();
    stats.matrix_cols = matrix.size1();
    stats.matrix_rows = matrix.size2();

//    print_dictionary(dictionary);

    stats.print_stats();

/*
    std::cout << "First words with their numbers <word : number> " << endl;

    for (auto i = 0; i < words_array.size(); ++i) {
        std::cout << words_array[i]  << " : " << first_word_prob[i] << endl;
    }
*/
/*

    std::cout << "Last words with their numbers <word : number> " << endl;

    for (auto i = 0; i < words_array.size(); ++i) {
        std::cout << words_array[i]  << " : " << last_word_prob[i] << endl;
    }

    std::cout << matrix << std::endl;
*/
/*
    std::cout << "Print non zero probabilities of dependings of words <first word [number] last word >" << endl;

    for (unsigned i = 0; i < matrix.size1 (); ++ i) {
        for (unsigned j = 0; j < matrix.size2(); ++j) {
            auto cell = matrix(i, j);
            if (cell != 0) {
                std::cout << words_array[i] << " [col=" << i << " row=" << j << " cell="<< cell << "] " << words_array[j] << std::endl;
            }
        }
    }
*/

    // ----- generation --------

    std::mt19937 gen { std::random_device()() };

    std::string sentence;
    cout << "Generated text: " << endl;

    for( int i = 0; i < 10; ++i) {
        make_sentence(sentence, matrix, words_array, first_word_prob, last_word_prob, words_prob_array, gen);
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
