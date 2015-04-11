#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <unistd.h>

#include "commands.h"
#include "debug.h"
#include "inode.h"
#include "util.h"

//
// scan_options
//    Options analysis:  The only option is -Dflags.
//
void scan_options(int argc, char** argv) {
    opterr = 0;
    for (;;) {
        int option = getopt(argc, argv, "@:");
        if (option == EOF) break;
        switch(option) {
            case '@':
                debugflags::setflags(optarg);
                break;
            default:
                util::complain() << "-" <<(char) option << ": invalid option"
                    << std::endl;
                break;
        }
    }
    if (optind < argc) {
        util::complain() << "operands not permitted" << std::endl;
    }
}


//
// main -
//    Main program which loops reading commands until end of file.
//    TODO:
//      - namespace new_name = old_name;
//      - using std::cout;
//      - (re-)indent main
//
int main(int argc, char** argv) {
   util::execname(argv[0]);
   std::cout << std::boolalpha; // Print false or true instead of 0 or 1.
   std::cerr << std::boolalpha;
   std::cout << argv[0] << " build " << __DATE__ << " " << __TIME__ << std::endl;
   scan_options(argc, argv);
   bool need_echo = util::want_echo();
   commands cmdmap;
   inode_state state;
   try {
       for (;;) {
           try {
               // Read a line, break at EOF, and echo print the prompt
               // if one is needed.
               std::cout << state.get_prompt();
               std::string line;
               getline(std::cin, line);
               if (std::cin.eof()) {
                   if (need_echo)
                       std::cout << "^D";
                   std::cout << std::endl;
                   DEBUGF('y', "EOF");
                   break;
               }
               if (need_echo)
                   std::cout << line << std::endl;
               // Ignore empty lines, causes issues later on
               if (!line.compare(""))
                   continue;

               // Split the line into words and lookup the appropriate
               // function.  Complain or call it.
               util::wordvec words = util::split(line, " \t");
               std::string cmd = words.at(0);
               // If it's a comment, ignore it
               if (!std::string(1, cmd[0]).compare("#"))
                   continue;
               std::vector<std::string> args
                   = std::vector<std::string>(words.begin() + 1, words.end());
               command_fn fn = cmdmap.at(words.at(0));
               fn(state, args);
           } catch(util::yshell_exn& exn) {
               // If there is a problem discovered in any function, an
               // exn is thrown and printed here.
               DEBUGF('x', "EXITING");
               util::complain() << exn.what() << std::endl;
           }
       }
   } catch(ysh_exit_exn& _) {
       // This catch intentionally left blank.
       DEBUGF('x', "EXITING");
   }

   return exit_status_message();
}

