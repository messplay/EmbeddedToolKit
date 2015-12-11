CC=clang++
CFLAGS=-c -g -Wall -Wextra -std=c++11 -I./inc -I/usr/include/eigen3 -Wno-deprecated-register
LDFLAGS= -lc
SOURCES=$(wildcard src/*.cpp) $(wildcard tests/*.cpp)
HEADERS=$(wildcard inc/emblib/*.h)
OBJECTS=$(patsubst src/%.cpp,.obj/%.o,$(wildcard src/*.cpp)) 
TEST_OBJECTS=$(patsubst tests/%.cpp,.obj/tests/%.o,$(wildcard tests/*.cpp))
EXECUTABLE=bin/main

all: inc/emblib/version.h $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(HEADERS) $(OBJECTS) $(TEST_OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(TEST_OBJECTS) -o $@
	#./bin/main
	ar rcs libetk.a $(OBJECTS) 

.obj/%.o:src/%.cpp
	$(CC) $(CFLAGS) $< -o $@

.obj/tests/%.o:tests/%.cpp $(HEADERS) 
	$(CC) $(CFLAGS) $< -o $@

inc/emblib/version.h:
	.obj/autoversion

clean:
	find . -name \*.o -execdir rm {} \;
	rm -f $(EXECUTABLE)
	rm inc/emblib/version.h

