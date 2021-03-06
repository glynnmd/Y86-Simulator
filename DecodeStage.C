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

   uint64_t stat = dreg->getstat()->getOutput();
   uint64_t icode = dreg->geticode()->getOutput();
   uint64_t ifun = dreg->getifun()->getOutput();
   uint64_t valC = dreg->getvalC()->getOutput();
   uint64_t valP = dreg->getvalP()->getOutput();
   uint64_t dstE = d_dstE(icode, dreg->getrB());
   uint64_t dstM = d_dstM(icode, dreg->getrA());

   //grabs srca/srcb from D register, grabs Cnd from e stage and calculates the control signals
   srcA = d_srcA(icode, dreg->getrA());
   srcB = d_srcB(icode, dreg->getrB());
   uint64_t cnd = e_stage->getCnd();
   calculateControlSignals(ereg->geticode()->getOutput(),ereg->getdstM()->getOutput(), getsrcA(), getsrcB(), cnd);


   uint64_t valA = d_valA(e_stage, mreg, wreg, srcA, m_stage, valP, icode);
   uint64_t valB = d_valB(e_stage, mreg, wreg, srcB, m_stage);
   setEInput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB);
   return false;
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
   if(!E_bubble)
   {
      normal(ereg);
   }
   else
   {
      bubble(ereg);
   }
}
/* doClockHigh
 * applies the appropriate control signal to the D
 * and E register intances
 *
 * @param: ereg
 * @param: stat
 * @param: icode
 * @param: ifun
 * @param: valC
 * @param: valA
 * @param: valB
 * @param: dstE
 * @param: dstM
 * @param: srcA
 * @param: srcB
 */
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


/* d_srcA
 * 
 * @param: d_icode
 * @param: rA
 */
uint64_t DecodeStage::d_srcA(uint64_t d_icode, PipeRegField * rA)
{
   if(d_icode == IRRMOVQ || d_icode == IRMMOVQ || d_icode == IOPQ || d_icode == IPUSHQ)
   {
      return rA->getOutput();
   }
   else if (d_icode == IPOPQ || d_icode == IRET)
   {
      return RSP;
   }
   
    return RNONE;
   
}

/* d_srcB
 * 
 * @param: d_icode
 * @param: rB
 */
uint64_t DecodeStage::d_srcB(uint64_t d_icode, PipeRegField * rB)
{
   if(d_icode == IRMMOVQ || d_icode == IOPQ || d_icode == IMRMOVQ)
   {
      return rB->getOutput();
   }
   else if (d_icode == IPOPQ || d_icode == IRET || d_icode == IPUSHQ || d_icode == ICALL)
   {
      return RSP;
   }
    return RNONE;

}

/* d_icode
 * 
 * @param: d_icode
 * @param: rB
 */

uint64_t DecodeStage::d_dstE(uint64_t d_icode, PipeRegField * rB)
{
   if(d_icode == IRRMOVQ || d_icode == IOPQ || d_icode == IIRMOVQ)
   {
      return rB->getOutput();
   }
   else if (d_icode == IPOPQ || d_icode == IRET || d_icode == IPUSHQ || d_icode == ICALL)
   {
      return RSP;
   }
   
    return RNONE;
}


/* d_dstM
 * 
 * @param: d_icode
 * @param: rA
 */
uint64_t DecodeStage::d_dstM(uint64_t d_icode, PipeRegField * rA)
{
   if(d_icode == IMRMOVQ || d_icode == IPOPQ)
   {
      return rA->getOutput();
   }
    return RNONE;
  
}

/* d_valA
 * Decodes the ValA
 * @param: e_stage
 * @param: m_reg
 * @param: w_reg
 * @param: m_stage
 * @param: valP
 * @param: icode
 */
uint64_t DecodeStage::d_valA(ExecuteStage *e_stage, M * m_reg, W *w_reg, uint64_t d_srcA, MemoryStage *m_stage, uint64_t valP, uint64_t icode)
{

   uint64_t e_dstE = e_stage->get_dstE();
   uint64_t M_dstE = m_reg->getdstE()->getOutput();
   uint64_t W_dstE = w_reg->getdstE()->getOutput();

   uint64_t M_dstM = m_reg->getdstM()->getOutput();
   uint64_t W_dstM = w_reg->getdstM()->getOutput();
   bool flag = false;
   if(icode == ICALL || icode == IJXX)
   {
      return valP;
   }
   if(d_srcA == RNONE)
   {
        return 0; 
   }

   if(d_srcA == e_dstE)
   {
      return e_stage->get_valE();
   }

   if(d_srcA == M_dstM)
   {
      return m_stage->get_valM();
   }

   if(d_srcA == M_dstE)
   {
      return m_reg->getvalE()->getOutput();
   }

   if(d_srcA == W_dstM)
   {
      return w_reg->getvalM()->getOutput();
   }

   if(d_srcA == W_dstE)
   {
      return w_reg->getvalE()->getOutput();
   }


   RegisterFile * reg = RegisterFile::getInstance();
   return reg->readRegister(d_srcA,flag);

}

/* d_valB
 * Decodes the ValB
 * @param: e_stage
 * @param: m_reg
 * @param: w_reg
 * @param: m_stage
 * @param: d_srcB
 */

uint64_t DecodeStage::d_valB(ExecuteStage *e_stage, M * m_reg, W *w_reg, uint64_t d_srcB, MemoryStage *m_stage)
{

   uint64_t e_dstE = e_stage->get_dstE();
   uint64_t M_dstE = m_reg->getdstE()->getOutput();
   uint64_t W_dstE = w_reg->getdstE()->getOutput();

   uint64_t M_dstM = m_reg->getdstM()->getOutput();
   uint64_t W_dstM = w_reg->getdstM()->getOutput();
   bool flag = false;

   if(d_srcB == RNONE)
   {
   	  return 0; 
   }

   if(d_srcB == e_dstE)
   {
      return e_stage->get_valE();
   }

   if(d_srcB == M_dstM)
   {
      return m_stage->get_valM();
   }

   if(d_srcB == M_dstE)
   {
      return m_reg->getvalE()->getOutput();
   }

   if(d_srcB == W_dstM)
   {
      return w_reg->getvalM()->getOutput();
   }

   if(d_srcB == W_dstE)
   {
      return w_reg->getvalE()->getOutput();
   }


   RegisterFile * reg = RegisterFile::getInstance();
   return reg->readRegister(d_srcB,flag);
}

/*
*Fowards srcA
*/
uint64_t DecodeStage::getsrcA()
{
   return srcA;
}

/*
*Fowards srcB
*/
uint64_t DecodeStage::getsrcB()
{
   return srcB;
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
void DecodeStage::calculateControlSignals(uint64_t eicode, uint64_t E_dstM,uint64_t srcA, uint64_t srcB, uint64_t e_Cnd)
{
   E_bubble = (eicode == IJXX && !e_Cnd) || ((eicode == IMRMOVQ || eicode == IPOPQ) && (E_dstM == srcA || E_dstM == srcB));
}

/* 
 * Spit from doClockHigh
 * Normal
 *
 * @param: ereg
 */
void DecodeStage::normal(E * ereg)
{
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

/* 
 * Spit from doClockHigh
 * Bubble
 *
 * @param: ereg
 */
void DecodeStage::bubble(E * ereg)
{
      ereg->getstat()->bubble(SAOK);
      ereg->geticode()->bubble(INOP);
      ereg->getifun()->bubble();
      ereg->getvalC()->bubble();
      ereg->getvalA()->bubble();
      ereg->getvalB()->bubble();
      ereg->getdstE()->bubble(RNONE);
      ereg->getdstM()->bubble(RNONE);
      ereg->getsrcA()->bubble(RNONE);
      ereg->getsrcB()->bubble(RNONE);
}
