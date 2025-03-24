#include "lexer-handmade.h"

const char *g_token_name[] = {
#define __PUNC(n, s) [TOK_##n] = #n,
#define __KEYWORD(n, s) [TOK_##n] = #n,
  TOKEN_LIST
#undef __PUNC
#undef __KEYWORD
  [TOK_OVER]    = "OVER",
  [TOK_IDENT]   = "IDENT",
  [TOK_NUMBER]  = "NUMBER",
  [TOK_INVALID] = "INVALID",
};

#define PEEKC(l) (l)->current + 1 >= (l)->buffer.len ? -1 : l->buffer.data[l->current + 1]
#define CURC(l) l->buffer.data[l->current]
#define END(l) (l->current >= (l)->buffer.len)

static inline void advance(lexer_t *l)
{
  l->current++;
  if (l->current < l->buffer.len) {
    if (l->buffer.data[l->current] == '\n') {
      l->file_col = -1;
      l->file_line++;
    }
    l->file_col++;
  } else {
    l->current = l->buffer.len;
  }
}

void lexer_init(lexer_t *l, c8 *src, usize src_len)
{
  assert(l);
  l->buffer = (c8arr_t){.data = src, .len = src_len};
  l->current = 0;
  l->file_line = 1;
  l->file_col= 1;
  l->file = NULL;
}

i32 lexer_lex(lexer_t *l, token_data_t *token, arena_t *arena)
{
  i32 token_result;
  do // Ignore whitespace, block comment and line comment
  {
    // Space
    while(!END(l) && isspace(CURC(l))) {
      advance(l);
    }

    if(END(l)) return TOK_OVER;

    // Multiline comment
    if(CURC(l) == '/' && PEEKC(l) == '*')
    {
      do
      {
        advance(l);
      } while(!END(l) && (CURC(l) != '*' || PEEKC(l) != '/'));
      advance(l);
      advance(l);
      continue;
    }
    // Line comment
    if(CURC(l) == '/' && PEEKC(l) == '/')
    {
      do
      {
        advance(l);
      } while(!END(l) && (CURC(l) != '\n'));
      advance(l);
      continue;
    }
    break;
  } while(true);

  while(!END(l) && isspace(CURC(l))) {
    advance(l);
  }

  if(END(l)) return TOK_OVER;

  if(token) {
    memset(token, 0, sizeof(token_data_t));
    token->file_line = l->file_line;
    token->file_col = l->file_col;
  }

  if(isalpha(CURC(l)))
  {
    c8da_t ident = {};
    do 
    {
      *append(&ident, arena) = CURC(l);
      advance(l);
    }
    while (!END(l) && isalnum(CURC(l)));

#define __PUNC(n, s)
#define __KEYWORD(n, s)                                                        \
  if (strncmp(ident.data, s, ident.len) == 0) {                                \
    if (token) {                                                               \
      token->token_type = TOK_##n;                                             \
    }                                                                          \
    return TOK_##n;                                                            \
  }
    TOKEN_LIST
#undef __PUNC
#undef __KEYWORD
  
    bool end_with_num = false;
    bool valid_ident = true;
    foreach(c, ident){
      if(!end_with_num && isascii(*c)){
         valid_ident = false;
      }
      if(isdigit(*c)) {
        end_with_num = true;
      }
    }
    if(valid_ident) {
      printf("IDENTIFIER ERROR: UPL only accpept identifier that start with "
             "character and end with number, get %.*s\n",
             (i32)ident.len, ident.data);
      return TOK_INVALID;
    }
    if (token) {
      token->token_type = TOK_IDENT;
      token->identifer.name = (c8arr_t){ident.data, ident.len};
    }
    return TOK_IDENT;
  }

  if(isdigit(CURC(l)))
  {
    char* num = l->buffer.data + l->current;
    do
    {
      advance(l);
    }while(!END(l) && isdigit(CURC(l)));
    // Delegate this to the parser
    // Check if digit is followed by a character
    // if(isascii(PEEKC(l)){
    //   printf("NUMBER ERROR: UPL only accpept identifier that start with "
    //          "character and end with number, get %.*s\n",
    //          (i32)ident.len, ident.data);
    // }
    if (token) {
      token->token_type = TOK_NUMBER;
      token->number.value = 0; // TODO
    }
    return TOK_NUMBER;
  }

  switch(CURC(l))
  {
  case '+':
    advance(l);
    if(token) token->token_type = TOK_PLUS;
    return TOK_PLUS;
  case '*':
    advance(l);
    if(token) token->token_type = TOK_STAR;
    return TOK_STAR;
  case ';':
    advance(l);
    if(token) token->token_type = TOK_SEMICOLON;
    return TOK_SEMICOLON;
  case '(':
    advance(l);
    if(token) token->token_type = TOK_OPAREN;
    return TOK_OPAREN;
  case ')':
    advance(l);
    if(token) token->token_type = TOK_CPAREN;
    return TOK_CPAREN;
  case '{':
    advance(l);
      if(token) token->token_type = TOK_OCURLY;
    return TOK_OCURLY;
  case '}':
    advance(l);
      if(token) token->token_type = TOK_CCURLY;
    return TOK_CCURLY;
  case '>':
    advance(l);
    if(CURC(l) == '=')
    {
      advance(l);
      if(token) token->token_type = TOK_GTEQUAL;
      return TOK_GTEQUAL;
    }
    if(token) token->token_type = TOK_GT;
    return TOK_GT;
  case '=':
    advance(l);
    if(CURC(l) == '=')
    {
      advance(l);
      if(token) token->token_type = TOK_DEQUAL;
      return TOK_DEQUAL;
    }
    if(token) token->token_type = TOK_EQUAL;
    return TOK_EQUAL;
  }
  return TOK_INVALID;
}
