
#if !defined(SERVER_HPP)
#define SERVER_HPP

#include <vector>
#include <map>
#include <algorithm>
#include <poll.h>
#include <unistd.h>

#include "Cluster.hpp"
#include "Internal_Config.hpp"
class Server
{
	private:
		std::map<int, Cluster*> _clusters;//Correspondency between fd of socket and it cluster
		unsigned int _num_connections_and_open_fds;
		struct pollfd *_fd_events_sockets_or_fd;
		std::map <int, Cluster*> _open_fds;//Correspondency bettween fd and clusters


		//------------ Private Methods ---------------
				
		//Methods to insert new fds in poll fds
		void insert_fds(std::vector<int> &fds_to_insert);

		//Methods to delete fds in poll fds
		void delete_fds(std::vector<int> &fds_to_delete);

	public:
		//--------------- Constructors and destructor -------------
		Server(void);
		~Server();

		//----------------- Public Methods --------------
		void request_response_loop(void);
};

#endif
