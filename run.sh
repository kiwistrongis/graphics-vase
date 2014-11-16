#!/bin/bash
## this script compiles and runs this project

# make clean build
make freshen
# gen wood and marble textures
bin/texgen
# test wood texture on vase model
bin/model_test
