#ifndef ASM_PATTERN_H
#define ASM_PATTERN_H

struct Pattern {
    char const * const src;
    char const * const dst;
};

Pattern asm_pattern[] = {
// no downside patterns (always makes the string smaller)
/* bool testing */ {
    "\tsbc\ta, a\n\tbit\t0, a\n",
    "\tsbc\ta, a\n",
},

/* */ {
    "\trrc\ta\n\tsbc\ta, a\n",
    "\trrca\n\tsbc\ta, a\n",
},

/* */ {
    "\trlc\ta\n\tsbc\ta, a\n",
    "\trlca\n\tsbc\ta, a\n",
},

// canonicalization
/* fabsf */ {
    "\tcall\t_fabs\n",
    "\tcall\t_fabsf\n",
},

// inline functions
/* fabsf */ {
    "\tpush\tde\n\tpush\thl\n\tcall\t_fabsf\n\tpop\tbc\n\tpop\tbc\n",
    "\tres\t7, e\n",
},

/* fabsf */ {
    "\tcall\t_fabsf\n\tpop\tbc\n\tpop\tbc\n",
    "\tpop\thl\n\tpop\tde\n\tres\t7, e\n",
},

/* */ {
    "\tpush\thl\n\tpop\tbc\n\tld\ta, e\n\tpop\thl\n\tpop\thl\n",
    "\tex\t(sp), hl\n\tpop\tbc\n\tpop\thl\n\tld\ta, e\n",
},

// duplicate value register loads
/* */ {
    "\tld\tbc, 0\n\tld\tiy, 0\n",
    "\tld\tiy, 0\n\tlea\tbc, iy\n",
},

// destructive register loads
/* */ {
    "\tpush\thl\n\tpop\tde\n\tpop\thl\n",
    "\tex\tde, hl\n\tpop\thl\n",
},
/* */ {
    "\tpush\thl\n\tpop\tde\n\tld\thl,",
    "\tex\tde, hl\n\tld\thl,",
},
/* */ {
    "\tpush\thl\n\tpop\tde\n\tlea\thl,",
    "\tex\tde, hl\n\tlea\thl,",
},
/* */ {
    "\tld\tiy, 0\n\tlea\thl, iy\n\tld\tiy,",
    "\tld\thl, 0\n\tld\tiy,",
},

// sign extension
/* */ {
    "\tld\tl, a\n\trlc\tl\n\tsbc\thl, hl\n",
    "\trlca\n\trrca\n\tsbc\thl, hl\n",
},

/* */ {
    "\tpush\thl\n\tpop\tiy\n\tadd\tiy, iy\n\tsbc\thl, hl\n",
    "\tadd\thl, hl\tpush\thl\n\tpop\tiy\n\tsbc\thl, hl\n",
},


// __ishl
/* UHL <<= 1 */ {
    "\tld\tc, 1\n\tcall\t__ishl\n",
    "\tld\tc, 1\n\tadd\thl, hl\n"
},
/* UHL <<= 2 */ {
    "\tld\tc, 2\n\tcall\t__ishl\n",
    "\tld\tc, 2\n\tadd\thl, hl\n\tadd\thl, hl\n"
},
/* UHL <<= 3 */ {
    "\tld\tc, 3\n\tcall\t__ishl\n",
    "\tld\tc, 3\n\tadd\thl, hl\n\tadd\thl, hl\n\tadd\thl, hl\n"
},
/* UHL <<= 4 */ {
    "\tld\tc, 4\n\tcall\t__ishl\n",
    "\tld\tc, 4\n\tadd\thl, hl\n\tadd\thl, hl\n\tadd\thl, hl\n\tadd\thl, hl\n"
},

// pop push __ishl
/* UHL <<= 1 */ {
    "\tld\tc, 1\n\tpush\tde\n\tpop\thl\n\tcall\t__ishl\n",
    "\tld\tc, 1\n\tpush\tde\n\tpop\thl\n\tadd\thl, hl\n",
},
/* UHL <<= 1 */ {
    "\tld\tc, 1\n\tpush\tbc\n\tpop\thl\n\tcall\t__ishl\n",
    "\tld\tc, 1\n\tpush\tbc\n\tpop\thl\n\tadd\thl, hl\n",
},

// other __ishl
/* UHL <<= 1 */ {
    "\tld\tc, 1\n\tlea\thl, iy\n\tcall\t__ishl\n",
    "\tld\tc, 1\n\tlea\thl, iy\n\tadd\thl, hl\n",
},

// __bshl
/* A <<= 1 */ {
    "\tld\tb, 1\n\tcall\t__bshl\n",
    "\tld\tb, 1\n\tadd\ta, a\n"
},
/* A <<= 2 */ {
    "\tld\tb, 2\n\tcall\t__bshl\n",
    "\tld\tb, 2\n\tadd\ta, a\n\tadd\ta, a\n"
},
/* A <<= 3 */ {
    "\tld\tb, 3\n\tcall\t__bshl\n",
    "\tld\tb, 3\n\tadd\ta, a\n\tadd\ta, a\n\tadd\ta, a\n"
},
/* A <<= 4 */ {
    "\tld\tb, 4\n\tcall\t__bshl\n",
    "\tld\tb, 4\n\tadd\ta, a\n\tadd\ta, a\n\tadd\ta, a\n\tadd\ta, a\n"
},
/* A <<= 6 */ {
    "\tld\tb, 6\n\tcall\t__bshl\n",
    "\tld\tb, 6\n\trrca\n\trrca\n\tand\ta, 192\n"
},
/* A <<= 7 */ {
    "\tld\tb, 7\n\tcall\t__bshl\n",
    "\tld\tb, 7\n\trrca\n\tand\ta, 128\n"
},

// __bshru
/* A >>= 1 */ {
    "\tld\tb, 1\n\tcall\t__bshru\n",
    "\tld\tb, 1\n\tsrl\ta\n"
},
/* A >>= 2 */ {
    "\tld\tb, 2\n\tcall\t__bshru\n",
    "\tld\tb, 2\n\tsrl\ta\n\tsrl\ta\n"
},
/* A >>= 6 */ {
    "\tld\tb, 6\n\tcall\t__bshru\n",
    "\tld\tb, 6\n\trlca\n\trlca\n\tand\ta, 3\n"
},
/* A >>= 7 */ {
    "\tld\tb, 7\n\tcall\t__bshru\n",
    "\tld\tb, 7\n\trlca\n\tand\ta, 1\n"
},

// __bshrs
/* A >>= 1 */ {
    "\tld\tb, 1\n\tcall\t__bshrs\n",
    "\tld\tb, 1\n\tsra\ta\n"
},
/* A >>= 2 */ {
    "\tld\tb, 2\n\tcall\t__bshrs\n",
    "\tld\tb, 2\n\tsra\ta\n\tsra\ta\n"
},
/* A >>= 7 */ {
    "\tld\tb, 7\n\tcall\t__bshrs\n",
    "\tld\tb, 7\n\trlca\n\tsbc\ta, a\n"
},

// __sand
/* UHL &= 0x7FFF */ {
    "\tld.sis\tbc, 32767\n\tcall\t__sand\n",
    "\tld.sis\tbc, 32767\n\tres\t7, h\n"
},

// __iand
/* UHL &= 0xFFFF */ {
    "\tld\tbc, 65535\n\tcall\t__iand\n",
    "\tld\tbc, 65535\n\tinc\thl\n\tdec.s\thl\n"
},
/* UHL &= 0xFF */ {
    "\tld\tbc, 255\n\tcall\t__iand\n",
    "\tld\tbc, 255\n\tinc\thl\n\tdec.s\thl\n\tld\th, b\n"
},

// __land
/* A >>= 1 */ {
    "\tld\tbc, 255\n\txor\ta, a\n\tcall\t__land\n",
    "\tld\tbc, 255\n\t\n\txor\ta, a\n\tinc\thl\n\tdec.s\thl\n\tld\th, b\n\tld\te, b\n"
},

// __imulu
/* UHL *= 3 */ {
    "\tld\tbc, 3\n\tcall\t__imulu\n",
    "\tpush\thl\n\tpop\tbc\n\tadd\thl, bc\n\tadd\thl, bc\n\tld\tbc, 3\n"
},

// __imulu cleanup
/* */ {
    "\tld\tbc, 3\n\tpush\thl\n\tpop\tbc\n",
    "\tpush\thl\n\tpop\tbc\n",
},
/* */ {
    "\tpush\tbc\n\tpop\thl\n\tpush\thl\n\tpop\tbc\n",
    "\tpush\tbc\n\tpop\thl\n",
},
};

#endif /* ASM_PATTERN_H */
