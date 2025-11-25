#include <raylib.h>

#include "scene_manager.hpp"
#include "all_scenes.hpp"

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Scene and Resource Management");

    SceneManager scene_manager;

    TitleScene title_scene;
    title_scene.SetSceneManager(&scene_manager);

    GameScene game_scene;
    game_scene.SetSceneManager(&scene_manager);

    LeaderboardScene leaderboard_scene;
    leaderboard_scene.SetSceneManager(&scene_manager);

    PauseScene pause_scene;
    pause_scene.SetSceneManager(&scene_manager);

    DayEndScene day_end_scene;
    day_end_scene.SetSceneManager(&scene_manager);

    EndGameScene end_game_scene;
    end_game_scene.SetSceneManager(&scene_manager);

    scene_manager.RegisterScene(&title_scene, 0);
    scene_manager.RegisterScene(&game_scene, 1);
    scene_manager.RegisterScene(&leaderboard_scene, 3);
    scene_manager.RegisterScene(&pause_scene, 4);
    scene_manager.RegisterScene(&day_end_scene, 5);
    scene_manager.RegisterScene(&end_game_scene, 6);

    scene_manager.SwitchScene(0);

    while(!WindowShouldClose()) {
        Scene* active_scene = scene_manager.GetActiveScene();

        BeginDrawing();
        ClearBackground(Color{221, 161, 94, 255});

        if (active_scene != nullptr) {
            active_scene->Update();
            active_scene->Draw();
        }

        EndDrawing();
    }

    Scene* active_scene = scene_manager.GetActiveScene();
    if (active_scene != nullptr) {
        active_scene->End();
    }

    ResourceManager::GetInstance()->UnloadAllTextures();

    CloseWindow();
    return 0;
}