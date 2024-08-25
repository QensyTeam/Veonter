int toupper(int sym) {
    if(sym >= 'a' && sym <= 'z') {
        sym -= 32;
    }

    return sym;
};
