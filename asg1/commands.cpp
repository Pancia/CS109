#include "commands.h"
#include "debug.h"
#include "util.h"

ysh_exit_exn::ysh_exit_exn(int exit_status) {
    exit_status::set(exit_status);
}

commands::commands(): map({
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
}){}

command_fn commands::at(const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type(string)
   // So: iterator->second is mapped_type(command_fn)
   command_map::const_iterator result = map.find(cmd);
   if(result == map.end()) {
      throw yshell_exn(cmd + ": no such function");
   }
   return result->second;
}

void fn_cat(inode_state& state, const wordvec& args){
   DEBUGF('c', state);
   DEBUGF('c', args);
   state.cat(args);
}

void fn_cd(inode_state& state, const wordvec& args){
   DEBUGF('c', state);
   DEBUGF('c', args);
   if (args.size() > 1) {
       throw yshell_exn("cd: Expecting only one path arg");
   }
   state.cd(args.size() > 0
           ? (args[0] == "/"
               ? wordvec{"/"}
               : split(args[0], "/"))
           : wordvec{"/"});
}

void fn_echo(inode_state& state, const wordvec& args){
   DEBUGF('c', state);
   DEBUGF('c', args);
   cout << args << endl;
}

void fn_exit(inode_state& state, const wordvec& args){
   DEBUGF('c', state);
   DEBUGF('c', args);
   throw ysh_exit_exn(stoi(args[0]));
}

void fn_ls(inode_state& state, const wordvec& args){
   DEBUGF('c', state);
   DEBUGF('c', args);
   state.ls(args, false);
}

void fn_lsr(inode_state& state, const wordvec& args){
   DEBUGF('c', state);
   DEBUGF('c', args);
   state.ls(args, true);
}

void fn_make(inode_state& state, const wordvec& args){
   DEBUGF('c', state);
   DEBUGF('c', args);
   state.make(args);
}

void fn_mkdir(inode_state& state, const wordvec& args){
   DEBUGF('c', state);
   DEBUGF('c', args);
   state.mkdir(args);
}

void fn_prompt(inode_state& state, const wordvec& args){
   DEBUGF('c', state);
   DEBUGF('c', args);
   state.set_prompt(intercalate(args, ""));
}

void fn_pwd(inode_state& state, const wordvec& args){
   DEBUGF('c', state);
   DEBUGF('c', args);
   cout << state.get_wd() << endl;
}

void fn_rm(inode_state& state, const wordvec& args){
   DEBUGF('c', state);
   DEBUGF('c', args);
   state.rm(args, false);
}

void fn_rmr(inode_state& state, const wordvec& args){
   DEBUGF('c', state);
   DEBUGF('c', args);
   state.rm(args, true);
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

