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
                cout << "PRINTING VAL FOR KEY: " << line << endl;
                cout << *map.find(line) << endl;
            } else {
                cout << "KEY '" << line << "' NOT FOUND" << endl;
            }
        }
        return;
    }
    string key = trim(line.substr(0, eq_pos));
    string val = trim(line.substr(eq_pos+1, line.size()));
    auto pair = str_str_pair(key, val);
    cout << pair << endl;
    if (!key.empty() && val.empty()) {
        cout << "ERASING VAL FOR KEY: " << key << endl;
        map.erase(map.find(key));
    } else if (!key.empty() && !val.empty()) {
        cout << "INSERTING" << endl;
        map.insert(pair);
    } else if (key.empty() && val.empty()) {
        cout << "PRINTING ALL" << endl;
        map.print();
    } else if (key.empty() && !val.empty()) {
        cout << "PRINTING KEYS FOR VAL: " << val << endl;
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
            cout << "-: " << ++linenr
                << ": " << line << endl;
            parse_line(line, map);
        }
    } else {
        for (int i = 0; i < argc; ++i) {
            ifstream file (argv[i]);
            if (file.is_open()) {
                while (getline(file, line)) {
                    cout << argv[i] << ": " << ++linenr
                        << ": " << line
                        << endl;
                    parse_line(line, map);
                }
                file.close();
            } else {
                cout << "Unable to open file: " << argv[i] << endl;
            }
        }
    }

    cout << "EXIT_SUCCESS" << endl;
    return EXIT_SUCCESS;
}
