#include <iostream>
#include <fstream>
#include <istream>
#include <stdio.h>
#include <vector>
#include <stack>
#include <thread>
#include <chrono>
#include <cstdlib>


// Constante de atualizacao do labirinto
#define LOOP_SLEEP_MS 15


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
bool isWalkPossible (Position pos, int direction){

    Position clone = pos;

    switch (direction){

        case 0:
            clone.col=clone.col+1;
            if(isValidPosition(clone)){

                if (maze[pos.row][pos.col+1] == 'x' || maze[pos.row][pos.col+1] == 's')
                return true;
                return false;

            }
            return false;
        break;

        case 1:
            clone.col=clone.col-1;
            if(isValidPosition(clone)){

                if (maze[pos.row][pos.col-1] == 'x' || maze[pos.row][pos.col-1] == 's')
                return true;
                return false;

            }
            return false;
        break;  

        case 2:
            clone.row=clone.row-1;
            if(isValidPosition(clone)){

                if (maze[pos.row-1][pos.col] == 'x' || maze[pos.row-1][pos.col] == 's')
                return true;
                return false;

            }
            return false;
        break;

        case 3:
            clone.row=clone.row+1;
            if(isValidPosition(clone)){

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
std::vector<int> validMoves(Position pos){

    // Por definicao, a ordem eh: direita, esquerda, cima, baixo. 
    std::vector<int> vetor= {0, 0, 0, 0};
    Position clone = pos;

    for (int i=0; i<4;i++)
    vetor [i] = isWalkPossible(pos,i);

    return vetor;
    

}


// Verifica se chegou no final
bool win (Position pos, Position exit){

    if (pos.row == exit.row && pos.col==exit.col){
    printf("\nYou Won!\n");
    return true;
    }
    
    return false;
}


// Inicia o processo andar pelo labirinto ate achar a saida
bool walk(Position start, Position exit){

    bool won = false;
    // Stack com caminho percorrido
    std::stack<Position> route;
    route.push(start);
    maze[start.row][start.col] = 'o';

    while(!route.empty()) {
        Position pos = route.top();

        // Verifica se chegou ao final
        if(win(pos, exit)){
            maze[exit.row][exit.col] = 'o';
            won = true;
        }

        // Limpa o terminal e printa o labirinto
        system("clear");
        printMaze();

        if(won) {
            std::cout << "You escaped!" << std::endl;
            return true;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_SLEEP_MS));

        // Obtem quais sao os mivmentos possiveis a partir da posicao atual
        std::vector<int> moves = validMoves(pos);
        bool moved = false;

        for(size_t i = 0; i < moves.size(); i++) {
            if(moves[i]) {
                Position new_position = pos; 
            
                // Verifica qual direcao e valida
                switch(i) {
                    case 0:
                        new_position.col += 1;
                        break;
                    case 1:
                        new_position.col -= 1;
                        break;
                    case 2:
                        new_position.row -= 1;
                        break;
                    case 3:
                        new_position.row += 1;
                        break;
                    default:
                        std::cout << "Invalid move vector size" << std::endl;
                        break;
                }
                
                // Movimenta para a nova posicao e marca o local ja percorrido
                if(maze[new_position.row][new_position.col] != '.') {
                    maze[new_position.row][new_position.col] = 'o';
                    maze[pos.row][pos.col] = '.';
                    route.push(new_position);
                    moved = true;
                    break;
                }

            }

        }

        // Se nao pode mover, volta ate o ultimo ponto com movimentos validos
        if(!moved) {
            maze[pos.row][pos.col] = '.';
            route.pop();
        }

    }

    std::cerr << "Could not find the exit." << std::endl;
    return false;
}


int main() {
    // Caminho para o arquivo do labirinto que deseja-se ler
    std::string file_name = "data/maze6.txt"; 

    // Abre o arquivo
    std::ifstream file = openFile(file_name);
    if (!file) return -1;

    // Le suas dimenoes e armazena em num_rows e num_cols
    readDimensions(file);
    readMaze(file);
    
    // Procurando a entrada
    Position start = {-1,-1};
    start = searchEntry();

    // Procurando a saida
    Position exit = {-1,-1};
    exit = searchExit();

    // Inicia o andar do personagem
    walk(start, exit);
    return 0;
}


// Nota sobre o uso de std::this_thread::sleep_for:
// 
// A função std::this_thread::sleep_for é parte da biblioteca <thread> do C++11 e posteriores.
// Ela permite que você pause a execução do thread atual por um período especificado.
// 
// Para usar std::this_thread::sleep_for, você precisa:
// 1. Incluir as bibliotecas <thread> e <chrono>
// 2. Usar o namespace std::chrono para as unidades de tempo
// 
// Exemplo de uso:
// std::this_thread::sleep_for(std::chrono::milliseconds(50));
// 
// Isso pausará a execução por 50 milissegundos.
// 
// Você pode ajustar o tempo de pausa conforme necessário para uma melhor visualização
// do processo de exploração do labirinto.
