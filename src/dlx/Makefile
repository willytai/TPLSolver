CXX       = g++
CSRCS     = $(wildcard *.cpp) $(wildcard *.c)
CHDRS     = $(wildcard *.h)

CFLAGS = -g -Wall -DDEBUG_MODE -std=c++11
CFLAGS = -O3 -Wall -std=c++11

DLX: $(CSRCS) $(CHDRS)
	@$(CXX) $(CFLAGS) $(CSRCS) -o DLX

clean:
	rm -rf DLX
