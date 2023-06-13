
#if !defined(INTERNAL_CONFIG_HPP)
#define INTERNAL_CONFIG_HPP
#include <sys/socket.h>

//Color constants to display colored text
#define GREEN "\033[32m"
#define RED "\033[31m"
#define BLUE "\033[34m"
#define YELLOW "\033[33m"
#define NC "\033[39m"

//Constants for internal configuration of socket
#define SOCKET_DOMAIN AF_INET
#define TYPE SOCK_STREAM
#define PROTOCOL 0

//Number of qeued connections on socket
#define BACKLOG_SOCKET 100

//Maximum number of opened file descriptors, it is used in poll
#define	NUM_MAX_FDS 256
#endif
