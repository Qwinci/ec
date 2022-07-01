#pragma once
#include "lexer.h"
#include "ast.h"

typedef struct {
	Token** tokens;
	size_t tokens_size;
	const char* file;
} Parser;

Parser parser_new(Token** tokens, size_t tokens_size, const char* file);
Node* parser_parse(Parser* parser);