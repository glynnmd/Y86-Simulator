
class DecodeStage: public Stage
{
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void setEInput(E * ereg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t valC, uint64_t valA,
                           uint64_t valB, uint64_t dstE, uint64_t dstM, 
                           uint64_t srcA, uint64_t srcB);
      void doClockHigh(PipeReg ** pregs);
      uint64_t d_srcA(uint64_t d_icode, uint64_t D_rA);
      uint64_t d_srcB(uint64_t d_icode, uint64_t D_rB);
      uint64_t d_dstE(uint64_t d_icode, uint64_t D_rB);
      uint64_t d_dstM(uint64_t d_icode, uint64_t D_rA);
      uint64_t d_valA(ExecuteStage *e_stage,  M * m_reg, W * w_reg, uint64_t d_srcA);
      uint64_t d_valB(ExecuteStage *e_stage, M * m_reg, W *w_reg, uint64_t d_srcB);
};
