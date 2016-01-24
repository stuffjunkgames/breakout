
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <sstream>

#define GAME_TOP 0
#define GAME_LEFT 0
#define GAME_WIDTH 1024
#define GAME_HEIGHT 768
#define GAME_RIGHT GAME_LEFT+GAME_WIDTH
#define GAME_BOTTOM GAME_TOP+GAME_HEIGHT

#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 20
#define PADDLE_VPOS GAME_HEIGHT - PADDLE_HEIGHT - 20

#define BALL_RADIUS 8

#define BALL_SPEED 600
#define MAX_BLOCKS 50
#define BLOCK_SIZE 20

#define MS_PER_FRAME 16    //16 milliseconds per frame = 60fps

void Reset_Ball(sf::CircleShape *ball, sf::RectangleShape paddle);
float getDistance(sf::Vector2f point1, sf::Vector2f point2);
sf::Vector2f collide(sf::CircleShape ball, sf::RectangleShape block);
float Vector_Length(sf::Vector2f v);

int main()
{
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Breakout");
    sf::RectangleShape background = sf::RectangleShape(sf::Vector2<float>(GAME_WIDTH, GAME_HEIGHT));
    sf::RectangleShape paddle = sf::RectangleShape(sf::Vector2<float>(PADDLE_WIDTH, PADDLE_HEIGHT));
    sf::CircleShape ball = sf::CircleShape(BALL_RADIUS);
    sf::RectangleShape blocks[MAX_BLOCKS];
    bool blockVisible[MAX_BLOCKS];
    sf::Vector2<float> ball_velocity = sf::Vector2f();

    background.setPosition(GAME_LEFT, GAME_TOP);
    background.setFillColor(sf::Color::Blue);

    paddle.setPosition((GAME_WIDTH / 2 - PADDLE_WIDTH / 2), PADDLE_VPOS);
    paddle.setFillColor(sf::Color::Green);

    for(int i = 0; i < MAX_BLOCKS; i++)
    {
        blocks[i].setFillColor(sf::Color::Yellow);
        blocks[i].setSize(sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE));
        blockVisible[i] = 1;
    }

    srand(time(0));
    double randNum;
    double angle;
    double collisionAngle;

    sf::Vector2f projection;

    sf::Vector2<int> moveVector;
    sf::Time dt = sf::Time::Zero;
    sf::Time t = sf::Time::Zero;
    sf::Clock clock;

    std::stringstream ss (std::stringstream::in | std::stringstream::out);
    sf::String ups = sf::String();
    sf::Font font;

    if(!font.loadFromFile("arial.ttf"))
    {
        return 1;
    }
    sf::Text upsText = sf::Text();
    upsText.setFont(font);
    //sf::Font font;
    //if(!font.loadFromFile())

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
                randNum = (double)rand() / RAND_MAX;
                angle = randNum * M_PI_2 + M_PI_4;
                ball_velocity = sf::Vector2<float>(BALL_SPEED * cos(angle),-BALL_SPEED * sin(angle));
                ball_on_paddle = 0;
            }
        }

        // handle collisions with wall
        if(ball.getPosition().x < GAME_LEFT)
        {
            ball_velocity.x = abs(ball_velocity.x);
        }
        if(ball.getPosition().x > GAME_RIGHT - 2 * BALL_RADIUS)
        {
            ball_velocity.x = -abs(ball_velocity.x);
        }
        if(ball.getPosition().y < GAME_TOP)
        {
            ball_velocity.y = abs(ball_velocity.y);
        }
        if(ball.getPosition().y > GAME_BOTTOM - 2 * BALL_RADIUS)
        {
            ball_on_paddle = 1;
        }

        // collisions with paddle
        // TODO make this suck less.
        sf::Vector2f impulse = collide(ball, paddle);
        if(impulse.x != 0 || impulse.y != 0)
        {
            collisionAngle = acos((double)ball_velocity.y/(double)Vector_Length(ball_velocity)) +
                                acos((double)impulse.y/(double)Vector_Length(impulse)) + M_PI;

            projection = Vector_Length(ball_velocity) * (float)cos(collisionAngle) * impulse;
            ball_velocity = ball_velocity + 2.0f * projection;
            //std::cout << impulse.x << ", " << impulse.y << std::endl;
        }

        dt = clock.restart();
        t += dt;

        ss.str("");
        ss.clear();
        ss << (1/dt.asSeconds());
        ups = ss.str();
        upsText.setString(ups);

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
            window.draw(upsText);
            window.display();

            t = t.Zero;
        }

    }

    return 0;
}

void Reset_Ball(sf::CircleShape *ball, sf::RectangleShape paddle)
{
    ball->setPosition(paddle.getPosition().x + PADDLE_WIDTH / 2 - BALL_RADIUS, paddle.getPosition().y - BALL_RADIUS * 2 - 1);
}

float getDistance(sf::Vector2f point1, sf::Vector2f point2)
{
    return sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2));
}

float Vector_Length(sf::Vector2f v)
{
    return sqrt(pow(v.x,2) + pow(v.y,2));
}

sf::Vector2f collide(sf::CircleShape ball, sf::RectangleShape block)
{
    sf::Vector2f ball_center = ball.getPosition() + sf::Vector2f(ball.getRadius(), ball.getRadius());
    sf::Vector2f block_center = block.getPosition() + sf::Vector2f(block.getSize().x/2, block.getSize().y/2);

    sf::Vector2f diff = ball_center - block_center;

    // Not intersecting
    if(abs(diff.x) > block.getSize().x/2 + ball.getRadius() || abs(diff.y) > block.getSize().y/2 + ball.getRadius())
    {
        return sf::Vector2f(0,0);
    }
    std::cout << diff.x << ", " << diff.y << std::endl;

    // hit edge of block (top/bottom)
    if(abs(diff.x) <= PADDLE_WIDTH/2)
    {
        if(diff.y <= 0)
        {
            return sf::Vector2f(0,-1);
        }
        else if(diff.y > 0)
        {
            return sf::Vector2f(0,1);
        }
    }
    // hit edge of block (left/right)
    else if(abs(diff.y) <= PADDLE_HEIGHT/2)
    {
        if(diff.x <= 0)
        {
            return sf::Vector2f(-1,0);
        }
        else if(diff.x > 0)
        {
            return sf::Vector2f(1,0);
        }
    }

    float distance = getDistance(ball_center, block.getPosition());
    if(distance <= ball.getRadius())
    {
        return (ball_center - block.getPosition()) / distance;
    }
    sf::Vector2f block_corner = block.getPosition() + sf::Vector2f(block.getSize().x,0);
    distance = getDistance(ball_center, block_corner);
    if(distance <= ball.getRadius())
    {
        return (ball_center - block_corner) / distance;
    }
    block_corner = block.getPosition() + sf::Vector2f(0, block.getSize().y);
    distance = getDistance(ball_center, block_corner);
    if(distance <= ball.getRadius())
    {
        return (ball_center - block_corner) / distance;
    }
    block_corner = block.getPosition() + block.getSize();
    distance = getDistance(ball_center, block_corner);
    if(distance <= ball.getRadius())
    {
        return (ball_center - block_corner) / distance;
    }

    // Not intersecting, even in the corners
    return sf::Vector2f(0,0);
}

void Generate_Level(sf::RectangleShape *blocks)
{
    // Put blocks in places to create a level
    // Later:  Procedurally generate locations
    // More fun:  Give blocks different toughness so it takes more than one hit to break
    // For now, just put them in a rectangle...


}
