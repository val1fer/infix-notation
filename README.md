# Infix notation calculator

This program on C can count difficult expressions like `-1234 + 3 * 2 + (6 - 2) ** 2` 
To count an expression firstly I use shunting yard algorithm to turn infix notation into reverse polish notation (postfix notation).

## Supported operations
Binary:
`a * b`, `a / b`, `a % b`, `a + b`, `a - b`,

`a << b`, `a >> b`, `a & b`, `a | b`, `a ^ b`, `a * b`, 

`a ** b` - exponentiation, `(a + b) * c` - precedence management
#
Unary: `-a`, `+a`, `~a`

about signs: https://en.cppreference.com/w/cpp/language/operator_precedence.html


## Usage
Launch calc.exe by passing an expression in " " there as a first parameter
You should put spaces between binary signs, overwise it will be counted as unary ones (`a-b` -> `a`, `-b`  ;  `a - b` -> ✓)

Ex.: `./calc.exe "(-1234 + 3 * 2) | ((6 - 2) << 4) ** 2 % 17 + 5 & ((3 ** (2 + 1)) / 4)"`

Output: -1228
