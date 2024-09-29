with open("teste1.pas", "r") as arquivo:
    linhas = arquivo.readlines()
    string = "".join(linhas)


def gera_lista(delimitadores, string):
    temp = ""
    resultado = []
    i = 0
    while i < len(string):
        for delimitador in delimitadores:
            if string[i : i + len(delimitador)] == delimitador:
                if temp:
                    resultado.append(temp.strip())
                    temp = ""
                if delimitador != " ":
                    resultado.append(delimitador)
                i += len(delimitador) - 1
                break
        else:
            temp += string[i]
        i += 1
    if temp:
        resultado.append(temp.strip())
    return resultado


def gera_matriz(resultado: list):
    resultado_matriz = []
    linha_atual = []
    for token in resultado:
        if token == "\n":
            if linha_atual:
                resultado_matriz.append(linha_atual)
                linha_atual = []
        else:
            linha_atual.append(token)
    if linha_atual:
        resultado_matriz.append(linha_atual)
    return resultado_matriz


class AnalisadorSintatico:
    def __init__(self, matriz):
        self.matriz = matriz
        self.indice_linha = 0
        self.indice_token = 0
        self.token_atual = self.obter_token_atual()

    def obter_token_atual(self):
        if self.indice_linha < len(self.matriz):
            if self.indice_token < len(self.matriz[self.indice_linha]):
                return self.matriz[self.indice_linha][self.indice_token]
        return None

    def avancar(self):
        self.indice_token += 1
        if self.indice_token >= len(self.matriz[self.indice_linha]):
            self.indice_linha += 1
            self.indice_token = 0
        self.token_atual = self.obter_token_atual()

    def imprimir_token(self):
        if self.token_atual is not None:
            if self.token_atual == "identificador":
                # O próximo item deve ser o nome do identificador
                if self.indice_token + 1 < len(self.matriz[self.indice_linha]):
                    nome_identificador = self.matriz[self.indice_linha][
                        self.indice_token + 1
                    ]
                    print(
                        f"# {self.indice_linha + 1}: {self.token_atual} | {nome_identificador}"
                    )
                    self.avancar()  # Avança para o próximo token (nome do identificador)
                self.avancar()  # Avança para o próximo token
            else:
                print(f"# {self.indice_linha + 1}: {self.token_atual}")

    def espera(self, token_esperado):
        if self.token_atual == token_esperado:
            self.imprimir_token()
            self.avancar()
        else:
            self.erro_sintatico(token_esperado)

    def erro_sintatico(self, token_esperado):
        linha = self.indice_linha + 1
        print(
            f"# {linha}: erro sintatico, esperado [{token_esperado}] encontrado [{self.token_atual}]"
        )
        raise Exception("Análise interrompida devido a erro de sintaxe.")

    def analisar_programa(self):
        self.espera("program")  # Espera a palavra-chave 'program'
        # Aqui, após 'program', aceitamos qualquer identificador
        if self.token_atual:
            self.imprimir_token()  # Imprime o nome do programa
            self.avancar()  # Avança para o próximo token
        else:
            self.erro_sintatico("identificador")

        self.espera(";")  # Espera o ponto e vírgula
        self.analisar_bloco()  # Analisa o bloco de código
        self.espera(".")  # Espera o ponto final

    def analisar_bloco(self):
        self.analisar_declaracao_de_variaveis()
        self.analisar_comando_composto()

    def analisar_declaracao_de_variaveis(self):
        while self.token_atual in ["integer", "boolean"]:
            self.analisar_tipo()
            self.analisar_lista_variavel()
            self.espera(";")

    def analisar_tipo(self):
        if self.token_atual == "integer":
            self.avancar()
        elif self.token_atual == "boolean":
            self.avancar()
        else:
            self.erro_sintatico("tipo")

    def analisar_lista_variavel(self):
        self.espera("identificador")  # Espera o primeiro identificador
        while self.token_atual == ",":
            self.avancar()
            self.espera("identificador")  # Espera identificadores adicionais

    def analisar_comando_composto(self):
        self.espera("begin")
        self.analisar_comando()
        while self.token_atual == ";":
            self.avancar()
            self.analisar_comando()
        self.espera("end")

    def analisar_comando(self):
        if self.token_atual == "set":
            self.analisar_comando_atribuicao()
        elif self.token_atual == "if":
            self.analisar_comando_condicional()
        elif self.token_atual == "for":
            self.analisar_comando_repeticao()
        elif self.token_atual == "read":
            self.analisar_comando_entrada()
        elif self.token_atual == "write":
            self.analisar_comando_saida()
        elif self.token_atual == "begin":
            self.analisar_comando_composto()
        else:
            self.erro_sintatico("comando")

    def analisar_comando_atribuicao(self):
        self.espera("set")
        self.espera("identificador")  # Espera um identificador como variável
        self.espera("to")
        self.analisar_expressao()

    def analisar_comando_condicional(self):
        self.espera("if")
        self.analisar_expressao()
        self.espera(":")
        self.analisar_comando()
        if self.token_atual == "elif":
            self.avancar()
            self.analisar_comando()

    def analisar_comando_repeticao(self):
        self.espera("for")
        self.espera("identificador")  # Espera um identificador para a variável do loop
        self.espera("of")
        self.analisar_expressao()
        self.espera("to")
        self.analisar_expressao()
        self.espera(":")
        self.analisar_comando()

    def analisar_comando_entrada(self):
        self.espera("read")
        self.espera("(")
        self.analisar_lista_variavel()
        self.espera(")")

    def analisar_comando_saida(self):
        self.espera("write")
        self.espera("(")
        self.analisar_expressao()
        while self.token_atual == ",":
            self.avancar()
            self.analisar_expressao()
        self.espera(")")

    def analisar_expressao(self):
        self.analisar_expressao_logica()
        while self.token_atual == "or":
            self.avancar()
            self.analisar_expressao_logica()

    def analisar_expressao_logica(self):
        self.analisar_expressao_relacional()
        while self.token_atual == "and":
            self.avancar()
            self.analisar_expressao_relacional()

    def analisar_expressao_relacional(self):
        self.analisar_expressao_simples()
        if self.token_atual in ["<", "<=", "=", "/=", ">", ">="]:
            self.avancar()
            self.analisar_expressao_simples()

    def analisar_expressao_simples(self):
        self.analisar_termo()
        while self.token_atual in ["+", "−"]:
            self.avancar()
            self.analisar_termo()

    def analisar_termo(self):
        self.analisar_fator()
        while self.token_atual in ["*", "/"]:
            self.avancar()
            self.analisar_fator()

    def analisar_fator(self):
        if self.token_atual == "identificador":  # Identificadores válidos
            self.imprimir_token()  # Chama a impressão do identificador
            self.avancar()
        elif self.token_atual == "numero":  # Números válidos
            self.imprimir_token()  # Chama a impressão do número
            self.avancar()
        elif self.token_atual in ["true", "false"]:  # Booleanos
            self.imprimir_token()  # Chama a impressão do booleano
            self.avancar()
        elif self.token_atual == "not":
            self.avancar()
            self.analisar_fator()
        elif self.token_atual == "(":
            self.avancar()
            self.analisar_expressao()
            self.espera(")")  # Espera fechar parêntese
        else:
            self.erro_sintatico("fator")

    def analisar(self):
        self.analisar_programa()
        if self.token_atual is not None:
            raise Exception("Erro de sintaxe: tokens restantes não processados.")


resultado = gera_lista(
    delimitadores=[
        ";",
        ".",
        "(",
        ")",
        "{",
        "}",
        ":",
        "<=",
        "<",
        "/=",
        "=",
        ">=",
        ">",
        "+",
        "−",
        "*",
        "/",
        " ",
        "\n",
    ],
    string=string,
)
matriz = gera_matriz(resultado)
analisador = AnalisadorSintatico(matriz)
print(matriz)
try:
    analisador.analisar()
    print("Análise sintática bem-sucedida!")
except Exception as e:
    print(e)
