#include "Server.hpp"

class Client 
{
	public:
		Client();
		Client(long fd);
		Client(const Client &copy);
		~Client();

		Client operator=(const Client &src);

		class EpollErrorExc: public std::exception
		{
			public:
				const char* what() const throw();
		};
	
	private:
		long		_fd;
		Request*	_request; // to do
		Response*	_response; // to create

}