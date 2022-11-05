// Copyright 2009-2022 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2022, NTESS
// All rights reserved.
//
// Portions are copyright of other developers:
// See the file CONTRIBUTORS.TXT in the top level directory
// of the distribution for more information.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#ifndef _H_VANADIS_MUL_HIGH
#define _H_VANADIS_MUL_HIGH

#include "inst/vinst.h"

namespace SST {
namespace Vanadis {

template<typename resultType, typename left_format, typename right_format, typename tmpType, bool signed_left, bool signed_right>
class VanadisMultiplyHighInstruction : public VanadisInstruction
{
public:
    VanadisMultiplyHighInstruction(
        const uint64_t addr, const uint32_t hw_thr, const VanadisDecoderOptions* isa_opts, const uint16_t dest,
        const uint16_t src_1, const uint16_t src_2) :
        VanadisInstruction(addr, hw_thr, isa_opts, 2, 1, 2, 1, 0, 0, 0, 0)
    {

        isa_int_regs_in[0]  = src_1;
        isa_int_regs_in[1]  = src_2;
        isa_int_regs_out[0] = dest;
    }

    VanadisMultiplyHighInstruction* clone() override { return new VanadisMultiplyHighInstruction(*this); }

    VanadisFunctionalUnitType getInstFuncType() const override { return INST_INT_ARITH; }
    const char*               getInstCode() const override
    {
        if( signed_left && signed_right) {
           return "MULH";
        }
        if( ! signed_left && ! signed_right) {
           return "MULHU";
        }
        if( signed_left && ! signed_right) {
           return "MULHSU";
        }
    }

    void printToBuffer(char* buffer, size_t buffer_size) override
    {
        snprintf(
            buffer, buffer_size,
            "%s   %5" PRIu16 " <- %5" PRIu16 " * %5" PRIu16 " (phys: %5" PRIu16 " <- %5" PRIu16 " * %5" PRIu16 ")",
            getInstCode(), isa_int_regs_out[0], isa_int_regs_in[0], isa_int_regs_in[1], phys_int_regs_out[0], phys_int_regs_in[0],
            phys_int_regs_in[1]);
    }

    void execute(SST::Output* output, VanadisRegisterFile* regFile) override
    {
#ifdef VANADIS_BUILD_DEBUG
        output->verbose(
            CALL_INFO, 16, 0,
            "Execute: 0x%llx %s phys: out=%" PRIu16 " in=%" PRIu16 ", %" PRIu16 ", isa: out=%" PRIu16
            " / in=%" PRIu16 ", %" PRIu16 "\n",
            getInstructionAddress(), getInstCode(), phys_int_regs_out[0], phys_int_regs_in[0], phys_int_regs_in[1],
            isa_int_regs_out[0], isa_int_regs_in[0], isa_int_regs_in[1]);
#endif

        const left_format src_1 = regFile->getIntReg<left_format>(phys_int_regs_in[0]);
        const right_format src_2 = regFile->getIntReg<right_format>(phys_int_regs_in[1]);
        tmpType tmp = (tmpType)( src_1 * src_2 ) >> ( sizeof(resultType) * 8 );
        
        regFile->setIntReg<resultType>( phys_int_regs_out[0], (resultType) tmp );

        markExecuted();
    }
};

} // namespace Vanadis
} // namespace SST

#endif
