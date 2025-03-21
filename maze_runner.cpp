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

    if (num_rows <= 0 || num_cols <= 0) {
        std::cerr << "Error: Invalid file size" << std::endl;
        return false;
    }
    return true;
}

bool searchStart(std::ifstream& file, Position& start) {
    maze.resize(num_rows, std::vector<char>(num_cols));
    char ch;
    bool found_start = false;

    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            file >> ch;
            maze[i][j] = ch;

            if (ch == 'e') {
                start = {i, j};
                found_start = true;
                return found_start;
            }
        }
    }
    
    std::cerr << "Error: Start position 'e' not found in the maze." << std::endl;
    return found_start;
}

Position loadMaze(const std::string& fileName) {
    std::ifstream file = openFile(fileName);
    if (!file) return {-1, -1};

    if (!readDimensions(file)) return {-1, -1};

    Position start = {-1, -1};

    bool found_start = searchStart(file, start);

    file.close();

    if (!found_start) return {-1, -1};

    return start; 
}

void printMaze() {
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            std::cout << maze[i][j];
        }
        std::cout << '\n';
    }
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

    Position start = loadMaze(file_name);
    if (start.row == -1) {
        std::cerr << "Erro ao carregar o labirinto!\n";
        return 1;
    }

    std::cout << num_cols << num_cols << std::endl;

    std::cout << "Labirinto carregado com sucesso!\n";
    printMaze();

    // Testando a função isValidPosition
    std::cout << "\nTestando posições válidas:\n";

    Position test1 = {1, 2};
    Position test2 = {0, 0};
    Position test3 = {3, 2};

    std::cout << "Posição (" << test1.row << ", " << test1.col << ") é "
              << (isValidPosition(test1) ? "válida!\n" : "inválida!\n");

    std::cout << "Posição (" << test2.row << ", " << test2.col << ") é "
              << (isValidPosition(test2) ? "válida!\n" : "inválida!\n");

    std::cout << "Posição (" << test3.row << ", " << test3.col << ") é "
              << (isValidPosition(test3) ? "válida!\n" : "inválida!\n");

    return 0;
}

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
