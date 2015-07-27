
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cstdlib>
#include <iostream>

#define GAME_TOP 0
#define GAME_LEFT 0
#define GAME_WIDTH 1024
#define GAME_HEIGHT 768
#define GAME_RIGHT GAME_LEFT+GAME_WIDTH
#define GAME_BOTTOM GAME_TOP+GAME_HEIGHT

#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 20
#define PADDLE_VPOS GAME_HEIGHT - PADDLE_HEIGHT - 20

#define BALL_RADIUS 7

#define MS_PER_FRAME 16    //16 milliseconds per frame = 60fps

void Reset_Ball(sf::CircleShape *ball, sf::RectangleShape paddle);

int main()
{
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Breakout");
    sf::RectangleShape background = sf::RectangleShape(sf::Vector2<float>(GAME_WIDTH, GAME_HEIGHT));
    sf::RectangleShape paddle = sf::RectangleShape(sf::Vector2<float>(PADDLE_WIDTH, PADDLE_HEIGHT));
    sf::CircleShape ball = sf::CircleShape(BALL_RADIUS);
    sf::Vector2<float> ball_velocity = sf::Vector2f();

    background.setPosition(GAME_LEFT, GAME_TOP);
    background.setFillColor(sf::Color::Blue);

    paddle.setPosition((GAME_WIDTH / 2 - PADDLE_WIDTH / 2), PADDLE_VPOS);
    paddle.setFillColor(sf::Color::Green);

    sf::Vector2<int> moveVector;
    sf::Time dt = sf::Time::Zero;
    sf::Time t = sf::Time::Zero;
    sf::Clock clock;

    bool ball_on_paddle = 1;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // mouse movement
            if(event.type == sf::Event::MouseMoved)
            {
                paddle.setPosition(event.mouseMove.x - PADDLE_WIDTH / 2, paddle.getPosition().y);
            }

            if(event.type == sf::Event::MouseButtonPressed && ball_on_paddle)
            {
                ball_velocity = sf::Vector2<float>(400,-400);
                ball_on_paddle = 0;
            }
        }

        // handle collisions with wall
        if(ball.getPosition().x < GAME_LEFT || ball.getPosition().x > GAME_RIGHT - 2 * BALL_RADIUS)
        {
            ball_velocity.x = -ball_velocity.x;
        }
        if(ball.getPosition().y < GAME_TOP)
        {
            ball_velocity.y = -ball_velocity.y;
        }
        if(ball.getPosition().y > GAME_BOTTOM - 2 * BALL_RADIUS)
        {
            ball_on_paddle = 1;
        }

        // collisions with paddle
        // TODO implement this.

        dt = clock.restart();
        t += dt;

        if(ball_on_paddle)
        {
            Reset_Ball(&ball, paddle);
        }
        else
        {
            ball.move(ball_velocity * dt.asSeconds());
        }

        if(t.asMilliseconds() > MS_PER_FRAME)
        {
            // draw
            window.clear();
            window.draw(background);
            window.draw(paddle);
            window.draw(ball);
            window.display();

            t = t.Zero;
        }


    }

    return 0;
}

void Reset_Ball(sf::CircleShape *ball, sf::RectangleShape paddle)
{
    ball->setPosition(paddle.getPosition().x + PADDLE_WIDTH / 2 - BALL_RADIUS, paddle.getPosition().y - BALL_RADIUS * 2);
}
