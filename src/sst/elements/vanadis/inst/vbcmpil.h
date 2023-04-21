// Copyright 2009-2023 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2023, NTESS
// All rights reserved.
//
// Portions are copyright of other developers:
// See the file CONTRIBUTORS.TXT in the top level directory
// of the distribution for more information.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#ifndef _H_VANADIS_BRANCH_REG_COMPARE_IMM_LINK
#define _H_VANADIS_BRANCH_REG_COMPARE_IMM_LINK

#include "inst/vcmptype.h"
#include "inst/vspeculate.h"
#include "util/vcmpop.h"

namespace SST {
namespace Vanadis {

template <typename register_format, VanadisRegisterCompareType compareType>
class VanadisBranchRegCompareImmLinkInstruction : public VanadisSpeculatedInstruction
{
public:
    VanadisBranchRegCompareImmLinkInstruction(
        const uint64_t addr, const uint32_t hw_thr, const VanadisDecoderOptions* isa_opts, const uint64_t ins_width,
        const uint16_t src_1, const register_format imm, const int64_t offst, const uint16_t link_reg,
        const VanadisDelaySlotRequirement delayT) :
        VanadisSpeculatedInstruction(addr, hw_thr, isa_opts, ins_width, 1, 1, 1, 1, 0, 0, 0, 0, delayT),
        imm_value(imm),
        offset(offst)
    {
        isa_int_regs_in[0]  = src_1;
        isa_int_regs_out[0] = link_reg;
    }

    VanadisBranchRegCompareImmLinkInstruction* clone() override
    {
        return new VanadisBranchRegCompareImmLinkInstruction(*this);
    }
    const char* getInstCode() const override {
        if(sizeof(register_format) == 4) {
            return "BCMPIL32";
        } else if (sizeof(register_format) == 8) {
            return "BCMPIL64"; 
        } else {
            return "BCMPILER";
        }
    }

    void printToBuffer(char* buffer, size_t buffer_size) override
    {
        std::ostringstream ss;
        ss << getInstCode();
        ss << " isa-in: "     << isa_int_regs_in[0] << " / phys-in: "    << phys_int_regs_in[0];
        ss << " / imm: " << imm_value << " / offset: " << offset;
        ss << " / isa-link: " << isa_int_regs_out[0] << " / phys-link: " << phys_int_regs_out[0];
        strncpy( buffer, ss.str().c_str(), buffer_size );
    }

    void execute(SST::Output* output, VanadisRegisterFile* regFile) override
    {
#ifdef VANADIS_BUILD_DEBUG
        if(output->getVerboseLevel() >= 16) {
            std::ostringstream ss;
            ss << "Execute: 0x" << std::hex << getInstructionAddress() << std::dec << " " << getInstCode();
            ss << " isa-in: "     <<  isa_int_regs_in[0]  << " / phys-in: "   << phys_int_regs_in[0];
            ss << " / imm: " << imm_value << " / offset: " << offset;
            ss << " / isa-link: " <<  isa_int_regs_out[0] << " / phys-link: " << phys_int_regs_out[0];
            output->verbose( CALL_INFO, 16, 0, "%s\n", ss.str().c_str());
        }
#endif
        bool compare_result = false;

        compare_result =
                registerCompareImm<compareType, register_format>(regFile, this, output, phys_int_regs_in[0], imm_value);

        if ( compare_result ) {
            takenAddress = (uint64_t)(((int64_t)getInstructionAddress()) + offset);

            // Update the link address
            // The link address is the address of the second instruction after the
            // branch (so the instruction after the delay slot)
            const uint64_t link_address = calculateStandardNotTakenAddress();
            regFile->setIntReg<uint64_t>(phys_int_regs_out[0], link_address);
        }
        else {
            takenAddress = calculateStandardNotTakenAddress();
        }

        markExecuted();
    }

protected:
    const int64_t offset;
    const register_format imm_value;
};

} // namespace Vanadis
} // namespace SST

#endif
