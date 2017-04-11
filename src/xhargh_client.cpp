#include "battlesnake_api.hpp"
#include <stdexcept>
#include <set>

// Callback that will be called when a new game starts (on start request).
// See https://stembolthq.github.io/battle_snake/#post-start
nlohmann::json battlesnake_start(const std::string& game_id, const int width, const int height) {
    return {
        {"color", "#FF0000"},
        {"secondary_color", "#00FF00"},
        //{"head_url", "http://placecage.com/c/100/100"},
        {"name", "Cage Snake"},
        {"taunt", "OH GOD NOT THE BEES"},
        {"head_type", "pixel"},
        {"tail_type", "pixel"}
    };
}

bool allowedMove(char* battlefield, const int width, const int height, const int x, const int y) {
    if ((x < 0) || (y < 0) || (x >= width) || (y >= height)) {
        std::cout << "outside" << std::endl;
        return false;
    }
    char pt = battlefield[x+y*width];
    return ('#' == pt) || ('*' == pt) || ('.' == pt); // Food or clear field
}

// Callback that will be called on move requests.
Move_response battlesnake_move(
        const std::string& game_id,
        const int width,
        const int height,
        const Points& food,
        const Snakes& snakes,
        const Snakes& dead_snakes,
        const size_t you) {
    using namespace std;
    char* battlefield = new char[width*height]();
    memset(battlefield, '.', width*height);

    // Draw alive snakes (lower case letters)
    int c = 'a';
    for (auto& snake : snakes) {
        cout << "snake: " << snake.id << " " << snake.name << endl;
        for (auto& pt : snake.coords) {
            battlefield[pt.x+pt.y*width] = c;
        }
        c++;
    }

    // Draw dead snakes (upper case letters)
    c = 'A';
    for (auto& snake : dead_snakes) {
        for (auto& pt : snake.coords) {
            battlefield[pt.x+pt.y*width] = c;
        }
        c++;
    }

    // Draw food
    for (auto& f : food) {
        battlefield[f.x + f.y * width] = '*';
    }

    // Assuming 'you' is the index of my snake
    auto myHead = snakes[you].coords[0];
    battlefield[myHead.x + myHead.y * width] = '@';

    // Find closest food
    Point closestFood;
    double minDistance = sqrt(height * height + width * width) + 1;
    for (auto& f : food) {
        int a = f.x - myHead.x;
        int b = f.y - myHead.y;
        double distance = sqrt(a*a + b*b);
        if (distance < minDistance) {
            closestFood = f;
            minDistance = distance;
        }
    }
    battlefield[closestFood.x + closestFood.y * width] = '#';

    set<Direction> allowedMoves;

    if (allowedMove(battlefield, width, height, myHead.x, myHead.y+1)) {
        allowedMoves.insert(Direction::down);
    }
    if (allowedMove(battlefield, width, height, myHead.x, myHead.y-1)) {
        allowedMoves.insert(Direction::up);
    }
    if (allowedMove(battlefield, width, height, myHead.x+1, myHead.y)) {
        allowedMoves.insert(Direction::right);
    }
    if (allowedMove(battlefield, width, height, myHead.x-1, myHead.y)) {
        allowedMoves.insert(Direction::left);
    }

    for (auto& move : allowedMoves) {
        switch (move) {
            case Direction::down: cout << "down" << endl; break;
            case Direction::up: cout << "up" << endl; break;
            case Direction::left: cout << "left" << endl; break;
            case Direction::right: cout << "right" << endl; break;
        }
    }

    Direction heading = Direction::down;
    bool headingDecided = false;
    // Direction to closest food
    {
        int a = closestFood.x - myHead.x;
        int b = closestFood.y - myHead.y;

        if (abs(a) > abs(b) && (allowedMoves.count(Direction::right) || allowedMoves.count(Direction::left))) {
            // Try to close in on x axis
            if (closestFood.x > myHead.x && allowedMoves.count(Direction::right)) {
                heading = Direction::right;
                headingDecided = true;
            } else if (allowedMoves.count(Direction::left)) {
                heading = Direction::left;
                headingDecided = true;
            }
        }
        if (!headingDecided) {
            // Try to close in on y axis
            if (closestFood.y > myHead.y && allowedMoves.count(Direction::down)) {
                heading = Direction::down;
                headingDecided = true;
            } else if (allowedMoves.count(Direction::up)) {
                heading = Direction::up;
                headingDecided = true;
            }
        }
        if (!headingDecided) {
            cout << "undecided" << endl;
            heading = *allowedMoves.begin();
        }
    }

    // Print battlefield
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cout << battlefield[x+y*width];
        }
        cout << endl;
    }
    cout << you << " -------------------------------------------------------" << endl;

    delete[] battlefield;
    return Move_response(heading, "optional taunt here!");
}
