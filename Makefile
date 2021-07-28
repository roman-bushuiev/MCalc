# "make clean" to clean bin file and folder with object files
# "make" or "make compile" to create folder with object files
# "make run" to run the program

CXX=g++
LD=g++
CFLAGS=-Wall -pedantic -Wno-long-long -O2 -g
BIN=./bushurom

SRC=src/
DST=bin/

all:
	make compile

compile: $(BIN)

run: $(BIN)
	# attribute "test" to set return value to 0
	$(BIN) test

clean:
	rm -rf $(BIN) bin

doc:

$(BIN): $(DST)main.o $(DST)mcalcmatrix.o $(DST)mcalcinput.o $(DST)mcalccompute.o
	$(LD) $^ -o $@

$(DST)mcalcinput.o: $(SRC)mcalcinput.cpp $(SRC)mcalcinput.h $(SRC)mcalcmatrix.h
	mkdir -p bin
	$(CXX) $(CFLAGS) -c $< -o $@

$(DST)mcalcmatrix.o: $(SRC)mcalcmatrix.cpp $(SRC)mcalcmatrix.h $(SRC)mcalcinput.h
	mkdir -p bin
	$(CXX) $(CFLAGS) -c $< -o $@

$(DST)mcalccompute.o: $(SRC)mcalccompute.cpp $(SRC)mcalccompute.h $(SRC)mcalcinput.h $(SRC)mcalcmatrix.h
	mkdir -p bin
	$(CXX) $(CFLAGS) -c $< -o $@

$(DST)main.o: $(SRC)main.cpp $(SRC)mcalcinput.h $(SRC)mcalcmatrix.h $(SRC)mcalccompute.h
	mkdir -p bin
	$(CXX) $(CFLAGS) -c $< -o $@
