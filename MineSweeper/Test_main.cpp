#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>


std::vector<std::vector<std::string> > BOARD;
std::vector<std::vector<std::string> > MINES;
std::vector<std::vector<bool> > visited;

std::pair<int, int> BOARD_SIZE;
int NUM_MINES;
bool MARK;


int getConfig(const std::string &keyword) {
    int value = 0;
    std::ifstream configFile("./config");

    if (!configFile.is_open()) {
        std::cerr << "Nelze otevřít soubor " << "./config" << std::endl;
        return value;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        std::istringstream iss(line);
        std::string key;
        int x;
        char equalSign;
        if ((iss >> key >> std::ws >> equalSign >> std::ws >> x) && (equalSign == '=' && iss.eof())) {
            if (key == keyword) {
                value = x;
            }
        }
    }
    configFile.close();
    return value;
}

int getRandomNumber() {
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<int> distribution(1, getConfig("d"));

    return distribution(gen);
}


void printBoard() {

    //qtmodel
    int i;
    std::cout << " \n " << std::endl;
    std::cout << "   ";
    for (i = 1; i < BOARD_SIZE.first + 1; i++) {
        std::cout << " " << i;
    }
    std::cout << std::endl << "   ";
    for (i = 0; i < BOARD_SIZE.first; i++) {
        std::cout << "--";
    }
    std::cout << std::endl;
    i = 1;
    for (auto &y: BOARD) {
        std::cout << i << " | ";
        for (const auto &x: y) {
            std::cout << x;
        }
        std::cout << std::endl;

        i++;
    }
}


void creatingBoard() {

    if (!BOARD.empty()) {
        BOARD.clear();
        MINES.clear();
        visited.clear();
    }
    BOARD_SIZE.first = getConfig("x");
    BOARD_SIZE.second = getConfig("y");

    for (int y = 0; y < getConfig("y"); y++) {
        std::vector<std::string> rowForBOARD;
        std::vector<std::string> rowForMINES;
        std::vector<bool> rowForVisit;
        for (int x = 0; x < getConfig("x"); x++) {
            if (getRandomNumber() == 1) {
                NUM_MINES++;
                rowForMINES.emplace_back("X ");
            } else {
                rowForMINES.emplace_back("█ ");
            }
            rowForVisit.emplace_back(false);
            rowForBOARD.emplace_back("█ ");
        }
        visited.push_back(rowForVisit);
        MINES.push_back(rowForMINES);
        BOARD.push_back(rowForBOARD);
    }
    if (NUM_MINES == 0) {
        printBoard();
        BOARD.clear();
        MINES.clear();
        visited.clear();
        creatingBoard();
    }
}


void printMines() {

    std::cout << " \n " << std::endl;

    for (auto &y: MINES) {
        for (const auto &x: y) {
            std::cout << x;
        }
        std::cout << std::endl;
    }
}


void markMine(int x, int y) {
    if (!visited[y][x]) {
        if (BOARD[y][x] == "M ") {
            BOARD[y][x] = "█ ";
        } else {
            BOARD[y][x] = "M ";
        }
        MARK = false;
    }
}


int revealCell(int x, int y, int revealedCells) {

    if (MARK) {
        markMine(x, y);
        return revealedCells;
    }

    if (x < 0 || y < 0 || x >= BOARD_SIZE.first || y >= BOARD_SIZE.second) {
        return revealedCells;
    }
    if (visited[y][x]) {
        return revealedCells;
    }

    visited[y][x] = true;

    if (MINES[y][x] == "X ") {
        return revealedCells;
    } else {
        int num_mines_in_range = 0;


        for (int yi = y - 1; yi <= y + 1; yi++) {
            for (int xi = x - 1; xi <= x + 1; xi++) {
                if (xi >= 0 && xi < BOARD_SIZE.first && yi >= 0 && yi < BOARD_SIZE.second && MINES[yi][xi] == "X ") {
                    num_mines_in_range++;
                }
            }
        }

        std::string s = std::to_string(num_mines_in_range) + " ";
        BOARD[y][x] = s;
        revealedCells++;

        if (num_mines_in_range == 0) {
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int xi = x + dx;
                    int yi = y + dy;

                    if (xi != x || yi != y || xi < 0 || yi < 0 || xi >= BOARD_SIZE.first || yi >= BOARD_SIZE.second) {
                        revealedCells = revealCell(xi, yi, revealedCells);
                    }
                }
            }
        }
    }
    return revealedCells;
}


void showHelp() {
    std::string r;
    std::cout << "This game is simple here is how to play" << std::endl;
    std::cout
            << "It will show you plane of squares and ask you to put some values (x and y) \n dont worry if you put wrong numbers it will try to tell you that you need to correct it\n you can mark bombs if you think there are in the spot by just putting f after your coordination\nso the input can look like this: 5 3 or like this: 2 6 f"
            << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help    Show help message" << std::endl;

    if (!BOARD.empty()) {
        std::cout << "c  to configure your game" << std::endl;
        std::cout << "p takes you back to the game" << std::endl;
        std::cout << "r resets the games" << std::endl;
        std::cout << "q quit the games" << std::endl;
        char ch;
        std::cin >> ch;
        if (ch == 'p') {
            printBoard();
            return;
        } else if (ch == 'r') {
            creatingBoard();
            printBoard();
        } else if (ch == 'c') {
            std::cout << "You can configure this app in files in config.txt file sorry this doesnt work" << std::endl;
            showHelp();
        } else {
            showHelp();
        }
    }
}

void setConfig() {
    std::fstream file("./config", std::ios::in | std::ios::out);

    if (!file.is_open()) {
        std::cerr << "File could not be open." << std::endl;
        return;
    } else {
        std::cout << "You open configuration system in which you can set your parameters" << std::endl;
    }
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
        lines.emplace_back(line);
    }


    // Přečtení řádků ze souboru do vektoru



    std::string searchKey;
    while (true) {
        std::cout << "Zadej klíč pro vyhledání: ";
        std::cin >> searchKey;
        if (searchKey == "q") {
            break;
        }
        for (auto &lin: lines) {
            std::cout << std::endl << lin;
            if (lin.compare(searchKey)) {
                // if (lin == searchKey) {
                std::cout << "\nPlease replace with your own settings :";
                std::cin >> lin;
                //}
            }
        }
    }

    // Přesuneme se zpět na začátek souboru
    file.seekp(0, std::ios::beg);

    // Zápis upravených řádků zpět do souboru
    for (const auto &li: lines) {
        file << li << '\n';
    }
    file.close(); // Uzavřeme soubor
}


std::pair<int, int> inputFunk() {
    std::pair<int, int> xy;
    xy.first = -1;
    xy.second = -1;
    std::string flag;
    std::string s;
    std::cout << "Enter the coordinates (x y) MAX=(" << BOARD_SIZE.first << " " << BOARD_SIZE.second
              << ") MIN=(1 1) to reveal a cell: ";
    std::getline(std::cin, s);
    std::istringstream iss(s);


    if (s == "--help") {
        showHelp();
        return xy;
    } else if (iss >> xy.first >> std::ws >> xy.second >> flag) {
        if (flag == "f") {
            MARK = true;
            std::cout << "MARK" << std::endl;
            xy.first--;
            xy.second--;

            return xy;
        }
    } else {

        xy.first--;
        xy.second--;

        return xy;
    }
    return xy;
}



int main() {
    // Inicializace hry
    creatingBoard();

    // Ověření, že velikost desky odpovídá očekávané hodnotě
    assert(BOARD_SIZE.first == getConfig("x"));
    assert(BOARD_SIZE.second == getConfig("y"));

    // Ověření, že deska byla správně vytvořena
    assert(!BOARD.empty());
    assert(BOARD.size() == getConfig("y"));
    assert(BOARD[0].size() == getConfig("x"));

    // Ověření, že pole min bylo správně vytvořeno
    assert(!MINES.empty());
    assert(MINES.size() == getConfig("y"));
    assert(MINES[0].size() == getConfig("x"));

    // Ověření, že pole navštívených buněk bylo správně vytvořeno
    assert(!visited.empty());
    assert(visited.size() == getConfig("y"));
    assert(visited[0].size() == getConfig("x"));

    return 0;
}
