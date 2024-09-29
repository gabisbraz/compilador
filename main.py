# Função para verificar se a linha contém um comentário
def analisar_comentario(linha):
    if "{-" in linha:  # Comentário multilinha completo
        return "{- comentario", len(linha)
    elif "#" in linha:  # Comentário de uma linha
        return "# comentario", len(linha)
    return False, 0  # Não é um comentário


# Função para analisar o início do programa
def analisar_programa(linha, num_linha):
    if "program " in linha:
        # Encontrar o espaço após 'program'
        pos = linha.find("program ") + len("program ")
        if pos < len(linha):
            # Encontrar o próximo espaço ou ';'
            fim_identificador = linha.find(";", pos)
            if fim_identificador != -1:
                identificador = linha[pos:fim_identificador].strip()
                print(f"# {num_linha}: program")
                print(f"# {num_linha}: identificador | {identificador}")
                print(f"# {num_linha}: ;")
                return True
    return False


# Função para analisar declaração de variáveis
def analisar_declaracao_de_variaveis(linha, num_linha):
    if linha.startswith("integer ") or linha.startswith("boolean "):
        tipo = linha[: linha.find(" ")].strip()  # Tipo: 'integer' ou 'boolean'
        variaveis_str = linha[linha.find(" ") + 1 : linha.find(";")].strip()
        variaveis_lista = [v.strip() for v in variaveis_str.split(",")]

        print(f"# {num_linha}: {tipo}")
        for i, var in enumerate(variaveis_lista):
            print(f"# {num_linha}: identificador | {var.strip()}")
            if i != len(variaveis_lista) - 1:
                print(f"# {num_linha}: ,")
        print(f"# {num_linha}: ;")
        return True
    return False


# Função para analisar comandos compostos
def analisar_comando_composto(linha, num_linha):
    if linha == "begin":
        print(f"# {num_linha}: begin")
        return True
    elif linha == "end.":
        print(f"# {num_linha}: end")
        print(f"# {num_linha}: .")
        return True
    return False


# Função para analisar comandos (geral)
def analisar_comando(linha, num_linha):
    linha = linha.strip()

    # Verifica se há um comentário no final da linha
    comentario = ""
    if "#" in linha:
        linha, comentario = linha.split("#", 1)
        comentario = comentario.strip()

    # Comando de atribuição: set identificador to expressão
    if "set " in linha:
        pos_set = linha.find("set ") + len("set ")
        pos_to = linha.find("to", pos_set)
        pos_ponto_e_virgula = linha.find(";", pos_to)

        if pos_to != -1 and pos_ponto_e_virgula != -1:
            identificador1 = linha[pos_set:pos_to].strip()
            identificador2 = linha[pos_to + len("to") : pos_ponto_e_virgula].strip()
            print(f"# {num_linha}: set")
            print(f"# {num_linha}: identificador | {identificador1}")
            print(f"# {num_linha}: to")
            print(f"# {num_linha}: identificador | {identificador2}")
            print(f"# {num_linha}: ;")

            if comentario:
                print(f"# {num_linha}: comentario")
            return True

    # Comando condicional: if expressão :
    if "if " in linha and linha.strip().endswith(":"):
        pos_if = linha.find("if ") + len("if ")
        condicao = linha[pos_if:-1].strip()  # Remove 'if ' e ':'
        print(f"# {num_linha}: if")
        for cond in condicao.split():
            if cond.isidentifier():  # Verifica se é um identificador
                print(f"# {num_linha}: identificador | {cond}")
            else:
                print(f"# {num_linha}: {cond}")
        print(f"# {num_linha}: :")

        if comentario:
            print(f"# {num_linha}: comentario")
        return True

    # Comando de repetição: for identificador of expressão to expressão :
    if "for " in linha and linha.strip().endswith(":"):
        pos_for = linha.find("for ") + len("for ")
        pos_of = linha.find("of", pos_for)
        pos_to = linha.find("to", pos_of)

        if pos_of != -1 and pos_to != -1:
            print(f"# {num_linha}: for")
            identificador = linha[pos_for:pos_of].strip()
            print(f"# {num_linha}: identificador | {identificador}")
            print(f"# {num_linha}: of")
            print(f"# {num_linha}: expressão")
            print(f"# {num_linha}: to")
            print(f"# {num_linha}: expressão")
            print(f"# {num_linha}: :")

            if comentario:
                print(f"# {num_linha}: comentario")
            return True

    # Comando de entrada: read(lista_variaveis)
    if "read(" in linha and ";" in linha:
        print(f"# {num_linha}: read")
        print(f"# {num_linha}: (")
        inicio_lista = linha.find("read(") + len("read(")
        fim_lista = linha.find(";", inicio_lista)

        lista_variaveis = linha[inicio_lista:fim_lista].strip()  # Remove 'read(' e ';'
        variaveis = [v.strip() for v in lista_variaveis.split(",")]
        for var in variaveis:
            print(f"# {num_linha}: identificador | {var}")
        print(f"# {num_linha}: )")
        print(f"# {num_linha}: ;")

        if comentario:
            print(f"# {num_linha}: comentario")
        return True

    # Comando de saída: write(expressao)
    if "write(" in linha and ")" in linha:
        print(f"# {num_linha}: write")
        print(f"# {num_linha}: (")
        inicio_expressao = linha.find("write(") + len("write(")
        fim_expressao = linha.find(")", inicio_expressao)

        expressao = linha[
            inicio_expressao:fim_expressao
        ].strip()  # Remove 'write(' e ')'
        print(f"# {num_linha}: expressão | {expressao}")
        print(f"# {num_linha}: )")

        if comentario:
            print(f"# {num_linha}: comentario")
        return True

    return False


# Função para analisar operadores relacionais
def analisar_operador_relacional(linha, num_linha):
    operadores = ["<", "<=", "=", "/=", ">", ">="]
    for operador in operadores:
        if operador in linha:
            print(f"# {num_linha}: operador_relacional | {operador}")
            return True
    return False


# Função para analisar fatores
def analisar_fator(linha, num_linha):
    fatores = linha.split()
    for fator in fatores:
        if fator.isidentifier() or fator.isdigit() or fator in ["true", "false"]:
            print(f"# {num_linha}: {fator}")
            return True
        elif fator.startswith("(") and fator.endswith(")"):
            print(f"# {num_linha}: {fator}")
            return True
    return False


# Função principal para analisar o arquivo
def analisar_arquivo(arquivo):
    with open(arquivo, "r") as f:
        linhas = f.readlines()

    dentro_bloco = False
    dentro_comentario_multilinha = False
    comentario_buffer = ""

    for num_linha, linha in enumerate(linhas, start=1):
        linha = linha.strip()

        # Analisa cada parte da gramática conforme a linha lida
        if "program " in linha:
            if analisar_programa(linha, num_linha):
                continue
        elif "integer " in linha or "boolean " in linha:
            if analisar_declaracao_de_variaveis(linha, num_linha):
                continue
        elif linha == "begin":
            dentro_bloco = True
            analisar_comando_composto(linha, num_linha)
        elif linha == "end.":
            analisar_comando_composto(linha, num_linha)
            dentro_bloco = False
        elif dentro_bloco:
            if analisar_comando(linha, num_linha):
                continue
            elif analisar_operador_relacional(linha, num_linha):
                continue
            else:
                print(f"Erro sintático na linha {num_linha}: {linha}")

        # Se estamos dentro de um comentário multilinha
        if dentro_comentario_multilinha:
            comentario_buffer += linha + " "
            if "-}" in linha:
                dentro_comentario_multilinha = False
                print(f"# {num_linha}: comentario")
            continue

        # Ignorar comentários no início ou no meio do código
        comentario, tamanho_comentario = analisar_comentario(linha)
        if comentario:
            if "{-" in linha and not linha.count("-}") > 0:
                dentro_comentario_multilinha = True
                comentario_buffer = linha + " "
                continue
            else:
                print(f"# {num_linha}: comentario")
                continue


# Exemplo de execução
analisar_arquivo("teste1.pas")
