#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define TAMANHO_MAXIMO_BUFFER 200

char bufferFonte[TAMANHO_MAXIMO_BUFFER]; 
char *ponteiroAtualChar;

typedef enum {
    ELEM_ERRO,
    ELEM_IDENTIFICADOR,
    ELEM_NUMERO,
    ELEM_PALAVRA_RESERVADA,
    ELEM_PONTO_VIRGULA,
    ELEM_ABRE_PAR,
    ELEM_FECHA_PAR,
    ELEM_COMENTARIO,
    ELEM_OP_RELACIONAL,  // Operador relacional (>, <, >=, <=, etc.)
    ELEM_OP_SOMA,        // Operadores de soma e subtração (+, -)
    ELEM_OP_MULT,        // Operadores de multiplicação e divisão (*, /)
    ELEM_VIRGULA,        // Operador de vírgula (,)
    ELEM_DOIS_PONTOS,    // Operador ":"
    ELEM_PONTO_FINAL,    // Ponto final "."
    ELEM_EOF             // Fim do arquivo
} TipoElemento;

typedef struct {
    TipoElemento tipo;
    int linha;
    float valorNumerico;
    char identificador[16];
} AnalisaElemento;

const char *print_elemento[] = {
    "erro",
    "identificador",
    "numero",
    "palavra_reservada",
    "ponto_virgula",
    "abre_parenteses",
    "fecha_parenteses",
    "comentario",
    "op_relacional",
    "op_soma",
    "op_mult",
    "virgula",
    "dois_pontos",
    "ponto_final",
    "fim_do_arquivo"
};

const char *palavrasReservadas[] = {"program", "begin", "end", "if", "elif", "for", "read", "write", "set", "to", "of", "integer", "boolean"};

int contadorLinha = 1;

AnalisaElemento prox_elemento;
AnalisaElemento identificar_numero();
AnalisaElemento obter_proximo_token();
AnalisaElemento identificar_simbolo();
AnalisaElemento identificar_comentario();
AnalisaElemento identificar_identificador();
AnalisaElemento identificar_operador_relacional();

void analisar_bloco();
void analisar_comando();
void analisar_programa();
void analisar_expressao();
void analisar_comando_composto();
void analisar_declaracao_variaveis();
void consumir_token(TipoElemento esperado);
void reportar_erro_sintatico(const char *esperado, const char *encontrado, int linha);

void consumir_token(TipoElemento esperado) {
    while (prox_elemento.tipo == ELEM_COMENTARIO) {
        printf("# %d: comentario\n", prox_elemento.linha);  // Exibe o comentário
        prox_elemento = obter_proximo_token();  // Avança para o próximo elemento
    }

    if (prox_elemento.tipo == esperado) {
        if (prox_elemento.tipo == ELEM_IDENTIFICADOR) {
            printf("# %d: %s | %s\n", prox_elemento.linha, print_elemento[prox_elemento.tipo], prox_elemento.identificador);
        } else if (prox_elemento.tipo == ELEM_PALAVRA_RESERVADA) {
            printf("# %d: %s\n", prox_elemento.linha, prox_elemento.identificador);  // Exibe o nome da palavra reservada
        } else if (prox_elemento.tipo == ELEM_NUMERO) {
            printf("# %d: %s | %f\n", prox_elemento.linha, print_elemento[prox_elemento.tipo], prox_elemento.valorNumerico);
        } else {
            printf("# %d: %s\n", prox_elemento.linha, print_elemento[prox_elemento.tipo]);
        }
        prox_elemento = obter_proximo_token();  // Avança para o próximo elemento
    } else {
        reportar_erro_sintatico(print_elemento[esperado], print_elemento[prox_elemento.tipo], prox_elemento.linha);
        exit(1);  // Encerra o programa em caso de erro
    }
}

void analisar_programa() {
    consumir_token(ELEM_PALAVRA_RESERVADA);  // "program"
    consumir_token(ELEM_IDENTIFICADOR);      // Nome do programa
    consumir_token(ELEM_PONTO_VIRGULA);      // ;
    analisar_bloco();                         // <bloco>
    consumir_token(ELEM_PONTO_FINAL);        // Consome o ponto final (.)
}

void analisar_bloco() {
    analisar_declaracao_variaveis();
    analisar_comando_composto();
}

void analisar_declaracao_variaveis() {
    while (prox_elemento.tipo == ELEM_PALAVRA_RESERVADA && 
          (strcmp(prox_elemento.identificador, "integer") == 0 || strcmp(prox_elemento.identificador, "boolean") == 0)) {
        consumir_token(ELEM_PALAVRA_RESERVADA);  // "integer" ou "boolean"
        consumir_token(ELEM_IDENTIFICADOR);      // Nome da variável
        while (prox_elemento.tipo == ELEM_VIRGULA) {
            consumir_token(ELEM_VIRGULA);        // Consome vírgula
            consumir_token(ELEM_IDENTIFICADOR);  // Mais identificadores
        }
        consumir_token(ELEM_PONTO_VIRGULA);      // ;
    }
}

void analisar_comando_composto() {
    consumir_token(ELEM_PALAVRA_RESERVADA);  // "begin"
    analisar_comando();
    while (prox_elemento.tipo == ELEM_PONTO_VIRGULA) {
        consumir_token(ELEM_PONTO_VIRGULA);  // ;
        analisar_comando();
    }
    consumir_token(ELEM_PALAVRA_RESERVADA);  // "end"
}

void analisar_comando() {
    if (strcmp(prox_elemento.identificador, "set") == 0) {
        consumir_token(ELEM_PALAVRA_RESERVADA);  // "set"
        consumir_token(ELEM_IDENTIFICADOR);      // Nome da variável
        consumir_token(ELEM_PALAVRA_RESERVADA);  // "to"
        analisar_expressao();
    } else if (strcmp(prox_elemento.identificador, "if") == 0) {
        consumir_token(ELEM_PALAVRA_RESERVADA);  // "if"
        analisar_expressao();                     // <expressao>
        consumir_token(ELEM_DOIS_PONTOS);        // ":"
        analisar_comando();                       // <comando>
        if (strcmp(prox_elemento.identificador, "elif") == 0) {
            consumir_token(ELEM_PALAVRA_RESERVADA);  // "elif"
            analisar_comando();                       // <comando>
        }
    } else if (strcmp(prox_elemento.identificador, "for") == 0) {
        consumir_token(ELEM_PALAVRA_RESERVADA);  // "for"
        consumir_token(ELEM_IDENTIFICADOR);      // Identificador
        consumir_token(ELEM_PALAVRA_RESERVADA);  // "of"
        analisar_expressao();                     // <expressao>
        consumir_token(ELEM_PALAVRA_RESERVADA);  // "to"
        analisar_expressao();                     // <expressao>
        consumir_token(ELEM_DOIS_PONTOS);        // ":"
        analisar_comando();                       // <comando>
    } else if (strcmp(prox_elemento.identificador, "read") == 0) {
        consumir_token(ELEM_PALAVRA_RESERVADA);  // "read"
        consumir_token(ELEM_ABRE_PAR);           // (
        consumir_token(ELEM_IDENTIFICADOR);      // Identificador
        while (prox_elemento.tipo == ELEM_VIRGULA) {
            consumir_token(ELEM_VIRGULA);        // Consome a vírgula
            consumir_token(ELEM_IDENTIFICADOR);  // Próximo identificador
        }
        consumir_token(ELEM_FECHA_PAR);          // )
    } else if (strcmp(prox_elemento.identificador, "write") == 0) {
        consumir_token(ELEM_PALAVRA_RESERVADA);  // "write"
        consumir_token(ELEM_ABRE_PAR);           // (
        analisar_expressao();                     // <expressao>
        while (prox_elemento.tipo == ELEM_VIRGULA) {
            consumir_token(ELEM_VIRGULA);        // Consome a vírgula
            analisar_expressao();                 // Próxima expressão
        }
        consumir_token(ELEM_FECHA_PAR);          // )
    }
}

void analisar_expressao() {
    // Implementação simplificada de análise de expressão
    consumir_token(ELEM_IDENTIFICADOR);  // Exemplo de elemento esperado
}

void reportar_erro_sintatico(const char *esperado, const char *encontrado, int linha) {
    printf("Erro sintatico na linha %d: esperado '%s', encontrado '%s'.\n", linha, esperado, encontrado);
}

AnalisaElemento obter_proximo_token() {
    AnalisaElemento elemento;
    elemento.linha = contadorLinha;
    elemento.tipo = ELEM_ERRO;

    // Ignorar espaços em branco, quebras de linha e tabulações
    while (*ponteiroAtualChar == ' ' || *ponteiroAtualChar == '\t' || *ponteiroAtualChar == '\n' || *ponteiroAtualChar == '\r') {
        if (*ponteiroAtualChar == '\n') {
            contadorLinha++;
        }
        ponteiroAtualChar++;
    }

    if (*ponteiroAtualChar == '\0') {
        elemento.tipo = ELEM_EOF;
        return elemento;  // Fim do arquivo
    }

    // Identifica palavras reservadas ou identificadores
    if (isalpha(*ponteiroAtualChar)) {
        return identificar_identificador();
    }

    // Identifica números
    if (isdigit(*ponteiroAtualChar)) {
        return identificar_numero();
    }

    // Identifica operadores ou símbolos
    return identificar_simbolo();
}

AnalisaElemento identificar_identificador() {
    AnalisaElemento elemento;
    elemento.tipo = ELEM_IDENTIFICADOR;
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
            elemento.tipo = ELEM_PALAVRA_RESERVADA;
            break;
        }
    }

    return elemento;
}

AnalisaElemento identificar_numero() {
    AnalisaElemento elemento;
    elemento.tipo = ELEM_NUMERO;
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

AnalisaElemento identificar_simbolo() {
    AnalisaElemento elemento;
    elemento.linha = contadorLinha;

    switch (*ponteiroAtualChar) {
        case ';':
            elemento.tipo = ELEM_PONTO_VIRGULA;
            ponteiroAtualChar++;
            break;
        case ',':
            elemento.tipo = ELEM_VIRGULA;
            ponteiroAtualChar++;
            break;
        case '(':
            elemento.tipo = ELEM_ABRE_PAR;
            ponteiroAtualChar++;
            break;
        case ')':
            elemento.tipo = ELEM_FECHA_PAR;
            ponteiroAtualChar++;
            break;
        case ':':
            if (*(ponteiroAtualChar + 1) == '=') {
                elemento.tipo = ELEM_OP_RELACIONAL;  // :=
                ponteiroAtualChar += 2;
            } else {
                elemento.tipo = ELEM_DOIS_PONTOS;
                ponteiroAtualChar++;
            }
            break;
        case '.':
            elemento.tipo = ELEM_PONTO_FINAL;
            ponteiroAtualChar++;
            break;
        case '+':
        case '-':
            elemento.tipo = ELEM_OP_SOMA;
            ponteiroAtualChar++;
            break;
        case '*':
        case '/':
            elemento.tipo = ELEM_OP_MULT;
            ponteiroAtualChar++;
            break;
        case '>':
        case '<':
            return identificar_operador_relacional();
        case '{':
            return identificar_comentario();
        default:
            elemento.tipo = ELEM_ERRO;
            ponteiroAtualChar++;  // Avança para ignorar caractere não reconhecido
    }

    return elemento;
}

AnalisaElemento identificar_operador_relacional() {
    AnalisaElemento elemento;
    elemento.tipo = ELEM_OP_RELACIONAL;
    elemento.linha = contadorLinha;

    if (*ponteiroAtualChar == '>') {
        ponteiroAtualChar++;
        if (*ponteiroAtualChar == '=') {
            ponteiroAtualChar++;
        }
    } else if (*ponteiroAtualChar == '<') {
        ponteiroAtualChar++;
        if (*ponteiroAtualChar == '=') {
            ponteiroAtualChar++;
        }
    }

    return elemento;
}

AnalisaElemento identificar_comentario() {
    AnalisaElemento elemento;
    elemento.tipo = ELEM_COMENTARIO;
    elemento.linha = contadorLinha;

    ponteiroAtualChar++;  // Ignora o '{'

    while (*ponteiroAtualChar != '}' && *ponteiroAtualChar != '\0') {
        if (*ponteiroAtualChar == '\n') {
            contadorLinha++;
        }
        ponteiroAtualChar++;
    }

    if (*ponteiroAtualChar == '}') {
        ponteiroAtualChar++;  // Ignora o '}'
    } else {
        elemento.tipo = ELEM_ERRO;  // Erro: Comentário não fechado
    }

    return elemento;
}

int main(int argc, char *argv[]) {
    // if (argc < 2) {
    //     printf("Uso: %s <arquivo_fonte>\n", argv[0]);
    //     return 1;
    // }

    FILE *arquivo = fopen("teste2.pas", "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo: %s\n", "teste1.pas");
        return 1;
    }

    size_t bytesLidos = fread(bufferFonte, 1, TAMANHO_MAXIMO_BUFFER - 1, arquivo);
    bufferFonte[bytesLidos] = '\0';  // Garante que o buffer seja uma string válida
    ponteiroAtualChar = bufferFonte;  // Inicializa o ponteiro para o buffer

    prox_elemento = obter_proximo_token();  // Obtém o primeiro elemento
    analisar_programa();                   // Inicia a análise sintática

    return 0;
}