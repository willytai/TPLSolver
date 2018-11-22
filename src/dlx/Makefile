CXX       = g++
CSRCS     = $(wildcard *.cpp) $(wildcard *.c)
CHDRS     = $(wildcard *.h)
COBJS     = $(addsuffix .o, $(basename $(CSRCS)))

CFLAGS = -g -Wall -DDEBUG_MODE -std=c++11
CFLAGS = -O3 -Wall -std=c++11

DLX: $(COBJS)
	@echo "> building DLX... <"
	@$(CXX) $(CFLAGS) $(COBJS) -o DLX

%.o: %.cpp
	@echo "> compling $<... <"
	@$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf DLX
	rm -rf $(COBJS)
