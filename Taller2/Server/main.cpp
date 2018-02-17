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

/*void RecivedFunction(vector<sf::TcpSocket> *sl,sf::TcpListener* listener, sf::SocketSelector* ss)
{
	char buffer[BUFFER_SIZE];
	string msn;
	size_t recived;
	bool found = false;
	int i = 0;

	//Cogemos la referencia del vector.
	vector<sf::TcpSocket> socketList = *sl;

	//Mientras haya elementos en el Socket Selector, esperara a que algun socket le envie algo.
	while (ss->wait())
	{
		while (!found || i<socketList.size())
		{
			if (ss->isReady(*listener))
			{
				//Se añade el socket al Socket Selector
				sf::TcpSocket socket;
				//Esperamos peticion de un cliente
				listener->accept(socket);
				ss->add(socket);
				//Indicamos que hemos encontrado el elemento disparador.
				found = true;
			}
			else if (ss->isReady(socketList[i]))
			{
				//Recive el paquete del socket destino
				sf::Socket::Status st = socketList[i].receive(buffer, sizeof(buffer), recived);

				//Si ha recibido el paquete
				if (st == sf::Socket::Status::Done)
				{
					//Pasar el contenido del buffer, al string de mensaje.
					msn = buffer;

					//Le enviamos el mensaje al los clientes
					for (int j = 0; j < socketList.size(); j++)
					{
						//Evitamos que el mensaje se envie al cliente original
						if (i != j)
						{
							socketList[j].send(msn.c_str(), msn.size()+1);
						}
					}
				}

				found = true;
			}
			else i++;
		}
		
		found = false;
		i = 0;
	}
}*/

int main()
{
	//ESTABLECER CONNECION
	sf::SocketSelector ss;
	vector<sf::TcpSocket> socketList;

	sf::TcpListener listener;

	char buffer[BUFFER_SIZE];
	string msn;
	size_t recived;
	bool found = false;
	int i = 0;
		
	
	//Le indicamos que no bloquee, ya que debe acceptar peticiones en cualquier momento
	listener.setBlocking(false);

	//Le indicamos a que puerto debe escuchar para el servidor
	listener.listen(SERVER_PORT);

	//Añadimos el listener al Socket Selector
	ss.add(listener);
	
	//Mientras haya elementos en el Socket Selector, esperara a que algun socket le envie algo.
	while (ss.wait())
	{
		//Miramos cual ha dado señal
		while (!found || i<socketList.size())
		{
			//Comprovamos si es un Listener
			if (ss.isReady(listener))
			{
				//Se añade el socket al Socket Selector
				sf::TcpSocket socket;
				//Esperamos peticion de un cliente
				sf::TcpListener::Status st=listener.accept(socket);
				if (st != sf::TcpListener::Status::Done)
				{
					socketList.push_back(socket);
				}
					
				ss.add(socket);
				//Indicamos que hemos encontrado el elemento disparador.
				found = true;
				cout << "New User" << endl;
			}

			//Comprovamos si es un socket
			else if (ss.isReady(socketList[i]))
			{
				//Recive el paquete del socket destino
				sf::Socket::Status st = socketList[i].receive(buffer, sizeof(buffer), recived);

				//Si ha recibido el paquete
				if (st == sf::Socket::Status::Done)
				{
					//Pasar el contenido del buffer, al string de mensaje.
					msn = buffer;

					//Le enviamos el mensaje al los clientes
					for (int j = 0; j < socketList.size(); j++)
					{
						//Evitamos que el mensaje se envie al cliente original
						if (i != j)
						{
							socketList[j].send(msn.c_str(), msn.size() + 1);
						}
					}
				}

				found = true;
			}
			else i++;
		}

		found = false;
		i = 0;
	}


	//Vaciar el Socket Selector
	ss.clear();

	//Paramos de escuchar el puerto, para así dejarlo libre.
	listener.close();

	//Desconnectamos todos los clientes
	for (sf::TcpSocket &socket : socketList)
	{
		socket.disconnect();
	}

}