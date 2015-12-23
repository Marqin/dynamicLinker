#
#  Copyright (c) Hubert Jarosz. All rights reserved.
#  Licensed under the MIT license. See LICENSE file in the project root for full license information.
#

CC = clang
CXX = clang++
CXXFLAGS = -Wall -Wextra -Werror -std=c++11
LDLIBS = -ldl

OS_NAME = $(shell uname -s)
ifeq ($(OS_NAME), Darwin)
  SONAME = -install_name
else
  SONAME = -soname
endif

.PHONY: all clean

all: dynamicLinker.a Makefile

dynamicLinker.a: dynamicLinker.cpp dynamicLinker.hpp Makefile
	$(CXX) $(CXXFLAGS) dynamicLinker.cpp -c
	ar rsc libdynamicLinker.a dynamicLinker.o

clean:
	rm -rf test dynamicLinker.o libdynamicLinker.a test.lib test.dSYM

test: libdynamicLinker.a dynamicLinker.hpp test.cpp Makefile test.lib
	$(CXX) $(CXXFLAGS) test.cpp -o test -L. -ldynamicLinker $(LDLIBS)
	./test

test.lib: testLib.c Makefile
	$(CC) -shared -Wl,$(SONAME),test.lib -o test.lib -Wall -Wextra -pedantic -fPIC testLib.c
