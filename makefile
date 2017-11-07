CC = g++
ifeq ($(std), c++11)
    CFLAGS = -g -c -Wall -std=c++11
else
    CFLAGS = -g -c -Wall -std=c++0x
endif
OBJ = yess.o FetchStage.o DecodeStage.o ExecuteStage.o MemoryStage.o WritebackStage.o F.o D.o E.o M.o W.o Simulate.o PipeReg.o PipeRegField.o Memory.o Tools.o RegisterFile.o ConditionCodes.o Loader.o
.C.o:
	$(CC) $(CFLAGS) $< -o $@

yess: $(OBJ)

yess.o: Memory.h RegisterFile.h ConditionCodes.h Loader.h FetchStage.h DecodeStage.h ExecuteStage.h MemoryStage.h WritebackStage.h F.h D.h E.h M.h W.h

FetchStage.o: RegisterFile.h PipeRegField.h ConditionCodes.h Loader.h FetchStage.h DecodeStage.h ExecuteStage.h MemoryStage.h WritebackStage.h F.h D.h E.h M.h W.h

DecodeStage.o: RegisterFile.h PipeRegField.h PipeReg.h F.h D.h E.h M.h W.h Stage.h DecodeStage.h Status.h Debug.h

ExecuteStage.o: RegisterFile.h PipeRegField.h PipeReg.h F.h D.h E.h M.h W.h Stage.h ExecuteStage.h

MemoryStage.o: RegisterFile.h PipeRegField.h PipeReg.h F.h D.h E.h M.h W.h Stage.h MemoryStage.h Status.h  Debug.h

WritebackStage.o: RegisterFile.h  PipeRegField.h PipeReg.h F.h D.h E.h M.h W.h Stage.h WritebackStage.h Status.h Debug.h

F.o: PipeRegField.h PipeReg.h F.h

D.o: PipeRegField.h PipeReg.h D.h

E.o: PipeRegField.h PipeReg.h E.h

M.o: PipeRegField.h PipeReg.h M.h

W.o: PipeRegField.h PipeReg.h W.h

Simulate.o: PipeRegField.h PipeReg.h F.h D.h E.h M.h W.h Stage.h FetchStage.h DecodeStage.h ExecuteStage.h MemoryStage.h WritebackStage.h Simulate.h Memory.h RegisterFile.h ConditionCodes.h

PipeReg.o: PipeReg.h

PipeRegField.o: PipeRegField.h

Memory.o: Memory.h Tools.h

Tools.o: Tools.h

RegisterFile.o: RegisterFile.h

RegisterFileTester.o: RegisterFile.h

ConditionCodes.o: ConditionCodes.h Tools.h

Loader.o: Loader.h Memory.h

clean:
	rm $(OBJ) lab8

run:
	make clean
	make yess
	./run.sh
