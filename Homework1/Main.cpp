#include <iostream>
#include <fstream>

#include "coordinates.h"

int initialize_chars(Coordinates* g, Coordinates* p, Coordinates* e);
// This validates settings.txt and initializes the grid, player, and enemy.

void generate_grid(Coordinates* g, Coordinates* p, Coordinates* e);
// This prints out the grid, along with the player and the enemy.

void input_validation(std::string i, Coordinates* g, Coordinates* p);
// This validates user input, and updates the player's position.

int main() 
{
    Coordinates grid, player, enemy;
    std::string input;

    if (initialize_chars(&grid, &player, &enemy) != 0)
    {
        std::cout << "Invalid values in settings.txt." << std::endl;
        return 0;
    }

    do
    {
        generate_grid(&grid, &player, &enemy);
        std::cin >> input;
        input_validation(input, &grid, &player);
    }
    while (input != "exit");
    
    return 0;
}

int initialize_chars(Coordinates* g, Coordinates* p, Coordinates* e)
{
    std::ifstream file("settings.txt");
    std::string temp;

    Coordinates* entities[3] = {g, p, e};
    int counter = 0;
    
    while (std::getline(file, temp))
    {
        int numtemp;

        try
        { // Handle exceptions, if any.
            numtemp = std::stoi(temp);

            if (numtemp < 1)
            {
                throw 1;
            }

            // If player and enemy values are beyond the grid.
            else if (counter/2 > 0) 
            {
                if (counter%2 == 0)
                {
                    if (numtemp > entities[0]->x)
                    {
                        throw 2;
                    }
                }

                else
                {
                    if (numtemp > entities[0]->y)
                    {
                        throw 3;
                    }
                }
            }
        }
        catch(int n)
        {
            if (n == 1)
            {
                std::cout << "Value must be greater than 0." << std::endl;
            }

            else if (n == 2)
            {
                std::cout << "Player/enemy x coordinate is greater than the grid size." << std::endl;
            }

            else if (n == 3)
            {
                std::cout << "Player/enemy y coordinate is greater than the grid size." << std::endl;
            }

            return 1;
        }
        catch(...) // Catches non-integer input.
        {
            std::cout << "Non-integer input." << std::endl;
            return 1;
        }
    
        // If no exception, assign values.
        if (counter%2 == 0)
        {
            entities[counter/2]->x = numtemp;
        }
        else
        {
            entities[counter/2]->y = numtemp;
        }
        
        counter++;
    };

    return 0;
}

void generate_grid(Coordinates* g, Coordinates* p, Coordinates* e) 
{
    std::string grid;

    char player = 'P';
    char enemy = 'E';

    int rows = (g->y * 2), cols = g->x - 1;

    int player_x = p->x - 1;
    int player_y = (p->y * 2) - 1;
    int enemy_x = e->x - 1;
    int enemy_y = (e->y * 2) - 1;

    for (int row = 0; row <= rows; row++) 
    {
        for (int col = 0; col <= cols; col++) 
        {
            bool hasPlayer = (row == player_y && col == player_x);
            bool hasEnemy = (row == enemy_y && col == enemy_x);

            if (hasPlayer || hasEnemy)
                {
                    grid += "|";

                    if (hasPlayer && hasEnemy)
                    {
                        grid += player;
                        grid += " ";
                        grid += enemy;
                    }
                    else if (hasPlayer)
                    {
                        grid += " ";
                        grid += player;
                        grid += " ";
                    }
                    else if (hasEnemy)
                    {
                        grid += " ";
                        grid += enemy;
                        grid += " ";
                    }
                    
                    if (col == cols)
                    {
                        grid += "|\n";
                    }
                }

            else if ((row == 0) || (row == rows)) // First or last row, border
            {
                if (cols == 0)
                {
                    grid += "x---x\n";
                }
                else if (col == 0)
                {
                    grid += "x---";
                }
                else if (col == cols)
                {
                    grid += "----x\n";
                }
                else
                {
                    grid += "----";
                }
            }

            else if (row % 2 == 0) // Border
            {
                if (cols == 0)
                {
                    grid += "|---|\n";
                }
                else if (col == 0)
                {
                    grid += "|---";
                }
                else if (col == cols)
                {
                    grid += "+---|\n";
                }
                else
                {
                    grid += "+---";
                }
            }

            else if (row % 2 == 1) // Cell
            {
                grid += "|   ";
                if (col == cols)
                {
                    grid += "|\n";
                }
            }
        }
    }

    std::cout << grid;

    return;
};

void input_validation(std::string i, Coordinates* g, Coordinates* p)
{
    if (i == "exit")
    {
        return;
    }

    std::string valid_input[8] = {"north", "south", "east", "west",
                                                        "n", "s", "e", "w"} ;

    bool found = false;

    for (int _ = 0; _ < sizeof(valid_input) / sizeof(valid_input[0]); _++ )
    {
        if (i == valid_input[_])
        {
            found = true;
        }
    }

    int temp;

    if (found)
    {
        if (i == "north" || i == "n" || i == "south" || i == "s")
        {
            if (i == "north" || i == "n")
            {
                temp = p->y - 1;
            }
            else
            {
                temp = p->y + 1;
            }

            if (temp == 0)
            {
                temp = g->y;
            }

            else if (temp == g->y + 1)
            {
                temp = 1;
            }

            p->y = temp;
        }

        else if (i == "east" || i == "e" || i == "west" || i == "w")
        {
            if (i == "east" || i == "e")
            {
                temp = p->x + 1;
            }
            else
            {
                temp = p->x - 1;
            }

            if (temp == 0)
            {
                temp = g->x;
            }

            else if (temp == g->x + 1)
            {
                temp = 1;
            }
            
            p->x = temp;
        }
    }

    else
    {
        std::cout << "Invalid input." << std::endl;
        return;
    }
};

/**
 * ideally
 * ╔═══╦═══╦═══╦═══╦═══╗ 
 * ║   ║   ║   ║   ║   ║
 * ╠═══╬═══╬═══╬═══╬═══╣
 * ║   ║   ║   ║   ║   ║
 * ╚═══╩═══╩═══╩═══╩═══╝
 * player = ඞ
 * enemy = ███
 */
