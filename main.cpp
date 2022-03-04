#include <SFML/Graphics.hpp>
#include <iostream>

float GetDist(float x, float y, int i, int j)
{
   // std::cout << "dist\n";
    if (x == j) return 0;
    if (y == i) return 0;
    float dist = sqrt((x - j*10 ) * (x - j*10 ) + (y - i*10 ) * (y - i*10 ))  ;
    return dist;
}

float GetForce(float x, float y, int i, int j, int**arr)
{
   // std::cout << "force\n";
    long float dist = GetDist(x, y, i, j) ;
    if (dist == 0) return 0;
    long float f = 0;
    if (arr[i][j] > 0)  f =  5000.0 / (dist * dist);
    else if (arr[i][j] < 0) f = -1000.0 / (dist * dist);
    return f;
}

float GetForceX(float x, float y, int i, int j, int** arr)
{
    //std::cout << "forcex\n";
    float f = GetForce(x, y, i, j, arr);
    if (f == 0) return 0;
    float dist = GetDist(x, y, i, j);
    float fx = f * (j*10 - x) / dist;
   // std::cout << "fx: " << fx << "\n";
    return fx;
}

float GetForceY(float x, float y, int i, int j, int** arr)
{
   // std::cout << "forcey\n";
    float f = GetForce(x, y, i, j, arr);
    if (f == 0) return 0;
    float dist = GetDist(x, y, i, j);
    float fy = f * (i*10 - y) / dist;
    //std::cout << "fy: " << fy << "\n";
    return fy;
}

struct Force
{
    float fx;
    float fy;
};
Force* TotalForce(float x, float y, int** arr, int n)
{
    //std::cout << "totalforce\n";
    float totalFx = 0;
    float totalFy = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (arr[i][j] != 0)
            {
                float fx = GetForceX(x, y, i, j, arr);
                float fy = GetForceY(x, y, i, j, arr);
                totalFx += fx;
                totalFy += fy;
            }
        }
    }
  
    Force* f = new Force;
    f->fx = totalFx;
    f->fy = totalFy;
    //std::cout << f->fx << " " << f->fy << "\n";
    return f;
}

void GenerateMatrix(int** arr, int n, sf::CircleShape** food)
{
    //std::cout << "matrix\n";
    int i = abs(rand() % n);
    int j = abs(rand() % n);
    if (arr[i][j] == 0)
    {
        int sign = abs(rand() % 2);
        if (sign == 0)
        {
            arr[i][j] = 1;
            food[i][j].setFillColor(sf::Color::Green);
        }
        else
        {
            arr[i][j] = -1;
            food[i][j].setFillColor(sf::Color::Red);
        }
    }
    else if (arr[i][j] == -1)
    {
        arr[i][j] = 0;
        food[i][j].setFillColor(sf::Color::Black);
    }
}


int main()
{
    srand(time(NULL));
    int n = 100;

    sf::RenderWindow window(sf::VideoMode(1100, 1100), "SFML works!");
    sf::CircleShape surv(10.f);
    surv.setFillColor(sf::Color::Yellow);
    surv.setPosition(505.5f, 505.5f);
    
    
    sf::CircleShape** food = new sf::CircleShape * [n];
    for (int i = 0; i < n; i++)
    {
        food[i] = new sf::CircleShape[n];
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            food[i][j].setRadius(5.f);
            food[i][j].setFillColor(sf::Color::Black);
            food[i][j].setPosition(10.f * j, 10.f * i);
        }
    }
    
    Force* force = nullptr;
    
    int** arr = new int*[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = new int[n];
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            arr[i][j] = 0;
        }
    }

    sf::Vector2f pos = surv.getPosition();
    float x = pos.x;
    float y = pos.y;
    while (window.isOpen())
    {
        int posy = (int)round(surv.getPosition().y) / 10;
        int posx = (int)round(surv.getPosition().x) / 10;
        for (int i = -1; i < 2; i++)
        {
            for (int j = -1; j < 2; j++)
            {
                if ((posy + i < 0) || (posy + i > 99) || (posx + j < 0) || (posx + j > 99)) continue;
                if (arr[posy + i][posx + j] > 0)
                {
                    arr[posy + i][posx + j] = 0;
                    food[posy + i][posx + j].setFillColor(sf::Color::Black);
                    break;
                }
            }
        }
 
        if (surv.getPosition().x >= 1000) surv.setPosition(50, surv.getPosition().y);
        if (surv.getPosition().x <= 0 ) surv.setPosition(950, surv.getPosition().y);
        if (surv.getPosition().y >= 1000) surv.setPosition(surv.getPosition().x, 50);
        if (surv.getPosition().y <= 0) surv.setPosition(surv.getPosition().x,950);
        

        if ((abs(x - surv.getPosition().x) > 100.0f) || (abs(surv.getPosition().y - y) > 100.0f))
        {
            x = surv.getPosition().x;
            y = surv.getPosition().y;
            GenerateMatrix(arr, n, food);
        }

        if (force) delete force;
        force = nullptr;
        force = TotalForce(surv.getPosition().x, surv.getPosition().y, arr, n);
        
        if ((force->fx == 0) && (force->fy == 0))
        {
            force->fx = 1;
            force->fy = 0;
        }
        
        while ((abs(force->fx) <= 0.5) && (abs(force->fy) <= 0.5))
        {
            force->fx *= 10;
            force->fy *= 10;
        }
       // std::cout << "main\n";
        surv.move(1.0f * force->fx, 1.0f * force->fy);

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(surv);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                window.draw(food[i][j]);
            }
        }
        window.display();
        
    }

    return 0;
}