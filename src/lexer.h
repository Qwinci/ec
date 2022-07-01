#pragma once
#include <stddef.h>

typedef struct {
	size_t line;
	size_t column;
} CodeLoc;

typedef enum {
	TOKENTYPE_LPAREN,
	TOKENTYPE_RPAREN,
	TOKENTYPE_LBRACE,
	TOKENTYPE_RBRACE,
	TOKENTYPE_LBRACKET,
	TOKENTYPE_RBRACKET,
	TOKENTYPE_INT_LITERAL,
	TOKENTYPE_STRING_LITERAL,
	TOKENTYPE_CHARACTER_LITERAL,
	TOKENTYPE_IDENTIFIER,

	TOKENTYPE_PLUS,
	TOKENTYPE_MINUS,
	TOKENTYPE_STAR,
	TOKENTYPE_SLASH,
	TOKENTYPE_MODULO,
	TOKENTYPE_OR,
	TOKENTYPE_AND,
	TOKENTYPE_GT,
	TOKENTYPE_LT,
	TOKENTYPE_EQ,
	TOKENTYPE_SEMICOLON,
	TOKENTYPE_NOT,
	TOKENTYPE_COMMA,

	TOKENTYPE_EOF
} TokenType;

typedef struct {
	char* text;
	size_t text_length;
	CodeLoc loc;
	TokenType type;
} Token;

Token* token_new(char* text, size_t text_length, CodeLoc loc, TokenType type);
void token_free(Token* token);
CodeLoc codeloc_new(size_t line, size_t column);

typedef struct {
	CodeLoc loc;
	const char* str;
	const char* file;
} Lexer;

Lexer lexer_new(const char* str, const char* file);
Token* lexer_next(Lexer* lexer);