/*
 * Recursive descent parser and evaluator for simple C expressions
 *
 * Here is th BNF for simple C expressions
 *
 *
 * <expression> ::= <term> { ("+" | "-") <term> }
 * <term>       ::= <factor> { ("*" | "/") <factor> }
 * <factor>     ::= <digit> | "(" <expression> ")"
 * <number>     ::= '0' ... '9'
 *
 * Note that no whitespace is allowed.
 * Here are some valid expressions:
 *
 * 3
 * 3*2
 * 3*2+9
 * 3*(2+9)
*/

int expression_c(char *expr_str, int *pos);
int term_c(char *expr_str, int *pos);
int factor_c(char *expr_str, int *pos);
int isdigit_c(char c);
int number_c(char *expr_str, int *pos);

int expression_c(char *expr_str, int *pos) {
    int value;
    char token;

    value = term_c(expr_str, pos);
    
    while (expr_str[*pos] == '+' || expr_str[*pos] == '-') {
        token = expr_str[*pos];
        *pos += 1;
        if (token == '+') {
            value += term_c(expr_str, pos);
        } else if (token == '-') {
            value -= term_c(expr_str, pos);
        }
    }
    return value;
}

int term_c(char *expr_str, int *pos) {
    int value;
    char token;

    value = factor_c(expr_str, pos);
    
    while (expr_str[*pos] == '*' || expr_str[*pos] == '/') {
        token = expr_str[*pos];
        *pos += 1;
        if (token == '*') {
            value *= factor_c(expr_str, pos);
        } else if (token == '/') {
            value /= factor_c(expr_str, pos);
        }
    }
    return value;
}

int factor_c(char *expr_str, int *pos) {
    int value;
    char token = expr_str[*pos];
    if (token == '(') {
        *pos += 1; 
        value = expression_c(expr_str, pos);
        token = expr_str[*pos];
        if (token != ')') {
            // Error
            return 0;
        }
        *pos += 1;
    } else {
        value = number_c(expr_str, pos);
    }
    return value;
}

int isdigit_c(char c) {
    return (c >= '0' && c <= '9');
}

int number_c(char *expr_str, int *pos) {
    int value = 0;
    char token;
    token = expr_str[*pos];

    if (!(isdigit_c(token))) {
        // Error
        return 0;
    }
    while (isdigit_c(token)) {
        value = value * 10 + (token - '0');
        *pos += 1;
        token = expr_str[*pos];
    }
    return value;
}

int eval_c(char *expr_str) {
    int pos = 0;
    
    return expression_c(expr_str, &pos);
}
