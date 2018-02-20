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

void DisconnectUser(vector<sf::TcpSocket*>& list, sf::SocketSelector *ss, int i)
{
	//Desconectamos el socket del cliente
	//list[i]->disconnect();

	//Lo eliminamos del Socket Selector
	ss->remove(*list[i]);
	delete(list[i]);

	//Eliminamos el socket de la lista
	list.erase(list.begin() + i, list.begin() + i + 1);

	
	//Informamos que un usuario se ha desconectado
	for (sf::TcpSocket* socket : list)
	{
		string outMsn = "A User Disconected";
		socket->send(outMsn.c_str(), outMsn.size() + 1);
	}

	cout << "User Disconnected" << endl;
}

int main()
{
	//ESTABLECER CONNECION
	sf::SocketSelector ss;
	vector<sf::TcpSocket*> socketList;
	sf::TcpSocket::Status socketStatus;

	sf::TcpListener listener;
	sf::TcpListener::Status listenerStatus;

	char buffer[BUFFER_SIZE];
	string msn;
	size_t recived;	

	//Le indicamos a que puerto debe escuchar para el servidor
	listenerStatus =listener.listen(SERVER_PORT);

	//Añadimos el listener al Socket Selector
	ss.add(listener);
	int i = 0;

	while (listenerStatus != sf::TcpListener::Status::Disconnected)
	{
		//Mientras haya elementos en el Socket Selector, esperara a que algun socket le envie algo.
		while (ss.wait())
		{
			i++;
			cout << i << endl;
			cout << "Salta evento wait\n";
			//Comprovamos si es un Listener
			if (ss.isReady(listener))
			{
				cout << "ready listener\n";
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

				cout << "New User Connecte" << endl;
			}

			//Si no es el listener, sera un socket
			else
			{

				//Miramos de que socket recivimos el mensaje
				for (int j = 0; j < socketList.size(); j++)
				{
					//Encontramos el socket
					if (ss.isReady(*socketList[j]))
					{
						cout << "Ha saltado un Socket" << endl;
						//Recivimos el mensaje.
						socketStatus = socketList[j]->receive(buffer, sizeof(buffer), recived);

						//Pasar el contenido del buffer, al string de mensaje.
						if (socketStatus == sf::TcpSocket::Status::Done)
						{
							cout << "Mensaje recibido" << endl;
							msn = buffer;

							//Enviamos el mensajes
							for (int i = 0; i < socketList.size(); i++)
							{
								sf::TcpSocket::Status st = socketList[i]->send(msn.c_str(), msn.size() + 1);

								if ( st == sf::TcpSocket::Status::Error)
								{
									cout << "Error al enviar" << endl;
								}
								
							}
						}

						//Si se ha desconnectado
						else if (socketStatus == sf::TcpSocket::Status::Disconnected)
						{
							cout << "Se desconecta un Usuario" << endl;
							DisconnectUser(socketList, &ss, j);
						}

						break;
					}
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

	//Limpiamos el vector
	socketList.clear();

}