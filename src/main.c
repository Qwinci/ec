#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include <stdbool.h>

int main(int argc, char* argv[]) {
	if (argc == 1) {
		fprintf(stderr, "usage: ec file");
		exit(EXIT_FAILURE);
	}
	FILE* file = fopen(argv[1], "r");
	if (!file) {
		fprintf(stderr, "file %s doesn't exist", argv[1]);
		exit(EXIT_FAILURE);
	}
	fseek(file, 0, SEEK_END);
	size_t file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	void* buffer = malloc(file_size);
	fread(buffer, file_size, 1, file);
	fclose(file);

	Lexer lexer = lexer_new(buffer, argv[1]);

	size_t size = 0;
	size_t max_size = 8;
	Token** tokens = malloc(8 * sizeof(Token*));
	Token* token = NULL;
	while (true) {
		token = lexer_next(&lexer);
		if (!token) break;
		if (size == max_size) {
			max_size *= 2;
			tokens = realloc(tokens, max_size * sizeof(Token*));
		}
		tokens[size++] = token;
		printf("%zu:%zu: %s\n", token->loc.line + 1, token->loc.column, token->text);
	}
	if (size == max_size) {
		++max_size;
		tokens = realloc(tokens, max_size * sizeof(Token*));
	}
	tokens[size++] = token_new(NULL, 0, lexer.loc, TOKENTYPE_EOF);

	Parser parser = parser_new(tokens, size, argv[1]);

	Node* node = parser_parse(&parser);

	for (size_t i = 0; i < size; ++i) free(tokens[i]);
	free(tokens);

	return 0;
}
