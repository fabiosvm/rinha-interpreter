**chunk:**

![chunk](images/chunk.svg)

```
chunk    ::= expr EOF
```

**expr:**

![expr](images/expr.svg)

```
expr     ::= and_expr ( '||' and_expr )*
```

referenced by:

* block
* call
* chunk
* prim_expr
* tuple

**and_expr:**

![and_expr](images/and_expr.svg)

```
and_expr ::= eq_expr ( '&&' eq_expr )*
```

referenced by:

* expr

**eq_expr:**

![eq_expr](images/eq_expr.svg)

```
eq_expr  ::= comp_expr ( ( '==' | '!=' ) comp_expr )*
```

referenced by:

* and_expr

**comp_expr:**

![comp_expr](images/comp_expr.svg)

```
comp_expr
         ::= add_expr ( ( '<' | '<=' | '>' | '>=' ) add_expr )*
```

referenced by:

* eq_expr

**add_expr:**

![add_expr](images/add_expr.svg)

```
add_expr ::= mul_expr ( ( '+' | '-' ) mul_expr )*
```

referenced by:

* comp_expr

**mul_expr:**

![mul_expr](images/mul_expr.svg)

```
mul_expr ::= prim_expr ( ( '*' | '/' | '%' ) prim_expr )*
```

referenced by:

* add_expr

**prim_expr:**

![prim_expr](images/prim_expr.svg)

```
prim_expr
         ::= bool
           | INT
           | STR
           | tuple
           | ( 'if' '(' expr ')' block 'else' )? block
           | ( 'fn' '(' ( IDENT ( ',' IDENT )* )? ')' '=>' | 'let' IDENT '=' expr ';' ) expr
           | ( 'print' | 'first' | 'second' )? '(' expr ')'
           | IDENT call?
```

referenced by:

* mul_expr

**bool:**

![bool](images/bool.svg)

```
bool     ::= 'false'
           | 'true'
```

referenced by:

* prim_expr

**tuple:**

![tuple](images/tuple.svg)

```
tuple    ::= '(' expr ',' expr ')'
```

referenced by:

* prim_expr

**block:**

![block](images/block.svg)

```
block    ::= '{' expr '}'
```

referenced by:

* prim_expr

**call:**

![call](images/call.svg)

```
call     ::= '(' ( expr ( ',' expr )* )? ')'
```

referenced by:

* prim_expr

## 
![rr-2.0](images/rr-2.0.svg) <sup>generated by [RR - Railroad Diagram Generator][RR]</sup>

[RR]: http://bottlecaps.de/rr/ui