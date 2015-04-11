#include <iomanip>
#include <algorithm>  // std::reverse

#include "debug.h"
#include "inode.h"

//====INODE====
int inode::next_inode_nr {1};

inode::inode(inode_t init_type):
    inode_nr(next_inode_nr++), type(init_type) {
        switch (type) {
            case PLAIN_INODE:
                contents = std::make_shared<plain_file>();
                break;
            case DIR_INODE:
                contents = std::make_shared<directory>();
                break;
        }
    }

std::ostream& operator<<(std::ostream& out, const inode& inode) {
    out << "[INODE "
        << "inodenr = " << inode.inode_nr
        << ", type = " << (inode.type == DIR_INODE ? "DIR" : "FILE")
        << "]";
    return out;
}

int inode::get_inode_nr() const {
    DEBUGF('i', "inode = " << inode_nr);
    return inode_nr;
}

plain_file_ptr plain_file_ptr_of(file_base_ptr ptr) {
    plain_file_ptr pfptr = std::dynamic_pointer_cast<plain_file>(ptr);
    if (pfptr == nullptr)
        throw std::invalid_argument("plain_file_ptr_of");
    return pfptr;
}

directory_ptr directory_ptr_of(file_base_ptr ptr) {
    directory_ptr dirptr = std::dynamic_pointer_cast<directory>(ptr);
    if (dirptr == nullptr)
        throw std::invalid_argument("directory_ptr_of");
    return dirptr;
}

//====FILE====
std::ostream& operator<<(std::ostream& out, const plain_file& file) {
    out << "[FILE "
        << "size = " << file.size()
        << ", data = " << file.data
        << "]";
    return out;
}

/*
 * The size of a file is the
 * number of characters in it
 */
size_t plain_file::size() const {
    std::string file_str = util::intercalate(this->data, "");
    size_t size {file_str.length()};
    DEBUGF('i', "size = " << size);
    return size;
}

const util::wordvec& plain_file::readfile() const {
    DEBUGF('i', data);
    return data;
}

void plain_file::writefile(const util::wordvec& args) {
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

std::ostream& operator<<(std::ostream& out, const directory& dir) {
    out << "[DIRECTORY files = (";
    for (auto elem : dir.dirents) {
        out << elem.first << "|";
    }
    out << ")]";
    return out;
}

inode& directory::mkdir(inode_ptr cwd, const std::string& dirname) {
    inode_ptr dir = std::make_shared<inode>(inode(DIR_INODE));
    auto maybe_dir = dirents.find(dirname);
    if (maybe_dir != dirents.end()) {
        throw util::yshell_exn("Directory "
                + dirname + " already exists");
    }
    dirents.emplace(dirname, dir);
    dir_map& new_dir = directory_ptr_of(dir->contents)->dirents;
    new_dir.emplace(".", dir);
    new_dir.emplace("..", cwd);
    return *dir.get();
}

void directory::remove(const std::string& filename) {
    DEBUGF('i', filename);
    auto maybe_file = dirents.find(filename);
    if (maybe_file->second->type == DIR_INODE
            && directory_ptr_of(maybe_file->second->contents)
                ->size() > 2) {
        throw util::yshell_exn("directory::remove: "
                "Cannot remove a non-empty directory");
    }
    dirents.erase(maybe_file);
}

//====INODE_STATE====
inode_state::inode_state() {
    root = std::make_shared<inode>(inode(DIR_INODE));
    directory_ptr root_dir = directory_ptr_of(root->contents);
    root_dir->dirents.emplace(".", root);
    root_dir->dirents.emplace("..", root);
    cwd = root;
}

void directory::df_clear() {
    for (auto it : this->dirents) {
        if (it.second->type == DIR_INODE
                && it.first != "."
                && it.first != "..") {
            directory_ptr_of(it.second->contents)->df_clear();
        }
    }
    this->dirents.clear();
}

inode_state::~inode_state() {
    directory_ptr_of(root->contents)->df_clear();
}

std::ostream& operator<<(std::ostream& out, const inode_state& state) {
    out << "[INODE_STATE "
        << "root = " << state.root
        << ", cwd = " << state.cwd
        << ", prompt = \"" << state.prompt << "\""
        << "]";
    return out;
}

void inode_state::set_prompt(std::string new_prompt) {
    this->prompt = new_prompt;
}

std::string inode_state::get_prompt() {
    return this->prompt;
}

//=====INODE_STATE COMMAND/SHELL FUNCTIONS=====
void inode_state::cat(const util::wordvec& args) {
    DEBUGF('i', args);
    inode_ptr old_cwd = cwd;

    for (std::string file_path : args) {
        inode_ptr tmp_cwd = cwd;

        util::wordvec path = util::split(file_path, "/");
        if (path.size() > 1) {
            this->cd(util::wordvec(path.begin(), path.end()-1));
        }

        std::string filename = path.back();
        dir_map this_dir = directory_ptr_of(cwd->contents)->dirents;
        auto maybe_file = this_dir.find(filename);
        if (maybe_file == this_dir.end()) {
            cwd = old_cwd;
            throw util::yshell_exn("cat: "
                    + filename + ": File not found");
        } else if (maybe_file->second->type == DIR_INODE) {
            cwd = old_cwd;
            throw util::yshell_exn("cat: "
                    + filename + ": Is a directory");
        }
        auto file = plain_file_ptr_of(maybe_file->second->contents);
        std::cout << file->readfile() << std::endl;

        cwd = tmp_cwd;
    }

    cwd = old_cwd;
}

void inode_state::cd(const util::wordvec& args) {
    if (args.size() == 0) {
        return;
    } else if (args[0] == "/") {
        cwd = root;
        return;
    }
    directory_ptr dir = directory_ptr_of(cwd->contents);
    auto maybe_dir = dir->dirents.find(args[0]);
    if (maybe_dir == dir->dirents.end()) {
        throw util::yshell_exn("cd: Could not find directory: "
                + args[0]);
    } else if (maybe_dir->second->type != DIR_INODE) {
        throw util::yshell_exn("cd: "
                + args[0] + " is not a directory");
    }
    inode_ptr old_cwd = cwd;

    // Change directory!
    cwd = maybe_dir->second;

    try {
        this->cd(util::wordvec(args.begin()+1, args.end()));
    } catch(util::yshell_exn& exn) {
        cwd = old_cwd;
        throw exn;
    }
}

void print_inode(inode_ptr inode, std::string dirname) {
    std::cout << std::setw(6) << inode->get_inode_nr() << "  "
        << std::setw(6) << inode->contents->size() << "  "
        << dirname << (inode->type == DIR_INODE ? "/" : "")
        << std::endl;
}

void inode_state::ls(const util::wordvec& args, bool recursive) {
    DEBUGF('i', args);
    inode_ptr old_cwd = cwd;

    util::wordvec arg_paths =
        (args.size() == 0
         ? util::wordvec{"."}
         : args);

    for (auto arg_path : arg_paths) {
        inode_ptr tmp_cwd = cwd;
        util::wordvec dir_stack;
        util::wordvec path = util::split(arg_path, "/");
        if (arg_path == "/") {
            this->cd(util::wordvec{"/"});
        } else if (path.size() > 0) {
            // cd to all but the last in path
            this->cd(util::wordvec(path.begin(), path.end()-1));
            // if path.back() is a dir => cd
            // else print & cleanup
            directory_ptr dir = directory_ptr_of(cwd->contents);
            auto maybe_dir = dir->dirents.find(path.back());
            if (maybe_dir->second->type == DIR_INODE) {
                this->cd(util::wordvec{path.back()});
            } else {
                print_inode(maybe_dir->second, maybe_dir->first);
                goto cleanup;
            }
        }
        std::cout << this->get_wd() << ":" << std::endl;
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
            for (std::string elem : dir_stack) {
                this->ls(util::wordvec{elem}, true);
            }
        }
cleanup:
        cwd = tmp_cwd;
    }
    cwd = old_cwd;
}

void inode_state::make(const util::wordvec& args) {
    DEBUGF('i', args);
    if (args.size() == 0) {
        throw util::yshell_exn("Please submit a valid path to make");
    }
    inode_ptr old_cwd = cwd;
    util::wordvec path = util::split(args[0], "/");
    if (path.size() > 1) {
        this->cd(util::wordvec(path.begin(), path.end()-1));
    }
    std::string filename = path.back();

    dir_map& cwd_dir = directory_ptr_of(cwd->contents)->dirents;
    auto maybe_inode = cwd_dir.find(filename);
    inode_ptr i_new_file;
    // If found, make sure is not a directory
    if (maybe_inode != cwd_dir.end()) {
        if (maybe_inode->second->type == DIR_INODE) {
            cwd = old_cwd;
            throw util::yshell_exn("make: " + filename
                    + ": File already exists");
        } else {
            i_new_file = maybe_inode->second;
        }
    } else { // otherwise, replace contents
        i_new_file = std::make_shared<inode>(inode{PLAIN_INODE});
        cwd_dir.emplace(filename, i_new_file);
    }
    // Maybe overwrite file
    if (args.size() > 1) {
        plain_file_ptr new_file
            = {plain_file_ptr_of(i_new_file->contents)};
        new_file->writefile(util::wordvec(args.begin()+1, args.end()));
    }
    cwd = old_cwd;
}

void inode_state::mkdir(const util::wordvec& args) {
    DEBUGF('i', args);
    inode_ptr old_cwd = cwd;

    util::wordvec path = util::split(args[0], "/");
    std::string dir_name = path.back();
    if (path.size() > 1) {
        path.pop_back();
        this->cd(path);
    }
    directory_ptr_of(cwd->contents)->mkdir(cwd, dir_name);

    cwd = old_cwd;
}

std::string inode_state::get_wd() {
    inode_ptr old_cwd = cwd;
    int prev_inode;
    util::wordvec path;
    std::string prev_dir;
    while (cwd != root) {
        // Store the id of the inode we're in
        prev_inode = cwd->get_inode_nr();
        // cd to cwd's parent
        auto parent = directory_ptr_of(cwd->contents)
            ->dirents.find("..");
        cwd = parent->second;
        // Get the name of the directory we were in
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
    cwd = old_cwd;

    reverse(path.begin(), path.end());
    return util::intercalate(path, "/") + "/";
}

void inode_state::rm(const util::wordvec& args, bool recursive) {
    DEBUGF('i', args);
    DEBUGF('i', recursive);
    inode_ptr old_cwd = cwd;

    if (args.size() == 0) {
        throw util::yshell_exn("rm: Please specify a path");
    } else if (args.size() != 1) {
        throw util::yshell_exn("rm: Can only delete 1 file at a time");
    } else if (args[0] == "/") {
        throw util::yshell_exn("rm: Cannot remove root");
    } else {
        util::wordvec path = util::split(args[0], "/");
        std::string filename;
        if (path.size() > 1) {
            this->cd(util::wordvec(path.begin(), path.end()-1));
        }
        filename = path.back();
        auto cur_dir = directory_ptr_of(cwd->contents);
        auto maybe_file = cur_dir->dirents.find(filename);
        if (maybe_file == cur_dir->dirents.end()) {
            cwd = old_cwd;
            throw util::yshell_exn("rm: Invalid path: " + filename);
        }
        if (recursive && maybe_file->second->type == DIR_INODE
                && maybe_file->second->contents->size() > 2) {
            // Go to filename
            inode_ptr prev_cwd = cwd;
            this->cd(util::wordvec{maybe_file->first});
            dir_map this_dir
                = directory_ptr_of(maybe_file->second->contents)
                ->dirents;
            // Remove all in this_dir
            for (auto item : this_dir) {
                if (item.first == "." || item.first == "..")
                    continue;
                inode_ptr tmp_cwd = cwd;
                this->rm(util::wordvec{item.first}, true);
                cwd = tmp_cwd;
            }
            cwd = prev_cwd;
            this->rm(util::wordvec{filename}, true);
        } else {
            cur_dir->remove(filename);
        }
    }
    cwd = old_cwd;
}

