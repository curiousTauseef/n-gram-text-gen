/*

Markov chain N-gram text generator for fast work with big number of N. Can work fast with N-grams where N can me much bigger than 10.
Copyright © 2018  Oleksandr Molchevskyi

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

Contact email molchevsky@gmail.com

*/

// Created by Oleksandr Molchevskyi on 8/20/18.


#include <iostream>
#include "hypercube.h"

/*
int asd() {

    hypercube<4, int> c(5);
    std::cout << "dims: " << c.get_dim() << '\n';
    size_t s = c.size() * // dim 1
                 c[0].size() * // dim 2
                 c[0][0].size() * // dim 3
                 c[0][0][0].size(); // dim 4
    std::cout << s << '\n'; // outputs: 625 -> 5 * 5 * 5 * 5 -> 5^4
    return 0;
}

*/