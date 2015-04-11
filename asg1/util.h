// util -
//    A utility class to provide various services not conveniently
//    included in other modules.
//

#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>

using std::string;

namespace util {
    using wordvec = std::vector<string>;

    //
    // yshell_exn -
    //   Extend runtime_error for throwing exceptions related to this
    //   program.
    //
    class yshell_exn: public std::runtime_error {
        public:
            explicit yshell_exn(const string& what);
    };

    //
    // setexecname -
    //   Sets the static string to be used as an execname.
    // execname -
    //   Returns the basename of the executable image, which is used in
    //   printing error messags.
    //
    void execname(const string&);
    string& execname();

    //
    // want_echo -
    //   We want to echo all of cin to cout if either cin or cout
    //   is not a tty.  This helps make batch processing easier by
    //   making cout look like a terminal session trace.
    //
    bool want_echo();

    //
    // exit_status -
    //   A static class for maintaining the exit status.  The default
    //   status is EXIT_SUCCESS(0), but can be set to another value,
    //   such as EXIT_FAILURE(1) to indicate that error messages have
    //   been printed.
    //
    class exit_status {
        private:
            static int status;
        public:
            static void set(int);
            static int get();
    };

    //
    // split -
    //    Split a string into a wordvec(as defined above).  Any sequence
    //    of chars in the delimiter string is used as a separator.  To
    //    Split a pathname, use "/".  To split a shell command, use " ".
    //
    wordvec split(const string& line, const string& delimiter);

    // intercalate -
    //   intersperse delim inbetween words, return the concat'ed string
    //   see haskell's intersperse
    string intercalate(wordvec words, string delim);

    // complain -
    //   Used for starting error messages.  Sets the exit status to
    //   EXIT_FAILURE, writes the program name to cerr, and then
    //   returns the cerr ostream.  Example:
    //     complain() << filename << ": some problem" << endl;
    //
    std::ostream& complain();
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec) {
    out << util::intercalate(vec, " ");
    return out;
}

#endif

