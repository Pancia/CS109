#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <iostream>
#include <utility>

#include "debug.h"

enum terminal_symbol {NUMBER, OPERATOR, SCANEOF};
struct token_t {
    terminal_symbol symbol;
    std::string lexinfo;
};

class scanner {
    private:
        bool seen_eof;
        char lookahead;
        void advance();
    public:
        scanner();
        token_t scan();
};

std::ostream& operator<< (std::ostream&, const terminal_symbol&);
std::ostream& operator<< (std::ostream&, const token_t&);

#endif
