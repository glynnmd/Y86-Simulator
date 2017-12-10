
class DecodeStage: public Stage
{

   private:
      //THIS IS CLEANED
      uint64_t srcA;
      uint64_t srcB;
      bool E_bubble; 
      uint64_t d_srcA(uint64_t d_icode, PipeRegField *  rA);
      uint64_t d_srcB(uint64_t d_icode, PipeRegField *  rB);
      uint64_t d_dstE(uint64_t d_icode, PipeRegField *  rB);
      uint64_t d_dstM(uint64_t d_icode, PipeRegField *  rA);
      uint64_t d_valA(ExecuteStage *e_stage,  M * m_reg, W * w_reg, uint64_t d_srcA, MemoryStage *m_stage, uint64_t valP, uint64_t icode);
      uint64_t d_valB(ExecuteStage *e_stage, M * m_reg, W *w_reg, uint64_t d_srcB,  MemoryStage *m_stage);
      void calculateControlSignals(uint64_t eicode, uint64_t E_dstM,uint64_t srcA, uint64_t srcB, uint64_t e_Cnd);
      void setEInput(E * ereg, uint64_t stat, uint64_t icode,
                     uint64_t ifun, uint64_t valC, uint64_t valA,
                     uint64_t valB, uint64_t dstE, uint64_t dstM,
                     uint64_t srcA, uint64_t srcB);
      void bubble(E * ereg);
      void normal(E * ereg);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
      uint64_t getsrcA();
      uint64_t getsrcB();
};
