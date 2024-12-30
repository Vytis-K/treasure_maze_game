#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <queue>
#include <algorithm>

const int WIDTH = 800;
const int HEIGHT = 600;
const int TILE_SIZE = 20;
const int ROWS = HEIGHT / TILE_SIZE;
const int COLS = WIDTH / TILE_SIZE;

enum Terrain { EMPTY, WALL, CHEST };

struct Position {
    int x, y;
};

void generateMaze(std::vector<std::vector<Terrain>>& grid, Position& start, Position& end) {
    srand(static_cast<unsigned>(std::time(nullptr)));
    // Initialize grid with walls
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            grid[i][j] = WALL;
        }
    }

    // Directions for maze generation (right, down, left, up)
    std::vector<std::pair<int, int>> directions = {{0, 2}, {2, 0}, {0, -2}, {-2, 0}};
    std::vector<Position> frontier;

    // Start from a random position and make it a path
    start = {2 * (rand() % (COLS / 2)), 2 * (rand() % (ROWS / 2))};
    grid[start.y][start.x] = EMPTY;
    frontier.push_back(start);

    Position lastCleared = start; // Initialize last cleared position

    while (!frontier.empty()) {
        // Randomly select a frontier cell
        std::random_shuffle(frontier.begin(), frontier.end());
        Position current = frontier.back();
        frontier.pop_back();

        for (auto& dir : directions) {
            Position neighbor = {current.x + dir.first, current.y + dir.second};
            if (neighbor.x > 0 && neighbor.x < COLS - 1 && neighbor.y > 0 && neighbor.y < ROWS - 1 && grid[neighbor.y][neighbor.x] == WALL) {
                // Check the cell between current and neighbor
                Position between = {current.x + dir.first / 2, current.y + dir.second / 2};
                if (grid[between.y][between.x] == WALL) {
                    grid[between.y][between.x] = EMPTY;
                    grid[neighbor.y][neighbor.x] = EMPTY;
                    frontier.push_back(neighbor);
                    lastCleared = neighbor; // Update last cleared position
                }
            }
        }
    }

    // Place the chest at the last cleared position
    end = lastCleared;
    grid[end.y][end.x] = CHEST; // Mark as chest
}

void drawGrid(sf::RenderWindow& window, const std::vector<std::vector<Terrain>>& grid) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            sf::RectangleShape tile(sf::Vector2f(TILE_SIZE - 1, TILE_SIZE - 1));
            tile.setPosition(j * TILE_SIZE, i * TILE_SIZE);
            switch(grid[i][j]) {
                case EMPTY: tile.setFillColor(sf::Color::White); break;
                case WALL: tile.setFillColor(sf::Color::Green); break;
                case CHEST: tile.setFillColor(sf::Color::Yellow); break; // Distinct color for the chest
            }
            window.draw(tile);
        }
    }
}

struct Player {
    int x, y;
    sf::RectangleShape shape;

    Player(Position start) : shape(sf::Vector2f(TILE_SIZE - 1, TILE_SIZE - 1)), x(start.x), y(start.y) {
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(x * TILE_SIZE, y * TILE_SIZE);
    }

    void move(int dx, int dy, const std::vector<std::vector<Terrain>>& grid) {
        int newX = x + dx;
        int newY = y + dy;
        if (newX >= 0 && newX < COLS && newY >= 0 && newY < ROWS && grid[newY][newX] == EMPTY) {
            x = newX;
            y = newY;
            shape.setPosition(x * TILE_SIZE, y * TILE_SIZE);
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "2D Maze Game");
    std::vector<std::vector<Terrain>> grid(ROWS, std::vector<Terrain>(COLS));

    Position start, end;
    generateMaze(grid, start, end);
    Player player(start); // Now the player starts at the beginning of the maze

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::W: player.move(0, -1, grid); break;
                    case sf::Keyboard::S: player.move(0, 1, grid); break;
                    case sf::Keyboard::A: player.move(-1, 0, grid); break;
                    case sf::Keyboard::D: player.move(1, 0, grid); break;
                    default: break;
                }
            }
        }

        window.clear();
        drawGrid(window, grid);
        window.draw(player.shape);
        window.display();
    }

    return 0;
}
