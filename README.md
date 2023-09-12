
# Rinha Interpreter

Esse projeto é um interpretador para a AST gerada pelo compilador [https://github.com/aripiprazole/rinha-de-compiler](https://github.com/aripiprazole/rinha-de-compiler).

## Estratégias de implementação

A fim de priorizar velocidade de execução, sem perder a simplicidade na implementação, o interpretador usará o seguinte:

- Compilador de um único passo (sem otimizações)
- Interpretador de bytecode
- Máquina baseada em pilha
- Tagged unions para representar valores
- Alocador de memória simples (Bump Allocator)

## Requisitos

Para compilar o projeto, você precisa ter instalado:

- [CMake](https://cmake.org) 3.15 ou superior

## Construindo

Execute o seguinte comando para construir o projeto:

```text
./build.sh
```

## Running

Depois de construído, o interpretador estará disponível em `build/rinha`. Dessa forma, para executar um programa, basta executar:

```text
./build/rinha examples/fib.json
```
