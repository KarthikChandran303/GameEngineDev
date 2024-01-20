//
//  Hello World client in C++
//  Connects REQ socket to tcp://localhost:5555
//  Sends "Hello" to server, expects "World" back
//
#include <zmq.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <string>
#include "nlohmann/json.hpp"
#include "shapes.h"
#include "vector.h"
#include "Setup.h"
#include <SFML/Graphics.hpp>

using json = nlohmann::json;

Player *createPlayer(sf::Texture &texture)
{
    Player *player = new Player(40.0f);
    player->setTexture(&texture);
    return player;
}

// Entity["player"] = false;
//         Entity["shape"] = "none";
//         Entity["texture"] = "none";
//         Entity["radius"] = 0;
//         Entity["sizeX"] = 0;
//         Entity["sizeY"] = 0;

void jsonToShapes(std::vector<sf::Shape *> &shapes, json data, std::vector<sf::Texture *> &textures, sf::Vector2f *playerVelocity, bool *playerJumping, int id, sf::View *view)
{
    for (int i = 0; i < data["Objects"].size(); i++)
    {
        json object = data["Objects"][i];
        sf::Shape *shape;
        if (object["shape"] == "circle")
        {
            shape = new sf::CircleShape(object["radius"]);
        }
        else if (object["shape"] == "rectangle")
        {
            shape = new sf::RectangleShape(sf::Vector2f(object["sizeX"], object["sizeY"]));
        }

        if (object["player"])
        {
            if (object["id"] == id)
            {
                playerVelocity->x = object["velocity"]["X"];
                playerVelocity->y = object["velocity"]["Y"];
                // print(*playerVelocity);
                *playerJumping = object["jumping"];
                bool right = object["right"];
                bool left = object["left"];
                if(right)
                {
                    view->move(800.0f, 0.0f);
                }
                else if(left)
                {
                    view->move(-800.0f, 0.0f);
                }
            }
            if (object["texture"] == "none") {
                shape->setTexture(textures[1]);
            }
            else {
                shape->setTexture(textures[2]);
            }
        }

        else if (object["texture"] == "color")
        {
            shape->setFillColor(sf::Color::Magenta);
        }

        else if (object["texture"] == "mine")
        {
            shape->setTexture(textures[0]);
        }

        setOrginCenter(*shape);
        shape->setPosition(object["position"]["X"], object["position"]["Y"]);
        shapes.push_back(shape);
    }
}

int main()
{
    sf::Texture platformTexture;
    sf::Texture playerTexture;
    sf::Texture playerTexture2;

    if (!platformTexture.loadFromFile("../Resources/textMine.jpeg"))
    {
        std::cout << "Cannot load texture!";
        return EXIT_FAILURE;
    }

    if (!playerTexture.loadFromFile("../Resources/awesomeface.png"))
    {
        std::cout << "Cannot load texture!";
        return EXIT_FAILURE;
    }

    if (!playerTexture2.loadFromFile("../Resources/test.png"))
    {
        std::cout << "Cannot load texture!";
        return EXIT_FAILURE;
    }

    std::vector<sf::Texture *> textures;
    textures.push_back(&platformTexture);
    textures.push_back(&playerTexture);
    textures.push_back(&playerTexture2);

    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socketReq(context, zmq::socket_type::req);

    std::cout << "Connecting to server..." << std::endl;
    socketReq.connect("tcp://localhost:5555");

    zmq::message_t request(7);
    memcpy(request.data(), "Connect", 7);
    socketReq.send(request, zmq::send_flags::none);

    zmq::message_t reply;
    socketReq.recv(reply, zmq::recv_flags::none);
    json j = json::parse(static_cast<char *>(reply.data()));

    int id = 0;
    int numOfPlayers = 0;

    // store json id in variable
    id = j["id"];
    std::string socket = j["port"];

    std::cout << "Connected with id: " << id << std::endl;
    std::cout << "socket created: " << socket << std::endl;

    sf::Vector2f *playerVelocity = new sf::Vector2f(0, 0);
    bool *playerJump = new bool(false);

    //  Connect as a subscriber
    zmq::socket_t subscriber(context, ZMQ_SUB);
    int conflate = 1;
    subscriber.setsockopt(ZMQ_CONFLATE, &conflate, sizeof(conflate));

    subscriber.connect("tcp://localhost:5556");
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "{", 1);
    // subscriber.set(zmq::sockopt::subscribe, "update");

    std::cout << "Connected as a subscriber" << std::endl;

    //  Connect as Pusher
    zmq::socket_t player0(context, ZMQ_PUSH);

    player0.connect(socket);

    // zmq::socket_t *playerSockets[3] = {&player0, &player1, &player2};

    std::vector<zmq::socket_t *> playerSockets;
    playerSockets.push_back(&player0);

    std::vector<sf::Shape *> objects;

    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
    sf::View view;
    // Initialize the view to a rectangle located at (100, 100) and with a size of 400x200
    view.reset(sf::FloatRect(0, 0, 800, 600));
    window.setView(view);

    bool closed = false;

    while (window.isOpen())
    {
        zmq::message_t broadcastMessage;
        subscriber.recv(broadcastMessage, zmq::recv_flags::none);
        // parse json data
        std::string data = static_cast<char *>(broadcastMessage.data());
        json renderData = json::parse(data);

        objects.clear();
        jsonToShapes(objects, renderData, textures, playerVelocity, playerJump, id, &view);

        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
            {
                window.close();
                closed = true;
            }
        }

        bool jumping = *playerJump;
        bool rightArrow = false;
        bool leftArrow = false;
        bool space = false;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && window.hasFocus())
        {
            leftArrow = true;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && window.hasFocus())
        {
            rightArrow = true;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && window.hasFocus())
        {
            space = true;
        }

        // push data to server
        json update;
        update["id"] = id;
        // serialize event data
        update["closed"] = closed;
        update["rightArrow"] = rightArrow;
        update["leftArrow"] = leftArrow;
        update["space"] = space;

        zmq::message_t updateMessage(update.dump().size() + 1);

        snprintf((char *)updateMessage.data(), update.dump().size() + 1, "%s", update.dump().c_str());
        std::string updateString = std::string(static_cast<char *>(updateMessage.data()), updateMessage.size());
        // std::cout << "Sending update " << updateString << std::endl;
        playerSockets[0]->send(updateMessage, zmq::send_flags::none);

        // clear the window with black color
        window.clear(sf::Color::Black);
        window.setView(view);

        // draw everything here...
        for (int i = 0; i < objects.size(); i++)
        {
            window.draw(*objects[i]);
        }
        // end the current frame
        
        window.display();
    }

    return 0;
}
