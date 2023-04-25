#include "expr.h"

#include <memory>
/*
void stack_to_expr(struct expr *rpn, struct stk *opstack) {
  expr_add_symbol(rpn, stk_peek_status(opstack), stk_peek(opstack));
  stk_pop(opstack);
}

struct expr *expr_shunt(const struct expr *infix) {
  struct stk *opstack = stk_new();
  struct expr *rpn = expr_new();
  for (struct ll_node *i = infix->head; i != NULL; i = i->next) {
    switch (i->state) {
      case OPERAND:
      case VARIABLE:
        expr_add_symbol(rpn, i->state, i->datum);
        break;
      case L_BRACKET:
        stk_push(opstack, i->state, i->datum);
        break;
      case R_BRACKET:
        while (stk_peek_status(opstack) != L_BRACKET)
          stack_to_expr(rpn, opstack);
        stk_pop(opstack);  // Забираем из стека открывающуюся скобку
        if (stk_peek_status(opstack) == FUNCTION) stack_to_expr(rpn, opstack);
        break;
      case FUNCTION:
      case UNARYOPERATOR:
      case OPERATOR:
        while (opstack->depth > 0 &&
               precedence(opstack->top) >= precedence(i) &&
               stk_peek_status(opstack) != L_BRACKET)
          stack_to_expr(rpn, opstack);
        stk_push(opstack, i->state, i->datum);
        break;
      case ERROR:
      default:
        i = NULL;
        assert(0);
    }
  }
  while (opstack->depth > 0) stack_to_expr(rpn, opstack);
  stk_destroy(&opstack);
  return rpn;
}
*/

void TokenList::make_unary_operator() {
  ExprToken *last = back();
  ExprToken *before = before_back();

  if ((last->state() == OPERATOR && syntax->is_unary_operator(last->name())) &&
      (before == nullptr ||
       ((before->state() == OPERATOR) || (before->state() == L_BRACKET)))) {
    pop_back();
    push_back(new FuncExprToken(last->name(),
                                syntax->get_data(last->name(), UNARYOPERATOR)));
    delete last;
  }
}

bool TokenList::check_syntax() {
  ExprToken *last = back();
  ExprToken *before = before_back();

  bool good = true;
  if (before != nullptr) {
    if (last->state() == ERROR) good = false;
    if (before->state() == UNARYOPERATOR &&
        (last->state() == OPERATOR || last->state() == R_BRACKET))
      good = false;

    if (last->state() == before->state() && last->state() != L_BRACKET &&
        last->state() != R_BRACKET)
      good = false;

    if (before->state() == OPERATOR && last->state() == R_BRACKET) good = false;
    if (before->state() == R_BRACKET &&
        (last->state() != OPERATOR && last->state() != R_BRACKET))
      good = false;

    if (before->state() == OPERAND && last->state() == VARIABLE) good = false;
    if (before->state() == VARIABLE && last->state() == OPERAND) good = false;

    if (before->state() != OPERATOR && before->state() != UNARYOPERATOR &&
        before->state() != L_BRACKET && last->state() == FUNCTION)
      good = false;
  } else {
    if (last->state() == OPERATOR) good = false;
  }
  if (brackets < 0) good = false;
  return good;
};

int TokenList::skip_spaces(string str) {
  int i = 0;
  while (str[i] && syntax->is_space(str[i])) i++;  // Skip spaces
  return i;
};

std::pair<int, token_type> TokenList::find_token(const string &str) {
  int l = syntax->is_operand(str);
  token_type t = OPERAND;
  return l != 0 ? make_pair(l, t) : syntax->is_token(str);
};

ExprToken *TokenList::create_token(const string &str_token, token_type t) {
  ExprToken *token;

  if (t == OPERATOR || t == UNARYOPERATOR || t == FUNCTION || t == L_BRACKET ||
      t == R_BRACKET)
    token = new FuncExprToken(str_token, syntax->get_data(str_token, t));
  if (t == L_BRACKET) {
    brackets++;
  } else if ((t == R_BRACKET)) {
    brackets--;
  }
  if ((t == OPERAND)) {
    token = new ExprToken(t, syntax->get_operand(str_token));
  }
  if ((t == VARIABLE)) {
    token = new VarExprToken(t, str_token);
  }

  return token;
};

void TokenList::make_list(std::string s) {
  bool good = true;
  if (s.size() != 0) {
    int i = 0;
    while (static_cast<size_t>(i) < s.size() && good) {  // Rest of the string
      i += skip_spaces(s);                               // Skip spaces
      if (s[i]) {
        int l;
        token_type t;
        tie(l, t) = find_token(s.substr(i));
        if (l) {
          push_back(create_token(s.substr(i, l), t));
          make_unary_operator();  //!
          good = check_syntax();
          i += l;
        } else {
          good = false;
        }
      }
    }
  } else {
    throw std::invalid_argument("Wrong expression string");
  }
  if (brackets != 0) throw std::invalid_argument("Wrong () pairs");
  if (good == false) throw std::invalid_argument("Parsing error");
};
