
#include "../inc/Cluster.hpp"

//Constructors and destructor of Connection class

Cluster::Cluster (short port)
{
	int n = 1;

	//Assign port and any ip
	this->_port = port;
	this->_ip = INADDR_ANY;

	//Configuring _addr_conf structure
	memset(&this->_addr_conf, 0, sizeof(this->_addr_conf));
	this->_addr_conf.sin_family = SOCKET_DOMAIN;	
	this->_addr_conf.sin_port = htons(this->_port);
	this->_addr_conf.sin_addr.s_addr = INADDR_ANY;

	//Creating socket with TCP protocol 
	this->_socket_fd = socket(SOCKET_DOMAIN, TYPE, PROTOCOL);
	if(this->_socket_fd < 0)
	{
		std::cout<<"Error port: "<<this->_port<<std::endl;
		perror("Error creating socket try to relaunch server: ");
		return ;
	}
	if (setsockopt(this->_socket_fd, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(int)) < 0)
	{
		perror("Error making socket reusable");
		return ;
	}
	//Binding socket to any ip and determined port
	if(bind(this->_socket_fd, (struct sockaddr *) &this->_addr_conf, sizeof(this->_addr_conf)) < 0)
	{
		
		std::cout<<"Error port: "<<this->_port<<std::endl;
		perror("Error binding socket to establish connection, try to relaunch server: ");
		return ;
	}
	
	//Socket listening
	if(listen(this->_socket_fd, BACKLOG_SOCKET) < -1)
	{
		
		std::cout<<"Error port: "<<this->_port<<std::endl;
		perror("Error listening on socket, try to relaunch server");
	}

}

Cluster::~Cluster()
{
	if(close(this->_socket_fd) < 0)
	{
		std::cout<<"Error port: "<<this->_port<<std::endl;
		perror("Error closing socket: ");
		return ;
	}

	std::cout<<"Socket with port "<<this->_port<<" closed"<<std::endl;
}

//----------- Methods for Cluster class ------------

//Methods to return socket file descriptor associated with cluster
int Cluster::get_socket_fd(void) const
{
	return this->_socket_fd;
}

//This method is gonna read the request and parse it
void Cluster::read_request(struct pollfd &accepted_fd)
{
	char buffer[1000];
	int leido = 0;

	std::cout<<"reading request"<<std::endl;
	leido = read(accepted_fd.fd, buffer, 1000);
	std::cout<<buffer<<std::endl;
}
