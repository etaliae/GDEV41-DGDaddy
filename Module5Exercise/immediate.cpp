#include <raylib.h>
#include <raymath.h>

#include <functional>
#include <iostream>
#include <string>
#include <vector>

// Struct to encapsulate our UI library
struct UiLibrary
{
    // ID of the currently hot widget
    int hot = -1;

    // ID of the currently active widget
    int active = -1;

    // Creates a button with the specified text and bounds
    // Returns true if this button was clicked in this frame
    bool Button(int id, const std::string& text, const Rectangle& bounds)
    {
        bool result = false;
        Color col = GRAY;
        Color txt = BLACK;

        // If this button is the currently active widget, that means
        // the user is currently interacting with this widget
        if (id == active)
        {
            col = RED;
            txt = WHITE;
            // If the user released the mouse button while we are active,
            // register as a click
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                // Make sure that we are still the hot widget
                // (User might have dragged mouse to another widget, in which case we will not count as a button click)
                if (id == hot)
                {
                    result = true;
                }

                // Set ourselves to not be active anymore
                active = -1;
            }
        }

        // If we are currently the hot widget
        else if (id == hot)
        {
            col = PINK;
            // If the user pressed the left mouse button, that means the user started
            // interacting with this widget, so we set this widget as active
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                active = id;
            }
        }

        // If the mouse cursor is hovering within our boundaries
        if (CheckCollisionPointRec(GetMousePosition(), bounds))
        {
            // Set this widget to be the hot widget
            hot = id;
        }
        // If the mouse cursor is not on top of this widget, and this widget
        // was previously the hot widget, set the hot widget to -1
        // We check if this widget was the previously hot widget since there is a possibility that
        // the hot widget is now a different widget, and we don't want to overwrite that with -1
        else if (hot == id)
        {
            hot = -1;
            col = GRAY;
        }

        // Draw our button regardless of what happens
        DrawRectangleRec(bounds, col);
        DrawText(text.c_str(), bounds.x, bounds.y, 14, txt);

        return result;
    }

    bool Checkbox(int id, bool checked, const std::string& text, const Rectangle& bounds)
    {
        if(id == active)
        {
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                checked = !checked;
                active = -1;
            }
        }

        else if(id == hot)
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                active = id;
            }
        }

        if (CheckCollisionPointRec(GetMousePosition(), bounds)){
            hot = id;
        }
        else if (hot == id){
            hot = -1;
        }

        DrawRectangleRec(bounds, GRAY);
        DrawRectangleLines(bounds.x + 10, bounds.y + 10, 10, 10, BLACK);
        if (checked){
            DrawRectangle(bounds.x + 12, bounds.y + 12, 6, 6, RED);
        }
        DrawText(text.c_str(), bounds.x + 30, bounds.y + 10, 14, BLACK);

        return checked;
    }

    void Label(const std::string& text, const Rectangle& bounds)
    {
        DrawText(text.c_str(), bounds.x, bounds.y, 14, BLACK);
        return;
    }
};

int main()
{
    int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Immediate Mode");
    SetTargetFPS(60);

    UiLibrary uiLibrary;
    bool checked = false;
    bool uiChecked = false;

    while (!WindowShouldClose())
    {
        ClearBackground(WHITE);
        BeginDrawing();
        if (uiLibrary.Button(0, "Hello!", { 10, 10, 80, 40 }))
        {
            std::cout << "Hello!" << std::endl;
        }
        if (uiLibrary.Button(1, "Hi!", { 100, 10, 80, 40 }))
        {
            std::cout << "Hi!" << std::endl;
        }
        checked = uiLibrary.Checkbox(2, checked, "Check me!", {10, 70, 150, 40});

        uiChecked = uiLibrary.Checkbox(3, uiChecked, "Lock screen size", {10, 120, 150, 40});

        uiLibrary.Label("Set Resolution:", {10, 180, 150, 40});

        if(uiLibrary.Button(4, "800x600", {170, 180, 100, 40}) && !uiChecked)
        {
            SetWindowSize(800, 600);
        }

        if(uiLibrary.Button(5, "1280x720", {280, 180, 100, 40}) && !uiChecked)
        {
            SetWindowSize(1280, 720);
        }

        if(uiLibrary.Button(6, "1366x768", {390, 180, 100, 40}) && !uiChecked)
        {
            SetWindowSize(1366, 768);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
