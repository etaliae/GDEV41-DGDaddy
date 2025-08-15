#include <iostream>

void generate_grid(int a, int b);

int main() 
{
    generate_grid(5, 5);
    return 0;
}

void generate_grid(int a, int b) 
{
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
                    grid += "╔═══";
                }
                else if (col == cols)
                {
                    grid += "╦═══╗\n";
                }
                else
                {
                    grid += "╦═══";
                }
            }
            else if (row == rows) // Last row, cell and border
            {
                grid += "║   ║";

                if (col == 0)
                {
                    closer += "╚═══";
                }
                else if (col == cols)
                {
                    grid += "\n";
                    closer += "╩═══╝";
                }
                else
                {
                    closer += "╩═══";
                }
                    
            }
            else if (row % 2 == 0) // Border
            {
                if (col == 0)
                {
                    grid += "╠═══";
                }
                else if (col == cols)
                {
                    grid += "\n";
                    closer += "╬═══╣";
                }
                else
                {
                    closer += "╬═══";
                }
            }
            else if (row % 2 == 1) // Cell
            {
                grid += "║   ║";
                if (col == cols)
                {
                    grid += "\n";
                }
    
            }
        }
    }
    grid += closer;

    std::cout << grid;
};

/**
 * ╔═══╦---+---+---+---╗ 
 * ║   |   |   |   |   |
 * +---+---+---+---+---+
 * player = ඞ
 * enemy = ███
 */
