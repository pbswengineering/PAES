#!/bin/bash

(cd aes && make clean)
(cd paes && make clean)
(cd query && make clean)
(cd test && rm -fr reports temp-*)
(cd thesis-slides && make clean)
(cd thesis-text && make clean)
