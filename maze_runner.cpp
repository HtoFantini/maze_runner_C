#include <iostream>
#include <fstream>
#include <istream>
#include <stdio.h>
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

// Estrutura para representar uma posição no labirinto
struct Position {
    int row;
    int col;
};

// Variáveis globais
Maze maze;
int num_rows;
int num_cols;
std::stack<Position> valid_positions;
std::mutex walk_mutex;
std::mutex print_mutex;
std::mutex won_mutex;
std::atomic<bool> won{false};

// Abre o arquivo
std::ifstream openFile(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Error: Couldn't open file" << std::endl;
    }
    return file;
}


// Le no arquivo as dimensoes do labirinto
bool readDimensions(std::ifstream& file) {
    file >> num_rows >> num_cols;

    if (num_rows <= 0 || num_cols <= 0) {
        std::cerr << "Error: Invalid file size" << std::endl;
        return false;
    }
    return true;
}


// Le e carrega o labirinto do arquivo escolhido
void readMaze(std::ifstream& file) {
    std::string line;
    maze.clear();

    std::getline(file, line);

    while (std::getline(file, line)) {  
        maze.push_back(std::vector<char>(line.begin(), line.end()));
    }
}


// Printa o labirinto
void printMaze() {
    // Trava o lock para printar na tela
    std::lock_guard<std::mutex> lock(print_mutex);
    system("clear");
    for (const auto& row : maze) {   
        for (char cell : row) {      
            std::cout << cell;
        }
        std::cout << '\n';  
    }
}


// Procura a entrada
Position searchEntry() {
    for(int row=0; row < num_rows; row++) {
        for(int col=0; col < num_cols; col++) {
            if(maze[row][col] == 'e') {
                return {row,col};
            }
        }
    }
    std::cerr << "Entry not found" << std::endl;
    return {-1,-1};
}


// Procura a saida
Position searchExit() {
    for(int row=0; row < num_rows; row++) {
        for(int col=0; col < num_cols; col++) {
            if(maze[row][col] == 's') {
                return {row,col};
            }
        }
    }
    std::cerr << "Exit not found" << std::endl;
    return {-1,-1};
}


// Verifica se a posicao e valido (nao e borda nem muro)
bool isValidPosition(const Position& pos) {
    if (pos.row < 0 || pos.row >= num_rows || pos.col < 0 || pos.col >= num_cols) {
        return false;
    }

    if (maze[pos.row][pos.col] == 'x' || maze[pos.row][pos.col] == 's') {
        return true;
    }

    return false;
}


// Verifica se ele pode ocupar a proxima casa
bool isWalkPossible (Position pos, int direction) {

    Position clone = pos;

    switch (direction){

        case 0:
            clone.col=clone.col+1;
            if(isValidPosition(clone)) {

                if (maze[pos.row][pos.col+1] == 'x' || maze[pos.row][pos.col+1] == 's')
                return true;
                return false;

            }
            return false;
        break;

        case 1:
            clone.col=clone.col-1;
            if(isValidPosition(clone)) {

                if (maze[pos.row][pos.col-1] == 'x' || maze[pos.row][pos.col-1] == 's')
                return true;
                return false;

            }
            return false;
        break;  

        case 2:
            clone.row=clone.row-1;
            if(isValidPosition(clone)) {

                if (maze[pos.row-1][pos.col] == 'x' || maze[pos.row-1][pos.col] == 's')
                return true;
                return false;

            }
            return false;
        break;

        case 3:
            clone.row=clone.row+1;
            if(isValidPosition(clone)) {

                if (maze[pos.row+1][pos.col] == 'x' || maze[pos.row+1][pos.col] == 's')
                return true;
                return false;

            }
            return false;
        break;

        default:
            return false;
        break;

    }

}


// Retorna os movimentos validos
std::vector<int> validMoves(Position pos) {

    // Por definicao, a ordem eh: direita, esquerda, cima, baixo. 
    std::vector<int> vetor= {0, 0, 0, 0};
    Position clone = pos;

    for (int i=0; i<4;i++)
    vetor [i] = isWalkPossible(pos,i);

    return vetor;
    

}


bool win(Position pos, Position exit) {
    return (pos.row == exit.row && pos.col == exit.col);
}

Position getNewPosition(Position pos, int direction) {
    Position new_pos = pos;
    switch(direction) {
        case 0: new_pos.col += 1; break;
        case 1: new_pos.col -= 1; break;
        case 2: new_pos.row -= 1; break;
        case 3: new_pos.row += 1; break;
    }
    return new_pos;
}

bool processMovement(Position& pos, Position new_pos, std::stack<Position>& route, bool is_exit) {
    if(maze[new_pos.row][new_pos.col] != '.') {
        std::lock_guard<std::mutex> lock(walk_mutex);
        if(is_exit) {
            maze[new_pos.row][new_pos.col] = 'o';
        } else {
            maze[new_pos.row][new_pos.col] = '.';
        }
        maze[pos.row][pos.col] = '.'; 
        route.push(new_pos);
        return true;
    }
    return false;
}

void walk(Position current_pos, Position exit, std::stack<Position> route) {

    while(!route.empty() && !won) {
        Position pos = route.top();

        if (win(pos,exit)) {
            std::lock_guard<std::mutex> won_lock(won_mutex);
                {
                    std::lock_guard<std::mutex> maze_lock(walk_mutex);
                    maze[exit.row][exit.col] = 'o';
                }
            printMaze();
            std::cout << "\nYou Won!\n";
            return;
        }

        {
            std::lock_guard<std::mutex> lock(walk_mutex);
            printMaze();
        }

        std::vector<int> moves = validMoves(pos);
        bool moved = false;

        int valid_paths = 0;
        for (size_t i=0; i < moves.size(); i++) {
            if(moves[i]) {
                valid_paths++;
            }
        }

        if (valid_paths > 0) {
            moved = false;
            
            for (size_t i = 0; i < moves.size(); i++) {
                if (moves[i] && !won) {
                    Position new_pos = getNewPosition(pos, i);
                    
                    if (valid_paths == 1) {
                        if (processMovement(pos, new_pos, route, win(new_pos, exit))) {
                            moved = true;
                            break;
                        }
                    } 
                    else {
                        if (maze[new_pos.row][new_pos.col] != '.') {
                            if (processMovement(pos, new_pos, route, win(new_pos, exit))) {
                                moved = true;
                                // Cria threads para caminhos extras
                                for (size_t j = i + 1; j < moves.size(); j++) {
                                    if (moves[j] && !won) {
                                        Position alt_pos = getNewPosition(pos, j);
                                        if (maze[alt_pos.row][alt_pos.col] != '.') {
                                            std::stack<Position> new_route = route;
                                            {
                                                std::lock_guard<std::mutex> lock(walk_mutex);
                                                maze[alt_pos.row][alt_pos.col] = 'o';
                                                maze[pos.row][pos.col] = '.';
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
        
            if(!moved) {
                std::lock_guard<std::mutex> lock(walk_mutex);
                maze[pos.row][pos.col] = '.';
                route.pop();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_SLEEP_MS));
    }
}

// Inicia o processo andar pelo labirinto ate achar a saida
void start_maze_runner(Position start, Position exit) {

    std::stack<Position> route;
    route.push(start);
    
    {
        std::lock_guard<std::mutex> lock(walk_mutex);
        maze[start.row][start.col] = 'o';
    }
    
    walk(start, exit, route);
    
    if(!won) {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cerr << "Could not find the exit." << std::endl;
    }
}


int main() {
    // Caminho para o arquivo do labirinto que deseja-se ler
    std::string file_name = "data/maze.txt"; 

    // Abre o arquivo
    std::ifstream file = openFile(file_name);
    if (!file) return -1;

    // Le suas dimenoes e armazena em num_rows e num_cols
    readDimensions(file);
    readMaze(file);
    
    // Procurando a entrada
    Position start = searchEntry();
    if(start.row == -1 || start.col == -1) return -1;

    // Procurando a saida
    Position exit = searchExit();
    if(exit.row == -1 || exit.col == -1) return -1;

    // Inicia o andar do personagem
    start_maze_runner(start, exit);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    return 1;
}