#include <cstdint>
#include <cassert>
#include <limits>
#include <bit>

using std::countl_zero;
using std::countl_one;
using std::countr_zero;
using std::countr_one;
using std::has_single_bit;
using std::bit_floor;
using std::popcount;

template<typename T>
T bit_reverse(T x);

template<>
inline uint8_t bit_reverse(uint8_t x) {
    return __builtin_bitreverse8(x);
}

template<>
inline uint16_t bit_reverse(uint16_t x) {
    return __builtin_bitreverse16(x);
}

template<>
inline uint32_t bit_reverse(uint32_t x) {
    return __builtin_bitreverse32(x);
}

template<>
inline uint64_t bit_reverse(uint64_t x) {
    return __builtin_bitreverse64(x);
}

template<typename T>
T swap_byte_order(T x);

template<>
inline uint8_t swap_byte_order(uint8_t x) {
    return x;
}

template<>
inline uint16_t swap_byte_order(uint16_t x) {
    return __builtin_bswap16(x);
}

template<>
inline uint32_t swap_byte_order(uint32_t x) {
    return __builtin_bswap32(x);
}

template<>
inline uint64_t swap_byte_order(uint64_t x) {
    return __builtin_bswap64(x);
}

#if 1

typedef unsigned _BitInt(24) uint24_t;
typedef signed _BitInt(24) int24_t;
typedef unsigned _BitInt(48) uint48_t;
typedef signed _BitInt(48) int48_t;

#define UINT24_C(x) (static_cast<uint24_t>(UINT32_C(x)))
#define INT24_C(x) (static_cast<int24_t>(INT32_C(x)))
#define UINT48_C(x) (static_cast<uint48_t>(UINT64_C(x)))
#define INT48_C(x) (static_cast<int48_t>(INT64_C(x)))

inline int countl_zero(uint24_t x) {
	return (x == 0) ? 24 : (countl_zero<uint32_t>(x) - 8);
}
inline int countl_zero(uint48_t x) {
	return (x == 0) ? 48 : (countl_zero<uint64_t>(x) - 16);
}

inline int countl_one(uint24_t x) {
	return countl_zero(~x);
}
inline int countl_one(uint48_t x) {
	return countl_zero(~x);
}

inline int countr_zero(uint24_t x) {
	return (x == 0) ? 24 : countr_zero<uint32_t>(x);
}
inline int countr_zero(uint48_t x) {
	return (x == 0) ? 48 : countr_zero<uint64_t>(x);
}

inline int countr_one(uint24_t x) {
	return countr_one<uint32_t>(x);
}
inline int countr_one(uint48_t x) {
	return countr_one<uint64_t>(x);
}

inline int popcount(uint24_t x) {
	return popcount(static_cast<uint32_t>(x));
}
inline int popcount(uint48_t x) {
	return popcount(static_cast<uint64_t>(x));
}

inline bool has_single_bit(uint24_t x) {
	return (x && !(x & (x - 1)));
}
inline bool has_single_bit(uint48_t x) {
	return (x && !(x & (x - 1)));
}

inline uint24_t bit_floor(uint24_t x) {
	return static_cast<uint24_t>(bit_floor(static_cast<uint32_t>(x)));
}
inline uint48_t bit_floor(uint48_t x) {
	return static_cast<uint48_t>(bit_floor(static_cast<uint64_t>(x)));
}

template<>
inline uint24_t bit_reverse(uint24_t x) {
    return static_cast<uint24_t>(__builtin_bitreverse32(static_cast<uint32_t>(x)) >> 8);
}

template<>
inline uint48_t bit_reverse(uint48_t x) {
    return static_cast<uint48_t>(__builtin_bitreverse64(static_cast<uint64_t>(x)) >> 16);
}

template<>
inline uint24_t swap_byte_order(uint24_t x) {
    return static_cast<uint24_t>(__builtin_bswap32(static_cast<uint32_t>(x)) >> 8);
}

template<>
inline uint48_t swap_byte_order(uint48_t x) {
    return static_cast<uint48_t>(__builtin_bswap64(static_cast<uint64_t>(x)) >> 16);
}

#else

typedef uint32_t uint24_t;
typedef int32_t int24_t;
typedef uint64_t uint48_t;
typedef int64_t int48_t;

#endif

class KBool {
    bool kfalse;
    bool ktrue;
    bool to_bool() const {
        assert(kfalse != ktrue);
        return ktrue;
    }
    void clear_conflict() {
        if (ktrue && kfalse) {
            ktrue = false;
            kfalse = false;
        }
    }
public:
    KBool& set_unknown() {
        kfalse = false;
        ktrue = false;
        return *this;
    }
    KBool& set_state(const KBool& b) {
        *this = b;
        return *this;
    }
    KBool& set_state(bool b) {
        kfalse = !b;
        ktrue = b;
        return *this;
    }
    KBool& set_true() {
        set_state(true);
        return *this;
    }
    KBool& set_false() {
        set_state(false);
        return *this;
    }
    KBool() {
        set_unknown();
    }
    KBool(bool b) {
        set_state(b);
    }
    KBool(bool btrue, bool bfalse) {
        ktrue = btrue;
        kfalse = bfalse;
        clear_conflict();
    }

    bool is_unknown() const {
        return (kfalse == ktrue);
    }
    bool is_known() const {
        return (kfalse != ktrue);
    }
    bool known_true() const {
        return ktrue;
    }
    bool known_false() const {
        return !kfalse;
    }
    KBool& invert_state() {
        if (is_known()) {
            kfalse = !kfalse;
            ktrue = !ktrue;
        }
        return *this;
    }

    static KBool Unknown() {
        KBool ret;
        ret.set_unknown();
        return ret;
    }
    static KBool KTrue() {
        KBool ret;
        ret.set_true();
        return ret;
    }
    static KBool KFalse() {
        KBool ret;
        ret.set_false();
        return ret;
    }
    static KBool Invert(const KBool& b) {
        KBool ret;
        ret.set_state(b);
        return ret;
    }
    static KBool Invert(bool b) {
        KBool ret;
        ret.set_state(b);
        return ret;
    }

    KBool is_true() const {
        return *this;
    }
    KBool is_false() const {
        KBool ret = *this;
        ret.invert_state();
        return ret;
    }
    KBool is_equal(const KBool& other) const {
        if (this->is_unknown() || other.is_unknown()) {
            return Unknown();
        }
        return KBool(this->to_bool() == other.to_bool());
    }
    KBool is_equal(bool other) const {
        if (this->is_unknown()) {
            return Unknown();
        }
        return KBool(this->to_bool() == other);
    }
    KBool is_notequal(const KBool& other) const {
        if (this->is_unknown() || other.is_unknown()) {
            return Unknown();
        }
        return KBool(this->to_bool() != other.to_bool());
    }
    KBool is_notequal(bool other) const {
        if (this->is_unknown()) {
            return Unknown();
        }
        return KBool(this->to_bool() != other);
    }

    KBool operator~() const {
        return Invert(*this);
    }
    KBool operator&(const KBool& other) const {
        if (this->known_false() || other.known_false()) {
            return KFalse();
        }
        if (this->known_true() && other.known_true()) {
            return KTrue();
        }
        return Unknown();
    }
    KBool operator|(const KBool& other) const {
        if (this->known_true() || other.known_true()) {
            return KTrue();
        }
        if (this->known_false() && other.known_false()) {
            return KFalse();
        }
        return Unknown();
    }
    KBool operator^(const KBool& other) const {
        return this->is_notequal(other);
    }
};

template<class T>
class KBI {
public:
    T zero;
    T ones;
    static constexpr T all_ones = std::numeric_limits<T>::max();
    static constexpr int bit_width = std::numeric_limits<T>::digits;
    void assert_index_range(int index) {
        assert(index >= 0 && index < bit_width);
    }
    T get_bit_mask(int index) const {
        index %= bit_width;
        if (index < 0) {
            index += bit_width;
        }
        return (T(1) << index);
    }
    int get_sign_index() const {
        return (bit_width - 1);
    }
    T get_sign_mask() const {
        return get_bit_mask(-1);
    }
public:
    void set_unknown() {
        zero = 0;
        ones = 0;
    }
    void set_all_zero() {
        zero = std::numeric_limits<T>::max();
        ones = 0;
    }
    void set_all_ones() {
        zero = 0;
        ones = std::numeric_limits<T>::max();
    }
    void set_value(T val) {
        zero = ~val;
        ones = val;
    }
    bool known_bit_set(int index) const {
        return (ones & get_bit_mask(index));
    }
    bool known_bit_clear(int index) const {
        return (zero & get_bit_mask(index));
    }
    bool is_known_bit(int index) const {
        return (known_bit_set(index) || known_bit_clear(index));
    }
    bool is_unknown_bit(int index) const {
        return !is_known_bit(index);
    }
    void bit_set(int index) {
        T mask = get_bit_mask(index);
        zero &= ~mask;
        ones |= mask;
    }
    void bit_clear(int index) {
        T mask = get_bit_mask(index);
        zero |= mask;
        ones &= ~mask;
    }
    void bit_unknown(int index) {
        T mask = get_bit_mask(index);
        zero &= ~mask;
        ones &= ~mask;
    }
    KBool bit_test(int index) {
        if (known_bit_set(index)) {
            return KBool::KTrue();
        }
        if (known_bit_clear(index)) {
            return KBool::KFalse();
        }
        return KBool::Unknown();
    }
    void bit_flip(int index) {
        T mask = get_bit_mask(index);
        if (known_bit_set()) {
            bit_set(index);
            return;
        }
        if (known_bit_clear()) {
            bit_clear(index);
            return;
        }
    }
    void bit_copy(int index, bool b) {
        if (b) {
            bit_set(index);
            return;
        }
        bit_clear(index);
    }
    void bit_copy(int index, KBool b) {
        if (b.known_true()) {
            bit_set(index);
            return;
        }
        if (b.known_false()) {
            bit_clear(index);
            return;
        }
        bit_unknown(index);
    }
    void bit_swap(int index, KBool& b) {
        KBool temp = bit_test(index);
        bit_copy(index, b);
        b = temp;
    }
    void sext(bool b) {
        if (b) {
            set_all_ones();
            return;
        }
        set_all_zero();
    }
    void sext(KBool b) {
        if (b.known_true()) {
            set_all_ones();
            return;
        }
        if (b.known_false()) {
            set_all_zero();
            return;
        }
        set_unknown();
    }
    void zext(bool b) {
        set_all_zero();
        bit_copy(0, b);
    }
    void zext(KBool b) {
        set_all_zero();
        bit_copy(0, b);
    }
    KBI() {
        set_unknown();
    }
    KBI(T val) {
        set_value(val);
    }
    KBI(bool b, bool sign_extend) {
        if (sign_extend) {
            sext(b);
            return;
        }
        zext(b);
    }
    KBI(KBool b, bool sign_extend) {
        if (sign_extend) {
            sext(b);
            return;
        }
        zext(b);
    }
    unsigned known_bits() const {
        return popcount(zero & ones);
    }
    unsigned unknown_bits() const {
        return bit_width - popcount(zero & ones);
    }
    bool is_fully_known() const {
        return (unknown_bits() == 0);
    }
    bool is_fully_unknown() const {
        return (known_bits() == 0);
    }
public:
    T get_value() const {
        assert(is_fully_known());
        return ones;
    }
public:

    /* zero tests */

    KBool is_zero() const {
        if (zero == all_ones) {
            return KBool::KTrue();
        }
        if (ones != 0) {
            return KBool::KFalse();
        }
        return KBool::Unknown();
    }
    KBool is_nonzero() const {
        return KBool::Invert(is_zero());
    }
    bool known_zero() const {
        return is_zero().known_true();
    }
    bool known_nonzero() const {
        return is_nonzero().known_true();
    }

    /* sign tests */

    KBool test_signbit() const {
        return bit_test(-1);
    }
    KBool known_signbit_set() const {
        return known_bit_set(-1);
    }
    KBool known_signbit_clear() const {
        return known_bit_set(-1);
    }
    KBool is_known_signbit() const {
        return is_known_bit(-1);
    }
    KBool is_unknown_signbit() const {
        return is_unknown_bit(-1);
    }

    KBool is_negative() const {
        return test_signbit();
    }

    KBool is_strictly_positive() const {
        return (KBool::Invert(test_signbit()) & is_nonzero());
    }

    /* arithmetic */

    T min_unsigned() const {
        return ~zero;
    }
    T max_unsigned() const {
        return ones;
    }
    T min_signed() const {
        T ret = ~zero;
        if (is_unknown_signbit()) {
            ret |= get_bit_mask(-1);
        }
        return ret;
    }
    T max_signed() const {
        T ret = ones;
        if (is_unknown_signbit()) {
            ret &= ~get_bit_mask(-1);
        }
        return ret;
    }
};

/* logical */

template<class T>
KBI<T> operator~(const KBI<T>& arg) {
    KBI<T> ret;
    ret.zero = arg.ones;
    ret.ones = arg.zero;
    return ret;
}

template<class T>
KBI<T> operator&(const KBI<T>& LHS, const KBI<T>& RHS) {
    KBI<T> ret;
    ret.zero = LHS.zero | RHS.zero;
    ret.ones = LHS.ones & RHS.ones;
    return ret;
}

template<class T>
KBI<T> operator|(const KBI<T>& LHS, const KBI<T>& RHS) {
    KBI<T> ret;
    ret.zero = LHS.zero & RHS.zero;
    ret.ones = LHS.ones | RHS.ones;
    return ret;
}

template<class T>
KBI<T> operator^(const KBI<T>& LHS, const KBI<T>& RHS) {
    KBI<T> ret;
    ret.zero = (LHS.zero & RHS.zero) | (LHS.ones & RHS.ones);
    ret.ones = (LHS.zero & RHS.ones) | (LHS.ones & RHS.zero);
    return ret;
}

/* shifts */

template<class T>
KBI<T> shift_left_logical(const KBI<T>& arg, int shift) {
    KBI<T>::assert_index_range(shift);
    KBI<T> ret;
    ret.ones =  ( arg.ones << shift);
    ret.zero = ~(~arg.zero << shift);
    return ret;
}

template<class T>
KBI<T> shift_right_logical(const KBI<T>& arg, int shift) {
    KBI<T>::assert_index_range(shift);
    KBI<T> ret;
    ret.ones =  ( arg.ones >> shift);
    ret.zero = ~(~arg.zero >> shift);
    return ret;
}

template<class T>
KBI<T> shift_left_ones(const KBI<T>& arg, int shift) {
    KBI<T>::assert_index_range(shift);
    KBI<T> ret;
    ret.ones = ~(~arg.ones << shift);
    ret.zero =  ( arg.zero << shift);
    return ret;
}

template<class T>
KBI<T> shift_right_ones(const KBI<T>& arg, int shift) {
    KBI<T>::assert_index_range(shift);
    KBI<T> ret;
    ret.ones = ~(~arg.ones >> shift);
    ret.zero =  ( arg.zero >> shift);
    return ret;
}

template<class T>
KBI<T> shift_left_unknown_bits(const KBI<T>& arg, int shift) {
    KBI<T>::assert_index_range(shift);
    KBI<T> ret;
    ret.ones = (arg.ones << shift);
    ret.zero = (arg.zero << shift);
    return ret;
}

template<class T>
KBI<T> shift_right_unknown_bits(const KBI<T>& arg, int shift) {
    KBI<T>::assert_index_range(shift);
    KBI<T> ret;
    ret.ones = (arg.ones >> shift);
    ret.zero = (arg.zero >> shift);
    return ret;
}

template<class T>
KBI<T> shift_right_arithmetic(const KBI<T>& arg, int shift) {
    if (arg.known_signbit_clear()) {
        return shift_right_logical(arg, shift);
    }
    if (arg.known_signbit_set()) {
        return shift_right_ones(arg, shift);
    }
    return shift_right_unknown_bits(arg, shift);
}

/* rotates */

template<class T>
KBI<T> rotate_left(const KBI<T>& arg, int shift) {
    KBI<T> ret;
    ret.ones = rotl(arg.ones, shift);
    ret.zero = rotl(arg.zero, shift);
    return ret;
}

template<class T>
KBI<T> rotate_right(const KBI<T>& arg, int shift) {
    KBI<T> ret;
    ret.ones = rotr(arg.ones, shift);
    ret.zero = rotr(arg.zero, shift);
    return ret;
}

template<class T>
KBI<T> rotate_left_with_carry(const KBI<T>& arg, int shift, KBool& carry_inout) {
    KBI<T> ret = arg;
    shift %= KBI<T>::bit_width;
    if (shift < 0) {
        shift += KBI<T>::bit_width;
    }
    KBI<T>::assert_index_range(shift);
    for (int i = 0; i < shift; i++) {
        KBool temp = ret.test_bit(-1);
        ret.zero <<= 1;
        ret.ones <<= 1;
        ret.copy_bit(0, carry_inout);
        carry_inout = temp;
    }
}

template<class T>
KBI<T> rotate_right_with_carry(const KBI<T>& arg, int shift, KBool& carry_inout) {
    KBI<T> ret = arg;
    shift %= KBI<T>::bit_width;
    if (shift < 0) {
        shift += KBI<T>::bit_width;
    }
    KBI<T>::assert_index_range(shift);
    for (int i = 0; i < shift; i++) {
        KBool temp = ret.test_bit(0);
        ret.zero >>= 1;
        ret.ones >>= 1;
        ret.copy_bit(-1, carry_inout);
        carry_inout = temp;
    }
}

/* merge */

template<class T>
KBI<T> create_from_unsigned_range(const T& min_bound, const T& max_bound) {
    KBI<T> ret;
    if (min_bound == max_bound) {
        ret.set_value(min_bound);
        return ret;
    }
    ret.set_unknown();
    if (min_bound > max_bound) {
        // not sure how to implement wrap around
        return ret;
    }
    for (size_t i = KBI<T>::bit_width; i --> 0;) {
        bool x = (min_bound & (static_cast<T>(1) << i));
        bool y = (max_bound & (static_cast<T>(1) << i));
        if (x != y) {
            break;
        }
        ret.bit_copy(i, x);
    }
}

/**
 * @brief sets bits to unknown on conflict
 */
template<class T>
void merge_bits_favor_unknown(const KBI<T>& x, const KBI<T>& y) {
    KBI<T> ret;
    ret.zero = x.zero & y.zero;
    ret.zero = x.ones & y.ones;
    return ret;
}

/* arithmetic */

template<class T>
KBI<T> increment(const KBI<T>& arg) {
    KBI<T> ret = arg;
    for (size_t i = 0; i < KBI<T>::bit_width; i++) {
        KBool X = ret.bit_test(i);
        // SUM = X ^ CIN
        // COUT = X & CIN
        ret.bit_copy(i, X ^ carry);
        carry = carry & X;
    }
    return ret;
}

template<class T>
KBI<T> decrement(const KBI<T>& arg) {
    KBI<T> ret = arg;
    for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
        KBool X = ret.bit_test(i);
		// DIFF = X ^ CIN
		// COUT = ~X & CIN
        ret.bit_copy(i, X ^ carry);
        carry = ~X & carry;
    }
    return ret;
}

template<class T>
KBI<T> negate(const KBI<T>& arg) {
    return increment(~arg);
}

template<class T>
KBI<T> absolute_value(const KBI<T>& arg) {
    if (arg.known_signbit_clear()) {
        return arg;
    }
    if (arg.known_signbit_set()) {
        return negate(arg);
    }
    KBI<T> ret;
    ret.set_unknown();
    return ret;
}



template<class T>
KBI<T> add_with_carry(const KBI<T>& LHS, const KBI<T>& RHS, KBool& carry) {
    KBI<T> ret;
    for (size_t i = 0; i < KBI<T>::bit_width; i++) {
        // 1 bit full adder
        KBool X = x.bit_test(i);
        KBool Y = y.bit_test(i);
        // SUM = X ^ Y ^ CIN
        // COUT = (X & Y) | (CIN & (X ^ Y))
        ret.bit_copy(i, X ^ Y ^ carry);
        carry = (X & Y) | (carry & (X ^ Y));
    }
    return ret;
}

template<class T>
KBI<T> add(const KBI<T>& LHS, const KBI<T>& RHS) {
    KBool carry = false;
    return add_with_carry(LHS, RHS, carry);
}

KBool add_overflow(const KBI<T>& x, const KBI<T>& y, const KBI<T>& result, const KBool& carry) {
    /*
    Signed Overflow Truth Table
    Overflow occurs when (pos + pos = neg) or (neg + neg = pos)
    +---+---+---+---+---+
    | X | Y |   | R |   |
    +---+---+---+---+---+
    |   |   | 0 | 1 | ? |
    +---+---+---+---+---+
    | 0 | 0 | 0 : 1 : ? :
    | 0 | 1 | 0 : 0 : 0 :
    | 0 | ? | 0 : ? : ? :
    +---+---+---+---+---+
    | 1 | 0 | 0 : 0 : 0 :
    | 1 | 1 | 1 : 0 : ? :
    | 1 | ? | ? : 0 : ? :
    +---+---+---+---+---+
    | ? | 0 | 0 : ? : ? :
    | ? | 1 | ? : 0 : ? :
    | ? | ? | ? : ? : ? :
    +---+---+---+---+---+
    */
    if (carry.known_false()) {
        if (x.is_known() && y.is_known()) {
            if (x != y) {
                // never overflows
                return KBool::KFalse();
            }
            if (result.is_known()) {
                if (x.to_bool() == result.to_bool()) {
                    // inputs and outputs are the same sign
                    return KBool::KFalse();
                }
                // (pos + pos = neg) or (neg + neg = pos)
                return KBool::KTrue();
            }
            // overflow cannot be determined
            return KBool::Unknown();
        }
        if (result.is_known()) {
            if (x.is_known()) {
                if (x.to_bool() == result.to_bool()) {
                    // inputs and outputs are the same sign
                    return KBool::KFalse();
                }
            }
            if (y.is_known()) {
                if (y.to_bool() == result.to_bool() ) {
                    // inputs and outputs are the same sign
                    return KBool::KFalse();
                }
            }
        }
        return KBool::Unknown();
    }
}

KBool sub_overflow(const KBI<T>& x, const KBI<T>& y, const KBI<T>& result, const KBool& carry) {
    return add_overflow(x, y, KBool::Invert(result));
}

template<class T>
KBI<T> add_with_flags(const KBI<T>& x, const KBI<T>& y, KBool& carry, KBool& overflow) {
    KBI<T> ret = add_with_carry(x, y, carry);
    T x_max = x.max_signed();
    T y_max = y.max_signed();
    T x_min = x.min_signed();
    T y_min = y.min_signed();
    T sum_max, sum_min, temp;
    bool max_overflow = __builtin_sadd_overflow(x_max, y_max, &sum_max);
    max_overflow = max_overflow || __builtin_sadd_overflow(sum_max, (!carry.known_false()) ? 1 : 0, &temp);
    bool min_overflow = __builtin_sadd_overflow(x_min, y_min, &sum_min);
    min_overflow = min_overflow || __builtin_sadd_overflow(sum_min, (carry.known_true()) ? 1 : 0, &temp);
    if (max_overflow == min_overflow) {
        overflow = KBool(max_overflow);
    } else {
        overflow = KBool::Unknown();
    }
    return ret;
}

template<class T>
KBI<T> sub_with_carry(const KBI<T>& LHS, const KBI<T>& RHS, KBool& carry) {
    KBI<T> ret;
    for (size_t i = 0; i < KBI<T>::bit_width; i++) {
        // 1 bit full subtractor
        KBool X = x.bit_test(i);
        KBool Y = y.bit_test(i);
        // DIFF = X ^ Y ^ CIN
        // COUT = X < (Y + C)
        // COUT = (~X & Y) | (CIN & ~(X ^ Y))
        ret.bit_copy(i, X ^ Y ^ carry);
        carry = (~X & Y) | (carry & ~(X ^ Y));
    }
    return ret;
}

template<class T>
KBI<T> sub(const KBI<T>& LHS, const KBI<T>& RHS) {
    KBool carry = false;
    return sub_with_carry(LHS, RHS, carry);
}

template<class T>
KBI<T> sub_with_flags(const KBI<T>& LHS, const KBI<T>& RHS, KBool& carry, KBool& overflow) {
    KBI<T> ret = sub_with_carry(x, y, carry);
    T x_max = x.max_signed();
    T y_max = y.max_signed();
    T x_min = x.min_signed();
    T y_min = y.min_signed();
    T diff_max, diff_min, temp;
    bool max_overflow = __builtin_ssub_overflow(x_max, y_min, &diff_max);
    max_overflow = max_overflow || __builtin_ssub_overflow(diff_max, (!carry.known_false()) ? 1 : 0, &temp);
    bool min_overflow = __builtin_ssub_overflow(x_min, y_max, &diff_min);
    min_overflow = min_overflow || __builtin_ssub_overflow(diff_min, (carry.known_true()) ? 1 : 0, &temp);
    if (max_overflow == min_overflow) {
        overflow = KBool(max_overflow);
    } else {
        overflow = KBool::Unknown();
    }
    return ret;
}

template<class T>
void compare_with_carry(const KBI<T>& LHS, const KBI<T>& RHS, KBool& carry, KBool& zero, KBool& sign, KBool& overflow) {
    KBI<T> result = sub_with_flags(LHS, RHS, carry, overflow);
    zero = result.is_zero();
    sign = result.test_signbit();
}

template<class T>
void compare(const KBI<T>& LHS, const KBI<T>& RHS, KBool& carry, KBool& zero, KBool& sign, KBool& overflow) {
    carry = false;
    compare(LHS, RHS, carry, zero, sign, overflow);
}

template<class T>
KBI<T> mul(const KBI<T>& LHS, const KBI<T>& RHS) {
    KBI<T> ret;
    if (LHS.is_fully_known() && RHS.is_fully_known()) {
        ret.set_value(LHS.get_value() * RHS.get_value());
        return ret;
    }
    if (LHS.known_zero() || RHS.known_zero()) {
        ret.set_all_zero();
        return ret;
    }
    ret.set_unknown();
    T max_mult, min_mult;
    if (__builtin_mul_overflow(LHS.max_unsigned(), RHS.max_unsigned())) {
        return ret;
    }
    if (__builtin_mul_overflow(LHS.min_unsigned(), RHS.min_unsigned())) {
        return ret;
    }
    return create_from_unsigned_range(min_mult, max_mult);
}

template<class T>
KBI<T> div_unsigned(const KBI<T>& LHS, const KBI<T>& RHS) {
    KBI<T> ret;
    if (LHS.is_fully_known() && RHS.is_fully_known()) {
        ret.set_value(LHS.get_value() / RHS.get_value());
        return ret;
    }
    if (RHS.known_nonzero()) {
        return create_from_unsigned_range(LHS.min_unsigned() / RHS.max_unsigned(), LHS.max_unsigned() / RHS.min_unsigned());
    }
    ret.set_unknown();
    return ret;
}

template<class T>
KBI<T> rem_unsigned(const KBI<T>& LHS, const KBI<T>& RHS) {
    KBI<T> ret;
    if (LHS.is_fully_known() && RHS.is_fully_known()) {
        ret.set_value(LHS.get_value() % RHS.get_value());
        return ret;
    }
    if (RHS.known_nonzero()) {
        if (LHS.max_unsigned() < RHS.min_unsigned()) {
            return LHS.max_unsigned();
        }
        return create_from_unsigned_range(0, RHS.max_unsigned() - 1);
    }
    ret.set_unknown();
    return ret;
}

template<class T>
KBI<T> div_signed(const KBI<T>& LHS, const KBI<T>& RHS) {
    if (LHS.known_signbit_clear() && RHS.known_signbit_clear()) {
        return div_unsigned(LHS, RHS);
    }
    KBI<T> ret;
    ret.set_unknown();
    return ret;
}

template<class T>
KBI<T> rem_signed(const KBI<T>& LHS, const KBI<T>& RHS) {
    if (LHS.known_signbit_clear() && RHS.known_signbit_clear()) {
        return rem_unsigned(LHS, RHS);
    }
    KBI<T> ret;
    ret.set_unknown();
    return ret;
}

template<class T>
KBI<uint8_t> popcount(const KBI<T>& arg) {
    uint8_t lower_bound = popcount(x.get_unsigned_minimum());
    uint8_t upper_bound = popcount(x.get_unsigned_maximum());
    return create_from_unsigned_range(lower_bound, upper_bound);
}

template<class T>
KBI<uint8_t> count_leading_zeros(const KBI<T>& arg) {
    uint8_t lower_bound = countl_zero(x.get_unsigned_minimum());
    uint8_t upper_bound = countl_zero(x.get_unsigned_maximum());
    return create_from_unsigned_range(lower_bound, upper_bound);
}

template<class T>
KBI<uint8_t> count_trailing_zeros(const KBI<T>& arg) {
    uint8_t lower_bound = countr_zero(x.get_unsigned_minimum());
    uint8_t upper_bound = countr_zero(x.get_unsigned_maximum());
    return create_from_unsigned_range(lower_bound, upper_bound);
}

template<class T>
KBI<T> bit_reverse(const KBI<T>& arg) {
    KBI<T> ret;
    ret.zero = bit_reverse(arg.zero);
    ret.ones = bit_reverse(arg.ones);
    return ret;
}

template<class T>
KBI<T> swap_byte_order(const KBI<T>& arg) {
    KBI<T> ret;
    ret.zero = swap_byte_order(arg.zero);
    ret.ones = swap_byte_order(arg.ones);
    return ret;
}

/* other shift */

template<class T>
KBI<T> shift_left_logical(const KBI<T>& LHS, const KBI<uint8_t>& RHS) {
    KBI<T> ret;
    if (RHS.max_unsigned() >= LHS::bit_width) {
        ret.set_unknown();
        return ret;
    }
    ret = shift_left_logical(LHS, RHS.min_unsigned());
    for (int i = RHS.min_unsigned() + 1; i <= RHS.max_unsigned(); i++) {
        // clear any conflicting bits
        KBI<T> temp = shift_left_logical(LHS, i);
        ret.zero &= temp.zero;
        ret.ones &= temp.ones;
    }
    return ret;
}

template<class T>
KBI<T> shift_right_logical(const KBI<T>& LHS, const KBI<uint8_t>& RHS) {
    KBI<T> ret;
    if (RHS.max_unsigned() >= LHS::bit_width) {
        ret.set_unknown();
        return ret;
    }
    ret = shift_right_logical(LHS, RHS.min_unsigned());
    for (int i = RHS.min_unsigned() + 1; i <= RHS.max_unsigned(); i++) {
        // clear any conflicting bits
        KBI<T> temp = shift_right_logical(LHS, i);
        ret.zero &= temp.zero;
        ret.ones &= temp.ones;
    }
    return ret;
}

template<class T>
KBI<T> shift_right_arithmetic(const KBI<T>& LHS, const KBI<uint8_t>& RHS) {
    KBI<T> ret;
    if (RHS.max_unsigned() >= LHS::bit_width) {
        ret.set_unknown();
        return ret;
    }
    ret = shift_right_arithmetic(LHS, RHS.min_unsigned());
    for (int i = RHS.min_unsigned() + 1; i <= RHS.max_unsigned(); i++) {
        // clear any conflicting bits
        KBI<T> temp = shift_right_arithmetic(LHS, i);
        ret.zero &= temp.zero;
        ret.ones &= temp.ones;
    }
    return ret;
}
