//class to perform the combinational logic of
//the Fetch stage
class FetchStage: public Stage
{
   //THIS IS CLEANED
   private:
      void setDInput(D * dreg, uint64_t stat, uint64_t icode, uint64_t ifun, 
                     uint64_t rA, uint64_t rB,
                     uint64_t valC, uint64_t valP);
      uint64_t selectPC(PipeReg ** pregs);
      bool needRegIds(uint64_t f_icode);
      bool needValC(uint64_t f_icode);
      uint64_t predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP);
      uint64_t PCincrement(uint64_t pc, bool reg, bool fvalC);
      void getRegIds(uint64_t f_pc, uint64_t *rA, uint64_t *rB);
      uint64_t buildValC(uint64_t f_pc, bool regId);
      bool instr_valid(uint64_t f_icode);
      uint64_t f_stat(uint64_t f_icode,bool mem_error);
      bool F_stall;
      bool D_stall;
      bool D_bubble;
      bool fstall(uint64_t eicode, uint64_t dicode, uint64_t micode, uint64_t E_dstM, uint64_t srcA, uint64_t srcB);
      bool dstall(uint64_t eicode, uint64_t E_dstM, uint64_t srcA, uint64_t srcB);
      void calculateControlSignals(uint64_t eicode, uint64_t dicode, uint64_t micode, uint64_t E_dstM, uint64_t srcA, uint64_t srcB, uint64_t e_Cnd);
      bool dbubble(uint64_t E_icode, uint64_t dicode, uint64_t micode, uint64_t e_Cnd, uint64_t srcA, uint64_t srcB, uint64_t E_dstM);

   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);

};
