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

int main()
{
	//Creamos el listener que esperara a 4 clientes, por su puerto correspondiente.
	sf::TcpListener listener;
	listener.listen(SERVER_PORT);

	//Esperamos las cuatro connexiones.
	for (int i = 0; i < 4; i++)
	{
		//Creamos un socket y escuchamos 
		sf::TcpSocket socket;
		listener.accept(socket);
		//posarlo a un vector de la estructura direccio, que conte la ip i el port.
		//informar s'ha connectat un client.
		socket.disconnect();
	}

	//Cerramos el listener
	listener.close();
}