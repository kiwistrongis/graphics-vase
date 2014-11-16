#!/bin/bash

# this script compiles and runs this project
make
bin/texgen
bin/model_test
