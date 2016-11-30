CC = g++

COMMON_OBJS = GraphElement.o Edge.o Node.o Graph.o DotParser.o DotWriter.o RandomGenerator.o RandomGraphGenerator.o ASDProjectSolver.o
SOLVER_OBJS = solver_main.o
TIME_TESTER_OBJS = time_tester_main.o ASDProjectTimeTester.o

CFLAGS = -Wall -c -std=c++11 -Icommon/include/
LFLAGS = -Wall -lm

all : solver time_tester cleanobj 

clean : cleanobj cleanbin

solver : $(COMMON_OBJS) $(SOLVER_OBJS)
	mkdir -p bin && $(CC) $(LFLAGS) $(COMMON_OBJS) $(SOLVER_OBJS) -o bin/solver

time_tester : $(COMMON_OBJS) $(TIME_TESTER_OBJS)
	mkdir -p bin && $(CC) $(LFLAGS) $(COMMON_OBJS) $(TIME_TESTER_OBJS) -o bin/time_tester

solver_main.o : solver/src/main.cpp common/include/ASDProjectSolver.h common/include/Graph.h common/include/DotWriter.h
	$(CC) $(CFLAGS) solver/src/main.cpp -o solver_main.o

time_tester_main.o : time_tester/src/main.cpp common/include/Graph.h common/include/RandomGraphGenerator.h common/include/RandomGenerator.h common/include/ASDProjectSolver.h time_tester/include/ASDProjectTimeTester.h common/include/DotWriter.h
	$(CC) $(CFLAGS) -Itime_tester/include time_tester/src/main.cpp -o time_tester_main.o

ASDProjectTimeTester.o : time_tester/include/ASDProjectTimeTester.h time_tester/src/ASDProjectTimeTester.cpp common/include/ASDProjectSolver.h common/include/Graph.h
	$(CC) $(CFLAGS) -Itime_tester/include time_tester/src/ASDProjectTimeTester.cpp

RandomGenerator.o : common/include/RandomGenerator.h common/src/RandomGenerator.cpp
	$(CC) $(CFLAGS) common/src/RandomGenerator.cpp

RandomGraphGenerator.o : common/include/RandomGraphGenerator.h common/src/RandomGraphGenerator.cpp common/include/Graph.h common/include/RandomGenerator.h
	$(CC) $(CFLAGS) common/src/RandomGraphGenerator.cpp

GraphElement.o : common/include/GraphElement.h common/src/GraphElement.cpp common/include/List.h
	$(CC) $(CFLAGS) common/src/GraphElement.cpp

Node.o : common/include/Node.h common/src/Node.cpp common/include/List.h common/include/GraphElement.h common/include/Edge.h
	$(CC) $(CFLAGS) common/src/Node.cpp

Edge.o : common/include/Edge.h common/src/Edge.cpp common/include/List.h common/include/GraphElement.h common/include/Node.h
	$(CC) $(CFLAGS) common/src/Edge.cpp

Graph.o : common/include/Graph.h common/src/Graph.cpp common/include/List.h common/include/Edge.h common/include/Node.h
	$(CC) $(CFLAGS) common/src/Graph.cpp

DotWriter.o : common/include/DotWriter.h common/src/DotWriter.cpp common/include/Graph.h
	$(CC) $(CFLAGS) common/src/DotWriter.cpp

DotParser.o : common/include/DotParser.h common/src/DotParser.cpp common/include/Graph.h
	$(CC) $(CFLAGS) common/src/DotParser.cpp

ASDProjectSolver.o : common/include/ASDProjectSolver.h common/src/ASDProjectSolver.cpp common/include/Graph.h common/include/DotParser.h
	$(CC) $(CFLAGS) common/src/ASDProjectSolver.cpp

.PHONY : cleanobj cleanbin

cleanobj :
	rm -f *.o

cleanbin :
	rm -rf bin