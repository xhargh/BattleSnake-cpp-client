#include "battlesnake_api.hpp"
#include "util.h"
#include <queue>
#include <set>
#include <climits>

static int seed = time(NULL);

std::vector<std::string> name = {
        "Sir Väs",
        "Ormen Långe",
        "Gorm",
        "Snakey Bastard",
        "Natrix",
        "Röde Orm",
        "Basil",
        "Boomer",
        "Slinky",
        "Nagini",
        "Kaa",
        "Salazar Slytherin",
        "Vipera",
        "Sir Francis Snake",
        "Knut",
        "Fafne",
        "Ormurin Langi",
        "Black Adder",
        "Adderall",
        "Serpentin",
        "Jörmungandr",
        "Nidhögg",
        "Elon Mask",
        "The Mask",
        "Slangen",
        "Doktor Snok",
        "Slinger",
        "Bo A. Orm",
        "Kramer",
        "Tungur Dubblur",
        "Essi Äsping",
        "Ormtunga",
        "Rattler",
        "Wormtongue",
        "Constrictadore",
        "Snake Rattlenroll",
        "Orminge",
        "Anja Konda"
        "Ormega",
        "Serpentina",
        "Snokrates",
        "Ouroboros"
};

// Callback that will be called when a new game starts (on start request).
// See https://stembolthq.github.io/battle_snake/#post-start
nlohmann::json battlesnake_start(const std::string &game_id, const int width, const int height) {
    srand(seed++);
    std::ostringstream color;
    color << "#" << std::hex << (rand() & 0xffffff);
    std::cout << "color : " << color.str() << std::endl;
    return {
            {"color",           color.str()},
            {"secondary_color", "#00FF00"},
            //{"head_url", "http://placecage.com/c/100/100"},
            {"name",            name[rand() % name.size()]},
            {"taunt",           "OH GOD NOT THE BEES"},
            {"head_type",       "pixel"},
            {"tail_type",       "pixel"}
    };
}

class Battlefield {
private:
    char *grid;
public:
    const int width;
    const int height;

    Battlefield(const int width, const int height) :
            width(width),
            height(height) {
        grid = new char[width * height]();
        memset(grid, '.', width * height);
    }

    ~Battlefield() {
        delete[] grid;
    }

    bool offMap(const Point p) const {
        return offMap(p.x, p.y);
    }

    bool offMap(const int x, const int y) const {
        return ((x < 0) || (y < 0) || (x >= width) || (y >= height));
    }

    bool allowedMove(const int x, const int y) const {
        if (offMap(x, y)) {
            return false;
        }
        char pt = grid[x + y * width];
        return ('#' == pt) || // closest food
               ('*' == pt) || // other food
               ('.' == pt) || // clear field
               (',' == pt) || // tail
               ('o' == pt);   // head of shorter snake
    }

    bool allowedMove(const Point p) const {
        return allowedMove(p.x, p.y);
    }

    void mark(const int x, const int y, char val) {
        if (offMap(x, y)) {
            return;
        }
        grid[x + y * width] = val;
    }

    void mark(const Point p, char val) {
        mark(p.x, p.y, val);
    }

    char Get(const int x, const int y) const {
        return grid[x + y * width];
    }

    char Get(const Point p) const {
        return Get(p.x, p.y);
    }
};

void printBattleField(const Battlefield &b, const int width, const int height);

Point operator+(const Point p, const Direction &dir) {
    switch (dir) {
        case Direction::down:
            return Point(p.x, p.y + 1);
        case Direction::up:
            return Point(p.x, p.y - 1);
        case Direction::left:
            return Point(p.x - 1, p.y);
        case Direction::right:
            return Point(p.x + 1, p.y);
    }
}

Point operator-(const Point p1, const Point p2) {
    return Point(p1.x - p2.x, p1.y - p2.y);
}

// represents a single pixel
class Node {
public:
    int idx;     // index in the flattened grid
    float cost;  // cost of traversing this pixel

    Node(int i, float c) : idx(i),cost(c) {}
};
bool operator<(const Node &n1, const Node &n2) {
    return n1.cost > n2.cost;
}
bool operator==(const Point &p1, const Point &p2) {
    return (p1.x == p2.x) && (p1.y == p2.y);
}
bool operator==(const Node &n1, const Node &n2) {
    return n1.idx == n2.idx;
}
int distance(Point p0, Point p1) {
    return std::abs(p0.x - p1.x) + std::abs(p0.y - p1.y);
}

struct queueNode
{
    Point pt;  // The cordinates of a cell
    int dist;  // cell's distance of from the source
};

// function to find the shortest path between
// a given source cell to a destination cell.
int BFS(const Battlefield &b, Point src, const Points &food)
{
    using namespace std;

    if (!b.allowedMove(src)) {
        return INT_MAX;
    }

    bool *visited = new bool[b.width * b.height];
    for (int i = 0; i < b.width * b.height; i++) {
        visited[i] = false;
    }

    // Mark the source cell as visited
    visited[src.x + src.y * b.width] = true;

    // Create a queue for BFS
    queue<queueNode> q;

    // distance of source cell is 0
    queueNode s = {src, 0};
    q.push(s);  // Enqueue source cell

    // Do a BFS starting from source cell
    while (!q.empty())
    {
        queueNode curr = q.front();
        Point pt = curr.pt;

        // If we have reached the destination cell,
        // we are done
        for (auto &f : food) {
            if (pt == f) {
                delete[] visited;
                return curr.dist;
            }
        }

        // Otherwise dequeue the front cell in the queue
        // and enqueue its adjacent cells
        q.pop();

        for (auto &dir : {Direction::down, Direction::up, Direction::left, Direction::right}) {
            Point rc = pt + dir;

            // if adjacent cell is allowed and not visited yet, enqueue it.
            if (b.allowedMove(rc) && !visited[rc.x + rc.y * b.width])
            {
                // mark cell as visited and enqueue it
                visited[rc.x + rc.y * b.width] = true;
                queueNode Adjcell = { rc,
                                      curr.dist + 1 };
                q.push(Adjcell);
            }
        }
    }
#if 0
    for (int y = 0; y < b.height; y++) {
        for (int x = 0; x < b.width; x++) {
            std::cout << (visited[x + y * b.width] ? "*":",");
        }
        std::cout << std::endl;
    }
#endif

    delete[] visited;
    //return -1 if destination cannot be reached
    return INT_MAX;
}

// Callback that will be called on move requests.
Move_response battlesnake_move(
        const std::string &game_id,
        const int width,
        const int height,
        const Points &food,
        const Snakes &snakes,
        const Snakes &dead_snakes,
        const size_t you) {
    using namespace std;
    Battlefield b(width, height);
    string taunt;

    // Draw alive snakes
    char c = 'a';
    for (auto &snake : snakes) {
        cout << "snake: " << snake.id << " " << snake.name << endl;
        for (auto &pt : snake.coords) {
            b.mark(pt, c);
        }
        c++;
    }

    // Don't draw dead snakes, since they can be passed

    // Draw food
    for (auto &f : food) {
        b.mark(f, '*');
    }

    // Assuming 'you' is the index of my snake
    auto myHead = snakes[you].coords[0];
    b.mark(myHead, '@');

    // Find closest food
    Point closestFood;
    double minDistance = width + height;
    for (auto &f : food) {
        int dist = distance(f, myHead);
        if (dist < minDistance) {
            closestFood = f;
            minDistance = dist;
        }
    }
    b.mark(closestFood, '#');

    // 'iterate' one step by marking all possible next steps
    for (int i = 0; i < snakes.size(); i++) {
        auto &snake = snakes[i];
        Point head = snake.coords[0];

        if (i != you) {
            char mark = snake.coords.size() >= snakes[you].coords.size() ? ('A' + i) : 'o';
            for (auto &dir : {Direction::down, Direction::up, Direction::left, Direction::right}) {
                if (b.allowedMove(head + dir)) {
                    b.mark(head + dir, mark);
                }
            }
        }
    }

    // printBattleField(b, width, height);

    set<Direction> allowedMoves;

    Direction heading = Direction::down;
    bool headingDecided = false;

    // Check allowed directions
    int minDist = INT_MAX;
    for (auto &dir : {Direction::down, Direction::up, Direction::left, Direction::right}) {
        if (b.allowedMove(myHead + dir)) {
            allowedMoves.insert(dir);

            int dist = BFS(b, myHead+dir, food);
            if (dist < minDist) {
                minDist = dist;
                heading = dir;
                headingDecided = true;
            }
            switch (dir) {
                case Direction::up:
                    cout << "Up: " << dist << endl;
                    break;
                case Direction::down:
                    cout << "Down" << dist << endl;
                    break;
                case Direction::left:
                    cout << "Left" << dist << endl;
                    break;
                case Direction::right:
                    cout << "Right" << dist << endl;
                    break;
            }
        }
    }

    if (!headingDecided) {
        // Direction to closest food
        Point p = closestFood - myHead;

        bool right = allowedMoves.count(Direction::right);
        bool left = allowedMoves.count(Direction::left);
        bool up = allowedMoves.count(Direction::up);
        bool down = allowedMoves.count(Direction::down);

        right = right && (p.x > 0);
        left = left && (p.x < 0);
        up = up && (p.y < 0);
        down = down && (p.y > 0);

        taunt = taunt + "...";

        if (right) {
            heading = Direction::right;
            headingDecided = true;
        } else if (left) {
            heading = Direction::left;
            headingDecided = true;
        } else if (up) {
            heading = Direction::up;
            headingDecided = true;
        } else if (down) {
            heading = Direction::down;
            headingDecided = true;
        }
    }

    if (!headingDecided) {
        // cout << "undecided" << endl;
        taunt = "Oh no! - " + taunt;
        heading = *allowedMoves.begin();
    }

    switch (heading) {
        case Direction::up:
            taunt += "Up";
            break;
        case Direction::down:
            taunt += "Down";
            break;
        case Direction::left:
            taunt += "Left";
            break;
        case Direction::right:
            taunt += "Right";
            break;
    }

    // printBattleField(b, width, height);
    // cout << you << " " << taunt << " -------------------------------------------------------" << endl;

    return Move_response(heading, taunt);
}

void printBattleField(Battlefield const &b, const int width, const int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::cout << b.Get(x, y);
        }
        std::cout << std::endl;
    }
}
