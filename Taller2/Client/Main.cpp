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
std::mutex mu;

//Escucha por un socket determinado y escribe el mensaje llegado.
//socket: Es el socket al cual debemos escuchar.
//recived: La longitud de los datos que recibiremos.
//aMensajes: Es donde contendra todos los mensajes del chat.
void RecivedFunction(sf::TcpSocket* socket,size_t* recived, vector<string>* aMensajes, sf::RenderWindow* window)
{
	while (window->isOpen())
	{
		mu.lock();
		char buffer[BUFFER_SIZE];
		sf::Socket::Status status = socket->receive(buffer, sizeof(buffer), *recived);
		string s = buffer;
		if (status != sf::Socket::Status::Disconnected)
		{
			aMensajes->push_back(s);
			if (aMensajes->size() > 25)
			{
				aMensajes->erase(aMensajes->begin(), aMensajes->begin() + 1);
			}
		}
		mu.unlock();
	}
	
}

int main()
{
	//ESTABLECER CONNECION
	sf::TcpSocket socket;
	size_t recived;

	// Obtenemos nuestra direccion ip, y nos connectamos con el puerto indicado y nuestra ip
	sf::IpAddress ip = sf::IpAddress::getLocalAddress();
	socket.connect(ip, CLIENT_PORT);

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
	
	//RECIVE
	//Se genera un thread (hilo), que escucha si llegan mensajes o no.
	thread t(RecivedFunction, &socket, &recived, &aMensajes, &window);

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
					socket.send(msn.c_str(), msn.size() + 1);

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
	t.join();

	socket.disconnect();

}