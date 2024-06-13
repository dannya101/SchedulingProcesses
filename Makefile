# CXX Make variable for compiler
CXX = g++
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         include information for symbolic debugger e.g. gdb
CXXFLAGS = -std=c++11 -Wall -g3 -c

# object files
OBJS = log.o scheduler.o main.o

# Program name
PROGRAM = schedule

# Rules format:
# target : dependency1 dependency2 ... dependencyN
#     Command to make target, uses default rules if not specified

# First target is the one executed if you just type make
# make target specifies a specific target
# $^ is an example of a special variable.  It substitutes all dependencies
$(PROGRAM) : $(OBJS)
	$(CXX) -o $(PROGRAM) $^

log.o : log.h log.cpp
	$(CXX) $(CXXFLAGS) log.cpp

scheduler.o : scheduler.h scheduler.cpp ourProcess.h
	$(CXX) $(CXXFLAGS) scheduler.cpp

main.o : log.h scheduler.h main.cpp
	$(CXX) $(CXXFLAGS) main.cpp

clean :
	rm -f *.o $(PROGRAM)
