# QuineMcCluskey #

C++ implementation of the [Quine-McCluskey algorithm](https://en.wikipedia.org/wiki/Quine%E2%80%93McCluskey_algorithm), designed to be fast.

The functions are generic to be able to operate on terms with any number of bits.

## Features ##

Some heuristics are included to find a subset of prime implicants covering minterms.

[Petrick's method](https://en.wikipedia.org/wiki/Petrick%27s_method) is also implemented.

This program was developed for [differential cryptanalysis](https://en.wikipedia.org/wiki/Differential_cryptanalysis) purposes: some [S-boxes](https://en.wikipedia.org/wiki/S-box) are included (the Rijndael S-box used in AES, and the four S-boxes of Minori128), as well as functions to generate DDTs and minterms from them.
