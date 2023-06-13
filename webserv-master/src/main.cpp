
#include <iostream>
#include "../inc/Server.hpp"
#include <signal.h>

bool active = true;

void check_leaks(void)
{
	system("leaks -q ./webserv");
}

int main(void)
{
	//atexit(check_leaks);
	try
	{
		Server s;
		s.request_response_loop();
	}
	catch(std::exception &e)
	{
		std::cout<<e.what()<<std::endl;
	}
	return 1;
}
