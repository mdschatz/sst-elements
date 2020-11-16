
#ifndef _H_VANADIS_GPR_2_FP
#define _H_VANADIS_GPR_2_FP

#include "inst/vinst.h"
#include "inst/vfpwidth.h"

namespace SST {
namespace Vanadis {

class VanadisGPR2FPInstruction : public VanadisInstruction {
public:
	VanadisGPR2FPInstruction(
		const uint64_t addr,
		const uint32_t hw_thr,
		const VanadisDecoderOptions* isa_opts,
		const uint16_t fp_dest,
		const uint16_t int_src,
		VanadisFPRegisterFormat fp_w
		 ) :
		VanadisInstruction(addr, hw_thr, isa_opts, 1, 0, 1, 0,
			0,
			( (fp_w == VANADIS_FORMAT_FP64 || fp_w == VANADIS_FORMAT_INT64) && ( VANADIS_REGISTER_MODE_FP32 == isa_opts->getFPRegisterMode() ) ) ? 2 : 1,
			0,
			( (fp_w == VANADIS_FORMAT_FP64 || fp_w == VANADIS_FORMAT_INT64) && ( VANADIS_REGISTER_MODE_FP32 == isa_opts->getFPRegisterMode() ) ) ? 2 : 1),
			move_width(fp_w) {

		isa_int_regs_in[0]  = int_src;

		if( ( (fp_w == VANADIS_FORMAT_FP64 || fp_w == VANADIS_FORMAT_INT64) &&
			( VANADIS_REGISTER_MODE_FP32 == isa_opts->getFPRegisterMode() ) ) ) {

			isa_fp_regs_out[0]  = fp_dest;
			isa_fp_regs_out[1]  = fp_dest + 1;
		} else {
			isa_fp_regs_out[0]  = fp_dest;
		}
	}

	virtual VanadisGPR2FPInstruction* clone() {
		return new VanadisGPR2FPInstruction( *this );
	}

	virtual VanadisFunctionalUnitType getInstFuncType() const {
		return INST_INT_ARITH;
	}

	virtual const char* getInstCode() const {
		switch( move_width ) {
		case VANADIS_FORMAT_INT32:
		case VANADIS_FORMAT_FP32:
			return "GPR2FP32";
		case VANADIS_FORMAT_INT64:
		case VANADIS_FORMAT_FP64:
			return "GPR2FP64";
		}
	}

	virtual void printToBuffer(char* buffer, size_t buffer_size) {
		snprintf(buffer, buffer_size, "%s fp-dest isa: %" PRIu16 " phys: %" PRIu16 " <- int-src: isa: %" PRIu16 " phys: %" PRIu16 "\n",
			getInstCode(),
			isa_fp_regs_out[0], phys_fp_regs_out[0],
			isa_int_regs_in[0], phys_int_regs_in[0] );
        }

	virtual void execute( SST::Output* output, VanadisRegisterFile* regFile ) {
		output->verbose(CALL_INFO, 16, 0, "Execute (addr=0x%llx) %s fp-dest isa: %" PRIu16 " phys: %" PRIu16 " <- int-src: isa: %" PRIu16 " phys: %" PRIu16 "\n",
			getInstructionAddress(), getInstCode(),
			isa_fp_regs_out[0], phys_fp_regs_out[0],
			isa_int_regs_in[0], phys_int_regs_in[0] );

		switch( move_width ) {
		case VANADIS_FORMAT_INT32:
		case VANADIS_FORMAT_FP32:
			{
				const int32_t v = regFile->getIntReg<int32_t>( phys_int_regs_in[0] );
				regFile->setFPReg( phys_fp_regs_out[0], v );
			}
			break;
		case VANADIS_FORMAT_INT64:
		case VANADIS_FORMAT_FP64:
			{
				if( VANADIS_REGISTER_MODE_FP32 == isa_options->getFPRegisterMode() ) {
					const int64_t v = regFile->getIntReg<int64_t>( phys_int_regs_in[0] );
					fractureToRegisters<int64_t>( regFile, phys_fp_regs_out[0], phys_fp_regs_out[1], v );
				} else {
					const int64_t v = regFile->getIntReg<int64_t>( phys_int_regs_in[0] );
					regFile->setFPReg( phys_fp_regs_out[0], v );
				}
			}
			break;
		}

		markExecuted();
	}

protected:
	VanadisFPRegisterFormat move_width;

};

}
}

#endif
