# Raft Distributed Consensus (C++ with OpenMPI)

This project implements the [Raft consensus algorithm](https://raft.github.io/) using C++ and OpenMPI.

## Features
- Leader election via randomized timeouts
- RequestVote and AppendEntries RPCs using MPI
- State transitions (Follower, Candidate, Leader)
- Log replication and commit index tracking (WIP)

## Technologies
- Language: C++
- Message Passing: OpenMPI
- Build: Make

## Build and Run

```bash
make
mpirun -np 3 ./raft
