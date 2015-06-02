#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream log(cout);
struct cix_exit: public exception {};

unordered_map<string,cix_command> command_map {
    {"exit", CIX_EXIT},
    {"help", CIX_HELP},
    {"ls"  , CIX_LS  },
    {"get" , CIX_GET },
    {"rm"  , CIX_RM  },
    {"put" , CIX_PUT },
};

void cix_help() {
    static vector<string> help = {
        "exit         - Exit the program.  Equivalent to EOF.",
        "get filename - Copy remote file to local host.",
        "help         - Print help summary.",
        "ls           - List names of files on remote server.",
        "put filename - Copy local file to remote host.",
        "rm filename  - Remove file from remote server.",
    };
    for (const auto& line: help) {
        cout << line << endl;
    }
}

void cix_ls(client_socket& server) {
    cix_header header;
    memset(&header, 0, sizeof header);
    header.command = CIX_LS;
    log << "sending header " << header << endl;
    send_packet(server, &header, sizeof header);
    recv_packet(server, &header, sizeof header);
    log << "received header " << header << endl;
    if (header.command != CIX_LSOUT) {
        log << "sent CIX_LS, server did not return CIX_LSOUT" << endl;
        log << "server returned " << header << endl;
        return;
    }
    char buffer[header.nbytes + 1];
    recv_packet(server, buffer, header.nbytes);
    log << "received " << header.nbytes << " bytes" << endl;
    buffer[header.nbytes] = '\0';
    cout << buffer;
}

void cix_get(client_socket& server, string filename) {
    cix_header header;
    memset(&header, 0, sizeof header);
    header.command = CIX_GET;
    strcpy(header.filename, filename.c_str());
    log << "sending header " << header << endl;
    send_packet(server, &header, sizeof header);
    recv_packet(server, &header, sizeof header);
    log << "received header " << header << endl;
    if (header.command != CIX_FILE) {
        log << "sent CIX_GET, server did not return CIX_FILE" << endl;
        log << "server returned " << header << endl;
        return;
    }
    char buffer[header.nbytes + 1];
    recv_packet(server, buffer, header.nbytes);
    log << "received " << header.nbytes << " bytes" << endl;
    buffer[header.nbytes] = '\0';
    cout << buffer;
    write_to_file(filename, buffer, header.nbytes);
}

void cix_put(client_socket& server, string filename) {
    cix_header header;
    memset(&header, 0, sizeof header);
    ifstream file (filename);
    if (file == NULL) {
        log << "invalid filename (" << filename << ")" << endl;
        return;
    }
    uint32_t nbytes = header.nbytes = file_size(filename);
    char output[nbytes];
    load_from_file(filename, output, nbytes);
    // send put cmd
    header.command = CIX_PUT;
    strcpy(header.filename, filename.c_str());

    log << "sending header " << header << endl;
    send_packet(server, &header, sizeof header);

    log << "sending: " << output << endl
        << "===" << endl;
    send_packet(server, output, sizeof output);

    // get ack or fail
    recv_packet(server, &header, sizeof header);
    log << "received header " << header << endl;
    if (header.command != CIX_ACK) {
        log << "sent CIX_PUT, server did not return CIX_ACK" << endl;
        log << "server returned " << header << endl;
        return;
    }
    log << "successfully sent file (" << filename << ")" << endl;
}

void cix_rm(client_socket& server, string filename) {
    cix_header header;
    memset(&header, 0, sizeof header);
    header.command = CIX_RM;
    strcpy(header.filename, filename.c_str());
    header.nbytes = 0;
    log << "sending header " << header << endl;
    send_packet(server, &header, sizeof header);

    recv_packet(server, &header, sizeof header);
    log << "received header " << header << endl;
    if (header.command != CIX_ACK) {
        log << "send CIX_RM, server did not return CIX_ACK" << endl;
        log << "server returned " << header << endl;
        return;
    }
    log << "file (" << filename << ") successfully rm'ed" << endl;
}

void usage() {
    cerr << "Usage: " << log.execname() << " [host] [port]" << endl;
    throw cix_exit();
}

int main(int argc, char** argv) {
    log.execname(basename(argv[0]));
    log << "starting" << endl;
    vector<string> args(&argv[1], &argv[argc]);
    string host;
    in_port_t port;
    if (args.size() > 2) {
        usage();
    } else if (args.size() == 1) {
        host = get_cix_server_host(args, 1);//for ignoring args
        port = get_cix_server_port(args, 0);
    } else {
        host = get_cix_server_host(args, 0);
        port = get_cix_server_port(args, 1);
    }
    log << to_string(hostinfo()) << endl;
    try {
        log << "connecting to " << host << " port " << port << endl;
        client_socket server(host, port);
        log << "connected to " << to_string(server) << endl;
        for (;;) {
            string line;
            getline(cin, line);
            if (cin.eof()) {
                throw cix_exit();
            }
            log << "line (" << line << ")" << endl;
            size_t fst_space_pos = line.find(" ");
            log << "fst_space_pos " << fst_space_pos << endl;
            string cmd_str = line.substr(0, fst_space_pos);
            string args = (fst_space_pos != string::npos
                    ? line.substr(fst_space_pos+1, line.size())
                    : "");
            log << "cmd_str: " << cmd_str << endl;
            log << "args: " << args << endl;
            if (cmd_str.size() == 0) {
                continue;
            }
            const auto& itor = command_map.find(cmd_str);
            cix_command cmd = itor == command_map.end()
                ? CIX_ERROR : itor->second;
            switch (cmd) {
                case CIX_EXIT:
                    throw cix_exit();
                    break;
                case CIX_HELP:
                    cix_help();
                    break;
                case CIX_LS:
                    cix_ls(server);
                    break;
                case CIX_GET:
                    cix_get(server, args);
                    break;
                case CIX_PUT:
                    cix_put(server, args);
                    break;
                case CIX_RM:
                    cix_rm(server, args);
                    break;
                default:
                    log << line << ": invalid command" << endl;
                    break;
            }
        }
    } catch (socket_error& error) {
        log << error.what() << endl;
    } catch (cix_exit& error) {
        log << "caught cix_exit" << endl;
    }
    log << "finishing" << endl;
    return 0;
}
