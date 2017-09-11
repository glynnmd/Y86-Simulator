CC = g++
ifeq ($(std), c++11)
    CFLAGS = -g -c -Wall -std=c++11
else
    CFLAGS = -g -c -Wall -std=c++0x 
endif
OBJ = lab4.o MemoryTester.o Memory.o Tools.o RegisterFile.o \
RegisterFileTester.o ConditionCodes.o ConditionCodesTester.o
.C.o:
	$(CC) $(CFLAGS) $< -o $@

lab4: $(OBJ)

clean:
	rm $(OBJ) lab4

run:
	make clean
	make lab4
	./run.sh

