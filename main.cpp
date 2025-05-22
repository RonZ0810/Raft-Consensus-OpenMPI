#include <mpi.h>
#include <iostream>
#include <vector>
using namespace std;

#define MAX_KEY_SIZE 128
#define MAX_LOG_ENTRIES 4096

enum MessageType {
    MSG_REQUESTVOTE,
    MSG_REQUESTVOTEREPLY,
    MSG_APPENDENTRIES,
    MSG_APPENDENTRIESREPLY
};

struct LogEntry {
    int term;
    char key[MAX_KEY_SIZE];
    int value;
};

struct Raft_Message {
    MessageType type;
    int term;
    int leaderId;
    int prevLogIndex;
    int prevLogTerm;
    int numEntries;
    LogEntry entries[MAX_LOG_ENTRIES];
};
