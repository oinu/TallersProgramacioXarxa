#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#define MAX_MENSAJES 30
#define SERVER_PORT 50000
#define CLIENT_PORT 50000
#define BUFFER_SIZE 2000


using namespace std;

int main()
{
	//ESTABLECER CONNECION
	sf::SocketSelector ss;
	vector<sf::TcpSocket*> socketList;

	sf::TcpListener listener;

	char buffer[BUFFER_SIZE];
	string msn;
	size_t recived;	

	//Le indicamos a que puerto debe escuchar para el servidor
	sf::TcpListener::Status stListener=listener.listen(SERVER_PORT);

	//Añadimos el listener al Socket Selector
	ss.add(listener);

	while (stListener != sf::TcpListener::Status::Disconnected)
	{
		//Mientras haya elementos en el Socket Selector, esperara a que algun socket le envie algo.
		while (ss.wait())
		{
			//Comprovamos si es un Listener
			if (ss.isReady(listener))
			{
				//Se añade el socket al Socket Selector
				sf::TcpSocket* socket = new sf::TcpSocket();
				//Esperamos peticion de un cliente
				sf::TcpListener::Status st = listener.accept(*socket);
				if (st == sf::TcpListener::Status::Done)
				{
					socketList.push_back(socket);
				}
				ss.add(*socket);

				//Les indicamos que se ha connectado un nuevo usuario
				msn = "New User Connected!";
				for (sf::TcpSocket* s : socketList)
				{
					if(s!=socket)s->send(msn.c_str(), msn.size() + 1);
				}
			}
			else
			{
				for (int j = 0; j < socketList.size(); j++)
				{
					if (ss.isReady(*socketList[j]))
					{
						socketList[j]->receive(buffer, sizeof(buffer), recived);
						//Pasar el contenido del buffer, al string de mensaje.
						msn = buffer;
						break;
					}
				}
				for (sf::TcpSocket* socket : socketList)
				{
					socket->send(msn.c_str(), msn.size() + 1);
				}
			}
		}
	}


	//Vaciar el Socket Selector
	ss.clear();

	//Paramos de escuchar el puerto, para así dejarlo libre.
	listener.close();

	//Desconnectamos todos los clientes
	for (sf::TcpSocket* &socket : socketList)
	{
		socket->disconnect();
	}

}