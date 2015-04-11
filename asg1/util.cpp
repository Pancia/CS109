#include <cstdlib>
#include <unistd.h>

using namespace std;

#include "util.h"
#include "debug.h"

util::yshell_exn::yshell_exn(const string& what):
    runtime_error(what) {}

int util::exit_status::status = EXIT_SUCCESS;
static string execname_string;

void util::exit_status::set(int new_status) {
    status = new_status;
}

int util::exit_status::get() {
    return status;
}

void util::execname(const string& name) {
    execname_string = name.substr(name.rfind('/') + 1);
    DEBUGF('u', execname_string);
}

string& util::execname() {
    return execname_string;
}

bool util::want_echo() {
    constexpr int CIN_FD {0};
    constexpr int COUT_FD {1};
    bool cin_is_not_a_tty = not isatty(CIN_FD);
    bool cout_is_not_a_tty = not isatty(COUT_FD);
    DEBUGF('u', "cin_is_not_a_tty = " << cin_is_not_a_tty
            << ", cout_is_not_a_tty = " << cout_is_not_a_tty);
    return cin_is_not_a_tty or cout_is_not_a_tty;
}

util::wordvec util::split(const string& line, const string& delimiters) {
    util::wordvec words;
    size_t end = 0;

    // Loop over the string, splitting out words, and for each word
    // thus found, append it to the output wordvec.
    for(;;) {
        size_t start = line.find_first_not_of(delimiters, end);
        if(start == string::npos) break;
        end = line.find_first_of(delimiters, start);
        words.push_back(line.substr(start, end - start));
    }
    DEBUGF('u', words);
    return words;
}

string util::intercalate(util::wordvec words, string delim) {
    stringstream ss;
    const int words_size = words.size();
    for (int i = 0; i < words_size; ++i) {
        if (i != 0) { ss << delim; }
        ss << words[i];
    }
    return ss.str();
}

ostream& util::complain() {
    util::exit_status::set(EXIT_FAILURE);
    cerr << execname() << ": ";
    return cerr;
}

