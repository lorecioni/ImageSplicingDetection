INC=../include
LIB=../lib
BIN=../bin
FLAGS= -O2 -Wall

$@.c: $(LIB)/libtexturedescriptors.a $@.c 
	cd ../; make; cd app
	gcc $(FLAGS) $@.c -o $(BIN)/$@ -I$(INC) -L$(LIB) -ltexturedescriptors -lm

clean:
	rm $(BIN)/*

