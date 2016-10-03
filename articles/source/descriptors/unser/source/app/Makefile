INC=../include
LIB=../lib
BIN=../bin
FLAGS= -O2 -Wall

$@.c: $(LIB)/libdescriptors.a $@.c 
	cd ../; make; cd app
	gcc $(FLAGS) $@.c -o $(BIN)/$@ -I$(INC) -L$(LIB) -ldescriptors -lm

clean:
	rm $(BIN)/*

