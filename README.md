
# Rinha Interpreter

Esse projeto é um interpretador para a AST gerada pelo compilador [https://github.com/aripiprazole/rinha-de-compiler](https://github.com/aripiprazole/rinha-de-compiler).

> **Aviso:** Esse projeto é um trabalho em andamento. No momento, o interpretador apenas verifica a sintaxe do programa e gera o bytecode correspondente. A máquina virtual ainda não foi implementada.

## Estratégias de implementação

A fim de priorizar velocidade de execução, sem perder a simplicidade na implementação, o interpretador usará o seguinte:

- Compilador de um único passo (sem otimizações)
- Interpretador de bytecode
- Máquina Virtual baseada em pilha
- Tagged unions para representar valores
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

## Running

Depois de construído, o interpretador estará disponível em `build/rinha`. Dessa forma, para executar um programa, basta executar:

```text
./build/rinha run examples/fib.json
```

## Road Map

O projeto está sendo desenvolvido em etapas. A seguir, a lista de etapas já concluídas e as que ainda estão por vir:

- [x] Scanner
- [x] Verificando a sintaxe
- [x] Gerando o bytecode
- [ ] Máquina Virtual

## License

Esse projeto é licenciado sob os termos da licença MIT. Veja o arquivo [LICENSE](LICENSE) para mais detalhes.
