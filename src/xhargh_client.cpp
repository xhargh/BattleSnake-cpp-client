#include "battlesnake_api.hpp"
#include <queue>
#include <set>
#include <climits>
//#include <chrono>

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
        "Anja Konda",
        "Ormega",
        "Serpentina",
        "Snokrates",
        "Ouroboros",
        "Nibbles",
        "Masken Mimmi",
        "Dagge Daggmask",
        "Severus Snake",
        "Lindorm",
        "Sir Pants",
        "Slingersvans",
        "Eelrond",
        "Earthworm Jim",
        "Orme",
        "Wurm",
        "Wormley",
        "Medusa",
        "Monty",
        "William Snakespeare",
        "Asmodeus Poisonteeth",
        "Mr. Crowley",
        "Nag",
        "Nagaina",
        "Salmissra",
        "Sammy the Snake",
        "Slither",
        "Fang"
};

std::vector<std::string> headType = {
    "bendr",
    "dead",
    "fang",
    "pixel",
    "regular",
    "safe",
    "sand-worm",
    "shades",
    "smile",
    "tongue"
};

std::vector<std::string> tailType = {
    "small-rattle",
    "round-bum",
    "regular",
    "pixel",
    "freckled",
    "fat-rattle",
    "curled",
    "block-bum"
};

// Callback that will be called when a new game starts (on start request).
// See https://stembolthq.github.io/battle_snake/#post-start
nlohmann::json battlesnake_start(const std::string &game_id, const int width, const int height) {
    srand(seed++);
    std::ostringstream color;
    int r = rand() % 128 + 128;
    int g = rand() % 128 + 128;
    int b = rand() % 128 + 128;
    color << "#" << std::hex << r << g << b;
    std::string n = name[rand() % name.size()];
    std::cout << "color : " << color.str() << "   " << n << std::endl;
    return {
            {"color",           color.str()},
            {"secondary_color", "#ffffff"},
            //{"head_url", "http://placecage.com/c/100/100"},
            {"name",            n},
            {"taunt",           "..."},
            {"head_type",       headType[rand() % headType.size()]},
            {"tail_type",       tailType[rand() % tailType.size()]}
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

    bool offMap(const int x, const int y) const {
        return ((x < 0) || (y < 0) || (x >= width) || (y >= height));
    }

    bool allowedMove(const int x, const int y) const {
        if (offMap(x, y)) {
            return false;
        }
        char pt = grid[x + y * width];
        return ('*' == pt) || // other food
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
    return p;
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

struct queueNode
{
    Point pt;  // The cordinates of a cell
    int dist;  // cell's distance of from the source
};

// function to find the shortest path between
// a given source cell to a destination cell.
int BFS(const Battlefield &b, Point src, const Points &dests)
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
        for (auto &f : dests) {
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

enum Space {
    Filled,
    Illegal,
    Untouched
};

/*
    Flood-fill (node, target-color, replacement-color):
    1. If target-color is equal to replacement-color, return.
    2. If the color of node is not equal to target-color, return.
    3. Set the color of node to replacement-color.
    4. Perform Flood-fill (one step to the south of node, target-color, replacement-color).
    Perform Flood-fill (one step to the north of node, target-color, replacement-color).
    Perform Flood-fill (one step to the west of node, target-color, replacement-color).
    Perform Flood-fill (one step to the east of node, target-color, replacement-color).
    5. Return.
*/
void floodFill(int* field, const int width, const int height, Point p) {
    if (p.x < 0 || p.y < 0 || p.x >= width || p.y >= height) {
        return;
    }
    int idx = p.x + p.y*width;

    if (field[idx] == Filled) {
        return;
    }
    if (field[idx] != Untouched) {
        return;
    }
    field[idx] = Filled;
    floodFill(field, width, height, p + Direction::up);
    floodFill(field, width, height, p + Direction::down);
    floodFill(field, width, height, p + Direction::left);
    floodFill(field, width, height, p + Direction::right);
}

size_t emptySpace(const Battlefield& b, Point p) {
    int *field = new int[b.width*b.height];
    for (int x = 0; x < b.width; x++) {
        for (int y = 0; y < b.height; y++) {
            field[x+y*b.width] = b.allowedMove(x,y) ? Untouched : Illegal;
        }
    }
    floodFill(field, b.width, b.height, p);
    size_t area = 0;
    for (int x = 0; x < b.width; x++) {
        for (int y = 0; y < b.height; y++) {
            if (field[x+y*b.width] == Filled) {
                area++;
            }
        }
    }
    delete[] field;
    return area;
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
    //using namespace std::chrono;
    //milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
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

    // 'iterate' one step by marking all possible next steps
    for (size_t i = 0; i < snakes.size(); i++) {
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

    set<Direction> allowedMoves;

    Direction heading = Direction::down;

    // Check allowed directions
    for (auto &dir : {Direction::down, Direction::up, Direction::left, Direction::right}) {
        if (b.allowedMove(myHead + dir)) {
            allowedMoves.insert(dir);
        }
    }

    bool headingDecided = false;

    map<Direction, int> bfsResults;
    map<Direction, size_t> areas;

    // Precalculate the bfs and areas
    for (auto &dir : allowedMoves) {
        bfsResults[dir] = BFS(b, myHead+dir, food);
        areas[dir] = emptySpace(b, myHead+dir);
    }

    while (!headingDecided && allowedMoves.size() > 0) {

        // Look for closest reachable food
        int minDist = INT_MAX;
        for (auto &dir : allowedMoves) {
            int dist = bfsResults[dir];
            if (dist < minDist) {
                minDist = dist;
                heading = dir;
                headingDecided = true;
            }
        }

        // If the area where the closest food is too small, try to find another heading
        if (headingDecided && areas[heading] < snakes[you].coords.size() + 1) {
            allowedMoves.erase(heading);
            headingDecided = false;
            taunt = "";
        } else if (!headingDecided) {
            // no direct paths to food, go to greatest area
            size_t greatestArea = 0;
            for (auto &dir : {Direction::up, Direction::down, Direction::left, Direction::right}) {
                size_t area = areas[dir];
                if (area > greatestArea) {
                    heading = dir;
                    greatestArea = area;
                }
            }
            taunt = "?";
            headingDecided = true;
        } else {
            taunt = "!";
        }
    }

    switch (heading) {
        case Direction::up:
            taunt = "Up" + taunt;
            break;
        case Direction::down:
            taunt = "Down" + taunt;
            break;
        case Direction::left:
            taunt = "Left" + taunt;
            break;
        case Direction::right:
            taunt = "Right" + taunt;
            break;
    }

    //milliseconds ms2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    //auto d = ms2.count() - ms.count();
    //taunt = taunt + " (" + to_string(d) + ")";
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
