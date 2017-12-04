//
// Created by Dude XPS on 12/3/2017.
//


#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#ifndef SFML_EXAMPLE_PLAYER_H
#define SFML_EXAMPLE_PLAYER_H


class Player {

public:

    sf::Sprite sprite;
    sf::Text text;
    int life;
    bool isHost;


    Player(sf::Sprite player, int startLife, bool isHost)
    {
        init(player, startLife, isHost);
    }
    ~Player()
    {

    }

    void init(sf::Sprite player, int startLife, bool isHosting)
    {
        sprite = player;

        life = startLife;
        isHost = isHosting;
    }


    int getLife()
    {
        return life;
    }

    void addLife(int addedLife)
    {
        life+=addedLife;
    }

    int rollDice(int sides)
    {
        sf::Time t;
        srand(time(NULL));
        return rand()%1+sides;
    }

};


#endif //SFML_EXAMPLE_PLAYER_H
