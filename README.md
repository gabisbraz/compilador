# README - Analisador Sintático em C

## Estrutura do Código

### Definições e Tipos

O código começa com a definição de constantes e tipos de dados. Um buffer de caracteres (bufferFonte) é criado para armazenar o conteúdo do arquivo fonte. Uma enumeração chamada TipoElemento categoriza os diferentes tipos de elementos que o analisador pode identificar, como variáveis, números, palavras reservadas e símbolos.

### Estruturas de Dados

Um struct chamado AnalisaElemento armazena informações sobre os elementos identificados, incluindo seu tipo, linha encontrada e seu valor numérico ou identificador.

#### Análise da Sintaxe
O núcleo do analisador sintático verifica se o código segue as regras gramaticais da linguagem Pascal. Isso é feito utilizando uma gramática, que define as regras de formação das sentenças da linguagem. O analisador percorre o código e, com base na gramática, identifica se a sequência de elementos está correta. Se a sequência não corresponder a essa regra, o analisador irá gerar um erro de sintaxe.

### Identificação de Elementos
Para identificar os diferentes elementos do código, o analisador utiliza funções que verificam cada caractere e agrupam caracteres em espaços de elementos, que são as unidades básicas de significado. Os espaços de elementos são então classificados com base em seu tipo, e essa classificação é armazenada na estrutura de dados mencionada anteriormente.

### Verificação de Erros
Quando um erro é encontrado, o analisador imprime mensagens que indicam o tipo de erro e sua localização no código, terminando o código.

### Funções Principais

1. **Análise Sintática:** As funções analisa_programa, analisa_bloco, e analisa_comando realizam a análise sintática do código fonte, verificando a estrutura geral do programa. Cada função é responsável por uma parte específica da linguagem, como a análise de declarações, comandos compostos, e expressões.

2. **Identificação de Elementos:** O núcleo da análise se dá nas funções identifica_var, identifica_num, e identifica_simb, que identificam variáveis, números e símbolos, respectivamente. Essas funções utilizam ponteiros para percorrer o buffer e processar os caracteres.

3. **Tratamento de Erros:** A função raise_erro é utilizada para relatar erros sintáticos encontrados durante a análise, indicando o que era esperado e o que foi encontrado.

4. **Leitura de Arquivo:** A função ler_arquivo lê o conteúdo de um arquivo de entrada e armazena no buffer. O programa espera que um arquivo fonte seja passado como argumento de linha de comando.

### Processamento de Comentários

Os comentários são tratados por funções específicas (identifica_coment e exibir_comentarios), que permitem que o analisador ignore partes do código que não afetam a sintaxe. Comentários são ignorados durante a análise, mas sua presença e linha são registradas.

### Compilação

gcc -g -Og -Wall compilador.c -o compilador
./compilador teste1.pas
