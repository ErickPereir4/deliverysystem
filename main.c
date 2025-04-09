#include <stdio.h>     // printf, scanf, fgets
#include <string.h>    // strcpy, strncpy, strcspn
#include <stdlib.h>    // realloc, free, system
#include <unistd.h>    // sleep
#include <locale.h>  // setlocale  

// Configuração para Windows
#ifdef _WIN32
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

void configurarTerminalWindows() {
    SetConsoleOutputCP(CP_UTF8);
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
    {"Margherita", "Molho de tomate, mussarela fresca, manjericão",                       45.90},
    {"Calabresa", "Molho de tomate, mussarela, calabresa fatiada, cebola",                52.50},
    {"Quatro Queijos", "Molho de tomate, mussarela, provolone, parmesão, gorgonzola",     58.00},
    {"Portuguesa", "Molho de tomate, mussarela, presunto, ovos, cebola, azeitonas",       55.75},
    {"Frango Catupiry", "Molho de tomate, mussarela, frango desfiado, catupiry",          59.90},
    {"Pepperoni", "Molho de tomate, mussarela, pepperoni em abundância",                  60.00},
    {"Vegetariana", "Molho de tomate, mussarela, pimentão, cogumelos, cebola, azeitonas", 56.50}
};
int total_clientes = 0;
int total_pedidos = 0;
int total_pizzas = 7;  
int capacidade_clientes = 0;
int capacidade_pedidos = 0;

void configurar_ambiente() {
    setlocale(LC_ALL, "en_US.UTF-8");
    #ifdef _WIN32
    configurarTerminalWindows();
    #endif
}

void limpar_tela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pausar() {
    printf(BLUE "\nPressione Enter para continuar..." RESET);
    limpar_buffer();
}

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
    printf(RED "7." BLUE " Editar Pedido\n" RESET);  
    printf(RED "8." BLUE " Simular Entrega\n" RESET);
    printf(RED "9." BLUE " Sair\n" RESET);
    printf(BOLD CYAN "=================================\n");
    printf(RESET BOLD "Escolha uma opção: " RESET);
}

void redimensionarVetor(void **array, int *capacidade, size_t tamanho_elemento) {
    int nova_capacidade = (*capacidade == 0) ? 1 : *capacidade * 2;
    void *novo = realloc(*array, nova_capacidade * tamanho_elemento);
    if (!novo) {
        printf(RED "\nErro ao alocar memória!\n" RESET);
        return;
    }
    *array = novo;
    *capacidade = nova_capacidade;
}

void mostrar_cardapio() {
    printf(BOLD CYAN "\n╔════╦══════════════════╦════════════════════════════════════════════════════════════════════╦════════════╗\n");
    printf("║ %-2s ║ %-16s ║ %-64s   ║ %-10s  ║\n", "Nº", "PIZZA", "INGREDIENTES", "PREÇO");
    printf("╠════╬══════════════════╬════════════════════════════════════════════════════════════════════╬════════════╣\n" RESET);
    
    for (int i = 0; i < total_pizzas; i++) {
        printf("║ %-2d ║ %-16s ║ %-67s ║ " GREEN "R$%-7.2f" RESET " ║\n", 
               i+1, 
               cardapio[i].nome, 
               cardapio[i].ingredientes, 
               cardapio[i].preco);
    }
    
    printf(BOLD CYAN "╚════╩══════════════════╩════════════════════════════════════════════════════════════════════╩════════════╝\n" RESET);
}
// Opção 01
//----------------------------------------------------------------------------------------------
void cadastrar_cliente() {
    limpar_tela();
    printf(BOLD CYAN "=== CADASTRAR CLIENTE ===\n\n" RESET);
    
    if (total_clientes >= capacidade_clientes) {
        redimensionarVetor((void **)&clientes, &capacidade_clientes, sizeof(Cliente));
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
// Opção 02
//----------------------------------------------------------------------------------------------
void listar_clientes() {
    limpar_tela();
    printf(BOLD CYAN "=== LISTA DE CLIENTES ===\n\n" RESET);
    
    if (total_clientes == 0) {
        printf(YELLOW "Nenhum cliente cadastrado.\n" RESET);
        pausar();
        return;
    }
    
    printf(BOLD WHITE "-------------------------------------------------\n");
    printf("| ID | NOME             | ENDEREÇO             |\n");
    printf("-------------------------------------------------\n" RESET);
    for (int i = 0; i < total_clientes; i++) {
        printf("| %-2d | %-15s  | %-20s |\n", 
               i+1, 
               clientes[i].nome, 
               clientes[i].endereco);
    }
    printf(BOLD WHITE "-------------------------------------------------\n" RESET);
    pausar();
}
// Opção 03
//----------------------------------------------------------------------------------------------
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
// Opção 04
//----------------------------------------------------------------------------------------------
void excluir_cliente() {
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
// Opção 05
//----------------------------------------------------------------------------------------------
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
        redimensionarVetor((void **)&pedidos, &capacidade_pedidos, sizeof(Pedido));
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
// Opção 06
//----------------------------------------------------------------------------------------------
void resumo_pedidos() {
    limpar_tela();
    printf(BOLD CYAN "=== RESUMO DE PEDIDOS ===\n\n" RESET);
    
    if (total_pedidos == 0) {
        printf(YELLOW "Nenhum pedido registrado.\n" RESET);
        pausar();
        return;
    }
    
    float total = 0;
    printf(BOLD WHITE "======================================================\n");
    printf("| ID | CLIENTE          | PIZZA            | VALOR    |\n");
    printf("------------------------------------------------------\n" RESET);
    for (int i = 0; i < total_pedidos; i++) {
        printf("| %-2d | %-16s | %-16s | " GREEN "R$%-6.2f" RESET " |\n", 
               i+1,
               pedidos[i].cliente, 
               pedidos[i].pizza, 
               pedidos[i].preco);
        total += pedidos[i].preco;
    }
    printf(BOLD WHITE "------------------------------------------------------\n");
    printf("| TOTAL                                    | " GREEN "R$%-6.2f" WHITE " |\n", total);
    printf("======================================================\n" RESET);
    
    pausar();
}
// Opção 07
//----------------------------------------------------------------------------------------------
void editar_pedido() {
    limpar_tela();
    printf(BOLD CYAN "=== EDITAR PEDIDO ===\n\n" RESET);
    
    if (total_pedidos == 0) {
        printf(YELLOW "Nenhum pedido registrado.\n" RESET);
        pausar();
        return;
    }
    
    resumo_pedidos();
    
    int id;
    printf(BLUE "\nDigite o ID do pedido que deseja editar: " RESET);
    if (scanf("%d", &id) != 1) {
        printf(RED "\nID inválido!\n" RESET);
        limpar_buffer();
        pausar();
        return;
    }
    limpar_buffer();
    
    if (id < 1 || id > total_pedidos) {
        printf(RED "\nID inválido!\n" RESET);
        pausar();
        return;
    }
    
    int index = id - 1;
    
    printf(BLUE "\nEditando pedido:\n" RESET);
    printf(BLUE "Cliente atual: %s\n" RESET, pedidos[index].cliente);
    printf(BLUE "Pizza atual: %s\n" RESET, pedidos[index].pizza);
    printf(BLUE "Preço atual: " GREEN "R$%.2f\n\n" RESET, pedidos[index].preco);
    
    // Mostrar clientes disponíveis
    printf(BOLD CYAN "=== CLIENTES DISPONÍVEIS ===\n" RESET);
    for (int i = 0; i < total_clientes; i++) {
        printf(BLUE "%d. %s\n" RESET, i+1, clientes[i].nome);
    }
    
    int novo_cliente;
    printf(BLUE "\nEscolha o novo cliente (0 para manter o atual): " RESET);
    if (scanf("%d", &novo_cliente) != 1) {
        printf(RED "\nEntrada inválida!\n" RESET);
        limpar_buffer();
        pausar();
        return;
    }
    limpar_buffer();
    
    if (novo_cliente > 0) {
        if (novo_cliente < 1 || novo_cliente > total_clientes) {
            printf(RED "\nCliente inválido!\n" RESET);
            pausar();
            return;
        }
        strncpy(pedidos[index].cliente, clientes[novo_cliente-1].nome, 50);
    }
    
    mostrar_cardapio();
    
    int nova_pizza;
    printf(BLUE "\nEscolha a nova pizza (0 para manter a atual): " RESET);
    if (scanf("%d", &nova_pizza) != 1) {
        printf(RED "\nEntrada inválida!\n" RESET);
        limpar_buffer();
        pausar();
        return;
    }
    limpar_buffer();
    
    if (nova_pizza > 0) {
        if (nova_pizza < 1 || nova_pizza > total_pizzas) {
            printf(RED "\nPizza inválida!\n" RESET);
            pausar();
            return;
        }
        strncpy(pedidos[index].pizza, cardapio[nova_pizza-1].nome, 50);
        pedidos[index].preco = cardapio[nova_pizza-1].preco;
    }
    
    printf(GREEN "\nPedido editado com sucesso!\n" RESET);
    printf(BLUE "-> Novo pedido: %s para %s - " GREEN "R$%.2f\n" RESET, 
           pedidos[index].pizza, 
           pedidos[index].cliente, 
           pedidos[index].preco);
    pausar();
}
// Opção 08
//----------------------------------------------------------------------------------------------
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

int main() {
    configurar_ambiente();
    
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
        
        switch(opcao) {
            case 1: cadastrar_cliente(); break;
            case 2: listar_clientes(); break;
            case 3: editar_cliente(); break;
            case 4: excluir_cliente(); break;
            case 5: fazer_pedido(); break;
            case 6: resumo_pedidos(); break;
            case 7: editar_pedido(); break;  
            case 8: simular_entrega(); break;
            case 9: printf(BLUE "\nSaindo do sistema...\n" RESET); break;
            default: printf(RED "\nOpção inválida! Tente novamente.\n" RESET); pausar(); break;
        }
    } while(opcao != 9);
    
    liberar_memoria();
    return 0;
}
