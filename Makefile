CC      := mpicc
CXX     := mpicxx

CXXFLAGS    := -std=c++11 -O3 -Wno-invalid-offsetof -Wno-format -DDEBUG
LFLAGS      := -O3 -Wall
ALL         := collisions-1 collisions-2 collisions-3
INDEPENDENT_SOURCES     := Algorithm.cc bounds.cc errors.cc cli.cc FileHeader.cc FileReader.cc Point.cc PMISerializable.cc Verbose.cc
INDEPENDENT_HEADERS     := bounds.hh Algorithm.hh errors.hh cli.hh FileHeader.hh FileReader.hh Point.hh PMISerializable.hh Verbose.hh
OTHER_SOURCES           := main.cpp Sender.cc
OTHER_HEADERS           := version.hh Sender.hh

INDEPENDENT_OBJ=$(patsubst %.cc,%.o,$(INDEPENDENT_SOURCES))

all : $(ALL)
	echo "all"

collisions-1: $(INDEPENDENT_OBJ) main-1.o Sender-1.o
	$(CXX) -o $@ $^ $(LFLAGS)

collisions-2: $(INDEPENDENT_OBJ) main-2.o Sender-2.o
	$(CXX) -o $@ $^ $(LFLAGS)

collisions-3: $(INDEPENDENT_OBJ) main-3.o Sender-3.o
	$(CXX) -o $@ $^ $(LFLAGS)

main-1.o: main.cpp
	$(CXX) -c main.cpp -o main-1.o $(CXXFLAGS) -DALG_V=1

Sender-1.o: Sender.cc
	$(CXX) -c Sender.cc -o Sender-1.o $(CXXFLAGS) -DALG_V=1

main-2.o: main.cpp
	$(CXX) -c main.cpp -o main-2.o $(CXXFLAGS) -DALG_V=2

Sender-2.o: Sender.cc
	$(CXX) -c Sender.cc -o Sender-2.o $(CXXFLAGS) -DALG_V=2

main-3.o: main.cpp
	$(CXX) -c main.cpp -o main-3.o $(CXXFLAGS) -DALG_V=3

Sender-3.o: Sender.cc
	$(CXX) -c Sender.cc -o Sender-3.o $(CXXFLAGS) -DALG_V=3

%.o: %.c
	$(CXX) -c -o $@ $^ $(CXXFLAGS)

clean:
	rm -f *.o *.out *.err res*.txt $(All)

