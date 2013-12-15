exec = efwe
src = main.cpp
comp = clang++
flags = -Wall -lgsl -lgslcblas -lm -O3 -DHAVE_INLINE -std=c++11

all: $(exec)

$(exec): $(src)
	$(comp) $(src) -o $(exec) $(flags)
