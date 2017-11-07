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
#include "Instructions.h"
#include "ExecuteStage.h"
#include "MemoryStage.h"
#include "DecodeStage.h"
#include "Status.h"
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

bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   D * dreg = (D*)pregs[DREG];
   E * ereg = (E*)pregs[EREG];
   W * wreg = (W*)pregs[WREG];
    M * mreg = (M*)pregs[MREG];
    ExecuteStage * e_stage = (ExecuteStage*)stages[ESTAGE];
    MemoryStage * m_stage = (MemoryStage*)stages[MSTAGE];

   // i took away f_pc = 0 might want to add back - idk - frabcis

   uint64_t icode = 0, ifun = 0, valC = 0, valA = 0, valB = 0; 
   uint64_t dstE = RNONE, dstM = RNONE, srcA = RNONE, srcB = RNONE, stat = SAOK;

   stat = dreg->getstat()->getOutput();
   icode = dreg->geticode()->getOutput();
   ifun = dreg->getifun()->getOutput();
   valC = dreg->getvalC()->getOutput();
   srcA = d_srcA(icode, dreg->getrA()->getOutput());
   srcB = d_srcB(icode, dreg->getrB()->getOutput());
   dstE = d_dstE(icode, dreg->getrB()->getOutput());
   dstM = d_dstM(icode, dreg->getrA()->getOutput());

   valA = d_valA(e_stage, mreg, wreg, srcA);
   valB = d_valB(e_stage, mreg, wreg, srcB);

   setEInput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB);
   return false;
	//copy and change from FetchStage.h
	//might need D.h
}

/* doClockHigh
 * applies the appropriate control signal to the D
 * and E register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void DecodeStage::doClockHigh(PipeReg ** pregs)
{
   E * ereg = (E *) pregs[EREG];

   ereg->getstat()->normal();
   ereg->geticode()->normal();
   ereg->getifun()->normal();
   ereg->getvalC()->normal();
   ereg->getvalA()->normal();
   ereg->getvalB()->normal();
   ereg->getdstE()->normal();
   ereg->getdstM()->normal();
   ereg->getsrcA()->normal();
   ereg->getsrcB()->normal();
}

void DecodeStage::setEInput(E * ereg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t valC, uint64_t valA,
                           uint64_t valB, uint64_t dstE, uint64_t dstM, 
                           uint64_t srcA, uint64_t srcB)
{
   ereg->getstat()->setInput(stat);
   ereg->geticode()->setInput(icode);
   ereg->getifun()->setInput(ifun);
   ereg->getvalC()->setInput(valC);
   ereg->getvalA()->setInput(valA);
   ereg->getvalB()->setInput(valB);
   ereg->getdstE()->setInput(dstE);
   ereg->getdstM()->setInput(dstM);
   ereg->getsrcA()->setInput(srcA);
   ereg->getsrcB()->setInput(srcB);
}

uint64_t DecodeStage::d_srcA(uint64_t d_icode, uint64_t D_rA)
{
   if(d_icode == IRRMOVQ || d_icode == IRMMOVQ || d_icode == IOPQ || d_icode == IPUSHQ)
   {
      return D_rA;
   }
   else if (d_icode == IPOPQ || d_icode == IRET)
   {
      return RSP;
   }
   else
   {
      return RNONE;
   }
}

uint64_t DecodeStage::d_srcB(uint64_t d_icode, uint64_t D_rB)
{
   if(d_icode == IRMMOVQ || d_icode == IOPQ || d_icode == IMRMOVQ)
   {
      return D_rB;
   }
   else if (d_icode == IPOPQ || d_icode == IRET || d_icode == IPUSHQ || d_icode == ICALL)
   {
      return RSP;
   }
   else
   {
      return RNONE;
   }
}

uint64_t DecodeStage::d_dstE(uint64_t d_icode, uint64_t D_rB)
{
   if(d_icode == IRRMOVQ || d_icode == IOPQ || d_icode == IIRMOVQ)
   {
      return D_rB;
   }
   else if (d_icode == IPOPQ || d_icode == IRET || d_icode == IPUSHQ || d_icode == ICALL)
   {
      return RSP;
   }
   else
   {
      return RNONE;
   }
}

uint64_t DecodeStage::d_dstM(uint64_t d_icode, uint64_t D_rA)
{
   if(d_icode == IMRMOVQ || d_icode == IPOPQ)
   {
      return D_rA;
   }
   else
   {
      return RNONE;
   }
}

uint64_t DecodeStage::d_valA(ExecuteStage *e_stage, M * m_reg, W *w_reg, uint64_t d_srcA)
{

   uint64_t e_dstE = e_stage->get_dstE();
   uint64_t M_dstE = m_reg->getdstE()->getOutput();
   uint64_t W_dstE = w_reg->getdstE()->getOutput();
   bool flag = false;


   if(d_srcA == e_dstE)
   {
      return e_stage->get_valE();
   }

   if(d_srcA == M_dstE)
   {
      return m_reg->getvalE()->getOutput();
   }

   if(d_srcA == W_dstE)
   {
      return w_reg->getvalE()->getOutput();
   }

   RegisterFile * reg = RegisterFile::getInstance();
   return reg->readRegister(d_srcA,flag);
   
}

uint64_t DecodeStage::d_valB(ExecuteStage *e_stage, M * m_reg, W *w_reg, uint64_t d_srcB)
{

   uint64_t e_dstE = e_stage->get_dstE();
   uint64_t M_dstE = m_reg->getdstE()->getOutput();
   uint64_t W_dstE = w_reg->getdstE()->getOutput();
   bool flag = false;


   if(d_srcB == e_dstE)
   {
      return e_stage->get_valE();
   }

   if(d_srcB == M_dstE)
   {
      return m_reg->getvalE()->getOutput();
   }

   if(d_srcB == W_dstE)
   {
      return w_reg->getvalE()->getOutput();
   }

   RegisterFile * reg = RegisterFile::getInstance();
   return reg->readRegister(d_srcB,flag);
}

