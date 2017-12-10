#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "MemoryStage.h"
#include "Status.h"
#include "Debug.h"
#include "Memory.h"
#include "Instructions.h"

/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */

bool MemoryStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];
   //MemoryStage * m_stage = (MemoryStage*)stages[MSTAGE];
   bool error = false;

   uint64_t icode = mreg->geticode()->getOutput(), valE = mreg->getvalE()->getOutput();
   uint64_t dstE = mreg->getdstE()->getOutput(), dstM = mreg->getdstM()->getOutput();
	//Bruh....
   valM = 0;
   
   uint64_t valA = mreg->getvalA()->getOutput();
   uint64_t addr = mem_addr(icode, valE, valA);
   Memory * mem = Memory::getInstance();
   if(mem_read(icode))
   {
   		valM = mem->getLong(addr, error);
   }
   if (mem_write(icode))
   {
   		mem->putLong(valA, addr, error);
   }

   if(error)
   {
       stat = SADR;
   }
   else
   {
       stat = mreg->getstat()->getOutput();
   } 

   setWInput(wreg, stat, icode, valE, valM, dstE, dstM);
   return false; 
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void MemoryStage::doClockHigh(PipeReg ** pregs)
{
   W * wreg = (W *) pregs[WREG];

   wreg->getstat()->normal();
   wreg->geticode()->normal();
   wreg->getvalE()->normal();
   wreg->getvalM()->normal();
   wreg->getdstE()->normal();
   wreg->getdstM()->normal();


}

void MemoryStage::setWInput(W * wreg, uint64_t stat, uint64_t icode, 
                           uint64_t valE, uint64_t valM,
                           uint64_t dstE, uint64_t dstM)
{
   wreg->getstat()->setInput(stat);
   wreg->geticode()->setInput(icode);
   wreg->getvalE()->setInput(valE);
   wreg->getvalM()->setInput(valM);
   wreg->getdstE()->setInput(dstE);
   wreg->getdstM()->setInput(dstM);
}

uint64_t MemoryStage::mem_addr(uint64_t m_icode,uint64_t m_valE,uint64_t m_valA)
{
	switch(m_icode)
	{
		case IRMMOVQ:
		case IPUSHQ:
		case ICALL:
		case IMRMOVQ:
			return m_valE;
		case IPOPQ:
		case IRET:
			return m_valA;
		default:
		return 0;
	}
}

bool MemoryStage::mem_read(uint64_t m_icode)
{
	return (m_icode == IMRMOVQ || m_icode == IPOPQ || m_icode == IRET);
}

bool MemoryStage::mem_write(uint64_t m_icode)
{
	return (m_icode == IRMMOVQ || m_icode == IPUSHQ || m_icode == ICALL);
}

uint64_t MemoryStage::get_valM() {
    return valM;
}

uint64_t MemoryStage::get_stat()
{
   return stat; 
}