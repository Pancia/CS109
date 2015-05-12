#include <cstdlib>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options(int argc, char** argv) {
    opterr = 0;
    for (;;) {
        int option = getopt(argc, argv, "@:");
        if (option == EOF) break;
        switch (option) {
            case '@':
                traceflags::setflags(optarg);
                break;
            default:
                complain() << "-" << (char) optopt << ": invalid option"
                    << endl;
                break;
        }
    }
}

void parse_line(string line, str_str_map& map) {
    int eq_pos = line.find_first_of("=");
    if (eq_pos == -1) {
        if (line.find_first_of("#") != 0) {
            if (map.find(line) != map.end()) {
                cout << *map.find(line) << endl;
            } else {
                cout << line << ": key not found" << endl;
            }
        }
        return;
    }
    string key = trim(line.substr(0, eq_pos));
    string val = trim(line.substr(eq_pos+1, line.size()));
    auto pair = str_str_pair(key, val);
    cout << pair << endl;
    if (!key.empty() && val.empty()) {
        map.erase(map.find(key));
    } else if (!key.empty() && !val.empty()) {
        map.insert(pair);
    } else if (key.empty() && val.empty()) {
        map.print();
    } else if (key.empty() && !val.empty()) {
        map.print(val);
    }
}

int main(int argc, char** argv) {
    sys_info::set_execname(argv[0]);
    scan_options(argc, argv);

    str_str_map map;
    string line;
    size_t linenr = 0;
    if (argc == 1) {
        while (cin) {
            getline(cin, line);
            if (line == "") { continue; }
            cout << "-: " << ++linenr
                << ": " << line << endl;
            parse_line(line, map);
        }
    } else {
        for (int i = 1; i < argc; ++i) {
            ifstream file (argv[i]);
            if (file.is_open()) {
                while (getline(file, line)) {
                    cout << argv[i] << ": " << ++linenr
                        << ": " << line
                        << endl;
                    if (line == "") { continue; }
                    parse_line(line, map);
                }
                file.close();
                linenr = 0;
            } else {
                cout << sys_info::get_execname()
                    << ": " << argv[i]
                    << " No such file or directory" << endl;
            }
        }
    }

    return EXIT_SUCCESS;
}
