#include <iostream>
#include <fstream>
#include <istream>
#include <stdio.h>
#include <vector>
#include <stack>
// #include <thread>
#include <chrono>


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


std::ifstream openFile(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Error: Couldn't open file" << std::endl;
    }
    return file;
}

bool readDimensions(std::ifstream& file) {
    file >> num_rows >> num_cols;

    std::cout << num_rows << " " << num_cols << std::endl;

    if (num_rows <= 0 || num_cols <= 0) {
        std::cerr << "Error: Invalid file size" << std::endl;
        return false;
    }
    return true;
}


void readMaze(std::ifstream& file) {
    std::string line;
    maze.clear();

    std::getline(file, line);

    while (std::getline(file, line)) {  
        maze.push_back(std::vector<char>(line.begin(), line.end()));
    }
}

void printMaze() {
    for (const auto& row : maze) {   
        for (char cell : row) {      
            std::cout << cell;
        }
        std::cout << '\n';  
    }
}

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

bool isValidPosition(const Position& pos) {
    if (pos.row < 0 || pos.row >= num_rows || pos.col < 0 || pos.col >= num_cols) {
        return false;
    }

    if (maze[pos.row][pos.col] == 'x') {
        return true;
    }

    return false;
}

bool walk(Position pos) {
    // TODO: Implemente a lógica de navegação aqui
    // 1. Marque a posição atual como visitada (maze[pos.row][pos.col] = '.')
    // 2. Chame print_maze() para mostrar o estado atual do labirinto
    // 3. Adicione um pequeno atraso para visualização:
    //    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // 4. Verifique se a posição atual é a saída (maze[pos.row][pos.col] == 's')
    //    Se for, retorne true
    // 5. Verifique as posições adjacentes (cima, baixo, esquerda, direita)
    //    Para cada posição adjacente:
    //    a. Se for uma posição válida (use is_valid_position()), adicione-a à pilha valid_positions
    // 6. Enquanto houver posições válidas na pilha (!valid_positions.empty()):
    //    a. Remova a próxima posição da pilha (valid_positions.top() e valid_positions.pop())
    //    b. Chame walk recursivamente para esta posição
    //    c. Se walk retornar true, propague o retorno (retorne true)
    // 7. Se todas as posições foram exploradas sem encontrar a saída, retorne false
    
    return false; // Placeholder - substitua pela lógica correta
}


int main() {
    std::string file_name = "data/maze.txt"; // Nome do arquivo do labirinto

    // Abre o arquivo
    std::ifstream file = openFile(file_name);
    if (!file) return -1;

    // Le suas dimenoes e armazena em num_rows e num_cols
    readDimensions(file);
    // Le e armazena todo o labirinto em maze
    readMaze(file);
    // Printa maze
    printMaze();

    // Verificando se a leitura esta pulando a primeira linha do arquivo
    printf("Maze first line: ");
    std::cout << std::string(maze[0].begin(), maze[0].end()) << std::endl;
    
    // Procurando a entrada
    Position start = {-1,-1};
    start = searchEntry();
    std::cout << start.row << " " << start.col << std::endl;

    // Procurando a saida
    Position exit = {-1,-1};
    exit = searchExit();
    std::cout << exit.row << " " << exit.col << std::endl;

    
    ////////// Testando isValidPosition() /////////////
    Position valid1 = {1,6};
    Position invalid1 = {-1,-1};
    Position valid2 = {0,0};
    Position invalid2 = {num_rows,num_cols};
    
    std::cout << isValidPosition(valid1) << std::endl;
    std::cout << isValidPosition(invalid1) << std::endl;
    std::cout << isValidPosition(valid2) << std::endl;
    std::cout << isValidPosition(invalid2) << std::endl;
    ////////////////////////////////////////////////////

    return 0;
}

// FUNCAO MAIN FEITA PELO PROFESSOR

// int main(int argc, char* argv[]) {
//     if (argc != 2) {
//         std::cerr << "Uso: " << argv[0] << " <arquivo_labirinto>" << std::endl;
//         return 1;
//     }

//     Position initial_pos = loadMaze(argv[1]);
//     if (initial_pos.row == -1 || initial_pos.col == -1) {
//         std::cerr << "Posição inicial não encontrada no labirinto." << std::endl;
//         return 1;
//     }

//     bool exit_found = walk(initial_pos);

//     if (exit_found) {
//         std::cout << "Saída encontrada!" << std::endl;
//     } else {
//         std::cout << "Não foi possível encontrar a saída." << std::endl;
//     }

//     return 0;
// }

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
