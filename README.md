# distributeddBRaft
servers_file : serverfile1.txt, serverfile2.txt, serverfile3.txt - contains the ip address of other servers in the system

clientfiles:
servers.txt - contains the ip addresses of all servers in the system
curr_leader.txt - contains the ip address of the current raft leader

starting servers:
./server <id>:<listen_port> <servers_file> [is_primary(yes)]

starting client:
./client servers.txt

Ensure levelDB and grpc are installed.