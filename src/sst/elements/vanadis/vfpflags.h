
#ifndef _H_VANADIS_FPFLAGS
#define _H_VANADIS_FPFLAGS

namespace SST {
namespace Vanadis {

enum class VanadisFPRoundingMode { ROUND_NEAREST, ROUND_TO_ZERO, ROUND_DOWN, ROUND_UP, ROUND_NEAREST_TO_MAX };

class VanadisFloatingPointFlags
{

public:
    VanadisFloatingPointFlags() :
        f_invalidop(false),
        f_divzero(false),
        f_overflow(false),
        f_underflow(false),
        f_inexact(false),
        round_mode(VanadisFPRoundingMode::ROUND_NEAREST)
    {}

    void setInvalidOp() { f_invalidop = true; }
    void setDivZero() { f_divzero = true; }
    void setOverflow() { f_overflow = true; }
    void setUnderflow() { f_underflow = true; }
    void setInexact() { f_inexact = true; }
    void setRoundingMode(VanadisFPRoundingMode newMode) { round_mode = newMode; }

    void clearInvalidOp() { f_invalidop = false; }
    void clearDivZero() { f_divzero = false; }
    void clearOverflow() { f_overflow = false; }
    void clearUnderflow() { f_underflow = false; }
    void clearInexact() { f_inexact = false; }
    void clearRoundingMode() { round_mode = VanadisFPRoundingMode::ROUND_NEAREST; }

    bool                  invalidOp() const { return f_invalidop; }
    bool                  divZero() const { return f_divzero; }
    bool                  overflow() const { return f_overflow; }
    bool                  underflow() const { return f_underflow; }
    bool                  inexact() const { return f_inexact; }
    VanadisFPRoundingMode getRoundingMode() const { return round_mode; }

	 void print(SST::Output* output) {
		output->verbose(CALL_INFO, 16, 0, "-> FP Status: IVLD: %c / DIV0: %c / OF: %c / UF: %c / INXCT: %c\n",
			f_invalidop ? 'y' : 'n',
			f_divzero ? 'y' : 'n',
			f_overflow ? 'y' : 'n',
			f_underflow ? 'y' : 'n',
			f_inexact ? 'y' : 'n');
	 }

protected:
    bool f_invalidop;
    bool f_divzero;
    bool f_overflow;
    bool f_underflow;
    bool f_inexact;

    VanadisFPRoundingMode round_mode;
};

} // namespace Vanadis
} // namespace SST

#endif
