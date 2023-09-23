# DistributedBRaft

Installations
- gRPC
- C++
- LevelDB

Commands
- starting servers:
  -`./server <id>:<listen_port> <servers_file> [is_primary(yes)]`

- starting client:
  -`./client servers.txt`

Necessary Files
- server files : contains the ip address of other servers in the system <br>
    serverfile1.txt <br>
    serverfile2.txt <br>
    serverfile3.txt <br>
  
- client files:
  - servers.txt - contains the ip addresses of all servers in the system <br>
  - curr_leader.txt - contains the ip address of the current raft leader
