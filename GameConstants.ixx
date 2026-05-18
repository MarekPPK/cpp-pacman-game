export module GameConstants;

export namespace Config {
    namespace Window {
        constexpr int Width = 800;
        constexpr int Height = 900;
    }

    namespace UI {
        constexpr int ButtonWidth = 250;
        constexpr int ButtonHeight = 60;
    }

    namespace Gameplay {
        constexpr float PacmanSpeed = 0.1f;
        constexpr float GhostSpeed = 0.1f;
        constexpr int InitialLives = 4;
        constexpr int PointsPerDot = 10;
        constexpr int PointsPerGhost = 200;
        constexpr int EatenAtStart = 0;
        constexpr int ZeroPoints = 0;
        constexpr float DotRadius = 10.f;
    }

    namespace Tile {
        constexpr float Size = 25.f;
        constexpr float OffsetX = 50.f;
        constexpr float OffsetY = 60.f;

        enum Type {
            Empty = 0,
            Wall = 1,
            Dot = 2,
            Pacman = 3,
            Ghost = 4,
            Gate = 5,
            SuperFruit = 6
        };
    }

    namespace Timer {
        constexpr float IntroDuration = 4.2f;
        constexpr float EndDuration = 3.0f;
    }
}