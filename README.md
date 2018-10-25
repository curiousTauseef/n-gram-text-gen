# n-gram-text-gen
Markov chain N-gram text generator for fast work with big number of N. Can work fast with N-grams where N can me much bigger than 10.

To build the project in Linux run "cmake ." in the project directory.
To build the project in Windows run "cmake ." and then build the generated solution for the MSVS.

Allowed options:
  --help                show help message
  --N arg               set N-gram order (2 by default)
  --generate arg        set how many sentences to generate (10 by default)
  --input arg           set input file for learning
  --matrix arg          set output file for saving of the matrix
  --print_stats         print the statistics data
  --print_chains        print the N-gram chains
  --print_dictionary    print the dictionary

Article with my experiments with this program is here: https://medium.com/@molchevsky/harry-potter-and-the-markov-chains-44429527b5d1
