
#if !defined(CONNECTION_ERROR_HPP)
#define CONNECTION_ERROR_HPP

#include <exception>
#include <string>
//Class to throw connections error in server
class ConnectionError: public std::exception
{
	private:
		std::string _msg;
	public:
		ConnectionError(void);
		ConnectionError(std::string msg);
		virtual ~ConnectionError() throw();
		const char* what() const throw();
};
#endif
