INC=../include
LIB=../lib
BIN=../bin
FLAGS= -O2 -Wall

$@.c: $(LIB)/libcolordescriptors.a $@.c 
	cd ../; make; cd app
	gcc $(FLAGS) $@.c -o $(BIN)/$@ -I$(INC) -L$(LIB) -lcolordescriptors -lm

clean:
	rm $(BIN)/*

