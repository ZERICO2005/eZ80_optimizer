#ifndef ASM_PATTERN_H
#define ASM_PATTERN_H

struct Pattern {
    char const * const src;
    char const * const dst;
    int byte_diff;
};

Pattern asm_pattern[] = {
// no downside patterns (always makes the string smaller)
/* bool testing */ {
    "\tsbc\ta, a\n\tbit\t0, a\n",
    "\tsbc\ta, a\n",
    2,
},
/* bool testing */ {
    "\tsbc\ta, a\n\tinc\ta\n\tbit\t0, a\n",
    "\tsbc\ta, a\n\tinc\ta\n",
    2,
},
/* bool testing */ {
    "\tand\ta, 1\n\tbit\t0, a\n",
    "\tand\ta, 1\n",
    2,
},
/* bool testing */ {
    "\tand\ta, 1\n\tor\ta, a\n",
    "\tand\ta, 1\n",
    1,
},
/* */ {
    "\tex\tde, hl\n\tld\te, iyl\n\tld\td, iyh\n\tex\tde, hl\n\tor\ta, a\n\tsbc.sis\thl, bc\n",
    "\tlea\thl, iy\n\tor\ta, a\n\tsbc.sis\thl, bc\n",
    3,
},
/* */ {
    "\tex\tde, hl\n\tld\te, iyl\n\tld\td, iyh\n\tex\tde, hl\n\tadd.sis\thl, bc\n",
    "\tlea\thl, iy\n\tadd.sis\thl, bc\n",
    3,
},
/* scmpzero */ {
    "\tadd.sis\thl, bc\n\tor\ta, a\n\tsbc.sis\thl, bc\n",
    "\tadd\thl, bc\n\tor\ta, a\n\tsbc.sis\thl, bc\n",
    1,
},

// canonicalization
/* fabsf */ {
    "\tcall\t_fabs\n",
    "\tcall\t_fabsf\n",
    0,
},
/* __sshl */ {
    "\tcall\t__ishl\n",
    "\tcall\t__sshl\n",
    0,
},

// inline functions
/* fabsf */ {
    "\tpush\tde\n\tpush\thl\n\tcall\t_fabsf\n\tpop\tbc\n\tpop\tbc\n",
    "\tres\t7, e\n",
    6,
},

/* fabsf */ {
    "\tcall\t_fabsf\n\tpop\tbc\n\tpop\tbc\n",
    "\tpop\thl\n\tpop\tde\n\tres\t7, e\n",
    2,
},

/* calling convention conversion */ {
    "\tpush\thl\n\tpop\tbc\n\tld\ta, e\n\tpop\thl\n\tpop\thl\n",
    "\tex\t(sp), hl\n\tpop\tbc\n\tpop\thl\n\tld\ta, e\n",
    1,
},

// destructive register loads
/* */ {
    "\tpush\thl\n\tpop\tde\n\tpop\thl\n",
    "\tex\tde, hl\n\tpop\thl\n",
    1,
},
/* */ {
    "\tpush\thl\n\tpop\tde\n\tld\thl,",
    "\tex\tde, hl\n\tld\thl,",
    1,
},
/* */ {
    "\tpush\thl\n\tpop\tde\n\tlea\thl,",
    "\tex\tde, hl\n\tlea\thl,",
    1,
},
/* */ {
    "\tpush\thl\n\tpop\tde\n\tadd\tiy, de\n\tld\thl,",
    "\tex\tde, hl\n\tadd\tiy, de\n\tld\thl,",
    1,
},
/* */ {
    "\tpush\thl\n\tpop\tde\n\tadd\tiy, de\n\tlea\thl,",
    "\tex\tde, hl\n\tadd\tiy, de\n\tlea\thl,",
    1,
},
/* */ {
    "\tpush\thl\n\tpop\tde\n\tpush\tbc\n\tpop\thl\n",
    "\tex\tde, hl\n\tpush\tbc\n\tpop\thl\n",
    1,
},

// stack frame
/* */ {
    "\tpush\thl\n\tpop\tbc\n\tpop\thl\n\tpop\thl\n",
    "\tex\t(sp), hl\n\tpop\tbc\n\tpop\thl\n",
    1,
},
/* */ {
    "\tpush\thl\n\tpop\tiy\n\tpop\thl\n\tpop\thl\n",
    "\tex\t(sp), hl\n\tpop\tiy\n\tpop\thl\n",
    1,
},
/* */ {
    "\tpush\thl\n\tpop\tiy\n\tld\tsp, iy\n",
    "\tpush\thl\n\tpop\tiy\n\tld\tsp, hl\n",
    1,
},


// sign extension
/* */ {
    "\tld\tl, a\n\trlc\tl\n\tsbc\thl, hl\n",
    "\trlca\n\trrca\n\tsbc\thl, hl\n",
    1,
},
/* */ {
    "\trlc\ta\n\tsbc\ta, a\n",
    "\trlca\n\tsbc\ta, a\n",
    1,
},
/* */ {
    "\trrc\ta\n\tsbc\ta, a\n",
    "\trrca\n\tsbc\ta, a\n",
    1,
},

// absolute value probably
/* */ {
    "\tpush\tde\n\tpop\thl\n\tpush\tde\n\tpop\tiy\n\tadd\tiy, iy\n\tsbc\thl, hl\n",
    "\tpush\tde\n\tpop\thl\n\tadd\thl, hl\n\tpush\thl\n\tpop\tiy\n\tsbc\thl, hl\n",
    1,
},

/* */ {
    "\tpush\tbc\n\tpop\thl\n\tpush\tbc\n\tpop\tiy\n\tadd\tiy, iy\n\tsbc\thl, hl\n",
    "\tpush\tbc\n\tpop\thl\n\tadd\thl, hl\n\tpush\thl\n\tpop\tiy\n\tsbc\thl, hl\n",
    1,
},

/* */ {
    "\tpush\thl\n\tpop\tde\n\tpush\tde\n\tpop\tiy\n\tadd\tiy, iy\n\tsbc\thl, hl\n",
    "\tpush\thl\n\tpop\tde\n\tadd\thl, hl\n\tpush\thl\n\tpop\tiy\n\tsbc\thl, hl\n",
    1,
},

/* */ {
    "\tpush\thl\n\tpop\tbc\n\tpush\tbc\n\tpop\tiy\n\tadd\tiy, iy\n\tsbc\thl, hl\n",
    "\tpush\thl\n\tpop\tbc\n\tadd\thl, hl\n\tpush\thl\n\tpop\tiy\n\tsbc\thl, hl\n",
    1,
},

/* */ {
    "\tpush\thl\n\tpop\tiy\n\tadd\tiy, iy\n\tsbc\thl, hl\n",
    "\tadd\thl, hl\n\tpush\thl\n\tpop\tiy\n\tsbc\thl, hl\n",
    1,
},

/* */ {
    "\tlea\thl, iy\n\tadd\tiy, iy\n\tsbc\thl, hl\n",
    "\tadd\tiy, iy\n\tsbc\thl, hl\n",
    3,
},

// lea imm8
/* */ {
    "\tlea\thl, iy\n\tinc\thl\n",
    "\tlea\thl, iy + 1\n",
    1,
},
/* */ {
    "\tlea\thl, iy\n\tdec\thl\n",
    "\tlea\thl, iy - 1\n",
    1,
},
/* */ {
    "\tlea\tde, iy\n\tinc\tde\n",
    "\tlea\tde, iy + 1\n",
    1,
},
/* */ {
    "\tlea\tde, iy\n\tdec\tde\n",
    "\tlea\tde, iy - 1\n",
    1,
},
/* */ {
    "\tlea\tbc, iy\n\tinc\tbc\n",
    "\tlea\tbc, iy + 1\n",
    1,
},
/* */ {
    "\tlea\tbc, iy\n\tdec\tbc\n",
    "\tlea\tbc, iy - 1\n",
    1,
},

// __ishl special
/* UHL <<= 1 */ {
    "\tld\ta, 1\n\tld\tc, a\n\tcall\t__ishl\n\tpush\thl\n\tpop\tbc\n",
    "\tld\ta, 1\n\tadd\thl, hl\n\tpush\thl\n\tpop\tbc\n",
    4,
},
/* UHL <<= 1 */ {
    "\tld\tc, 1\n\tcall\t__ishl\n\tpush\thl\n\tpop\tbc\n",
    "\tadd\thl, hl\n\tpush\thl\n\tpop\tbc\n",
    5,
},
/* UHL <<= 2 */ {
    "\tld\tc, 2\n\tcall\t__ishl\n\tpush\thl\n\tpop\tbc\n",
    "\tadd\thl, hl\n\tadd\thl, hl\n\tpush\thl\n\tpop\tbc\n",
    4,
},

// __ishl common
/* UHL <<= 1 */ {
    "\tld\tc, 1\n\tcall\t__ishl\n",
    "\tld\tc, 1\n\tadd\thl, hl\n",
    3,
},
/* UHL <<= 2 */ {
    "\tld\tc, 2\n\tcall\t__ishl\n",
    "\tld\tc, 2\n\tadd\thl, hl\n\tadd\thl, hl\n",
    2,
},
/* UHL <<= 3 */ {
    "\tld\tc, 3\n\tcall\t__ishl\n",
    "\tld\tc, 3\n\tadd\thl, hl\n\tadd\thl, hl\n\tadd\thl, hl\n",
    1,
},
/* UHL <<= 4 */ {
    "\tld\tc, 4\n\tcall\t__ishl\n",
    "\tld\tc, 4\n\tadd\thl, hl\n\tadd\thl, hl\n\tadd\thl, hl\n\tadd\thl, hl\n",
    0,
},

// pop push __ishl
/* UHL <<= 1 */ {
    "\tld\tc, 1\n\tpush\tde\n\tpop\thl\n\tcall\t__ishl\n",
    "\tld\tc, 1\n\tpush\tde\n\tpop\thl\n\tadd\thl, hl\n",
    3,
},
/* UHL <<= 1 */ {
    "\tld\tc, 1\n\tpush\tbc\n\tpop\thl\n\tcall\t__ishl\n",
    "\tld\tc, 1\n\tpush\tbc\n\tpop\thl\n\tadd\thl, hl\n",
    3,
},

// other __ishl
/* UHL <<= 1 */ {
    "\tld\ta, 1\n\tld\tc, a\n\tcall\t__ishl\n",
    "\tld\ta, 1\n\tld\tc, a\n\tadd\thl, hl\n",
    3,
},
/* UHL <<= 1 */ {
    "\tld\tc, 1\n\tlea\thl, iy\n\tcall\t__ishl\n",
    "\tld\tc, 1\n\tlea\thl, iy\n\tadd\thl, hl\n",
    3,
},
/* UHL <<= 1 */ {
    "\tld\tc, 1\n\tex\tde, hl\n\tcall\t__ishl\n",
    "\tld\tc, 1\n\tex\tde, hl\n\tadd\thl, hl\n",
    3,
},

// __bshl
/* A <<= 1 */ {
    "\tld\tb, 1\n\tcall\t__bshl\n",
    "\tld\tb, 1\n\tadd\ta, a\n",
    3,
},
/* A <<= 2 */ {
    "\tld\tb, 2\n\tcall\t__bshl\n",
    "\tld\tb, 2\n\tadd\ta, a\n\tadd\ta, a\n",
    2,
},
/* A <<= 3 */ {
    "\tld\tb, 3\n\tcall\t__bshl\n",
    "\tld\tb, 3\n\tadd\ta, a\n\tadd\ta, a\n\tadd\ta, a\n",
    1,
},
/* A <<= 4 */ {
    "\tld\tb, 4\n\tcall\t__bshl\n",
    "\tld\tb, 4\n\tadd\ta, a\n\tadd\ta, a\n\tadd\ta, a\n\tadd\ta, a\n",
    0,
},
/* A <<= 6 */ {
    "\tld\tb, 6\n\tcall\t__bshl\n",
    "\tld\tb, 6\n\trrca\n\trrca\n\tand\ta, 192\n",
    0,
},
/* A <<= 7 */ {
    "\tld\tb, 7\n\tcall\t__bshl\n",
    "\tld\tb, 7\n\trrca\n\tand\ta, 128\n",
    1,
},

// __bshru
/* A >>= 1 */ {
    "\tld\tb, 1\n\tcall\t__bshru\n",
    "\tld\tb, 1\n\tsrl\ta\n",
    2,
},
/* A >>= 2 */ {
    "\tld\tb, 2\n\tcall\t__bshru\n",
    "\tld\tb, 2\n\tsrl\ta\n\tsrl\ta\n",
    0,
},
/* A >>= 6 */ {
    "\tld\tb, 6\n\tcall\t__bshru\n",
    "\tld\tb, 6\n\trlca\n\trlca\n\tand\ta, 3\n",
    0,
},
/* A >>= 7 */ {
    "\tld\tb, 7\n\tcall\t__bshru\n",
    "\tld\tb, 7\n\trlca\n\tand\ta, 1\n",
    1,
},

// __bshrs
/* A >>= 1 */ {
    "\tld\tb, 1\n\tcall\t__bshrs\n",
    "\tld\tb, 1\n\tsra\ta\n",
    2,
},
/* A >>= 2 */ {
    "\tld\tb, 2\n\tcall\t__bshrs\n",
    "\tld\tb, 2\n\tsra\ta\n\tsra\ta\n",
    0,
},
/* A >>= 7 */ {
    "\tld\tb, 7\n\tcall\t__bshrs\n",
    "\tld\tb, 7\n\trlca\n\tsbc\ta, a\n",
    2,
},

// __sshru
/* HL >>= 1 */ {
    "\tld\tc, 1\n\tcall\t__sshru\n",
    "\tld\tc, 1\n\tsrl\th\n\trr\tl\n",
    0,
},

// __sshrs
/* HL >>= 1 */ {
    "\tld\tc, 1\n\tcall\t__sshrs\n",
    "\tld\tc, 1\n\tsra\th\n\trr\tl\n",
    0,
},

// __sand
/* UHL &= 0x7FFF */ {
    "\tld.sis\tbc, 32767\n\tcall\t__sand\n",
    "\tld.sis\tbc, 32767\n\tres\t7, h\n",
    2,
},

// __iand
/* UHL &= 0xFFFF */ {
    "\tld\tbc, 65535\n\tcall\t__iand\n",
    "\tld\tbc, 65535\n\tinc\thl\n\tdec.s\thl\n",
    1,
},
/* UHL &= 0xFF */ {
    "\tld\tbc, 255\n\tcall\t__iand\n",
    "\tld\tbc, 255\n\tinc\thl\n\tdec.s\thl\n\tld\th, b\n",
    0,
},

// __imulu

/* UHL *= 3 */ {
    "\tpush\tde\n\tpop\thl\n\tld\tbc, 3\n\tcall\t__imulu\n",
    "\tpush\tde\n\tpop\thl\n\tadd\thl, de\n\tadd\thl, de\n\tld\tbc, 3\n",
    2,
},

/* UHL *= 3 */ {
    "\tld\tbc, 3\n\tpush\tde\n\tpop\thl\n\tcall\t__imulu\n",
    "\tpush\tde\n\tpop\thl\n\tadd\thl, de\n\tadd\thl, de\n\tld\tbc, 3\n",
    2,
},

/* UHL *= 3 */ {
    "\tld\tbc, 3\n\tcall\t__imulu\n",
    "\tpush\thl\n\tpop\tbc\n\tadd\thl, bc\n\tadd\thl, bc\n\tld\tbc, 3\n",
    0,
},

/* UHL *= 3 */ {
    "\tld\tbc, 3\n\tlea\thl, iy\n\tcall\t__imulu\n",
    "\tlea\thl, iy\tpush\thl\n\tpop\tbc\n\tadd\thl, bc\n\tadd\thl, bc\n\tld\tbc, 3\n",
    0,
},

// __imulu cleanup
/* */ {
    "\tld\tbc, 3\n\tpush\thl\n\tpop\tbc\n",
    "\tpush\thl\n\tpop\tbc\n",
    4,
},
/* */ {
    "\tld\tbc, 3\n\tpush\tde\n\tpop\tbc\n",
    "\tpush\tde\n\tpop\tbc\n",
    4,
},
/* */ {
    "\tpush\tbc\n\tpop\thl\n\tpush\thl\n\tpop\tbc\n",
    "\tpush\tbc\n\tpop\thl\n",
    2,
},
/* */ {
    "\tpush\thl\n\tpop\thl\n",
    "",
    2,
},
/* */ {
    "\tex\tde, hl\n\tex\tde, hl\n",
    "\tex\tde, hl\n",
    1,
},
};

#endif /* ASM_PATTERN_H */
