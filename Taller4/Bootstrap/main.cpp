#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#define MAX_MENSAJES 30
#define SERVER_PORT 50000
#define BUFFER_SIZE 2000


using namespace std;
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

sf::Packet ParseToPacket(vector<Direccion> d)
{
	sf::Packet send;

	for (Direccion dir : d)
	{
		send << dir.ip << dir.port;
	}

	return send;
}

int main()
{
	//Creamos el listener que esperara a 4 clientes, por su puerto correspondiente.
	sf::TcpListener listener;
	listener.listen(SERVER_PORT);
	vector<Direccion> dList;


	//Esperamos las cuatro connexiones.
	for (int i = 0; i < 4; i++)
	{
		//Creamos un socket y escuchamos 
		sf::TcpSocket socket;
		listener.accept(socket);
		if (i != 0)
		{
			sf::Packet p;

			//Indicamos el numero de paquetes que recibira
			p << dList.size();
			socket.send(p);

			//Enviamos tantos paquetes como direcciones conocemos
			for (int j = 0; j < dList.size(); j++)
			{
				p.clear();
				p << dList[j].ip << dList[j].port;
				socket.send(p);
			}			
		}
		//posarlo a un vector de la estructura direccio, que conte la ip i el port.
		Direccion d(socket.getRemoteAddress, socket.getRemotePort);
		dList.push_back(d);
		
		socket.disconnect();
	}

	//Cerramos el listener
	listener.close();
}