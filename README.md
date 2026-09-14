# Compiladores - Simulador AFD

Projeto de compiladores que implementa um **Simulador de Autômato Finito Determinístico (AFD)** para análise léxica de números inteiros e ponto flutuante, com visualização gráfica animada.

## Funcionalidades

- Entrada de texto interativa com validação em tempo real
- Visualização animada dos estados do AFD (q0 a q6)
- Reconhecimento de números inteiros e floats (com sinal +/- e vírgula)
- Indicação visual do estado atual durante o processamento
- Mensagem de aceitação/rejeição com exibição do lexema reconhecido

## Estrutura do AFD

```
q0 --[+/-]--> q1 --[D]--> q2 --[,]--> q3 --[D]--> q4
                   |                        |
                  [D]                      [D]
                   v                        v
                  q5 (Final Int)           q6 (Final Float)
```

- **q0**: Estado inicial
- **q1**: Leitura de sinal (+/-)
- **q2**: Leitura de dígitos (parte inteira)
- **q3**: Leitura da vírgula
- **q4**: Leitura de dígitos (parte decimal)
- **q5**: Estado final - número inteiro aceito
- **q6**: Estado final - número float aceito

## Requisitos

- [GCC (MinGW)](https://www.mingw-w64.org/) ou equivalente
- [Raylib](https://www.raylib.com/) (já incluso na pasta: `raylib.h`, `libraylib.dll`, `glfw3.dll`)

## Como Compilar

```bash
gcc codigoem.c -o afd_animado.exe -I. -L. -lraylib -lgdi32 -lwinmm
```

Ou utilize a extensão **C/C++ Runner** no VS Code (configuração já inclusa em `.vscode/`).

## Como Executar

```bash
./afd_animado.exe
```

> **Nota:** Os arquivos `libraylib.dll` e `glfw3.dll` devem estar no mesmo diretório do executável.

## Controles

| Tecla/Ação | Descrição |
|------------|-----------|
| Digitar     | Insere caracteres na entrada |
| Backspace   | Remove último caractere |
| Enter       | Inicia o processamento |
| Botão "Limpar" | Reseta a entrada |

## Exemplos de Entrada

| Entrada    | Resultado      |
|------------|----------------|
| `42`       | Token aceito (Int) |
| `-3,14`    | Token aceito (Float) |
| `+100`     | Token aceito (Int) |
| `abc`      | Nenhum token reconhecido |
| `3.14`     | Nenhum token reconhecido (usa vírgula, não ponto) |

## Tecnologias

- **Linguagem:** C
- **Gráficos:** Raylib
- **Compilador:** GCC (MinGW)
