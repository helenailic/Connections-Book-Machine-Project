CXX=clang++
INCLUDES=-Iincludes/
CXXFLAGS=-std=c++20 $(INCLUDES) -Wall -Wextra -Werror -pedantic -O0 -gdwarf-4
#CXXFLAGS=-std=c++20 -gdwarf-4 -Wall -Wextra -pedantic -Wno-error=unused $(INCLUDES) -fno-omit-frame-pointer
exec: bin/exec
tests: bin/tests
bin/exec: ./src/driver.cc ./includes/illini_book.hpp ./src/illini_book.cc ./includes/utilities.hpp ./src/utilities.cc 
	$(CXX) $(CXXFLAGS) ./src/driver.cc ./src/illini_book.cc ./src/utilities.cc -o $@

.DEFAULT_GOAL := exec
.PHONY: clean exec tests
clean: rm -fr bin/* obj/*