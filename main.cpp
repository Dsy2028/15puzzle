// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <array>
#include <algorithm>
#include <vector>
#include <random>
#include <cmath>
#include <iterator>
#include <numeric>
static int generateRandomInt() {
    // Use random_device to seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 4);
    // Generate and return the random number
    return dis(gen);
}

class Direction {
public:
    enum Type {
        up,
        down,
        left,
        right,
        max_directions
    };

    Direction() {
        m_direction = generateRandomDirection().m_direction;
        std::cout << "Generating random direction... " << directionToString() << '\n';
    }
    Direction(const Type& type) : m_direction{ type } {}
    Direction(int type) : m_direction{ static_cast<Type>(type) } {}

    std::string_view directionToString() const {
        switch (m_direction) {
        case up: return "up";
        case down: return "down";
        case left: return "left";
        case right: return "right";
        case max_directions: return "max directions";
        default: return "unknown";
        }
    }

    Type getVal() const { return m_direction; }

    Direction operator-() const {
        Type new_direction;
        switch (m_direction) {
        case up: new_direction = down; break;
        case left: new_direction = right; break;
        case down: new_direction = up; break;
        default: new_direction = left; break;
        }
        return Direction(new_direction);
    }

    friend std::ostream& operator<<(std::ostream& stream, const Direction& direction) {
        stream << direction.directionToString();
        return stream;
    }

    static Direction generateRandomDirection() {
        return Direction(generateRandomInt());
    }

private:
    Type m_direction{};
};

struct Point {
    int x;
    int y;

    Point getAdjacentPoint(const Direction& d) const {
        switch (d.getVal()) {
        case Direction::up: return Point{ x, y - 1 };
        case Direction::down: return Point{ x, y + 1 };
        case Direction::left: return Point{ x - 1, y };
        case Direction::right: return Point{ x + 1, y };
        default: return *this; // Return the same point if direction is unknown
        }
    }
};

//static bool operator==(const Point& p, const Point& y) {
 //   return p.x == y.x && p.y == y.y;
//}

//static bool operator!=(const Point& p, const Point& y) {
  //  return !(p == y);
//}

class Tile {
public:
    Tile() = default;
    Tile(int a) : m_tileValue{ a } {}

    int getNum() { return m_tileValue; }
    bool isEmpty() { return !m_tileValue; }

    friend std::ostream& operator<<(std::ostream& stream, Tile tile) {
        if (tile.m_tileValue > 9) // if two digit number
            stream << " " << tile.m_tileValue << " ";
        else if (tile.m_tileValue > 0) // if one digit number
            stream << "  " << tile.m_tileValue << " ";
        else if (tile.m_tileValue == 0) // if empty spot
            stream << "    ";
        return stream;
    }

    friend bool operator==(const Tile& p, const Tile& y) {
        return p.m_tileValue == y.m_tileValue;
    }

private:
    int m_tileValue{};
};

constexpr int g_consoleLines{ 25 };

class Board {
public:
    Board() = default;

    static void printEmptyLines(int count) {
        for (int i = 0; i < count; ++i)
            std::cout << '\n';
    }

    friend std::ostream& operator<<(std::ostream& stream, Board& gboard) {
        printEmptyLines(g_consoleLines);
        for (const auto& element : gboard.m_gameBoard) {
            for (const auto& e : element) {
                stream << e;
            }
            stream << '\n';
        }
        return stream;
    }

    bool isPointValid(Point& p) {
        return (p.x <= 3 && p.x >= 0) && (p.y <= 3 && p.y >= 0);
    }

    Point positionOfEmptyTile() {
        Tile tile{ 0 };
        for (int rowIndex{ 0 }; rowIndex < std::size(m_gameBoard); ++rowIndex) {
            for (int colIndex{ 0 }; colIndex < std::size(m_gameBoard); ++colIndex) {
                if (m_gameBoard[rowIndex][colIndex] == tile) {
                    return Point{ colIndex, rowIndex };
                }
            }
        }
        return Point{ -1, -1 }; // Indicate an error if no empty tile is found
    }
    void swapTiles(Point& emptyTile, Point& adjacentTile) {
        std::swap(m_gameBoard[adjacentTile.y][adjacentTile.x], m_gameBoard[emptyTile.y][emptyTile.x]);
    }
    bool moveTile(Direction dir) {
        Point emptyTile{ positionOfEmptyTile() };
        Point adjacentTile{ emptyTile.getAdjacentPoint(-dir) };
        if (isPointValid(adjacentTile)) {
            swapTiles(emptyTile, adjacentTile);
            return true;
        }
        else {
            return false;
        }
    }
   void randomize() {
        for (int index{ 0 }; index < 1001; ++index) {
            Direction randomDirection{ Direction::generateRandomDirection() };
            moveTile(randomDirection);
        }
   }
   friend bool operator==(const Board& board, const Board& board2) {
       return board.m_gameBoard == board2.m_gameBoard;
   }
   bool playerWon() const{
       static Board finishedBoard{};
       return *this == finishedBoard;
   }

private:
    int m_garbageValue{};
    std::array<std::array<Tile, 4>, 4> m_gameBoard{ {
        {Tile{1}, Tile{2}, Tile{3}, Tile{4}},
        {Tile{5}, Tile{6}, Tile{7}, Tile{8}},
        {Tile{9}, Tile{10}, Tile{11}, Tile{12}},
        {Tile{13}, Tile{14}, Tile{15}, Tile{0}}
    } };
};

namespace UserInput {
    bool checkWASD(const char c) {
        switch (c) {
        case 97:  // 'a'
        case 119: // 'w'
        case 115: // 's'
        case 100: // 'd'
        case 113: // 'q'
            return false;
        default:
            return true;
        }
    }

    Direction commandToDirection(char c) {
        switch (c) {
        case 119: return Direction(Direction::up);
        case 115: return Direction(Direction::down);
        case 100: return Direction(Direction::right);
        case 97: return Direction(Direction::left);
        case 113: return Direction(Direction::max_directions);
        default: return Direction();
        }
    }

    char getCommandFromUser() {
        char x{};
        while (true) {
            std::cin >> x;
            if (std::cin.fail() || checkWASD(x)) {
                std::cin.clear(); // Clear the error flag
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            }
            else {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
                std::cout << "Valid command: " << x << '\n';
                return x; // Return the valid input
            }
        }
    }
}

int main()
{
    Board board{};
    board.randomize();
    std::cout << "Enter a command: ";
    while (true)
    {
        char ch{ UserInput::getCommandFromUser() };

        // Handle non-direction commands
        if (ch == 'q')
        {
            std::cout << "\n\nBye!\n\n";
            return 0;
        }

        // Handle direction commands
        Direction dir{ UserInput::commandToDirection(ch) };

        bool userMoved{ board.moveTile(dir) };
        if (userMoved)
            std::cout << board;
        if (board.playerWon()) {
            std::cout << "\n\nYou won!\n\n";
            return 0;
        }
    }
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
