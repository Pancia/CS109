#include <cstdlib>
#include <sstream>

#include "util.h"

util::ydc_exn::ydc_exn(const std::string& what):
    runtime_error(what) {
}

const std::string util::octal(long decimal) {
    std::ostringstream ostring;
    ostring.setf(std::ios::oct);
    ostring << decimal;
    return ostring.str();
}

// Must be initialized from main().
std::string util::sys_info::execname_;
int util::sys_info::status_ = EXIT_SUCCESS;

void util::sys_info::execname(const std::string& argv0) {
    execname_ = argv0;
    std::cout << std::boolalpha;
    std::cerr << std::boolalpha;
    DEBUGF ('Y', "execname = " << execname_);
}

std::ostream& util::complain() {
    util::sys_info::status(EXIT_FAILURE);
    std::cerr << util::sys_info::execname() << ": ";
    return std::cerr;
}
