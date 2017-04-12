#include "battlesnake_api.hpp"
#include "util.h"
#include <stdexcept>
#include <set>

static int seed = time(NULL);

// Callback that will be called when a new game starts (on start request).
// See https://stembolthq.github.io/battle_snake/#post-start
nlohmann::json battlesnake_start(const std::string& game_id, const int width, const int height) {
    srand(seed++);
    std::ostringstream color;
    color << "#" << std::hex << (rand() & 0xffffff);
    std::cout << "color : " << color.str() << std::endl;
    return {
        {"color", color.str()},
        {"secondary_color", "#00FF00"},
        //{"head_url", "http://placecage.com/c/100/100"},
        {"name", "Cage Snake"},
        {"taunt", "OH GOD NOT THE BEES"},
        {"head_type", "pixel"},
        {"tail_type", "pixel"}
    };
}

class Battlefield {
private:
    const int width;
    const int height;
    char* grid;
public:
    Battlefield(const int width, const int height) :
            width(width),
            height(height)
    {
        grid = new char[width*height]();
        memset(grid, '.', width*height);
    }

    ~Battlefield() {
        delete[] grid;
    }

    bool allowedMove(const int x, const int y) {
        if ((x < 0) || (y < 0) || (x >= width) || (y >= height)) {
            return false;
        }
        char pt = grid[x+y*width];
        return ('#' == pt) || // closest food
               ('*' == pt) || // other food
               ('.' == pt) || // clear field
               (',' == pt) || // tail
               ('o' == pt);   // head of shorter snake
    }

    bool allowedMove(const Point p) {
        return allowedMove(p.x, p.y);
    }

    void mark(const int x, const int y, char val) {
        if ((x < 0) || (y < 0) || (x >= width) || (y >= height)) {
            return;
        }
        grid[x+y*width] = val;
    }

    void mark(const Point p, char val) {
        mark(p.x, p.y, val);
    }

    char Get(const int x, const int y) {
        return grid[x + y * width];
    }

    char Get(const Point p) {
        return Get(p.x, p.y);
    }
};

Point operator+(const Point p, const Direction& dir) {
    switch (dir) {
        case Direction::down: return Point(p.x, p.y+1);
        case Direction::up: return Point(p.x, p.y-1);
        case Direction::left: return Point(p.x-1, p.y);
        case Direction::right: return Point(p.x+1, p.y);
    }
}

Point operator-(const Point p1, const Point p2) {
    return Point(p1.x - p2.x, p1.y - p2.y);
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
    Battlefield b(width, height);

    // Draw alive snakes
    char c = 'a';
    for (auto& snake : snakes) {
        cout << "snake: " << snake.id << " " << snake.name << endl;
        for (auto& pt : snake.coords) {
            b.mark(pt, c);
        }
        c++;
    }

    // Don't draw dead snakes, since they can be passed

    // Draw food
    for (auto& f : food) {
        b.mark(f, '*');
    }

    // Assuming 'you' is the index of my snake
    auto myHead = snakes[you].coords[0];
    b.mark(myHead, '@');

    // Find closest food
    Point closestFood;
    double minDistance = width+height;
    for (auto& f : food) {
        int dist = util::distance(f, myHead);
        if (dist < minDistance) {
            closestFood = f;
            minDistance = dist;
        }
    }
    b.mark(closestFood, '#');

    // 'iterate' one step by marking all possible next steps + removing the tails
    for (int i = 0; i < snakes.size(); i++) {
        auto& snake = snakes[i];
        Point head = snake.coords[0];
        Point tail = snake.coords[snake.coords.size()-1];

        if (i != you) {
            char mark = snake.coords.size() > snakes[you].coords.size() ? ('A'+i) : 'o';
            for (auto &dir : {Direction::down, Direction::up, Direction::left, Direction::right}) {
                if (b.allowedMove(head + dir)) {
                    b.mark(head + dir, mark);
                }
            }
        }
        b.mark(tail, '.');
    }

    set<Direction> allowedMoves;

    // Check allowed directions
    for (auto& dir : {Direction::down, Direction::up, Direction::left, Direction::right}) {
        if (b.allowedMove(myHead + dir)) {
            allowedMoves.insert(dir);
        }
    }

    Direction heading = Direction::down;
    bool headingDecided = false;
    // Direction to closest food
    {
        Point p = closestFood - myHead;

        if (abs(p.x) > abs(p.y) && (allowedMoves.count(Direction::right) || allowedMoves.count(Direction::left))) {
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
            cout << b.Get(x, y);
        }
        cout << endl;
    }
    cout << you << " -------------------------------------------------------" << endl;

    return Move_response(heading, "optional taunt here!");
}
