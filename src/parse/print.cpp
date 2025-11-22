asm_line::print() {
    using enum instruction_type;
    switch (type) {
        default:
        case VERBATIM: {
            return arg;
        } break;
        case LABEL: {
            return arg + ":";
        }

    }
}
