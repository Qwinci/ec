#include "lexer.h"
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

Lexer lexer_new(const char* str, const char* file) {
	Lexer lexer = {.loc = (CodeLoc) {.column = 0, .line = 1}, .str = str, .file = file};
	return lexer;
}

TokenType get_type(const char* text) {
	TokenType type;
	if (*text == '+') type = TOKENTYPE_PLUS;
	else if (*text == '-') type = TOKENTYPE_MINUS;
	else if (*text == '*') type = TOKENTYPE_STAR;
	else if (*text == '/') type = TOKENTYPE_SLASH;
	else if (*text == '%') type = TOKENTYPE_MODULO;
	else if (*text == '|') type = TOKENTYPE_OR;
	else if (*text == '&') type = TOKENTYPE_AND;
	else if (*text == '>') type = TOKENTYPE_GT;
	else if (*text == '<') type = TOKENTYPE_LT;
	else if (*text == '=') type = TOKENTYPE_EQ;
	else if (*text == '(') type = TOKENTYPE_LPAREN;
	else if (*text == ')') type = TOKENTYPE_RPAREN;
	else if (*text == '{') type = TOKENTYPE_LBRACE;
	else if (*text == '}') type = TOKENTYPE_RBRACE;
	else if (*text == '[') type = TOKENTYPE_LBRACKET;
	else if (*text == ']') type = TOKENTYPE_RBRACKET;
	else if (*text == ';') type = TOKENTYPE_SEMICOLON;
	else if (*text == '!') type = TOKENTYPE_NOT;
	else if (*text == ',') type = TOKENTYPE_COMMA;
	else {
		bool is_num = true;
		for (const char* s = text; *s; ++s) {
			if (!isdigit(*s)) {
				is_num = false;
				break;
			}
		}

		if (is_num) return TOKENTYPE_INT_LITERAL;
		else return TOKENTYPE_IDENTIFIER;
	}

	return type;
}

Token* lexer_next(Lexer* lexer) {
	while (true) {
		CodeLoc start_loc = lexer->loc;
		char character = *lexer->str;
		if (character == 0) return NULL;
		else if (character == '\n') {
			++lexer->loc.line;
			lexer->loc.column = 0;
			++lexer->str;
		}
		else if (isspace(character)) {
			if (character != '\r') ++lexer->loc.column;
			++lexer->str;
		}
		else if (character == '/' && lexer->str[1] && lexer->str[1] == '/') {
			while (character && character != '\n') {
				++lexer->loc.column;
				++lexer->str;
				character = *lexer->str;
			}
		}
		else if (character == '/' && lexer->str[1] && lexer->str[1] == '*') {
			while (character) {
				if (character == '*' && lexer->str[1] && lexer->str[1] == '/') {
					lexer->loc.column += 2;
					lexer->str += 2;
					break;
				}
				else if (character == '\n') {
					++lexer->loc.line;
					lexer->loc.column = 0;
				}
				else {
					++lexer->loc.column;
				}
				++lexer->str;
				character = *lexer->str;
			}
		}
		else if (character == '+' || character == '-' || character == '*' || character == '/' || character == '%' ||
				character == '|' || character == '&' || character == '>' || character == '<' || character == '=' ||
				character == '(' || character == ')' || character == '{' || character == '}' || character == ';' ||
				character == '!' || character == ',' || character == '[' || character == ']') {
			char* text = malloc(2);
			text[0] = character;
			text[1] = 0;
			Token* token = token_new(text, 1, start_loc, get_type(text));
			++lexer->loc.column;
			++lexer->str;
			return token;
		}
		else if (character == '\'' || character == '"') {
			char start_character = character;
			++lexer->str;
			character = *lexer->str;
			char* text = malloc(255);
			size_t length = 0;
			size_t max_length = 255;
			while (character && character != '\n' && character != start_character) {
				if (length == max_length) {
					max_length += 255;
					text = realloc(text, max_length);
				}
				text[length++] = character;
				++lexer->loc.column;
				++lexer->str;
				character = *lexer->str;
			}

			if (character == '\n' || character != start_character) {
				fprintf(stderr, "%s:%zu:%zu: error: expected '%c'\n",
						lexer->file, lexer->loc.line, lexer->loc.column, start_character);
				exit(1);
			}
			++lexer->str;

			if (length == max_length) text = realloc(text, max_length + 1);
			text[length] = 0;

			Token* token = token_new(text, length, start_loc, TOKENTYPE_STRING_LITERAL);
			if (start_character == '\'') token->type = TOKENTYPE_CHARACTER_LITERAL;
			return token;
		}
		else {
			char* text = malloc(255);
			size_t length = 0;
			size_t max_length = 255;
			while (character && !isspace(character) &&
					character != '+' && character != '-' && character != '*' && character != '/' && character != '%' &&
					character != '|' && character != '&' && character != '>' && character != '<' && character != '=' &&
					character != '(' && character != ')' && character != '{' && character != '}' && character != '\'' &&
					character != '"' && character != ';' && character != '!' && character != ',' && character != '[' &&
					character != ']') {
				if (length == max_length) {
					max_length += 255;
					text = realloc(text, max_length);
				}
				text[length++] = character;
				++lexer->loc.column;
				++lexer->str;
				character = *lexer->str;
			}
			if (length == max_length) text = realloc(text, max_length + 1);
			text[length] = 0;

			Token* token = token_new(text, length, start_loc, get_type(text));
			return token;
		}
	}
}

Token* token_new(char* text, size_t text_length, CodeLoc loc, TokenType type) {
	Token* token = malloc(sizeof(Token));
	token->text = text;
	token->text_length = text_length;
	token->loc = loc;
	token->type = type;
	return token;
}

void token_free(Token* token) {
	if (token) {
		if (token->text) free(token->text);
		free(token);
	}
}

CodeLoc codeloc_new(size_t line, size_t column) {
	CodeLoc loc = {.line = line, .column = column};
	return loc;
}