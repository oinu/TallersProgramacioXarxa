#include <string>
struct Direccion
{
	std::string ip;
	unsigned short port;

	Direccion::Direccion(std::string aIp, unsigned short aPort)
	{
		ip = aIp;
		port = aPort;
	}
};