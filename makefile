CC = g++
ifeq ($(std), c++11)
    CFLAGS = -g -c -Wall -std=c++11
else
    CFLAGS = -g -c -Wall -std=c++0x
endif
OBJ = lab5.o Memory.o Tools.o RegisterFile.o \
ConditionCodes.o Loader.o
.C.o:
	$(CC) $(CFLAGS) $< -o $@

lab5: $(OBJ)
	$(CC) $(OBJ) -o lab5

lab5.o: Memory.h RegisterFile.h ConditionCodes.h Loader.h

Tools.o: Tools.h

RegisterFile.o: RegisterFile.h

RegisterFileTester.o: RegisterFile.h

ConditionCodes.o: ConditionCodes.h Tools.h

ConditionCodesTester.o: ConditionCodes.h

Loader.o: Loader.h Memory.h

clean:
	rm -f $(OBJ) lab5

run:
	make clean
	make lab5
	./run.sh
