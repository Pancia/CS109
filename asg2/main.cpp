#include <deque>
#include <iostream>
#include <map>
#include <stdexcept>
#include <utility>

#include <unistd.h>

#include "bigint.h"
#include "debug.h"
#include "iterstack.h"
#include "scanner.h"
#include "util.h"

using bigint_stack = iterstack<bigint>;

void do_arith(bigint_stack& stack, const char oper) {
    if (stack.size() < 2)
        throw util::ydc_exn("stack empty");
    bigint right = stack.top();
    stack.pop();
    DEBUGF('d', "right = " << right);
    bigint left = stack.top();
    stack.pop();
    DEBUGF('d', "left = " << left);
    bigint result;
    switch (oper) {
        case '+': result = left + right; break;
        case '-': result = left - right; break;
        case '*': result = left * right; break;
        case '/': result = left / right; break;
        case '%': result = left % right; break;
        case '^': result = pow(left, right); break;
        default: throw std::invalid_argument
                 (std::string("do_arith operator is ") + oper);
    }
    DEBUGF('d', "result = " << result);
    stack.push (result);
}

void do_clear(bigint_stack& stack, const char) {
    DEBUGF('d', "");
    stack.clear();
}

void do_dup(bigint_stack& stack, const char) {
    bigint top = stack.top();
    DEBUGF('d', top);
    stack.push (top);
}

void do_printall(bigint_stack& stack, const char) {
    for (const auto &elem: stack) {
        std::cout << elem << std::endl;
    }
}

void do_print(bigint_stack& stack, const char) {
    std::cout << stack.top() << std::endl;
}

void do_debug(bigint_stack& stack, const char) {
    (void) stack; // SUPPRESS: warning: unused parameter 'stack'
    std::cout << "Y not implemented" << std::endl;
}

class ydc_quit: public std::exception {};
void do_quit(bigint_stack&, const char) {
    throw ydc_quit();
}

using function_t = void (*)(bigint_stack&, const char);
using fn_map = std::map<std::string,function_t>;
fn_map do_functions = {
    {"+", do_arith},
    {"-", do_arith},
    {"*", do_arith},
    {"/", do_arith},
    {"%", do_arith},
    {"^", do_arith},
    {"Y", do_debug},
    {"c", do_clear},
    {"d", do_dup},
    {"f", do_printall},
    {"p", do_print},
    {"q", do_quit},
};

void scan_options(int argc, char** argv) {
    if (util::sys_info::execname().size() == 0)
        util::sys_info::execname (argv[0]);
    opterr = 0;
    for (;;) {
        int option = getopt(argc, argv, "@:");
        if (option == EOF) break;
        switch (option) {
            case '@':
                debugflags::setflags(optarg);
                break;
            default:
                util::complain() << "-" << (char) optopt
                    << ": invalid option" << std::endl;
                break;
        }
    }
    if (optind < argc) {
        util::complain() << "operand not permitted" << std::endl;
    }
}

int main(int argc, char** argv) {
    util::sys_info::execname (argv[0]);
    scan_options(argc, argv);
    bigint_stack operand_stack;
    scanner input;
    try {
        for (;;) {
            try {
                token_t token = input.scan();
                if (token.symbol == SCANEOF) break;
                fn_map::const_iterator fn;
                switch (token.symbol) {
                    case NUMBER:
                        operand_stack.push (token.lexinfo);
                        break;
                    case OPERATOR:
                        fn = do_functions.find(token.lexinfo);
                        if (fn == do_functions.end()) {
                            throw util::ydc_exn(
                                    util::octal(token.lexinfo[0])
                                    + " is unimplemented");
                        }
                        fn->second(operand_stack, token.lexinfo.at(0));
                        break;
                    default:
                        break;
                }
            } catch(util::ydc_exn& exn) {
                std::cout << exn.what() << std::endl;
            }
        }
    } catch(ydc_quit&) {/*...*/}
    return util::sys_info::status();
}
