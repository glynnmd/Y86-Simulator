
class MemoryStage: public Stage
{
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
      void setWInput(W * wreg, uint64_t stat, uint64_t icode, 
                           uint64_t valE, uint64_t valM,
                           uint64_t dstE, uint64_t dstM);
      bool mem_read(uint64_t m_icode);
      bool mem_write(uint64_t m_icode);
      uint64_t mem_addr(uint64_t m_icode,uint64_t m_valE,uint64_t m_valA);
      uint64_t get_valM();
       uint64_t valM;
};
