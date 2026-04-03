# SALc — Compilador para a Linguagem SAL

**Guilherme Teodoro de Oliveira — 10425362**  
**Vinícius Brait Lorimier — 10420046**

Implementação em C de um compilador para a linguagem **SAL – Simple Academic Language**.

---

## Compilação

Requer `gcc` com suporte a C99.

```bash
make          # produz o binário salc
make clean    # remove objetos e binário
```

Flags utilizadas: `-Wall -Wextra -std=c99`

---

## Uso

```
salc <arquivo.sal> [--tokens] [--symtab] [--trace]
```

| Opção | Efeito |
|-------|--------|
| `--tokens` | Gera `<arquivo>.tk` com a lista de tokens reconhecidos |
| `--symtab` | Gera `<arquivo>.ts` com a tabela de símbolos consolidada |
| `--trace`  | Gera `<arquivo>.trc` com o rastro da análise sintática |

```bash
salc exemplo01.sal
salc exemplo03.sal --tokens --symtab --trace
```

---

## Estrutura de módulos

| Módulo | Arquivos | Responsabilidade |
|--------|----------|------------------|
| `main` | `main.c` | Orquestração: processa CLI, inicializa e encerra todos os módulos |
| `lex`  | `lex.h/c` | Analisador léxico (AFD); interface pública: `lex_next` |
| `parser` | `parser.h/c` | ASDR; uma função por não-terminal; interface pública: `parse_program` |
| `symtab` | `symtab.h/c` | Tabela de símbolos com escopo; interface pública: `ts_insert`, `ts_lookup` |
| `diag` | `diag.h/c` | Diagnósticos centralizados; interface pública: `diag_error`, `diag_info` |
| `opt`  | `opt.h/c` | Interpretação da linha de comando; interface pública: `opts_parse`, `opts_get` |
| `log`  | `log.h/c` | Geração dos arquivos de log opcionais (`.tk`, `.ts`, `.trc`) |

A comunicação entre módulos ocorre exclusivamente via cabeçalhos públicos. Nenhum módulo compartilha estado interno com outro.

---

## Formato dos arquivos de log

### `.tk` — Lista de tokens

Gerado com `--tokens`. Cada linha registra um token no formato:

```
<linha>  <CATEGORIA>  "<lexema>"
```

Exemplo:
```
1     sMODULE           "module"
1     sIDENTIF          "Exemplo_03"
1     sPONTEVIRG        ";"
3     sFN               "fn"
3     sIDENTIF          "SOMA"
```

### `.ts` — Tabela de símbolos

Gerado com `--symtab`. Entradas em ordem de escopo (global → locais por sub-rotina), preservando a ordem de inserção:

```
SCOPE=<descr>  id="<lexema>"  cat=<categoria>  tipo=<tipo>  extra=<atrib>
```

O campo `extra` armazena:
- tamanho do vetor (variáveis do tipo vetor)
- número de parâmetros (sub-rotinas)
- `0` nos demais casos

Exemplo:
```
SCOPE=global                          id="SOMA"   cat=funcao    tipo=int   extra=2
SCOPE=fn:SOMA.locals                  id="A"      cat=parametro tipo=int   extra=0
SCOPE=fn:SOMA.locals                  id="B"      cat=parametro tipo=int   extra=0
SCOPE=global                          id="main"   cat=proc      tipo=void  extra=0
```

### `.trc` — Rastro sintático

Gerado com `--trace`. Registra entrada (`>>`) e saída (`<<`) de cada não-terminal:

```
>> ini (linha 1)
>> glob (linha 3)
<< glob
>> func (linha 5)
>> param (linha 5)
<< param
...
<< func
<< ini
```

---

## Escopos da tabela de símbolos

| Descrição do escopo | Quando é criado |
|---------------------|-----------------|
| `global` | Início da análise (nível 0) |
| `fn:NOME.locals` | Cabeçalho de uma função `fn` |
| `proc:NOME.locals` | Cabeçalho de um procedimento `proc` |
| `proc:NOME.block#N` | Cada bloco `start...end` aninhado (N = sequencial) |

---
