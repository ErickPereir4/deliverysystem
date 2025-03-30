#include <stdio.h>     // Funções para entrada e saída padrão (printf, scanf, etc.)
#include <string.h>    // Funções para manipulação de strings (strcmp, strcpy, strlen, etc.)
#include <stdlib.h>    // Funções gerais da biblioteca padrão, como alocação de memória (malloc, free), conversões (atoi, atof), etc.
#include <ctype.h>     // Funções para manipulação de caracteres, como verificar se é alfanumérico, maiúsculo, minúsculo, etc. (isalpha, isdigit, toupper, etc.)
#include <unistd.h>    // Funções para operações de sistema, como manipulação de arquivos e diretórios (sleep, fork, read, write, etc.)
#include <locale.h>    // Funções para manipulação de configurações de localidade (setlocale) que afetam formatação de data, hora, moeda, etc.


// Configuração para Windows
#ifdef _WIN32
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

void habilitar_cores_windows() {
    // Configurar para UTF-8 primeiro
    SetConsoleOutputCP(CP_UTF8);
    
    // Depois habilitar processamento de terminal virtual
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

// Definições de cores ANSI
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define WHITE   "\x1B[37m"
#define RESET   "\x1B[0m"
#define BOLD    "\x1B[1m"

// Serve para criar instâncias.
typedef struct {
    char nome[50];
    char endereco[100];
} Cliente;

typedef struct {
    char nome[50];
    char ingredientes[200];  
    float preco;
} Pizza;

typedef struct {
    char cliente[50];
    char pizza[50];
    float preco;
} Pedido;

// Variáveis globais
Cliente *clientes = NULL;
Pedido *pedidos = NULL;
Pizza cardapio[] = {
    {"Margherita", "Molho de tomate, mussarela fresca, manjericão", 45.90},
    {"Calabresa", "Molho de tomate, mussarela, calabresa fatiada, cebola", 52.50},
    {"Quatro Queijos", "Molho de tomate, mussarela, provolone, parmesão, gorgonzola", 58.00},
    {"Portuguesa", "Molho de tomate, mussarela, presunto, ovos, cebola, azeitonas", 55.75},
    {"Frango Catupiry", "Molho de tomate, mussarela, frango desfiado, catupiry", 59.90}
};
int total_clientes = 0;
int total_pedidos = 0;
int total_pizzas = 5;
int capacidade_clientes = 0;
int capacidade_pedidos = 0;

void configurar_ambiente() {
    // Configurar locale para suportar UTF-8
    setlocale(LC_ALL, "en_US.UTF-8");
    
    #ifdef _WIN32
    habilitar_cores_windows();
    #endif
}
//É usada para limpar o console antes de exibir novas informações, melhorando a organização da saída.
void limpar_tela() {
    // Usado para limpar o console do windons.
    #ifdef _WIN32
        system("cls");
    #else
    // Usado para limpar o console do MAC.
        system("clear");
    #endif
}
//limpar os caracteres que sobraram no buffer de entrada do teclado, 
// principalmente após o uso de scanf(), evitando comportamentos inesperados na leitura de dados.
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
// Executado no final de cada função do menu principal.
void pausar() {
    printf(BLUE "\nPressione Enter para continuar..." RESET);
    limpar_buffer();
}

// Menu Principal.
void menu_principal() {
    limpar_tela();
    printf(BOLD CYAN "=================================\n");
    printf("   " YELLOW "🍕 SISTEMA DE DELIVERY 🍕\n" CYAN);
    printf("=================================\n" RESET);
    printf(RED "1." BLUE " Cadastrar Cliente\n" RESET);
    printf(RED "2." BLUE " Listar Clientes\n" RESET);
    printf(RED "3." BLUE " Editar Cliente\n" RESET);
    printf(RED "4." BLUE " Remover Cliente\n" RESET);
    printf(RED "5." BLUE " Fazer Pedido\n" RESET);
    printf(RED "6." BLUE " Resumo de Pedidos\n" RESET);
    printf(RED "7." BLUE " Simular Entrega\n" RESET);
    printf(RED "8." BLUE " Sair\n" RESET);
    printf(BOLD CYAN "=================================\n");
    printf(RESET BOLD "Escolha uma opção: " RESET);
}

//Sempre que o array atingir sua capacidade máxima (ou seja, quando o buffer "estourar"), 
// essa função dobra o tamanho disponível, garantindo que novos elementos possam ser armazenados sem erro.
void redimensionar_array(void **array, int *capacidade, size_t tamanho_elemento) {
    int nova_capacidade = (*capacidade == 0) ? 1 : *capacidade * 2;
    void *novo = realloc(*array, nova_capacidade * tamanho_elemento);
    if (!novo) {
        printf(RED "\nErro ao alocar memória!\n" RESET);
        return;
    }
    *array = novo;
    *capacidade = nova_capacidade;
}

// Menu : Opção 01 - Cadastrar Cliente.
void cadastrar_cliente() {
    limpar_tela();
    printf(BOLD CYAN "=== CADASTRAR CLIENTE ===\n\n" RESET);
    
    if (total_clientes >= capacidade_clientes) {
        redimensionar_array((void **)&clientes, &capacidade_clientes, sizeof(Cliente));
    }
    
    printf(BLUE "Nome: " RESET);
    if (fgets(clientes[total_clientes].nome, 50, stdin) == NULL) {
        printf(RED "\nErro ao ler nome!\n" RESET);
        return;
    }
    clientes[total_clientes].nome[strcspn(clientes[total_clientes].nome, "\n")] = '\0';
    
    printf(BLUE "Endereço: " RESET);
    if (fgets(clientes[total_clientes].endereco, 100, stdin) == NULL) {
        printf(RED "\nErro ao ler endereço!\n" RESET);
        return;
    }
    clientes[total_clientes].endereco[strcspn(clientes[total_clientes].endereco, "\n")] = '\0';
    
    total_clientes++;
    printf(GREEN "\nCliente cadastrado com sucesso!\n" RESET);
    pausar();
}

// Menu : Opção 02 - Listar Clientes.
void listar_clientes() {
    limpar_tela();
    printf(BOLD CYAN "=== LISTA DE CLIENTES ===\n\n" RESET);
    
    if (total_clientes == 0) {
        printf(YELLOW "Nenhum cliente cadastrado.\n" RESET);
        pausar();
        return;
    }
    
    printf(BOLD WHITE "-------------------------------------------------\n");
    printf("| ID | NOME             | ENDEREÇO           |\n");
    printf("-------------------------------------------------\n" RESET);
    for (int i = 0; i < total_clientes; i++) {
        printf("| %-2d | %-15s | %-20s |\n", 
               i+1, 
               clientes[i].nome, 
               clientes[i].endereco);
    }
    printf(BOLD WHITE "-------------------------------------------------\n" RESET);
    pausar();
}

// Menu : Opção 03 - Editar Cliente.
void editar_cliente() {
    limpar_tela();
    printf(BOLD CYAN "=== EDITAR CLIENTE ===\n\n" RESET);
    
    if (total_clientes == 0) {
        printf(YELLOW "Nenhum cliente cadastrado.\n" RESET);
        pausar();
        return;
    }
    
    listar_clientes();
    
    int id;
    printf(BLUE "\nDigite o ID do cliente que deseja editar: " RESET);
    if (scanf("%d", &id) != 1) {
        printf(RED "\nID inválido!\n" RESET);
        limpar_buffer();
        pausar();
        return;
    }
    limpar_buffer();
    
    if (id < 1 || id > total_clientes) {
        printf(RED "\nID inválido!\n" RESET);
        pausar();
        return;
    }
    
    int index = id - 1;
    
    printf(BLUE "\nEditando cliente: %s\n" RESET, clientes[index].nome);
    printf(BLUE "Novo nome (atual: %s): " RESET, clientes[index].nome);
    if (fgets(clientes[index].nome, 50, stdin) == NULL) {
        printf(RED "\nErro ao ler nome!\n" RESET);
        return;
    }
    clientes[index].nome[strcspn(clientes[index].nome, "\n")] = '\0';
    
    printf(BLUE "Novo endereço (atual: %s): " RESET, clientes[index].endereco);
    if (fgets(clientes[index].endereco, 100, stdin) == NULL) {
        printf(RED "\nErro ao ler endereço!\n" RESET);
        return;
    }
    clientes[index].endereco[strcspn(clientes[index].endereco, "\n")] = '\0';
    
    printf(GREEN "\nCliente editado com sucesso!\n" RESET);
    pausar();
}

// Menu : Opção 04 - Remover Cliente.
void remover_cliente() {
    limpar_tela();
    printf(BOLD CYAN "=== REMOVER CLIENTE ===\n\n" RESET);
    
    if (total_clientes == 0) {
        printf(YELLOW "Nenhum cliente cadastrado.\n" RESET);
        pausar();
        return;
    }
    
    listar_clientes();
    
    int id;
    printf(BLUE "\nDigite o ID do cliente que deseja remover: " RESET);
    if (scanf("%d", &id) != 1) {
        printf(RED "\nID inválido!\n" RESET);
        limpar_buffer();
        pausar();
        return;
    }
    limpar_buffer();
    
    if (id < 1 || id > total_clientes) {
        printf(RED "\nID inválido!\n" RESET);
        pausar();
        return;
    }
    
    int index = id - 1;
    char nome[50];
    strcpy(nome, clientes[index].nome);
    
    for (int i = index; i < total_clientes - 1; i++) {
        strcpy(clientes[i].nome, clientes[i+1].nome);
        strcpy(clientes[i].endereco, clientes[i+1].endereco);
    }
    
    total_clientes--;
    printf(GREEN "\nCliente '%s' removido com sucesso!\n" RESET, nome);
    pausar();
}

// Menu : Opção 05 - Mostrar Cárdapio.
void mostrar_cardapio() {
    printf(BOLD CYAN "\n================================ CARDÁPIO ================================\n");
    printf("| Nº | PIZZA            | INGREDIENTES                              | PREÇO   |\n");
    printf("---------------------------------------------------------------------------\n" RESET);
    for (int i = 0; i < total_pizzas; i++) {
        printf("| %-2d | %-15s | %-40s | " GREEN "R$%-6.2f" RESET " |\n", 
               i+1, 
               cardapio[i].nome, 
               cardapio[i].ingredientes, 
               cardapio[i].preco);
    }
    printf(BOLD CYAN "---------------------------------------------------------------------------\n" RESET);
}

// Menu : Opção 06 - Fazer Pedido.
void fazer_pedido() {
    limpar_tela();
    printf(BOLD CYAN "=== FAZER PEDIDO ===\n\n" RESET);
    
    if (total_clientes == 0) {
        printf(YELLOW "Nenhum cliente cadastrado. Cadastre um cliente primeiro.\n" RESET);
        pausar();
        return;
    }
    
    printf(BOLD CYAN "=== CLIENTES CADASTRADOS ===\n" RESET);
    for (int i = 0; i < total_clientes; i++) {
        printf(BLUE "%d. %s\n" RESET, i+1, clientes[i].nome);
    }
    printf("\n");
    
    int cliente_idx;
    printf(BLUE "Escolha o cliente pelo número: " RESET);
    if (scanf("%d", &cliente_idx) != 1) {
        printf(RED "\nEntrada inválida!\n" RESET);
        limpar_buffer();
        pausar();
        return;
    }
    limpar_buffer();
    
    if (cliente_idx < 1 || cliente_idx > total_clientes) {
        printf(RED "\nCliente inválido!\n" RESET);
        pausar();
        return;
    }
    
    mostrar_cardapio();
    
    int pizza_idx;
    printf(BLUE "\nEscolha a pizza pelo número: " RESET);
    if (scanf("%d", &pizza_idx) != 1) {
        printf(RED "\nEntrada inválida!\n" RESET);
        limpar_buffer();
        pausar();
        return;
    }
    limpar_buffer();
    
    if (pizza_idx < 1 || pizza_idx > total_pizzas) {
        printf(RED "\nPizza inválida!\n" RESET);
        pausar();
        return;
    }
    
    if (total_pedidos >= capacidade_pedidos) {
        redimensionar_array((void **)&pedidos, &capacidade_pedidos, sizeof(Pedido));
    }
    
    strncpy(pedidos[total_pedidos].cliente, clientes[cliente_idx-1].nome, 50);
    strncpy(pedidos[total_pedidos].pizza, cardapio[pizza_idx-1].nome, 50);
    pedidos[total_pedidos].preco = cardapio[pizza_idx-1].preco;
    total_pedidos++;
    
    printf(GREEN "\nPedido registrado com sucesso!\n" RESET);
    printf(BLUE "-> %s para %s - " GREEN "R$%.2f\n" RESET, 
           cardapio[pizza_idx-1].nome, 
           clientes[cliente_idx-1].nome, 
           cardapio[pizza_idx-1].preco);
    pausar();
}

// Menu : Opção 07 - Resumo Pedidos.
void resumo_pedidos() {
    limpar_tela();
    printf(BOLD CYAN "=== RESUMO DE PEDIDOS ===\n\n" RESET);
    
    if (total_pedidos == 0) {
        printf(YELLOW "Nenhum pedido registrado.\n" RESET);
        pausar();
        return;
    }
    
    float total = 0;
    printf(BOLD WHITE "===========================================\n");
    printf("| CLIENTE          | PIZZA            | VALOR   |\n");
    printf("-------------------------------------------\n" RESET);
    for (int i = 0; i < total_pedidos; i++) {
        printf("| %-16s | %-16s | " GREEN "R$%-6.2f" RESET " |\n", 
               pedidos[i].cliente, 
               pedidos[i].pizza, 
               pedidos[i].preco);
        total += pedidos[i].preco;
    }
    printf(BOLD WHITE "-------------------------------------------\n");
    printf("| TOTAL                             | " GREEN "R$%-6.2f" WHITE " |\n", total);
    printf("===========================================\n" RESET);
    
    pausar();
}

void simular_entrega() {
    limpar_tela();
    printf(BOLD CYAN "=== SIMULAR ENTREGA ===\n\n" RESET);
    
    if (total_pedidos == 0) {
        printf(YELLOW "Nenhum pedido para entregar.\n" RESET);
        pausar();
        return;
    }
    
    printf(BLUE "Pedido em andamento:\n" RESET);
    printf(BLUE "-> Cliente: %s\n" RESET, pedidos[0].cliente);
    printf(BLUE "-> Pizza: %s\n" RESET, pedidos[0].pizza);
    printf(BLUE "-> Valor: " GREEN "R$%.2f\n\n" RESET, pedidos[0].preco);
    
    printf(BLUE "Simulando entrega" RESET);
    for (int i = 0; i < 3; i++) {
        printf(YELLOW "." RESET);
        fflush(stdout);
        sleep(1);
    }
    
    for (int i = 0; i < total_pedidos - 1; i++) {
        strncpy(pedidos[i].cliente, pedidos[i+1].cliente, 50);
        strncpy(pedidos[i].pizza, pedidos[i+1].pizza, 50);
        pedidos[i].preco = pedidos[i+1].preco;
    }
    total_pedidos--;
    
    printf(GREEN "\n\nEntrega concluída com sucesso!\n" RESET);
    pausar();
}

void liberar_memoria() {
    free(clientes);
    free(pedidos);
}

// Menu principal 
int main() {
    // Chama essa função para Permite o UTF8 para gerar os emojis e permite palavras acentuadas.
    configurar_ambiente();
    //Gerar erro caso a opção inserida não seja uma das 8 opções.
    int opcao;
    do {
        menu_principal();
        if (scanf("%d", &opcao) != 1) {
            printf(RED "\nEntrada inválida!\n" RESET);
            limpar_buffer();
            pausar();
            continue;
        }
        limpar_buffer();
        // cada opção chama a função correspondente.
        switch(opcao) {
            case 1: cadastrar_cliente(); break;
            case 2: listar_clientes(); break;
            case 3: editar_cliente(); break;
            case 4: remover_cliente(); break;
            case 5: fazer_pedido(); break;
            case 6: resumo_pedidos(); break;
            case 7: simular_entrega(); break;
            case 8: printf(BLUE "\nSaindo do sistema...\n" RESET); break;
            default: printf(RED "\nOpção inválida! Tente novamente.\n" RESET); pausar(); break;
        }
    } while(opcao != 8);
    
    liberar_memoria();
    return 0;
}