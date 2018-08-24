//
// Created by sanya on 8/20/18.
//


#include <iostream>
#include "hypercube.h"


void asd() {

    hypercube<4> c(5);
    unsigned s = c.size() * // dim 1
                 c[0].size() * // dim 2
                 c[0][0].size() * // dim 3
                 c[0][0][0].size(); // dim 4
    std::cout << s << '\n'; // outputs: 625 -> 5 * 5 * 5 * 5 -> 5^4

}

