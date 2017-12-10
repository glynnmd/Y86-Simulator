
class ExecuteStage: public Stage
{
   //THIS HAS BEEN CLEANED
   private:
    uint64_t dstE, valE;
    bool M_bubble;
    uint64_t Cnd;
    void setMInput(M * mreg, uint64_t stat, uint64_t icode,
                           uint64_t Cnd, uint64_t valE, uint64_t valA,
                           uint64_t dstE, uint64_t dstM);
    uint64_t aluA(uint64_t e_icode, uint64_t e_valA, uint64_t e_valC);
    uint64_t aluB(uint64_t icode, uint64_t e_valB);
    uint64_t alufun(uint64_t e_icode,uint64_t e_ifun);
    bool set_cc(uint64_t e_icode, uint64_t mstat, uint64_t wstat);
    uint64_t e_dstE(uint64_t e_icode, uint64_t e_dstE, uint64_t e_cnd);
    uint64_t condtionalLogic(uint64_t ifun, uint64_t icode);
    uint64_t ALU(uint64_t alufun, uint64_t aluA, uint64_t aluB, bool condtionscheck);
    bool calculateControlSignals(uint64_t mstat,uint64_t wstat);
    void bubble(M * mreg);
    void normal(M * mreg);
   public:
  
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
     uint64_t get_dstE();
     uint64_t get_valE();
     uint64_t getCnd();
};
