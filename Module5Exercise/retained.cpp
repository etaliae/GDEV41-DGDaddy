#include <raylib.h>
#include <raymath.h>

#include <iostream>
#include <string>
#include <vector>

void press_hello();
void press_hi();
void smallify_window();
void medify_window();
void biggify_window();

// Generic UI component
struct UIComponent
{
    // Rectangle reprsenting the bounds of the UI component
    Rectangle bounds;

    // Draws this particular UI component
    // Set as abstract so that child widgets will implement this for us
    virtual void Draw() = 0;

    // Handles a mouse click event
    // Set as abstract so that child widgets will implement this for us
    // Returns a boolean indicating whether this UI component successfully handled the event
    virtual bool HandleClick(Vector2 click_position) = 0;
};

// Generic UI component that can contain other UI components as children
struct UIContainer : public UIComponent
{
    std::vector<UIComponent*> children;

    // Adds a child to the container
    void AddChild(UIComponent* child)
    {
        children.push_back(child);
    }

    // Draw
    void Draw() override
    {
        // Since we are just a container for other widgets, we simply
        // call the draw function of other widgets.
        // This results in a pre-order traversal when we also draw child widgets that are also containers
        for (size_t i = 0; i < children.size(); ++i)
        {
            children[i]->Draw();
        }
    }

    // Handles a mouse click event
    // Returns a boolean indicating whether this UI component successfully handled the event
    bool HandleClick(Vector2 click_position) override
    {
        // Since we are just a container for other widgets, we call the HandleClick function of our child widgets
        // Since later children are drawn last, we do the opposite of draw where we start from the last child.
        // This results in a pre-order traversal but in the reverse order.
        for (size_t i = children.size(); i > 0; --i)
        {
            // If a child already handles the click event, we instantly return so no more child widgets handle the click
            if (children[i - 1]->HandleClick(click_position))
            {
                return true;
            }
        }

        return false;
    }
};

// Button widget
struct Button : public UIComponent
{
    // Text displayed by the button
    std::string text;
    void (*func)(void);
    bool enabled = true;

    // Draw
    void Draw() override
    {
        DrawRectangleRec(bounds, GRAY);
        DrawText(text.c_str(), bounds.x, bounds.y, 14, BLACK);
    }

    // Handle mouse click
    // Returns a boolean indicating whether this UI component successfully handled the event
    bool HandleClick(Vector2 click_position) override
    {
        // Check if the mouse click position is within our bounds
        if (CheckCollisionPointRec(click_position, bounds) && enabled)
        {
            func();
            return true;
        }

        return false;
    }
};

// Text display widget
struct Label : public UIComponent
{
    // Text to be displayed
    std::string text;

    // Draw
    void Draw() override
    {
        DrawText(text.c_str(), bounds.x, bounds.y, 14, BLACK);
    }

    // Handle mouse click
    // Returns a boolean indicating whether this UI component successfully handled the event
    bool HandleClick(Vector2 click_position) override
    {
        // Always return false since we're not going to handle click events for this particular widget
        // (unless you have to)
        return false;
    }
};

struct CheckBox : public UIComponent
{
    bool checked = false;
    std::string text;

    void Draw() override 
    {
        DrawRectangleRec(bounds, LIGHTGRAY);
        DrawRectangleLines(bounds.x + 10, bounds.y + 10, 10, 10, BLACK);
        DrawText(text.c_str(), bounds.x + 30, bounds.y + 10, 14, BLACK);

        if (checked){
            DrawLine(bounds.x + 10, bounds.y + 10, bounds.x + 20, bounds.y + 20, RED);
            DrawLine(bounds.x + 20, bounds.y + 10, bounds.x + 10, bounds.y + 20, RED);
        }
    }

    bool HandleClick(Vector2 click_position) override
    {
        if (CheckCollisionPointRec(click_position, bounds))
        {
            checked = !checked;
            return true;
        }
        return false;
    }
};

// Struct to encapsulate our UI library
struct UILibrary
{
    // Root container
    UIContainer root_container;

    // Updates the current UI state
    void Update()
    {
        // If the left mouse button was released, we handle the click from the root container
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            root_container.HandleClick(GetMousePosition());
        }
    }

    // Draw
    void Draw()
    {
        root_container.Draw();
    }
};

int main()
{
    int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Retained Mode");
    SetTargetFPS(60);

    UILibrary ui_library;
    ui_library.root_container.bounds = { 10, 10, 600, 500 };

    Button button;
    button.text = "Hello!";
    button.func = press_hello;
    button.bounds = { 120, 10, 80, 40 };
    ui_library.root_container.AddChild(&button);

    Button button2;
    button2.text = "Hi!";
    button2.func = press_hi;
    button2.bounds = { 210, 10, 80, 40 };
    ui_library.root_container.AddChild(&button2);

    Label label;
    label.text = "This is a label";
    label.bounds = { 10, 20, 100, 40 };
    ui_library.root_container.AddChild(&label);

    CheckBox checkbox;
    checkbox.text = "Cross me out!";
    checkbox.bounds = { 10, 70, 150, 40 };
    checkbox.checked = false;
    ui_library.root_container.AddChild(&checkbox);

    CheckBox uiCheckbox;
    uiCheckbox.text = "Lock screen size";
    uiCheckbox.bounds = { 10, 120, 150, 40 };
    uiCheckbox.checked = false;
    ui_library.root_container.AddChild(&uiCheckbox);

    Label resolutionLabel;
    resolutionLabel.text = "Set Resolution:";
    resolutionLabel.bounds = { 10, 180, 150, 40 };
    ui_library.root_container.AddChild(&resolutionLabel);

    Button smallButton;
    smallButton.text = "800x600";
    smallButton.func = smallify_window;
    smallButton.enabled = !uiCheckbox.checked;
    smallButton.bounds = { 170, 170, 100, 40 };
    ui_library.root_container.AddChild(&smallButton);

    Button medButton;
    medButton.text = "1280x720";
    medButton.func = medify_window;
    medButton.enabled = !uiCheckbox.checked;
    medButton.bounds = { 280, 170, 100, 40 };
    ui_library.root_container.AddChild(&medButton);

    Button bigButton;
    bigButton.text = "1366x768";
    bigButton.func = biggify_window;
    bigButton.enabled = !uiCheckbox.checked;
    bigButton.bounds = { 390, 170, 100, 40 };
    ui_library.root_container.AddChild(&bigButton);

    while (!WindowShouldClose())
    {
        ui_library.Update();

        smallButton.enabled = !uiCheckbox.checked;
        medButton.enabled = !uiCheckbox.checked;
        bigButton.enabled = !uiCheckbox.checked;

        ClearBackground(WHITE);
        BeginDrawing();
        ui_library.Draw();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void press_hello()
{
    std::cout << "Hello!" << std::endl;
}

void press_hi()
{
    std::cout << "Hi!" << std::endl;
}

void smallify_window()
{
    SetWindowSize(800, 600);
}

void medify_window()
{
    SetWindowSize(1280, 720);
}

void biggify_window()
{
    SetWindowSize(1366, 768);
}