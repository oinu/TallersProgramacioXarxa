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

struct Direccion
{
	std::string ip;
	unsigned short port;

	Direccion::Direccion()
	{

	}
	Direccion::Direccion(std::string aIp, unsigned short aPort)
	{
		ip = aIp;
		port = aPort;
	}
};

/*vector<Direccion> ParseToDireccion(sf::Packet p, int length)
{
}*/

void RecivedFunction(vector<sf::TcpSocket*> socket, vector<string>* aMensajes, sf::SocketSelector* ss)
{
	char buffer[BUFFER_SIZE];
	string msn;
	size_t recived;
	while (ss->wait())
	{
		for (int i = 0; i < socket.size(); i++)
		{
			if (ss->isReady(*socket[i]))
			{
				sf::Socket::Status st = socket[i]->receive(buffer, sizeof(buffer), recived);
				if (st == sf::Socket::Status::Done)
				{
					msn = buffer;
					aMensajes->push_back(msn);
					if (aMensajes->size() > 25)
					{
						aMensajes->erase(aMensajes->begin(), aMensajes->begin() + 1);
					}
				}
			}
		}
	}
}

int main()
{
	//ESTABLECER CONNECION
	sf::TcpSocket socket;
	sf::TcpListener listener;
	sf::SocketSelector ss;

	char connectionType;
	char buffer[BUFFER_SIZE];
	size_t recived, sended;
	sf::Socket::Status socketStatus;
	sf::Packet p;

	vector<Direccion> direccionList;
	vector<sf::TcpSocket*> socketList;

	// Obtenemos nuestra direccion ip, y nos connectamos con el puerto indicado y nuestra ip
	sf::IpAddress ip = sf::IpAddress::getLocalAddress();
	socket.connect(ip, CLIENT_PORT);

	//Recibir del servidor la lista de usuarios
	socketStatus = socket.receive(p);
	if (socketStatus != sf::TcpSocket::Status::Done)
	{
		cout << "Error" << endl;
	}
	else
	{
		//Obtenemos cuantos paquetes vamos a esperar
		int lenght;
		p >> lenght;

		//Si es 0, quiere decir que somos los primeros
		if (lenght != 0)
		{
			//Si no somos los primeros, reciviremos tantos paquetes como peers conocidos
			for (int j = 0; j < lenght; j++)
			{
				//Recivimos los packetes
				socketStatus = socket.receive(p);

				//Comprovamos que no haya error
				if (socketStatus != sf::TcpSocket::Status::Done)
				{
					cout << "Error";
				}
				else
				{
					//Creamos una direccion
					Direccion d;

					//Obtenemos su ip y puerto
					p >> d.ip >> d.port;

					//Lo colocamos a la list.
					direccionList.push_back(d);
				}
			}

			//Por cada dirección
			for (Direccion d : direccionList)
			{
				//creamos un socket
				sf::TcpSocket* newSocket = new sf::TcpSocket();
				//Lo connectamos con el puerto deseado.
				newSocket->connect(d.ip, d.port);
				//Lo añadimos al socket selector
				ss.add(*newSocket);
				//Lo añadimos al vector de sockets
				socketList.push_back(newSocket);
			}
		}
		cout << "ERES EL " << lenght + 1 << " DE 4" << endl;
	}
	//Nos guardamos nuestro puerto
	int localPort = socket.getLocalPort();

	//Liberamos el socket
	socket.disconnect();

	//Iniciamos el listener
	listener.listen(localPort);

	//Escuchamos tantas peticiones como usuarios falent
	for (int i = socketList.size(); i < 3; i++)
	{
		//Creamos un nuevo socket por conexion nueva
		sf::TcpSocket* nSocket = new sf::TcpSocket();
		//Esperamos a que aparezcan
		listener.accept(*nSocket);
		//Lo introducimos en el vector
		socketList.push_back(nSocket);
		//Lo añadimos al socket selector
		ss.add(*nSocket);
	}
	
	//Cerramos el listener
	listener.close();
	std::vector<std::string> aMensajes;

	sf::Vector2i screenDimensions(800, 600);

	sf::RenderWindow window;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Chat");

	sf::Font font;
	if (!font.loadFromFile("Roboto-Bold.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	sf::String mensaje = " >";

	sf::Text chattingText(mensaje, font, 24);
	chattingText.setFillColor(sf::Color(0, 160, 0));
	chattingText.setStyle(sf::Text::Bold);


	sf::Text text(mensaje, font, 24);
	text.setFillColor(sf::Color(0, 160, 0));
	text.setStyle(sf::Text::Bold);
	text.setPosition(0, 560);

	sf::RectangleShape separator(sf::Vector2f(800, 5));
	separator.setFillColor(sf::Color(200, 200, 200, 255));
	separator.setPosition(0, 550);

	string msn;

	//Lanzamos un Thread para recivir mensajes
	thread t(RecivedFunction,socketList, &aMensajes, &ss);

	while (window.isOpen())
	{
		sf::Event evento;
		while (window.pollEvent(evento))
		{
			switch (evento.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (evento.key.code == sf::Keyboard::Escape)
					window.close();
				else if (evento.key.code == sf::Keyboard::Return)
				{
					aMensajes.push_back(mensaje);
					if (aMensajes.size() > 25)
					{
						aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
					}
					// SEND
					// Pasamos el mensaje a std::string para hacerlo mas facil en el momento de enviarlo.
					msn = mensaje;

					//Se tiene que enviar a cada Peer
					//socket.send(msn.c_str(), msn.size() + 1);
					for (sf::TcpSocket* s : socketList)
					{
						socketStatus=s->send(msn.c_str(), msn.size() + 1);
						if (socketStatus != sf::TcpSocket::Status::Done)
						{
							cout << "Error" << endl;
						}
					}

					mensaje = ">";
				}
				break;
			case sf::Event::TextEntered:
				if (evento.text.unicode >= 32 && evento.text.unicode <= 126)
					mensaje += (char)evento.text.unicode;
				else if (evento.text.unicode == 8 && mensaje.getSize() > 0)
					mensaje.erase(mensaje.getSize() - 1, mensaje.getSize());
				break;
			}
		}

		
		window.draw(separator);
		for (size_t i = 0; i < aMensajes.size(); i++)
		{
			std::string chatting = aMensajes[i];
			chattingText.setPosition(sf::Vector2f(0, 20 * i));
			chattingText.setString(chatting);
			window.draw(chattingText);
		}
		std::string mensaje_ = mensaje + "_";
		text.setString(mensaje_);
		window.draw(text);


		window.display();
		window.clear();
	}

	//Acabar el Thread
	ss.clear();
	t.join();
	

	socket.disconnect();

}