#include <mpi.h>
#include <iostream>
#include <cstring>  // for memcpy, strcpy

using namespace std;

#define MAX_KEY_SIZE 128
#define MAX_LOG_ENTRIES 4096
#define MAX_BUFFER_SIZE (sizeof(int) * 6 + MAX_LOG_ENTRIES * (sizeof(int) + MAX_KEY_SIZE + sizeof(int)))

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

void serialize(const Raft_Message &msg, char *buffer, int &size) {
    int pos = 0;
    memcpy(buffer + pos, &msg.type, sizeof(int)); pos += sizeof(int);
    memcpy(buffer + pos, &msg.term, sizeof(int)); pos += sizeof(int);
    memcpy(buffer + pos, &msg.leaderId, sizeof(int)); pos += sizeof(int);
    memcpy(buffer + pos, &msg.prevLogIndex, sizeof(int)); pos += sizeof(int);
    memcpy(buffer + pos, &msg.prevLogTerm, sizeof(int)); pos += sizeof(int);
    memcpy(buffer + pos, &msg.numEntries, sizeof(int)); pos += sizeof(int);

    for (int i = 0; i < msg.numEntries; i++) {
        memcpy(buffer + pos, &msg.entries[i].term, sizeof(int)); pos += sizeof(int);
        memcpy(buffer + pos, msg.entries[i].key, MAX_KEY_SIZE); pos += MAX_KEY_SIZE;
        memcpy(buffer + pos, &msg.entries[i].value, sizeof(int)); pos += sizeof(int);
    }
    size = pos;
}

void deserialize(const char *buffer, Raft_Message &msg) {
    int pos = 0;
    memcpy(&msg.type, buffer + pos, sizeof(int)); pos += sizeof(int);
    memcpy(&msg.term, buffer + pos, sizeof(int)); pos += sizeof(int);
    memcpy(&msg.leaderId, buffer + pos, sizeof(int)); pos += sizeof(int);
    memcpy(&msg.prevLogIndex, buffer + pos, sizeof(int)); pos += sizeof(int);
    memcpy(&msg.prevLogTerm, buffer + pos, sizeof(int)); pos += sizeof(int);
    memcpy(&msg.numEntries, buffer + pos, sizeof(int)); pos += sizeof(int);

    for (int i = 0; i < msg.numEntries; i++) {
        memcpy(&msg.entries[i].term, buffer + pos, sizeof(int)); pos += sizeof(int);
        memcpy(msg.entries[i].key, buffer + pos, MAX_KEY_SIZE); pos += MAX_KEY_SIZE;
        memcpy(&msg.entries[i].value, buffer + pos, sizeof(int)); pos += sizeof(int);
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_size < 2) {
        if (world_rank == 0)
            std::cerr << "Need at least 2 processes to run this example." << std::endl;
        MPI_Finalize();
        return 1;
    }

    // Starting Values
    int term = 0;
    //..................

    if (world_rank == 0) {
        Raft_Message msg;
        msg.type = MSG_APPENDENTRIES;
        msg.term = 1;
        msg.leaderId = 0;
        msg.prevLogIndex = 0;
        msg.prevLogTerm = 0;
        msg.numEntries = 1;

        strcpy(msg.entries[0].key, "foo");
        msg.entries[0].term = 1;
        msg.entries[0].value = 42;

        char buffer[MAX_BUFFER_SIZE];
        int size;
        serialize(msg, buffer, size);

        MPI_Send(buffer, size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        std::cout << "Process 0 sent message to Process 1\n";
    } else if (world_rank == 1) {
        char buffer[MAX_BUFFER_SIZE];
        MPI_Status status;

        MPI_Recv(buffer, MAX_BUFFER_SIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        Raft_Message msg;
        deserialize(buffer, msg);

        std::cout << "Process " << world_rank << " received message of type: " << msg.type
                  << ", key: " << msg.entries[0].key << ", value: " << msg.entries[0].value << std::endl;
        std::cout << "  From rank: " << status.MPI_SOURCE << ", tag: " << status.MPI_TAG << std::endl;
    }

    MPI_Finalize();
    return 0;
}
