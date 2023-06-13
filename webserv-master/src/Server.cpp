 
#include "../inc/Server.hpp"
#include "../inc/errors/ConnectionError.hpp"
//Constructors and destructor of Server class
extern bool active;

Server::Server(void)
	: _num_connections_and_open_fds(0)
{
	std::cout<<GREEN<<"Initializing server... "<<NC<<std::endl;
	int i;
	
	//Setting to null pointer
	this->_fd_events_sockets_or_fd = NULL;
	//Clusters hardcoded, it will be replaced later
	try
	{
		Cluster *c0 = new Cluster(8080);
		Cluster *c1 = new Cluster(8081);
		Cluster *c2 = new Cluster(8082);
		this->_clusters.insert(std::pair<int, Cluster *>(c0->get_socket_fd(), c0));
		this->_clusters.insert(std::pair<int, Cluster*>(c1->get_socket_fd(), c1));
		this->_clusters.insert(std::pair<int, Cluster*>(c2->get_socket_fd(), c2));
	}catch(std::bad_alloc &ex)
	{
		std::cout<<"Error allocating memory"<<ex.what()<<std::endl;
		throw std::bad_alloc();
	}

	//Allocating memory for initial structure of pollfds	
	this->_num_connections_and_open_fds = this->_clusters.size();
	this->_fd_events_sockets_or_fd = new struct pollfd[this->_num_connections_and_open_fds];

	//Setting poll
	i = 0;	
	for(std::map<int, Cluster *>::iterator it = this->_clusters.begin();
		it != this->_clusters.end(); it++)
	{
		memset(&(this->_fd_events_sockets_or_fd[i]), 0, sizeof(this->_fd_events_sockets_or_fd[i]));
		this->_fd_events_sockets_or_fd[i].fd = it->first;
		this->_fd_events_sockets_or_fd[i].events |= POLLIN;

		i++;
	}
	
}

Server::~Server()
{
	//Deleting clusters in map
	for(std::map<int, Cluster *>::iterator it = this->_clusters.begin(); 
			it != this->_clusters.end(); it++)
	{
		delete it->second;
	}
	
	//Deleting array of incomming connections
	if(this->_fd_events_sockets_or_fd != NULL)
		delete [] this->_fd_events_sockets_or_fd;	
}

//--------- Public Methods of Server class ----------------

void Server::request_response_loop(void)
{
	//This is to check number of events triggered
	int events;
	std::vector<int> fds_to_insert;
	std::vector<int> fds_to_delete;
	//int variable_borrar = 0;	
	
	events = 0;
	
	while(true)
	{

		std::cout<<"num: "<<this->_num_connections_and_open_fds<<std::endl;
		std::cout<<"Esperando una conexion o un evento en algun descriptor de fichero..."<<std::endl;
		events = poll(this->_fd_events_sockets_or_fd, this->_num_connections_and_open_fds, -1);
		std::cout<<"Ha llegado una conexion o un evento en algun descriptor de fichero"<<std::endl;

		if(events > 0)
		{
			//Associating incomming connections and his fds with clusters in a map
			for(unsigned int i = 0; i < this->_num_connections_and_open_fds; i++)
			{
				//This if checks if there is a new incomming connection
				if(this->_clusters.find(this->_fd_events_sockets_or_fd[i].fd) 
						!= this->_clusters.end())
				{
					if(this->_fd_events_sockets_or_fd[i].revents & POLLIN)
					{
						int fd = accept(this->_fd_events_sockets_or_fd[i].fd, NULL, NULL);
			
						int flags = fcntl(fd, F_GETFL);
						if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
						{
							//Here we should send a 500 internal error server!!!
							close(fd);
							continue;
						}
						this->_open_fds.insert(std::pair<int, Cluster *> (fd, this->_clusters[i]));
						fds_to_insert.push_back(fd);	
					}
				}
				else
				{
					//This checks if socket has hang up
					if(this->_fd_events_sockets_or_fd[i].revents & POLLHUP)
					{
						close(this->_fd_events_sockets_or_fd[i].fd);
						fds_to_delete.push_back(this->_fd_events_sockets_or_fd[i].fd);
						continue;
					}

					//This checks if there is data to read in fd
					if(this->_fd_events_sockets_or_fd[i].revents & POLLIN)
					{
						this->_fd_events_sockets_or_fd[i].events |= POLLHUP;
						std::map<int, Cluster *>::iterator it = 
						this->_open_fds.find(this->_fd_events_sockets_or_fd[i].fd);

						if(	it != this->_clusters.end())
						{
							it->second->read_request(this->_fd_events_sockets_or_fd[i]);	
						}
					}
				}
			}
		}
		else if(events< 0)
		{
			perror("Error polling connections: ");
		}
		
		//Inserting new file descriptors into polling array
		insert_fds(fds_to_insert);

		//Deleting file descriptors which are not being used
		delete_fds(fds_to_delete);
		std::cout<<"Num of active fds: "<<this->_num_connections_and_open_fds<<std::endl;
		std::cout<<"--------------------------"<<std::endl;
		//It remains close fds which are not being used
		
		//¡¡¡¡¡¡¡¡¡¡BORRAR ESTA PARTE!!!!!!!!!!!!
		/*variable_borrar++;
		if(variable_borrar == 5)
			break;*/
	}

	//When we leave the loop we should free all memory which it is not being used
	
}

//----------------- Private Methods of Server Class -------------------

//Method to add file descriptors to the array we are polling
void Server::insert_fds(std::vector<int> &fds_to_insert)
{
	struct pollfd *new_array_fds;
	unsigned int new_size_of_array_fds;
	unsigned int pos;

	new_size_of_array_fds = 0;
	if(fds_to_insert.empty() == true)
		return ;
		
	new_size_of_array_fds = this->_num_connections_and_open_fds + fds_to_insert.size();
	
	//Allocating memory for new pollfd array	
	try
	{
		new_array_fds =  new struct pollfd [new_size_of_array_fds];
	}
	catch(std::bad_alloc &ex)
	{
		std::cout<<"Error allocating memory: "<<ex.what()<<std::endl;
		return ;
	}
	
	//Clean the new array of pollfd
	for(unsigned int i = 0; i < new_size_of_array_fds; i++)
	{
		memset(&new_array_fds[i], 0, sizeof(new_array_fds[i]));	
	}

	//Assigning pollfd which remains in old pollfd array
	for(pos = 0; pos < this->_num_connections_and_open_fds; pos++)
	{
		new_array_fds[pos].fd = this->_fd_events_sockets_or_fd[pos].fd;
		new_array_fds[pos].events = this->_fd_events_sockets_or_fd[pos].events;
		new_array_fds[pos].revents = this->_fd_events_sockets_or_fd[pos].revents;
	}

	//Adding new fds in read mode
	for(std::vector<int>::iterator it = fds_to_insert.begin(); it != fds_to_insert.end(); it++)
	{
		new_array_fds[pos].fd = (*it);
		new_array_fds[pos].events |= POLLIN;
		new_array_fds[pos].events |= POLLHUP;
		new_array_fds[pos].revents = 0;
		pos++;
	}

	//Free old pullfd array
	if(this->_fd_events_sockets_or_fd != NULL)
		delete [] this->_fd_events_sockets_or_fd;
	
	//Update new array and it size 
	this->_fd_events_sockets_or_fd = new_array_fds;
	this->_num_connections_and_open_fds = new_size_of_array_fds;

	//Quit fds from vector which are already added to new pollfd array
	fds_to_insert.clear();
	
}


//Method to remove file descriptors in the array we are polling
void Server::delete_fds(std::vector<int> &fds_to_delete)
{
	struct pollfd *new_array_fds;
	unsigned int new_array_size;
	unsigned int i;
	new_array_size = this->_num_connections_and_open_fds - fds_to_delete.size();

	//Allocating memory for new pollfd array	
	try
	{
		new_array_fds = new struct pollfd[new_array_size];
	}
	catch(std::bad_alloc &ex)
	{
		std::cout<<"Error allocating memory"<<ex.what()<<std::endl;	
	}
	
	//Assigning data fds to new poll fds array
	i = 0;
	for(unsigned int pos = 0; pos < this->_num_connections_and_open_fds; pos++)
	{
		if(std::find(fds_to_delete.begin(), fds_to_delete.end(), this->_fd_events_sockets_or_fd[pos].fd) 
				== fds_to_delete.end())
		{
			new_array_fds[i].fd = this->_fd_events_sockets_or_fd[pos].fd;
			new_array_fds[i].events = this->_fd_events_sockets_or_fd[pos].events;
			new_array_fds[i].revents = this->_fd_events_sockets_or_fd[pos].revents;
			i++;
		}
	}
	
	//Deleting allocated memory
	if(this->_fd_events_sockets_or_fd != NULL)
		delete [] this->_fd_events_sockets_or_fd;

	//Update new array and it size
	this->_num_connections_and_open_fds = new_array_size;
	if(new_array_size == 0)
		this->_fd_events_sockets_or_fd = NULL;
	else
		this->_fd_events_sockets_or_fd = new_array_fds;

	
	//Cleaning vector of fds to delete
	fds_to_delete.clear();


}
