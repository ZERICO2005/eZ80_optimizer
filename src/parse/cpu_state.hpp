#include <cstdint>
#include <cassert>
#include <limits>
#include <bit>
#include "../ez80_type.h"

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
        return (!kfalse && ktrue);
    }
    bool known_false() const {
        return (kfalse && !ktrue);
    }
    void invert_state() {
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
            return KUnknown();
        }
        return KBool(this->to_bool() == other.to_bool());
    }
    KBool is_equal(bool other) const {
        if (this->is_unknown()) {
            return KUnknown();
        }
        return KBool(this->to_bool() == other);
    }
    KBool is_notequal(const KBool& other) const {
        if (this->is_unknown() || other.is_unknown()) {
            return KUnknown();
        }
        return KBool(this->to_bool() != other.to_bool());
    }
    KBool is_notequal(bool other) const {
        if (this->is_unknown()) {
            return KUnknown();
        }
        return KBool(this->to_bool() != other);
    }

    KBool operator~() const {
        return KInvert(*this);
    }
    KBool operator&(const KBool& other) const {
        if (this->known_flase() || other.known_false()) {
            return KFalse();
        }
        if (this->known_true() && other.known_true()) {
            return KTrue();
        }
        return KUnknown();
    }
    KBool operator|(const KBool& other) const {
        if (this->known_true() || other.known_true()) {
            return KTrue();
        }
        if (this->known_false() && other.known_false()) {
            return KFalse();
        }
        return KUnknown();
    }
    KBool operator^(const KBool& other) const {
        return this->is_notequal(other);
    }
};

template<class T>
class KBI<T> {
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
        return (digits - 1);
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
        if (known_bit_set()) {
            return KBool::KTrue();
        }
        if (known_bit_clear()) {
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
        set_all_zeros();
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
        set_all_zeros();
        bit_copy(0, b);
    }
    void zext(KBool b) {
        set_all_zeros();
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

template<class T>
KBI<T> bitwise_not(const KBI<T>& arg) {
    KBI<T> ret;
    ret.zero = arg.ones;
    ret.ones = arg.zero;
    return ret;
}

template<class T>
KBI<T> bitwise_and(const KBI<T>& LHS, const KBI<T>& RHS) {
    KBI<T> ret;
    ret.zero = LSH.zero | RHS.zero;
    ret.ones = LSH.ones & RHS.ones;
    return ret;
}

template<class T>
KBI<T> bitwise_or(const KBI<T>& LHS, const KBI<T>& RHS) {
    KBI<T> ret;
    ret.zero = LSH.zero & RHS.zero;
    ret.ones = LSH.ones | RHS.ones;
    return ret;
}

template<class T>
KBI<T> bitwise_xor(const KBI<T>& LHS, const KBI<T>& RHS) {
    KBI<T> ret;
    ret.zero = (LSH.zero & RHS.zero) | (LSH.ones & RHS.ones);
    ret.ones = (LSH.zero & RHS.ones) | (LSH.ones & RHS.zero);
    return ret;
}

template<class T>
KBI<T> increment(const KBI<T>& arg) {
    KBI<T> ret;
    if (arg.is_fully_known()) {
        ret.set_value(arg.get_value() + 1);
        return ret;
    }
    ret.set_unknown();
    return ret;
}

template<class T>
KBI<T> decrement(const KBI<T>& arg) {
    KBI<T> ret;
    if (arg.is_fully_known()) {
        ret.set_value(arg.get_value() - 1);
        return ret;
    }
    ret.set_unknown();
    return ret;
}

template<class T>
KBI<T> negate(const KBI<T>& arg) {
    KBI<T> ret;
    if (arg.is_fully_known()) {
        ret.set_value(-arg.get_value());
        return ret;
    }
    ret.set_unknown();
    return ret;
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
KBI<T> add(const KBI<T>& LHS, const KBI<T>& RHS) {
    KBI<T> ret;
    if (LHS.is_fully_known() && RHS.is_fully_known()) {
        ret.set_value(LHS.get_value() + RHS.get_value());
        return ret;
    }
    if (LHS.known_zero()) {
        return RHS;
    }
    if (RHS.known_zero()) {
        return LHS;
    }
    ret.set_unknown();
    return ret;
}

template<class T>
KBI<T> sub(const KBI<T>& LHS, const KBI<T>& RHS) {
    KBI<T> ret;
    if (LHS.is_fully_known() && RHS.is_fully_known()) {
        ret.set_value(LHS.get_value() - RHS.get_value());
        return ret;
    }
    if (LHS.known_zero()) {
        return RHS;
    }
    if (RHS.known_zero()) {
        return LHS;
    }
    ret.set_unknown();
    return ret;
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
    return ret;
}

template<class T>
KBI<T> div_unsigned(const KBI<T>& LHS, const KBI<T>& RHS) {
    KBI<T> ret;
    if (LHS.is_fully_known() && RHS.is_fully_known()) {
        ret.set_value(LHS.get_value() / RHS.get_value());
        return ret;
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
    if (known_signbit_clear()) {
        return shift_right_logical(arg, shift);
    }
    if (known_signbit_set()) {
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

struct CPU_State {
    int32_t HL;
    int32_t DE;
    int32_t BC;
    int32_t IX;
    int32_t IY;

};
