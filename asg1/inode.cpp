#include <iomanip>
#include <algorithm>  // std::reverse

#include "debug.h"
#include "inode.h"

using namespace std;

//====INODE====
int inode::next_inode_nr {1};

inode::inode(inode_t init_type):
    inode_nr(next_inode_nr++), type(init_type) {
        switch (type) {
            case PLAIN_INODE:
                contents = make_shared<plain_file>();
                break;
            case DIR_INODE:
                contents = make_shared<directory>();
                break;
        }
        DEBUGF('i', "inode " << inode_nr << ", type = " << type);
    }

ostream& operator<<(ostream& out, const inode& inode) {
    out << "[INODE "
        << "inodenr = " << inode.inode_nr
        << ", type = " << (inode.type == DIR_INODE ? "DIR" : "FILE")
        << ", contents = ";
    if (inode.type == DIR_INODE) {
        out << *directory_ptr_of(inode.contents).get();
    } else {
        out << *plain_file_ptr_of(inode.contents).get();
    }
    out << "]";
    return out;
}

int inode::get_inode_nr() const {
    DEBUGF('i', "inode = " << inode_nr);
    return inode_nr;
}

plain_file_ptr plain_file_ptr_of(file_base_ptr ptr) {
    plain_file_ptr pfptr = dynamic_pointer_cast<plain_file>(ptr);
    if (pfptr == nullptr)
        throw invalid_argument("plain_file_ptr_of");
    return pfptr;
}

directory_ptr directory_ptr_of(file_base_ptr ptr) {
    directory_ptr dirptr = dynamic_pointer_cast<directory>(ptr);
    if (dirptr == nullptr)
        throw invalid_argument("directory_ptr_of");
    return dirptr;
}

//====FILE====
ostream& operator<<(ostream& out, const plain_file& file) {
    out << "[FILE "
        << "size = " << file.size()
        << ", data = " << file.data
        << "]";
    return out;
    //  private:
    //      wordvec data;
    //  public:
    //      size_t size() const override;
}

/*
 * The size of a file is the
 * number of characters in it
 */
size_t plain_file::size() const {
    size_t size {0};
    size += data.size();
    DEBUGF('i', "size = " << size);
    return size;
}

const wordvec& plain_file::readfile() const {
    DEBUGF('i', data);
    return data;
}

void plain_file::writefile(const wordvec& args) {
    DEBUGF('i', args);
    data = args;
}

//====DIRECTORY====
/*
 * The size of a directory
 * is the sum of the number of files
 */
size_t directory::size() const {
    size_t size {0};
    size = dirents.size();
    DEBUGF('i', "size = " << size);
    return size;
}

ostream& operator<<(ostream& out, const directory& dir) {
    out << "[DIRECTORY files = (";
    for (auto elem : dir.dirents) {
        out << elem.first << "|";
    }
    out << ")]";
    return out;
}

inode& directory::mkdir(inode_ptr cwd, const string& dirname) {
    inode_ptr dir = make_shared<inode>(inode(DIR_INODE));
    auto maybe_dir = dirents.find(dirname);
    if (maybe_dir != dirents.end()) {
        throw yshell_exn("Directory " + dirname + " already exists");
    }
    dirents.emplace(dirname, dir);
    dir_map& new_dir = directory_ptr_of(dir->contents)->dirents;
    new_dir.emplace(".", dir);
    new_dir.emplace("..", cwd);
    return *dir.get();
}

void directory::remove(const string& filename) {
    DEBUGF('i', filename);
    auto maybe_file = dirents.find(filename);
    if (maybe_file->second->type == DIR_INODE
            && directory_ptr_of(maybe_file->second->contents)
                ->size() > 2) {
        throw yshell_exn("directory::remove: "
                "Cannot remove a non-empty directory");
    } else {
        dirents.erase(maybe_file);
    }
}

//====INODE_STATE====
inode_state::inode_state() {
    root = make_shared<inode>(inode(DIR_INODE));
    directory_ptr root_dir = directory_ptr_of(root->contents);
    root_dir->dirents.emplace(".", root);
    root_dir->dirents.emplace("..", root);
    cwd = root;
    DEBUGF('i', "root = " << root->contents << ", cwd = " << cwd
            << ", prompt = \"" << prompt << "\"");
}

ostream& operator<<(ostream& out, const inode_state& state) {
    out << "[INODE_STATE "
        << "root = " << state.root
        << ", cwd = " << state.cwd
        << ", prompt = \"" << state.prompt << "\""
        << "]";
    return out;
}

void inode_state::set_prompt(string new_prompt) {
    this->prompt = new_prompt;
}

string inode_state::get_prompt() {
    return this->prompt + " ";
}

void inode_state::cat(const wordvec& args) {
    DEBUGF('i', args);
    inode_ptr old_cwd = cwd;

    for (string file_path : args) {
        inode_ptr tmp_cwd = cwd;

        wordvec path = split(file_path, "/");
        if (path.size() > 1) {
            this->cd(wordvec(path.begin(), path.end()-1));
        }

        string filename = path.back();
        dir_map this_dir = directory_ptr_of(cwd->contents)->dirents;
        auto maybe_file = this_dir.find(filename);
        if (maybe_file == this_dir.end()) {
            cwd = old_cwd;
            throw yshell_exn("cat: " + filename + ": File not found");
        } else if (maybe_file->second->type == DIR_INODE) {
            cwd = old_cwd;
            throw yshell_exn("cat: " + filename + ": Is a directory");
        } else {
            auto file
                = plain_file_ptr_of(maybe_file->second->contents);
            cout << file->readfile() << endl;
        }
        cwd = tmp_cwd;
    }

    // Go back to old dir
    cwd = old_cwd;
}

void inode_state::cd(const wordvec& args) {
    if (args.size() == 0) {
        return;
    } else if (args[0] == "/") {
        cwd = root;
    } else {
        directory_ptr dir = directory_ptr_of(cwd->contents);
        auto maybe_dir = dir->dirents.find(args[0]);
        if (maybe_dir == dir->dirents.end()) {
            cwd = old_cwd;
            throw yshell_exn("cd: Could not find directory: "
                    + args[0]);
        } else if (maybe_dir->second->type != DIR_INODE) {
            cwd = old_cwd;
            throw yshell_exn("cd: " + args[0] + " is not a directory");
        } else {
            cwd = maybe_dir->second;
            this->cd(wordvec(args.begin()+1,args.end()));
        }
    }
}

void print_inode(inode_ptr inode, string dirname) {
    cout << setw(6) << inode->get_inode_nr() << "  "
        << setw(6) << inode->contents->size() << "  "
        << dirname << (inode->type == DIR_INODE ? "/" : "")
        << endl;
}

void inode_state::ls(const wordvec& args, bool recursive) {
    DEBUGF('i', args);
    inode_ptr old_cwd = cwd;

    wordvec arg_paths = (args.size() == 0 ? wordvec{"/"} : args);

    for (auto arg_path : arg_paths) {
        inode_ptr tmp_cwd = cwd;
        wordvec dir_stack;
        wordvec path = split(arg_path, "/");
        if (path.size() > 0) {
            // cd to all but the last in path
            this->cd(wordvec(path.begin(), path.end()-1));
            // if path.back() is a dir => cd
            // else print & cleanup
            directory_ptr dir = directory_ptr_of(cwd->contents);
            auto maybe_dir = dir->dirents.find(path.back());
            if (maybe_dir->second->type == DIR_INODE) {
                this->cd(wordvec{path.back()});
            } else {
                print_inode(maybe_dir->second, maybe_dir->first);
                goto cleanup;
            }
        }
        cout << this->get_wd() << ":" << endl;
        for (auto elem : directory_ptr_of(cwd->contents)->dirents) {
            print_inode(elem.second, elem.first);
            if (recursive) {
                inode_ptr prev_cwd = cwd;
                if (elem.second->type == DIR_INODE
                        && elem.first != "."
                        && elem.first != "..") {
                    dir_stack.push_back(elem.first);
                }
                cwd = prev_cwd;
            }
        }
        if (recursive) {
            for (string elem : dir_stack) {
                this->ls(wordvec{elem}, true);
            }
        }
cleanup:
        cwd = tmp_cwd;
    }
    // Go back to old dir
    cwd = old_cwd;
}

void inode_state::make(const wordvec& args) {
    DEBUGF('i', args);
    if (args.size() == 0) {
        throw yshell_exn("Please submit a valid path to make");
    }
    inode_ptr old_cwd = cwd;
    wordvec path = split(args[0], "/");
    if (path.size() > 1) {
        this->cd(wordvec(path.begin(), path.end()-1));
    }
    string filename = path.back();

    dir_map& cwd_dir = directory_ptr_of(cwd->contents)->dirents;
    auto maybe_inode = cwd_dir.find(filename);
    inode_ptr i_new_file;
    // If found, make sure is not a directory
    if (maybe_inode != cwd_dir.end()) {
        if (maybe_inode->second->type == DIR_INODE) {
            cwd = old_cwd;
            throw yshell_exn("make: " + filename
                    + ": File already exists");
        }
    } else { // otherwise, replace contents
        i_new_file = make_shared<inode>(inode{PLAIN_INODE});
        cwd_dir.emplace(filename, i_new_file);
    }
    // Maybe overwrite file
    if (args.size() > 1) {
        plain_file_ptr new_file
            = {plain_file_ptr_of(i_new_file->contents)};
        new_file->writefile(wordvec(args.begin()+1, args.end()));
    }
    // Go back to old dir
    cwd = old_cwd;
}

void inode_state::mkdir(const wordvec& args) {
    DEBUGF('i', args);
    inode_ptr old_cwd = cwd;

    wordvec path = split(args[0], "/");
    string dir_name = path.back();
    if (path.size() > 1) {
        path.pop_back();
        this->cd(path);
    }
    directory_ptr_of(cwd->contents)->mkdir(cwd, dir_name);

    // Go back to old dir
    cwd = old_cwd;
}

string inode_state::get_wd() {
    inode_ptr old_cwd = cwd;
    int prev_inode;
    wordvec path;
    string prev_dir;
    while (cwd != root) {
        // Store the id of the inode we're in
        prev_inode = cwd->get_inode_nr();
        // cd to cwd's parent
        auto parent = directory_ptr_of(cwd->contents)
            ->dirents.find("..");
        cwd = parent->second;
        // Find the name of the directory we were in
        dir_map cur_dir = directory_ptr_of(cwd->contents)->dirents;
        for (auto cur_dir_itor = cur_dir.begin();
                cur_dir_itor != cur_dir.end();
                cur_dir_itor++) {
            if (prev_inode == cur_dir_itor->second->get_inode_nr()
                    && cur_dir_itor->first != "..") {
                prev_dir = cur_dir_itor->first;
                break;
            }
        }
        path.push_back(prev_dir);
    }
    // Go back to old dir
    cwd = old_cwd;

    reverse(path.begin(), path.end());
    return intercalate(path, "/") + "/";
}

void inode_state::rm(const wordvec& args, bool recursive) {
    DEBUGF('i', args);
    DEBUGF('i', recursive);
    inode_ptr old_cwd = cwd;

    if (args.size() == 0) {
        throw yshell_exn("rm: Please specify a path");
    } else if (args.size() != 1) {
        throw yshell_exn("rm: Can only delete 1 file/dir at a time");
    } else if (args[0] == "/"){
        throw yshell_exn("rm: Cannot remove root");
    } else {
        wordvec path = split(args[0], "/");
        string filename;
        if (path.size() > 1) {
            this->cd(wordvec(path.begin(), path.end()-1));
        }
        filename = path.back();
        auto cur_dir = directory_ptr_of(cwd->contents);
        auto maybe_file = cur_dir->dirents.find(filename);
        if (maybe_file == cur_dir->dirents.end()){
            cwd = old_cwd;
            throw yshell_exn("rm: Invalid path: " + filename);
        }
        if (recursive && maybe_file->second->type == DIR_INODE
                && maybe_file->second->contents->size() > 2) {
            // Go to filename
            inode_ptr prev_cwd = cwd;
            this->cd(wordvec{maybe_file->first});
            dir_map this_dir
                = directory_ptr_of(maybe_file->second->contents)
                ->dirents;
            // Remove all in this_dir
            for (auto item : this_dir) {
                if (item.first == "." || item.first == "..")
                    continue;
                inode_ptr tmp_cwd = cwd;
                this->rm(wordvec{item.first}, true);
                cwd = tmp_cwd;
            }
            // Go back, then delete filename
            cwd = prev_cwd;
            this->rm(wordvec{filename}, true);
        } else {
            cur_dir->remove(filename);
        }
    }
    // Go back to old dir
    cwd = old_cwd;
}

