#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <mutex>
#include <atomic>


// Constante de atualizacao do labirinto
#define LOOP_SLEEP_MS 30


// Representação do labirinto
using Maze = std::vector<std::vector<char>>;


// Estrutura para representar uma posicao no labirinto
struct Position {
    int row;
    int col;
};


// Variaveis globais
Maze maze;
int num_rows;
int num_cols;
std::mutex walk_mutex;
std::mutex print_mutex;
std::atomic<bool> won{false};


std::ifstream openFile(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Error: Could not open the file." << std::endl;
    }
    return file;
}


// Le as dimensões do labirinto
bool readDimensions(std::ifstream& file) {
    file >> num_rows >> num_cols;
    return (num_rows > 0 && num_cols > 0);
}


// Le e carrega o labirinto do arquivo
void readMaze(std::ifstream& file) {
    std::string line;
    maze.clear();
    std::getline(file, line);
    while (std::getline(file, line)) {
        maze.push_back(std::vector<char>(line.begin(), line.end()));
    }
}


// Imprime o labirinto
void printMaze() {
    std::lock_guard<std::mutex> lock(print_mutex);
    system("clear");
    for (const auto& row : maze) {
        for (char cell : row) {
            std::cout << cell;
        }
        std::cout << '\n';
    }
}


// Procura a entrada do labirinto
Position searchEntry() {
    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_cols; col++) {
            if (maze[row][col] == 'e') {
                return {row, col};
            }
        }
    }
    return {-1, -1};
}


// Procura a saida do labirinto
Position searchExit() {
    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_cols; col++) {
            if (maze[row][col] == 's') {
                return {row, col};
            }
        }
    }
    return {-1, -1};
}


// Verifica se a posicao e válida
bool isValidPosition(const Position& pos) {
    return (pos.row >= 0 && pos.row < num_rows && pos.col >= 0 && pos.col < num_cols &&
            (maze[pos.row][pos.col] == 'x' || maze[pos.row][pos.col] == 's'));
}


// Retorna os movimentos validos
std::vector<int> validMoves(Position pos) {
    std::vector<int> moves = {0, 0, 0, 0};
    Position directions[4] = {
        {pos.row, pos.col + 1}, // Direita
        {pos.row, pos.col - 1}, // Esquerda
        {pos.row - 1, pos.col}, // Cima
        {pos.row + 1, pos.col}  // Baixo
    };
    for (int i = 0; i < 4; i++) {
        if (isValidPosition(directions[i])) {
            moves[i] = 1;
        }
    }
    return moves;
}


// Retorna a nova posicao com base na direção
Position getNewPosition(Position pos, int direction) {
    switch (direction) {
        case 0: pos.col += 1; break;  // Direita
        case 1: pos.col -= 1; break;  // Esquerda
        case 2: pos.row -= 1; break;  // Cima
        case 3: pos.row += 1; break;  // Baixo
    }
    return pos;
}


// Verifica se a posicao e a saida
bool isExit(Position pos, Position exit) {
    return (pos.row == exit.row && pos.col == exit.col);
}


// Processa o movimento do personagem
bool processMovement(Position& pos, Position new_pos, std::stack<Position>& route) {
    std::lock_guard<std::mutex> lock(walk_mutex);
    if (maze[new_pos.row][new_pos.col] != '.') {
        maze[new_pos.row][new_pos.col] = 'o';
        maze[pos.row][pos.col] = '.';
        route.push(new_pos);
        return true;
    }
    return false;
}


// Move o jogador pelo labirinto
void walk(Position pos, Position exit, std::stack<Position> route) {
    while (!route.empty() && !won) {
        pos = route.top();

        if (isExit(pos, exit)) {
            if (!won.exchange(true)) {
                std::lock_guard<std::mutex> lock(walk_mutex);
                maze[exit.row][exit.col] = 'o';

                printMaze();
            }
            return;
        }

        if (!won) {
            printMaze();
        }

        std::vector<int> moves = validMoves(pos);
        int possible_paths = 0;
        for (int move : moves) possible_paths += move;

        if (possible_paths > 0) {
            bool moved = false;
            for (int i = 0; i < 4; i++) {
                if (moves[i] && !won) {
                    Position new_pos = getNewPosition(pos, i);

                    if (possible_paths == 1) {  
                        if (processMovement(pos, new_pos, route)) {
                            moved = true;
                            break;
                        }
                    } else {
                        if (maze[new_pos.row][new_pos.col] != '.') {
                            if (processMovement(pos, new_pos, route)) {
                                moved = true;

                                for (int j = i + 1; j < 4; j++) {
                                    if (moves[j] && !won) {
                                        Position alt_pos = getNewPosition(pos, j);
                                        if (maze[alt_pos.row][alt_pos.col] != '.') {
                                            std::stack<Position> new_route = route;
                                            {
                                                std::lock_guard<std::mutex> lock(walk_mutex);
                                                maze[alt_pos.row][alt_pos.col] = 'o';
                                            }
                                            new_route.push(alt_pos);
                                            std::thread(walk, alt_pos, exit, new_route).detach();
                                        }
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }

            if (!moved) {
                std::lock_guard<std::mutex> lock(walk_mutex);
                maze[pos.row][pos.col] = '.';
                route.pop();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_SLEEP_MS));
    }
}


// Inicia o jogo
void start_maze_runner(Position start, Position exit) {
    std::stack<Position> route;
    route.push(start);

    {
        std::lock_guard<std::mutex> lock(walk_mutex);
        maze[start.row][start.col] = 'o';
    }

    walk(start, exit, route);

}

int main() {
    std::string file_name = "data/maze3.txt";
    std::ifstream file = openFile(file_name);
    if (!file) return -1;

    if (!readDimensions(file)) return -1;
    readMaze(file);

    Position start = searchEntry();
    if (start.row == -1) return -1;

    Position exit = searchExit();
    if (exit.row == -1) return -1;

    start_maze_runner(start, exit);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    if(won) std::cout << "\nYou Won!\n";

    if (!won) std::cerr << "\nCould not find the exit\n" << std::endl;

    return 0;
}
