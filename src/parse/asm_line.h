#include <cstdint>
#include <string>

using std::string;

enum class z80_arg {
	NONE,
	A,
	B,
	C,
	D,
	E,
	H,
	L,
	IXL,
	IXH,
	IYL,
	IYH,
	BC,
	DE,
	HL,
	IX,
	IY,
	SP,
	AF,
	PHL,
	PDE,
	PBC,
	PIY,
	PIX,
	I,
	MB,
	R,
	PC,
	IMM,
	ADDR,
	PADDR,
};

enum class instrution_suffix {
    NONE,
    S,
    L,
    IS,
    IL,
    SIS,
    SIL,
    LIS,
    LIL,
};

enum class instruction_type {
    VERBATIM,
    LABEL,
	ADC,
	ADD,
	ADD_WORD,
	AND,
	BIT,
	CALL,
	CALL_CC,
	CCF,
	CP,
	CPD,
	CPDR,
	CPI,
	CPIR,
	CPL,
	DAA,
	DEC,
	DEC_WORD,
	DI,
	DJNZ,
	EI,
	EX,
	EXX,
	HALT,
	IM,
	IN,
	IN0,
	INC,
	INC_WORD,
	IND,
	IND2,
	INDM,
	INDMR,
	INDR,
	INDRX,
	INI,
	INI2,
	INI2R,
	INIM,
	INIMR,
	INIR,
	INIRX,
	JP,
	JQ,
	JR,
	LD,
	ST,
	MV,
	LDD,
	LDDR,
	LDI,
	LDIR,
	LEA,
	MLT,
	NEG,
	NOP,
	OR,
	OTD2R,
	OTDM,
	OTDMR,
	OTIR,
	OTIRX,
	OUT,
	OUT0,
	OUTD,
	OUTD2,
	OUTI,
	OUTI2,
	PEA,
	POP,
	PUSH,
	RES,
	RET,
	RETI,
	RETN,
	RL,
	RLA,
	RLC,
	RLCA,
	RLD,
	RR,
	RRA,
	RRC,
	RRCA,
	RRD,
	RSMIX,
	RST,
	SBC,
	SCF,
	SET,
	SLA,
	SLL,
	SLP,
	SRA,
	SRL,
	STMIX,
	SUB,
	TST,
	XOR,
    CRT_ADD,
    CRT_SETFLAG,
    CRT_SUB,
    CRT_MUL,
    CRT_DIVU,
    CRT_DIVS,
    CRT_REMU,
    CRT_REMS,
    CRT_SHL,
    CRT_SHRU,
    CRT_SHRS,
    CRT_CMPU,
    CRT_CMPS,
    CRT_CMPZERO,
    CRT_CTTZ,
    CRT_CTLZ,
    CRT_POPCNT,
};


struct asm_line {
    instruction_type type;
    instrution_suffix suffix;
    z80_arg lhs;
    z80_arg rhs;
    int8_t offset;
    int32_t value;
    string arg;
    asm_line() {
        type = instruction_type::NONE;
        suffix = instrution_suffix::NONE;
        lhs = z80_arg::NONE;
        rhs = z80_arg::NONE;
        offset = 0;
        value = 0;
        arg = "";
    }
    asm_line(string str) {
        arg = str;
        type = instruction_type::VERBATIM;
        suffix = instrution_suffix::NONE;
        lhs = z80_arg::NONE;
        rhs = z80_arg::NONE;
        offset = 0;
        value = 0;
    }
    string print();
};
