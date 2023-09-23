
# Rinha Interpreter

Esse projeto é um interpretador para a AST gerada pelo compilador [https://github.com/aripiprazole/rinha-de-compiler](https://github.com/aripiprazole/rinha-de-compiler).

Muito da implementação desse projeto foi baseada no livro [Crafting Interpreters](https://craftinginterpreters.com), de Bob Nystrom.

> **Aviso:** Esse projeto é um trabalho em andamento. No momento, o interpretador apenas verifica a sintaxe do programa e gera o bytecode correspondente. A máquina virtual ainda não foi implementada.

## Estratégias de implementação

A fim de priorizar velocidade de execução, sem perder a simplicidade na implementação, o interpretador usará o seguinte:

- Compilador de um único passo (sem otimizações)
- Interpretador de bytecode
- Máquina Virtual baseada em pilha
- Tagged unions (ou tagged pointer) para representar valores
- Alocador de memória simples (Bump Allocator)
- Tail call dispatching (+ switch opcional)

## Requisitos

Para compilar o projeto, você precisa ter instalado:

- [CMake](https://cmake.org) 3.15 ou superior

## Construindo

Execute o seguinte comando para construir o projeto:

```text
./build.sh
```

## Rodando

Depois de construído, o interpretador estará disponível em `build/rinha`. Dessa forma, para executar um programa, basta executar:

```text
./build/rinha run examples/print.json
```

## Rodar usando o Docker

Se você não quiser compilar o projeto, pode usar o Docker para executar o interpretador. Para isso, execute os seguintes comandos:

```text
sudo docker build -t rinha-interpreter .
sudo docker run -v ./examples/print.json:/var/rinha/source.rinha.json rinha-interpreter
```

## Road Map

O projeto está sendo desenvolvido em etapas. A seguir, a lista de etapas já concluídas e as que ainda estão por vir:

- [x] Scanner
- [x] Verificando a sintaxe
- [x] Gerando o bytecode
- [x] Máquina Virtual

## License

Esse projeto é licenciado sob os termos da licença MIT. Veja o arquivo [LICENSE](LICENSE) para mais detalhes.
