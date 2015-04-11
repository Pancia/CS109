#include "commands.h"
#include "debug.h"
#include "util.h"

cmd::ysh_exit_exn::ysh_exit_exn(int exit_status) {
    util::exit_status::set(exit_status);
}

cmd::commands::commands(): map({
        {"cat"   , fn_cat   },
        {"cd"    , fn_cd    },
        {"echo"  , fn_echo  },
        {"exit"  , fn_exit  },
        {"ls"    , fn_ls    },
        {"lsr"   , fn_lsr   },
        {"make"  , fn_make  },
        {"mkdir" , fn_mkdir },
        {"prompt", fn_prompt},
        {"pwd"   , fn_pwd   },
        {"rm"    , fn_rm    },
        {"rmr"   , fn_rmr   },
        }) {}

cmd::command_fn cmd::commands::at(const std::string& cmd) {
    // Note: value_type is pair<const key_type, mapped_type>
    // So: iterator->first is key_type(string)
    // So: iterator->second is mapped_type(command_fn)
    command_map::const_iterator result = map.find(cmd);
    if(result == map.end()) {
        throw util::yshell_exn(cmd + ": no such function");
    }
    return result->second;
}

void cmd::fn_cat(inode_state& state, const util::wordvec& args) {
    DEBUGF('c', state);
    DEBUGF('c', args);
    state.cat(args);
}

void cmd::fn_cd(inode_state& state, const util::wordvec& args) {
    DEBUGF('c', state);
    DEBUGF('c', args);
    if (args.size() > 1) {
        throw util::yshell_exn("cd: Expecting only one path arg");
    }
    // If: empty args, cd to root (ie "/")
    //   elif: args[0] is "/" then cd to root
    //   else: split args[0] on "/" & cd
    state.cd(args.size() == 0
            ? util::wordvec{"/"}
            : (args[0] == "/"
                ? util::wordvec{"/"}
                : util::split(args[0], "/")));
}

void cmd::fn_echo(inode_state& state, const util::wordvec& args) {
    DEBUGF('c', state);
    DEBUGF('c', args);
    std::cout << args << std::endl;
}

void cmd::fn_exit(inode_state& state, const util::wordvec& args) {
    DEBUGF('c', state);
    DEBUGF('c', args);
    throw ysh_exit_exn(stoi(args[0]));
}

void cmd::fn_ls(inode_state& state, const util::wordvec& args) {
    DEBUGF('c', state);
    DEBUGF('c', args);
    state.ls(args, false);
}

void cmd::fn_lsr(inode_state& state, const util::wordvec& args) {
    DEBUGF('c', state);
    DEBUGF('c', args);
    state.ls(args, true);
}

void cmd::fn_make(inode_state& state, const util::wordvec& args) {
    DEBUGF('c', state);
    DEBUGF('c', args);
    state.make(args);
}

void cmd::fn_mkdir(inode_state& state, const util::wordvec& args) {
    DEBUGF('c', state);
    DEBUGF('c', args);
    state.mkdir(args);
}

void cmd::fn_prompt(inode_state& state, const util::wordvec& args) {
    DEBUGF('c', state);
    DEBUGF('c', args);
    state.set_prompt(util::intercalate(args, ""));
}

void cmd::fn_pwd(inode_state& state, const util::wordvec& args) {
    DEBUGF('c', state);
    DEBUGF('c', args);
    std::cout << state.get_wd() << std::endl;
}

void cmd::fn_rm(inode_state& state, const util::wordvec& args) {
    DEBUGF('c', state);
    DEBUGF('c', args);
    state.rm(args, false);
}

void cmd::fn_rmr(inode_state& state, const util::wordvec& args) {
    DEBUGF('c', state);
    DEBUGF('c', args);
    state.rm(args, true);
}

int cmd::exit_status_message() {
    int exit_status = util::exit_status::get();
    std::cout << util::execname()
        << ": exit(" << exit_status << ")"
        << std::endl;
    return exit_status;
}

