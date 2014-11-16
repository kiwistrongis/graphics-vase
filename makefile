#globals
default: build
freshen: clean build
clean:
	rm -rf bin/* obj/* gen/* pkg/*

#vars
includes = 
static_libs = lib/libtinyobjloader.a
libs = -lm -lstdc++ -lX11 -lGL -lGLU -lGLEW -lglut -lfreeimage
options = -g -Wall -O0  -funsigned-char -std=gnu++11
warnings =
#compilation flags
cflags = $(includes) $(options) 
#linking flags
#ldflags = $(lib_includes) $(libs) $(warnings)
ldflags = $(libs) $(warnings)
#other vars
package_file = pkg/kalev_100425828_assignment2.zip
lib_file = bin/lib.a

#includes
include deps.mk
include lists.mk

#compilation definitions
$(objects): obj/%.o : src/%.cpp
	gcc $(cflags) -c $< -o $@
$(binaries): bin/% : obj/%.o $(lib_file)
	gcc $(ldflags) $< $(lib_file) $(static_libs) -o $@

$(lib_file): $(lib_objects)
	ar rc $@ $(lib_objects)
	ranlib $@

#commands
build: $(binaries)
package: $(package_file)
$(package_file): $(binaries)
	zip -FSr $(package_file) \
		bin data gen lib obj shdr src \
		deps.mk lists.mk makefile report.md run.sh \
		pkg/.gitignore

#tests
test: texgen test-model

texgen: bin/texgen
	$<

test-model: bin/model_test
	primusrun $<
