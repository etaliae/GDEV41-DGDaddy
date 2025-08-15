#include <iostream>

struct Character { //Turn into a header file
    std::string name;
    int x;
    int y;
};


void generate_grid(int a, int b); //a is rows, b is columns

int main() 
{
    generate_grid(4, 5);
    return 0;
}

void generate_grid(int a, int b) 
{
    char player = 1;
    char enemy = 2;
    std::string grid, closer;

    int rows = (a * 2) - 1, cols = b - 1;

    for (int row = 0; row <= rows; row++) 
    {
        for (int col = 0; col <= cols; col++) 
        {
            if (row == 0) // First row, border
            {
                if (col == 0)
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
            else if (row == rows) // Last row, cell and border
            {
                grid += "|   ";

                if (col == 0)
                {
                    closer += "x---";
                }
                else if (col == cols)
                {
                    grid += enemy;
                    grid += "|\n";
                    closer += "----x";
                }
                else
                {
                    closer += "----";
                }
                    
            }
            else if (row % 2 == 0) // Border
            {
                if (col == 0)
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
    grid += closer;

    std::cout << grid;
    std::cout << enemy;
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
