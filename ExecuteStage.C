#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "E.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "Tools.h"
#include "Instructions.h"
#include "ExecuteStage.h"
#include "Status.h"
#include "ConditionCodes.h"
#include "Debug.h"


/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */

bool ExecuteStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
	//repeat from decode, E.h
	E * ereg = (E *) pregs[EREG];
	M * mreg = (M *) pregs[MREG];

	uint64_t stat = ereg->getstat()->getOutput();
	uint64_t icode = ereg->geticode()->getOutput();
	uint64_t Cnd = 0;
	uint64_t valE = 0;
	uint64_t valA = ereg->getvalA()->getOutput();
	dstE = ereg->getdstE()->getOutput();
	uint64_t dstM = ereg->getdstM()->getOutput();

	//get values
	uint64_t valC = ereg->getvalC()->getOutput();
	uint64_t ifun = ereg->getifun()->getOutput();
	uint64_t valB = ereg->getvalB()->getOutput();

	uint64_t alua = aluA(icode, valA, valC);
	uint64_t alub = aluB(icode, valB);
	uint64_t e_alufun = alufun(icode, ifun);

	//bool set for ALU execution
	bool bo = set_cc(icode);
	valE = ALU(e_alufun,alua, alub, bo);
	Cnd = condtionalLogic(ifun, icode);
	dstE = e_dstE(icode, dstE, Cnd);

	setMInput(mreg, stat, icode, Cnd, valE, valA, dstE, dstM);
	return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void ExecuteStage::doClockHigh(PipeReg ** pregs)
{
   M * mreg = (M *) pregs[MREG];
   mreg->getstat()->normal();
   mreg->geticode()->normal();
   mreg->getCnd()->normal();
   mreg->getvalE()->normal();
   mreg->getvalA()->normal();
   mreg->getdstE()->normal();
   mreg->getdstM()->normal();


}

void ExecuteStage::setMInput(M * mreg, uint64_t stat, uint64_t icode,
                           uint64_t Cnd, uint64_t valE, uint64_t valA,
                           uint64_t dstE, uint64_t dstM)
{
   mreg->getstat()->setInput(stat);
   mreg->geticode()->setInput(icode);
   mreg->getCnd()->setInput(Cnd);
   mreg->getvalE()->setInput(valE);
   mreg->getvalA()->setInput(valA);
   mreg->getdstE()->setInput(dstE);
   mreg->getdstM()->setInput(dstM);
}

/*gets the value from the ALU
 *
 *@param e_icode
 *@param e_valA
 *@param e_valC
 */
uint64_t ExecuteStage::aluA(uint64_t e_icode, uint64_t e_valA, uint64_t e_valC)
{
	switch(e_icode)
	{
		case IRRMOVQ:
		case IOPQ:
			return e_valA;
			break;
		case IIRMOVQ:
		case IRMMOVQ:
		case IMRMOVQ:
			return e_valC;
			break;
		case ICALL:
		case IPUSHQ:
		return -8;
		break;
		case IRET:
		case IPOPQ:
		return 8;
		break;
		default:
		return 0;
		break;

	}
}

uint64_t ExecuteStage::aluB(uint64_t e_icode, uint64_t e_valB)
{
	switch(e_icode)
	{
		case IRMMOVQ:
		case IMRMOVQ:
		case IOPQ:
        case ICALL:
		case IPUSHQ:
		case IRET:
		case IPOPQ:
			return e_valB;
			break;
		case IRRMOVQ:
		case IIRMOVQ:
			return 0;
			break;
		default:
			return 0;
			break;
	}
}

uint64_t ExecuteStage::alufun(uint64_t e_icode,uint64_t e_ifun)
{
	switch(e_icode)
	{
		case IOPQ:
			return e_ifun;
			break;
		default:
			return ADDQ;
			break;
	}
}

bool ExecuteStage::set_cc(uint64_t e_icode)
{
	return(e_icode == IOPQ);
}

uint64_t ExecuteStage::e_dstE(uint64_t e_icode, uint64_t e_dstE, uint64_t e_cnd)
{
	if(e_icode == IRRMOVQ && !e_cnd) return RNONE;
	return e_dstE;
}


uint64_t ExecuteStage::get_dstE()
{
	return dstE;
}

uint64_t ExecuteStage::get_valE()
{
	return valE;
}
 

uint64_t ExecuteStage::condtionalLogic(uint64_t ifun, uint64_t icode)
{
	ConditionCodes * codes = ConditionCodes::getInstance();
	bool flag = false;
	//FLAGS - FRANCIS was here :D
	uint8_t zeroFlag = codes->getConditionCode(ZF,flag);
	uint8_t overFLow = codes->getConditionCode(OF,flag);
	uint8_t signFlag = codes->getConditionCode(SF,flag);
	if(icode == ICMOVXX || icode == IJXX)
	{
		switch(ifun)
		{
			case EQUAL:
				return (zeroFlag == 1);
				break;
			case NOTEQUAL:
				return (zeroFlag == 0);
				break;
			case GREATER:
				return ((signFlag ^ overFLow)  == 0 && zeroFlag == 0); //this might need to be changed
				break;
			case GREATEREQ:
				return((signFlag ^ overFLow) == 0);
				break;
			case LESS:
				return (signFlag ^ overFLow); //this can be cleaned up
				break;
			case LESSEQ:
				return ((signFlag ^ overFLow) || zeroFlag == 1);
				break;
			case UNCOND:
				return 1;
				break;

		}
	}
	
		return 0;
	
}

uint64_t ExecuteStage::ALU(uint64_t alufun, uint64_t aluA, uint64_t aluB, bool condtionscheck)
{
	uint64_t answer = 0;
	bool set = 0;

	switch(alufun)
	{
		case XORQ:
			answer = aluA ^ aluB;
			break;
		case ANDQ:
			answer = aluA & aluB;
			break;
		case ADDQ:
			set = Tools::addOverflow(aluA,aluB);
			answer = aluA + aluB;
			break;
		case SUBQ:
			answer = aluB - aluA;
			set = Tools::subOverflow(aluA, aluB);
			 break;
	}

	if(condtionscheck)
	{
		bool checker = false;
		ConditionCodes * codes = ConditionCodes::getInstance();
		codes->setConditionCode((answer == 0), ZF, checker);
		codes->setConditionCode(Tools::sign(answer),SF, checker);
		codes->setConditionCode(set,OF, checker);
	}

	return answer;
}
