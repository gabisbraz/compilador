#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Lista de palavras reservadas
const char *palavrasReservadas[] = {
    "program", "begin", "end", 
    "if", "elif", "for", 
    "read", "write", "set", 
    "to", "of", "integer", 
    "boolean"
};

// Enum para os diferentes tipos de elementos que podem ser identificado
typedef enum {
    elem_var,
    elem_num,
    elem_reservada,
    elem_ponto_virgula,
    elem_abertura,
    elem_fechamento,
    elem_coment,
    elem_op_relacional,
    elem_op_soma,        
    elem_op_mult,        
    elem_virgula,        
    elem_2_pontos,    
    elem_ponto,    
    elem_end,       
} TipoElemento;

// Impressão correspondente para cada tipo de elemento
const char *print_elemento[] = {
    "identificador",
    "numero",
    "palavra_reservada",
    ";",
    "(",
    ")",
    "comentario",
    "op_relacional",
    "op_soma",
    "op_mult",
    ",",
    ":",
    ".",
    "end",
};

// Estrutura que representa um elemento analisado
typedef struct {
    TipoElemento tipo;      
    int linha;              // Linha onde o elemento foi encontrado
    float valorNumerico;   
    char identificador[16];
} AnalisaElemento;

int contadorLinha = 1; // Contador de linhas inicializado em 1
char bufferFonte[600]; // Buffer para armazenar o código fonte
char *ponteiroAtualChar; // Ponteiro para percorrer o buffer

AnalisaElemento prox_elemento;
AnalisaElemento identifica_num();
AnalisaElemento obtem_proximo_elemento();
AnalisaElemento identifica_simb();
AnalisaElemento identifica_coment();
AnalisaElemento identifica_var();
AnalisaElemento identifica_op_relacional();

void analisa_bloco();
void analisa_comando();
void analisa_programa();
void analisa_expressao();
void analisa_comando_if();
void analisa_comando_for();
void analisa_comando_read();
void analisa_comando_write();
void analisa_comando_composto();
void analisa_declaracao_variaveis();
void raise_erro(const char *esperado, const char *encontrado, int linha);

/**
 * Exibe os comentários encontrados no código, enquanto houver.
 */
void exibir_comentarios() {
    while (prox_elemento.tipo == elem_coment) {
        printf("# %d: comentario\n", prox_elemento.linha);  // Exibe o comentário
        prox_elemento = obtem_proximo_elemento();  // Avança para o próximo elemento
    }
}

/**
 * Exibe o elemento atual sendo analisado, com base no seu tipo.
 */
void exibir_elemento() {
    if (prox_elemento.tipo == elem_var) {
        printf("# %d: %s | %s\n", prox_elemento.linha, print_elemento[prox_elemento.tipo], prox_elemento.identificador);
    } else if (prox_elemento.tipo == elem_reservada) {
        printf("# %d: %s\n", prox_elemento.linha, prox_elemento.identificador);  // Exibe o nome da palavra reservada
    } else if (prox_elemento.tipo == elem_num) {
        printf("# %d: %s | %f\n", prox_elemento.linha, print_elemento[prox_elemento.tipo], prox_elemento.valorNumerico);
    } else {
        printf("# %d: %s\n", prox_elemento.linha, print_elemento[prox_elemento.tipo]);
    }
}

/**
 * Consome o elemento esperado, exibindo-o e verificando se está correto.
 * 
 * @param esperado Tipo do elemento que é esperado.
 */
void consome_elemento(int esperado) {
    exibir_comentarios(); // Exibe comentários antes do elemento esperado

    if (prox_elemento.tipo == esperado) {
        exibir_elemento(); // Exibe o elemento esperado
        prox_elemento = obtem_proximo_elemento();  // Avança para o próximo elemento
    } else {
        raise_erro(print_elemento[esperado], print_elemento[prox_elemento.tipo], prox_elemento.linha);
        exit(1);  // Encerra o programa em caso de erro
    }
}

/**
 * Inicia a análise do programa, consumindo os elementos iniciais.
 */
void analisa_programa() {
    consome_elemento(elem_reservada);  // "program"
    consome_elemento(elem_var);      // Nome do programa
    consome_elemento(elem_ponto_virgula);      // ;
    analisa_bloco();                         // <bloco>
    consome_elemento(elem_ponto);        // Consome o ponto final (.)
}

/**
 * Analisa o bloco de código, incluindo variáveis e comandos.
 */
void analisa_bloco() {
    analisa_declaracao_variaveis();
    analisa_comando_composto();
}

/**
 * Analisa a declaração de variáveis, que pode incluir múltiplas variáveis.
 */
void analisa_declaracao_variaveis() {
    while (prox_elemento.tipo == elem_reservada && 
          (strcmp(prox_elemento.identificador, "integer") == 0 || strcmp(prox_elemento.identificador, "boolean") == 0)) {
        consome_elemento(elem_reservada);  // "integer" ou "boolean"
        consome_elemento(elem_var);      // Nome da variável
        while (prox_elemento.tipo == elem_virgula) {
            consome_elemento(elem_virgula);        // Consome vírgula
            consome_elemento(elem_var);  // Mais identificadores
        }
        consome_elemento(elem_ponto_virgula);      // ;
    }
}

/**
 * Analisa um bloco de comandos, que começa com "begin" e termina com "end".
 */
void analisa_comando_composto() {
    consome_elemento(elem_reservada);  // "begin"
    analisa_comando();
    while (prox_elemento.tipo == elem_ponto_virgula) {
        consome_elemento(elem_ponto_virgula);  // ;
        analisa_comando();
    }
    consome_elemento(elem_reservada);  // "end"
}

/**
 * Analisa um comando, verificando seu tipo.
 */
void analisa_comando() {
    if (strcmp(prox_elemento.identificador, "set") == 0) {
        consome_elemento(elem_reservada);  // "set"
        consome_elemento(elem_var);      // Nome da variável
        consome_elemento(elem_reservada);  // "to"
        analisa_expressao();                     // <expressao>
    } else if (strcmp(prox_elemento.identificador, "if") == 0) {
        analisa_comando_if(); // Função auxiliar para o comando "if"
    } else if (strcmp(prox_elemento.identificador, "for") == 0) {
        analisa_comando_for(); // Função auxiliar para o comando "for"
    } else if (strcmp(prox_elemento.identificador, "read") == 0) {
        analisa_comando_read(); // Função auxiliar para o comando "read"
    } else if (strcmp(prox_elemento.identificador, "write") == 0) {
        analisa_comando_write(); // Função auxiliar para o comando "write"
    }
}

/**
 * Analisa um comando "if", incluindo sua condição e bloco de comandos.
 */
void analisa_comando_if() {
    consome_elemento(elem_reservada);  // "if"
    analisa_expressao();                     // <expressao>
    consome_elemento(elem_2_pontos);        // ":"
    analisa_comando();                       // <comando>
    if (strcmp(prox_elemento.identificador, "elif") == 0) {
        consome_elemento(elem_reservada);  // "elif"
        analisa_comando();                       // <comando>
    }
}

/**
 * Analisa um comando "for", incluindo o iterador e os limites.
 */
void analisa_comando_for() {
    consome_elemento(elem_reservada);  // "for"
    consome_elemento(elem_var);      // Identificador
    consome_elemento(elem_reservada);  // "of"
    analisa_expressao();                     // <expressao>
    consome_elemento(elem_reservada);  // "to"
    analisa_expressao();                     // <expressao>
    consome_elemento(elem_2_pontos);        // ":"
    analisa_comando();                       // <comando>
}

/**
 * Analisa um comando "read", incluindo a leitura de múltiplas variáveis.
 */
void analisa_comando_read() {
    consome_elemento(elem_reservada);  // "read"
    consome_elemento(elem_abertura);           // (
    consome_elemento(elem_var);      // Identificador
    while (prox_elemento.tipo == elem_virgula) {
        consome_elemento(elem_virgula);        // Consome a vírgula
        consome_elemento(elem_var);  // Próximo identificador
    }
    consome_elemento(elem_fechamento);          // )
}

/**
 * Analisa um comando "write", incluindo a impressão de múltiplas expressões.
 */
void analisa_comando_write() {
    consome_elemento(elem_reservada);  // "write"
    consome_elemento(elem_abertura);           // (
    analisa_expressao();                     // <expressao>
    while (prox_elemento.tipo == elem_virgula) {
        consome_elemento(elem_virgula);        // Consome a vírgula
        analisa_expressao();                 // Próxima expressão
    }
    consome_elemento(elem_fechamento);          // )
}

/**
 * Analisa uma expressão, que pode ser uma operação ou um identificador/número.
 */
void analisa_expressao() {
    // Implementação simplificada de análise de expressão
    consome_elemento(elem_var);  // Exemplo de elemento esperado
}

/**
 * Gera um erro, exibindo o que era esperado e o que foi encontrado.
 * 
 * @param esperado O que se esperava encontrar.
 * @param encontrado O que foi realmente encontrado.
 * @param linha A linha em que o erro ocorreu.
 */
void raise_erro(const char *esperado, const char *encontrado, int linha) {
    printf("Erro sintatico na linha %d: esperado '%s', encontrado '%s'.\n", linha, esperado, encontrado);
}

/**
 * Identifica o próximo elemento e atualiza o estado.
 * 
 * @return O próximo elemento identificado.
 */
void ignorar_espacos() {
    while (*ponteiroAtualChar == ' ' || *ponteiroAtualChar == '\t' || 
           *ponteiroAtualChar == '\n' || *ponteiroAtualChar == '\r') {
        if (*ponteiroAtualChar == '\n') {
            contadorLinha++;
        }
        ponteiroAtualChar++;
    }
}

/**
 * Identifica e retorna um número.
 * 
 * @return Um objeto AnalisaElemento representando um número.
 */
int verifica_fim_arquivo(AnalisaElemento* elemento) {
    if (*ponteiroAtualChar == '\0') {
        elemento->tipo = elem_end;
        return 1; // Indica que é o fim do arquivo
    }
    return 0; // Não é o fim do arquivo
}

/**
 * Identifica e retorna um símbolo.
 * 
 * @return Um objeto AnalisaElemento representando um símbolo.
 */
AnalisaElemento identifica_elemento() {
    AnalisaElemento elemento;
    elemento.linha = contadorLinha;
    elemento.tipo = elem_end; // Inicializa como erro

    if (isalpha(*ponteiroAtualChar)) {
        return identifica_var(); // Identifica variáveis ou palavras reservadas
    }

    if (isdigit(*ponteiroAtualChar)) {
        return identifica_num(); // Identifica números
    }

    return identifica_simb(); // Identifica operadores ou símbolos
}

/**
 * Identifica o próximo elemento e atualiza o estado.
 * 
 * @return O próximo elemento identificado.
 */
AnalisaElemento obtem_proximo_elemento() {
    AnalisaElemento elemento;
    ignorar_espacos(); // Ignora espaços em branco

    if (verifica_fim_arquivo(&elemento)) {
        return elemento; // Retorna se for o fim do arquivo
    }

    return identifica_elemento(); // Identifica o próximo elemento
}

/**
 * Identifica e retorna uma variável.
 * 
 * @return Um objeto AnalisaElemento representando uma variável.
 */
AnalisaElemento identifica_var() {
    AnalisaElemento elemento;
    elemento.tipo = elem_var;
    elemento.linha = contadorLinha;

    int i = 0;
    while (isalnum(*ponteiroAtualChar) || *ponteiroAtualChar == '_') {
        if (i < 15) {
            elemento.identificador[i++] = *ponteiroAtualChar;
        }
        ponteiroAtualChar++;
    }
    elemento.identificador[i] = '\0';

    // Verificar se o identificador é uma palavra reservada
    size_t quantidadeReservadas = sizeof(palavrasReservadas) / sizeof(palavrasReservadas[0]);
    
    for (size_t j = 0; j < quantidadeReservadas; j++) {
        if (strcmp(elemento.identificador, palavrasReservadas[j]) == 0) {
            elemento.tipo = elem_reservada;
            break;
        }
    }

    return elemento;
}

/**
 * Identifica e retorna um número.
 * 
 * @return Um objeto AnalisaElemento representando um número.
 */
AnalisaElemento identifica_num() {
    AnalisaElemento elemento;
    elemento.tipo = elem_num;
    elemento.linha = contadorLinha;

    char numeroStr[16];
    int i = 0;
    while (isdigit(*ponteiroAtualChar) || *ponteiroAtualChar == '.') {
        if (i < 15) {
            numeroStr[i++] = *ponteiroAtualChar;
        }
        ponteiroAtualChar++;
    }
    numeroStr[i] = '\0';
    elemento.valorNumerico = atof(numeroStr);  // Converte string para número float

    return elemento;
}

/**
 * Identifica e retorna um símbolo.
 * 
 * @return Um objeto AnalisaElemento representando um símbolo.
 */
void iniciar_elemento_simb(AnalisaElemento* elemento) {
    elemento->linha = contadorLinha;
}

/**
 * Processa um símbolo unário (como ponto e vírgula).
 * 
 * @param elemento O ponteiro para o elemento a ser atualizado.
 */
void processar_simbolo_unario(AnalisaElemento* elemento) {
    if (*ponteiroAtualChar == ';') {
        elemento->tipo = elem_ponto_virgula;
    } else if (*ponteiroAtualChar == ',') {
        elemento->tipo = elem_virgula;
    } else if (*ponteiroAtualChar == '(') {
        elemento->tipo = elem_abertura;
    } else if (*ponteiroAtualChar == ')') {
        elemento->tipo = elem_fechamento;
    } else if (*ponteiroAtualChar == '.') {
        elemento->tipo = elem_ponto;
    } else {
        elemento->tipo = elem_end; // Caractere não reconhecido
    }
    ponteiroAtualChar++; // Avança o ponteiro
}

/**
 * Processa o símbolo de dois pontos, identificando se é um operador relacional.
 * 
 * @param elemento O ponteiro para o elemento a ser atualizado.
 */
void processar_dois_pontos(AnalisaElemento* elemento) {
    if (*(ponteiroAtualChar + 1) == '=') {
        elemento->tipo = elem_op_relacional; // :=
        ponteiroAtualChar += 2; // Avança dois caracteres
    } else {
        elemento->tipo = elem_2_pontos;
        ponteiroAtualChar++; // Avança um caractere
    }
}

/**
 * Processa operadores de soma (+) e subtração (-).
 * 
 * @param elemento O ponteiro para o elemento a ser atualizado.
 */
void processar_op_soma_mult(AnalisaElemento* elemento) {
    if (*ponteiroAtualChar == '+') {
        elemento->tipo = elem_op_soma;
    } else if (*ponteiroAtualChar == '-') {
        elemento->tipo = elem_op_soma; // Considera ambos como soma
    }
    ponteiroAtualChar++;
}

/**
 * Identifica um símbolo com base no caractere atual.
 * 
 * @return O elemento identificado.
 */
AnalisaElemento identifica_simb() {
    AnalisaElemento elemento;
    iniciar_elemento_simb(&elemento);

    if (*ponteiroAtualChar == ';' || *ponteiroAtualChar == ',' || *ponteiroAtualChar == '(' || 
        *ponteiroAtualChar == ')' || *ponteiroAtualChar == '.') {
        processar_simbolo_unario(&elemento);
    } else if (*ponteiroAtualChar == ':') {
        processar_dois_pontos(&elemento);
    } else if (*ponteiroAtualChar == '+' || *ponteiroAtualChar == '-') {
        processar_op_soma_mult(&elemento);
    } else if (*ponteiroAtualChar == '*') {
        elemento.tipo = elem_op_mult;
        ponteiroAtualChar++;
    } else if (*ponteiroAtualChar == '/') {
        elemento.tipo = elem_op_mult; // Se preferir pode criar uma função específica para operador de divisão
        ponteiroAtualChar++;
    } else if (*ponteiroAtualChar == '>' || *ponteiroAtualChar == '<') {
        return identifica_op_relacional();
    } else if (*ponteiroAtualChar == '{') {
        return identifica_coment();
    } else {
        elemento.tipo = elem_end;
        ponteiroAtualChar++; // Ignora caractere não reconhecido
    }

    return elemento;
}

/**
 * Inicializa um elemento genérico.
 * 
 * @param elemento O ponteiro para o elemento a ser inicializado.
 */
void iniciar_elemento(AnalisaElemento* elemento) {
    elemento->tipo = elem_coment;
    elemento->linha = contadorLinha;
}

/**
 * Processa um símbolo maior (>) e opcionalmente verifica se é maior ou igual.
 */
void processar_maior() {
    ponteiroAtualChar++;  // Ignora o '>'
    if (*ponteiroAtualChar == '=') {
        ponteiroAtualChar++;  // Ignora o '='
    }
}

/**
 * Processa um símbolo menor (<) e opcionalmente verifica se é menor ou igual.
 */
void processar_menor() {
    ponteiroAtualChar++;  // Ignora o '<'
    if (*ponteiroAtualChar == '=') {
        ponteiroAtualChar++;  // Ignora o '='
    }
}

/**
 * Identifica e retorna um operador relacional.
 * 
 * @return Um objeto AnalisaElemento representando um operador relacional.
 */
AnalisaElemento identifica_op_relacional() {
    AnalisaElemento elemento;
    iniciar_elemento(&elemento);

    if (*ponteiroAtualChar == '>') {
        processar_maior();
    } else if (*ponteiroAtualChar == '<') {
        processar_menor();
    }

    return elemento;
}

/**
 * Ignora o símbolo de abertura de comentário.
 */
void ignorar_abertura() {
    ponteiroAtualChar++;  // Ignora o '{'
}

/**
 * Conta o número de linhas entre a abertura e o fechamento de um comentário.
 * 
 * Esta função percorre o buffer até encontrar o fechamento '}' ou o fim do buffer
 * e incrementa o contador de linhas sempre que encontra um '\n'.
 */
void contar_linhas() {
    while (*ponteiroAtualChar != '}' && *ponteiroAtualChar != '\0') {
        if (*ponteiroAtualChar == '\n') {
            contadorLinha++;
        }
        ponteiroAtualChar++;
    }
}

/**
 * Ignora o símbolo de fechamento de comentário.
 * 
 * @param elemento O ponteiro para o elemento que será atualizado.
 * @return 1 em caso de sucesso, 0 em caso de erro (comentário não fechado).
 */
int ignorar_fechamento(AnalisaElemento* elemento) {
    if (*ponteiroAtualChar == '}') {
        ponteiroAtualChar++;  // Ignora o '}'
        return 1;  // Sucesso
    } else {
        elemento->tipo = elem_end;  // Erro: Comentário não fechado
        return 0;  // Erro
    }
}

/**
 * Identifica um comentário, contando as linhas e verificando o fechamento.
 * 
 * @return O elemento do comentário identificado.
 */
AnalisaElemento identifica_coment() {
    AnalisaElemento elemento;
    iniciar_elemento(&elemento);
    ignorar_abertura();
    contar_linhas();
    ignorar_fechamento(&elemento);

    return elemento;
}

/**
 * Abre um arquivo para leitura.
 * 
 * @param nome_arquivo O nome do arquivo a ser aberto.
 * @return Um ponteiro para o arquivo aberto, ou NULL se houver um erro.
 */

FILE *abrir_arquivo(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo: %s\n", nome_arquivo);
        return NULL;
    }
    return arquivo;
}

/**
 * Lê o conteúdo de um arquivo para um buffer.
 * 
 * @param arquivo O ponteiro para o arquivo a ser lido.
 * @return O número de bytes lidos.
 */
size_t ler_arquivo(FILE *arquivo) {
    size_t bytesLidos = fread(bufferFonte, 1, 600 - 1, arquivo);
    bufferFonte[bytesLidos] = '\0';  // Garante que o buffer seja uma string válida
    ponteiroAtualChar = bufferFonte;  // Inicializa o ponteiro para o buffer
    return bytesLidos;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_fonte>\n", argv[0]);
        return 1;
    }

    FILE *arquivo = abrir_arquivo(argv[1]);
    if (!arquivo) {
        printf("Erro ao abrir o arquivo: %s\n", "teste1.pas");
        return 1;
    }

    ler_arquivo(arquivo);

    prox_elemento = obtem_proximo_elemento();  // Obtém o primeiro elemento
    analisa_programa();                   // Inicia a análise sintática

    return 0;
}