
#if !defined(CONNECTION_HPP)
#define CONNECTION_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include "Internal_Config.hpp"

/*
 * Class oriented to set up a port for incoming connections on whatever ip
 * on system
 * */
class Cluster
{
	private:
		short _port;
		unsigned long _ip;
		int _socket_fd;
		sockaddr_in _addr_conf;

	public:
		//------------- Constructors and destructor -----------
		Cluster(short port);
		~Cluster();

		//------------- Methods --------------
		//This method return socket file descriptor associated with cluster	
		int get_socket_fd(void) const;
		//This method is gonna read the request and parse it
		void read_request(struct pollfd &accepted_fd);
};
#endif
