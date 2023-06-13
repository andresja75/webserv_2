
#include "../../inc/errors/ConnectionError.hpp"

ConnectionError::ConnectionError(void)
	:_msg("ConnectionError: ")
{}

ConnectionError::ConnectionError(std::string msg)
	:_msg("ConnectionError: " + msg)
{}

ConnectionError::~ConnectionError() throw()
{}

const char* ConnectionError::what() const throw()
{
	return (this->_msg.c_str());
}
