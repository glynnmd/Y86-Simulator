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
#include "FetchStage.h"
#include "MemoryStage.h"
#include "ExecuteStage.h"
#include "Status.h"
#include "Debug.h"
#include "Memory.h"
#include "Tools.h"
#include "Instructions.h"
#include "DecodeStage.h"


/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool FetchStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];
   E * ereg = (E *) pregs[EREG];
   M * mreg = (M *) pregs[MREG];
   DecodeStage * d_stage = (DecodeStage*)stages[DSTAGE];
   ExecuteStage * e_stage = (ExecuteStage*)stages[ESTAGE];
   uint64_t  valC = 0, valP = 0;
   uint64_t rA = RNONE, rB = RNONE;
   bool err = false; 

   int32_t f_pc = selectPC(pregs);

   uint64_t icode = Memory::getInstance()->getByte(f_pc, err);
   uint64_t ifun = Tools::getBits(icode, 0, 3);
   icode = Tools::getBits(icode, 4, 7);
  
	uint64_t stat = f_stat(icode,err);

	if(err)
   {
		icode = INOP;
		ifun = FNONE;
	}

   bool needreg = needRegIds(icode);
   bool needval = needValC(icode);
   if(needreg)
   {
      uint8_t gByte = Memory::getInstance()->getByte(f_pc + 1, err);
      getRegIds(gByte, &rA, &rB);
   }
   if(needval)
   {     
      valC = buildValC(f_pc, needreg);
   }
   valP = PCincrement(f_pc, needreg, needval);
   uint64_t predictionPC = predictPC(icode, valC, valP);

   freg->getpredPC()->setInput(predictionPC);

   uint64_t dsrcA = d_stage->getsrcA();
   uint64_t dsrcB = d_stage->getsrcB();
   uint64_t cnd = e_stage->getCnd();
   calculateControlSignals(ereg->geticode()->getOutput(), dreg->geticode()->getOutput(), mreg->geticode()->getOutput(), ereg->getdstM()->getOutput(), dsrcA, dsrcB, cnd);

   setDInput(dreg, stat, icode, ifun, rA, rB, valC, valP);
   return false; 
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void FetchStage::doClockHigh(PipeReg ** pregs)
{
   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];

   if (!F_stall)
   {
   		freg->getpredPC()->normal();
   }
   if (D_bubble)
   {
   		dreg->getstat()->bubble(SAOK);
   		dreg->geticode()->bubble(INOP);
   		dreg->getifun()->bubble();
   		dreg->getrA()->bubble(RNONE);
   		dreg->getrB()->bubble(RNONE);
   		dreg->getvalC()->bubble();
   		dreg->getvalP()->bubble();
   }
   else if (!D_stall)
   {
   		dreg->getstat()->normal();
   		dreg->geticode()->normal();
   		dreg->getifun()->normal();
   		dreg->getrA()->normal();
   		dreg->getrB()->normal();
   		dreg->getvalC()->normal();
   		dreg->getvalP()->normal();
	}
}

/* setDInput
 * provides the input to potentially be stored in the D register
 * during doClockHigh
 *
 * @param: dreg - pointer to the D register instance
 * @param: stat - value to be stored in the stat pipeline register within D
 * @param: icode - value to be stored in the icode pipeline register within D
 * @param: ifun - value to be stored in the ifun pipeline register within D
 * @param: rA - value to be stored in the rA pipeline register within D
 * @param: rB - value to be stored in the rB pipeline register within D
 * @param: valC - value to be stored in the valC pipeline register within D
 * @param: valP - value to be stored in the valP pipeline register within D
*/
void FetchStage::setDInput(D * dreg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t rA, uint64_t rB,
                           uint64_t valC, uint64_t valP)
{
   dreg->getstat()->setInput(stat);
   dreg->geticode()->setInput(icode);
   dreg->getifun()->setInput(ifun);
   dreg->getrA()->setInput(rA);
   dreg->getrB()->setInput(rB);
   dreg->getvalC()->setInput(valC);
   dreg->getvalP()->setInput(valP);
}
/*
 * selectPC:
 * seects the vaue of the next pc 
 * .
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * 
 *         
 */
uint64_t FetchStage::selectPC(PipeReg ** pregs)
{
   F * freg = (F *) pregs[FREG];
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];

   if(mreg->geticode()->getOutput() == IJXX && !mreg->getCnd()->getOutput())
   {
      return mreg->getvalA()->getOutput();
   }
   else if(wreg->geticode()->getOutput() == IRET)
   {
      return wreg->getvalM()->getOutput();
   }
   else
   {
      return freg->getpredPC()->getOutput();
   }
}

/*
 * needRegIds
 * Determines if the instruction uses a register or not
 * .
 *
 * @param: f_icode the icdoe in the instrution
 * 
 *         
 */
bool FetchStage::needRegIds(uint64_t f_icode)
{
   if(f_icode == IRRMOVQ || f_icode == IOPQ
      || f_icode == IPUSHQ || f_icode == IPOPQ 
      || f_icode == IIRMOVQ || f_icode == IRMMOVQ || f_icode == IMRMOVQ)
   {
      return true;
   }
   return false;
}


/*
 * needValC
 * Determines if the instruction has a desnt(vaCl)
 * .
 *
 * @param: f_icode the icdoe in the instrution
 * 
 *         
 */

bool FetchStage::needValC(uint64_t f_icode)
{
   if(f_icode == IIRMOVQ || f_icode == IRMMOVQ
      || f_icode == IMRMOVQ || f_icode == IJXX 
      || f_icode == ICALL)
   {
      return true;
   }
   return false;
}

/*
 * predictPC
 * Determines if the instrution is a jump or not
 * aways take the jump and fix it later
 *
 * @param: f_icode the icdoe in the instrution
 * @param: f_valC the desntion or m
 * @param: f_valP the next address 
 *         
 */
uint64_t FetchStage::predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP)
{
   if (f_icode == IJXX || f_icode == ICALL)       
   {
      return f_valC;
   }
   else
   {
      return f_valP;
   }
}

/*
 * PCincrement
 * increments the PC
 * 
 *
 * @param: pc the current pc
 * @param: reg 
 * @param: f_valC the denst 
 *         
 */

uint64_t FetchStage::PCincrement(uint64_t pc, bool reg, bool fvalC)
{
   return pc + 1 + reg + (8 * fvalC);
}

/*
 * getRegIds
 * gets the reg ID
 * 
 *
 * @param: rbyte register byte
 * @param: rA
 * @param: rB 
 *         
 */
void FetchStage::getRegIds(uint64_t rbyte, uint64_t *rA, uint64_t *rB)
{
   *rA = Tools::getBits(rbyte, 4, 7);
   *rB = Tools::getBits(rbyte, 0, 3);
}

/*
 * buildValc
 * buids the ValC
 * 
 *
 * @param: f_pc fetch stage PC
 * @param: regId
 * 
 *         
 */
uint64_t FetchStage::buildValC(uint64_t f_pc, bool regId)
{
   Memory * memory = Memory::getInstance();
   bool error = false; 

   uint8_t c[8];
   if(regId == true)
   {
      for (int i = 0; i < 8; i++)
      {
         c[i] = memory->getByte(f_pc + i + 2, error);
      }
   }
   else
   {
      for (int i = 0; i < 8; i++)
      {
         c[i] = memory->getByte(f_pc + i + 1, error);
      }
   }
   return Tools::buildLong(c);
}
/*
 * buildValc
 * buids the ValC
 * @param: f_icode the icode fetched in the fetch stage        
 */
bool FetchStage::instr_valid(uint64_t f_icode)
{
   if(f_icode == INOP || f_icode == IHALT || f_icode == IIRMOVQ || f_icode == IRMMOVQ
      || f_icode == IRRMOVQ || f_icode == IMRMOVQ || f_icode == IOPQ || f_icode == IJXX
      || f_icode == ICALL || f_icode == IRET || f_icode == IPUSHQ || f_icode ==IPOPQ)
   {
      return true;
   }
   return false;
}
 /*
 * buildValc
 * buids the ValC
 * @param: f_icode the icode fetched in the fetch stage   
 * @param mem_error if there is a error or not     
 */

uint64_t FetchStage::f_stat(uint64_t f_icode,bool mem_error)
{
   //I tired making this a swtch but some test failed ifk why - Francis  11/25/2017
   if(mem_error)
   {
      return SADR;
   }
   else if(!instr_valid(f_icode))
   {
      return SINS;
   }
   else if(f_icode == IHALT)
   {
      return SHLT;
   }
   else
   {
      return SAOK;
   }
}
 /*
 * fstall
 * stals the fetch stage
 * buids 
 * @param: eicode  
 * @param: dicode    
 * @param: micode
 * @param: e_dstm
 * @param: srcA
 * @param: srcB
 */

bool FetchStage::fstall(uint64_t eicode, uint64_t dicode, uint64_t micode, uint64_t E_dstM, uint64_t srcA, uint64_t srcB)
{
   return ((eicode == IMRMOVQ || eicode == IPOPQ) && (E_dstM == srcA || E_dstM == srcB)) || (IRET == eicode || IRET == dicode || IRET == micode);
}
 /*
 * dstall
 * stals the decodestage
 * 
 * @param: eicode  
 * @param: dicode    
 * @param: micode
 * @param: e_dstm
 * @param: srcA
 * @param: srcB
 */
bool FetchStage::dstall(uint64_t eicode, uint64_t E_dstM, uint64_t srcA, uint64_t srcB)
{
   return (eicode == IMRMOVQ || eicode == IPOPQ) && (E_dstM == srcA || E_dstM == srcB);
}
 /*
 * calculateControlSignals
 * 
 * 
 * @param: eicode  
 * @param: dicode    
 * @param: micode
 * @param: e_dstm
 * @param: srcB
 * @param: e_Cnd
 */
void FetchStage::calculateControlSignals(uint64_t eicode, uint64_t dicode, uint64_t micode, uint64_t E_dstM, uint64_t srcA, uint64_t srcB, uint64_t e_Cnd)
{
	F_stall = fstall(eicode, dicode, micode, E_dstM, srcA, srcB);
	D_stall = dstall(eicode,E_dstM, srcA, srcB);
	D_bubble = dbubble(eicode, dicode, micode, e_Cnd, srcA, srcB, E_dstM);
}

 /*
 * dbubbe
 * 
 * 
 * @param: eicode  
 * @param: dicode    
 * @param: micode
 * @param: e_dstm
 * @param: srcB
 * @param: E_dstM
 */
bool FetchStage::dbubble(uint64_t E_icode, uint64_t dicode, uint64_t micode, uint64_t e_Cnd, uint64_t srcA, uint64_t srcB, uint64_t E_dstM)
{
	return (E_icode == IJXX && !e_Cnd) || (!((E_icode == IMRMOVQ || E_icode == IPOPQ) && (E_dstM == srcA || E_dstM == srcB)) && (IRET == E_icode || IRET == dicode || IRET == micode));
}