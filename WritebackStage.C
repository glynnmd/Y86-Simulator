#include <string>
#include <cstdint>
#include "PipeReg.h"
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "F.h"
#include "D.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "WritebackStage.h"
#include "Status.h"
#include "Debug.h"
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

bool WritebackStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
	W * wreg = (W *) pregs[WREG];
	//uint64_t icode = wreg->geticode()->getOutput();
	//RegisterFile * reg = RegisterFile::getInstance();
	if (wreg->getstat()->getOutput() != SAOK)
	{
		return true;
	}

	return false;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void WritebackStage::doClockHigh(PipeReg ** pregs)
{
	W * wreg = (W *) pregs[WREG];
	bool error = false;
	RegisterFile::getInstance()->writeRegister(wreg->getvalE()->getOutput(), wreg->getdstE()->getOutput(), error);
	RegisterFile::getInstance()->writeRegister(wreg->getvalM()->getOutput(), wreg->getdstM()->getOutput(), error);

}

