#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

Primary* parse_primary(Parser* parser) {
	Token* token = *parser->tokens;
	if (token->type == TOKENTYPE_IDENTIFIER || token->type == TOKENTYPE_INT_LITERAL ||
	token->type == TOKENTYPE_STRING_LITERAL || token->type == TOKENTYPE_CHARACTER_LITERAL) {
		Primary* primary = malloc(sizeof(Primary));
		if (token->type == TOKENTYPE_IDENTIFIER) {
			primary->type = PRIMARY_IDENTIFIER;
			primary->text = malloc(token->text_length + 1);
			memcpy(primary->text, token->text, token->text_length + 1);
		}
		else if (token->type == TOKENTYPE_STRING_LITERAL) {
			primary->type = PRIMARY_STRING_LITERAL;
			primary->text = malloc(token->text_length + 1);
			memcpy(primary->text, token->text, token->text_length + 1);
		}
		else if (token->type == TOKENTYPE_CHARACTER_LITERAL) {
			primary->type = PRIMARY_CHAR_LITERAL;
			primary->text = malloc(token->text_length + 1);
			memcpy(primary->text, token->text, token->text_length + 1);
		}
		else {
			primary->type = PRIMARY_INT_LITERAL;
			primary->int_value = (int) strtol(token->text, NULL, 10);
		}
		primary->loc = token->loc;
		++parser->tokens;
		return primary;
	}
	else return NULL;
}

const char* string_from_type(TokenType type) {
	const char* string;
	switch (type) {
		case TOKENTYPE_LPAREN:
			string = "(";
			break;
		case TOKENTYPE_RPAREN:
			string = ")";
			break;
		case TOKENTYPE_LBRACE:
			string = "{";
			break;
		case TOKENTYPE_RBRACE:
			string = "}";
			break;
		case TOKENTYPE_INT_LITERAL:
			string = "int literal";
			break;
		case TOKENTYPE_IDENTIFIER:
			string = "identifier";
			break;
		case TOKENTYPE_PLUS:
			string = "+";
			break;
		case TOKENTYPE_MINUS:
			string = "-";
			break;
		case TOKENTYPE_STAR:
			string = "*";
			break;
		case TOKENTYPE_SLASH:
			string = "/";
			break;
		case TOKENTYPE_MODULO:
			string = "%";
			break;
		case TOKENTYPE_OR:
			string = "|";
			break;
		case TOKENTYPE_AND:
			string = "&";
			break;
		case TOKENTYPE_GT:
			string = ">";
			break;
		case TOKENTYPE_LT:
			string = "<";
			break;
		case TOKENTYPE_EQ:
			string = "=";
			break;
		case TOKENTYPE_SEMICOLON:
			string = ";";
			break;
		case TOKENTYPE_NOT:
			string = "!";
			break;
		case TOKENTYPE_COMMA:
			string = ",";
			break;
		case TOKENTYPE_EOF:
			string = "EOF";
			break;
		default:
			__builtin_unreachable();
	}
	return string;
}

typedef enum {
	SEVERITY_INFO,
	SEVERITY_WARNING,
	SEVERITY_ERROR
} Severity;

Token* parser_expect_type(Parser* parser, Severity severity, TokenType type, const char* msg) {
	Token* token = *parser->tokens;
	const char* severity_string;
	switch (severity) {
		case SEVERITY_INFO:
			severity_string = "info";
			break;
		case SEVERITY_WARNING:
			severity_string = "warning";
			break;
		case SEVERITY_ERROR:
			severity_string = "error";
			break;
		default:
			break;
	}
	if (token->type == type) {
		++parser->tokens;
		return token;
	}
	else if (token->type == TOKENTYPE_EOF) {
		fprintf(stderr, "../tests/test.c:%zu:%zu: %s: %s but got EOF\n",
				token->loc.line, token->loc.column, severity_string, msg);
	}
	else fprintf(stderr, "%s:%zu:%zu: %s: %s\n",
				 parser->file, token->loc.line, token->loc.column, severity_string, msg);
	if (severity == SEVERITY_ERROR)
		exit(EXIT_FAILURE);

	return token;
}
Token* parser_expect_string(Parser* parser, Severity severity, const char* str, const char* msg) {
	Token* token = *parser->tokens;
	const char* severity_string;
	switch (severity) {
		case SEVERITY_INFO:
			severity_string = "info";
			break;
		case SEVERITY_WARNING:
			severity_string = "warning";
			break;
		case SEVERITY_ERROR:
			severity_string = "error";
			break;
		default:
			break;
	}
	if (strcmp(token->text, str) == 0) {
		++parser->tokens;
		return token;
	}
	else if (token->type == TOKENTYPE_EOF) {
		fprintf(stderr, "../tests/test.c:%zu:%zu: %s: %s but got EOF\n",
		        token->loc.line, token->loc.column, severity_string, msg);
	}
	else fprintf(stderr, "%s:%zu:%zu: %s: %s\n",
	             parser->file, token->loc.line, token->loc.column, severity_string, msg);
	if (severity == SEVERITY_ERROR)
		exit(EXIT_FAILURE);

	return token;
}

Prototype parse_prototype(Parser* parser, Token name, Primary type) {
	TextLoc name_loc;
	name_loc.text = malloc(name.text_length + 1);
	memcpy(name_loc.text, name.text, name.text_length + 1);
	name_loc.loc = name.loc;

	TextLoc type_loc;
	type_loc.text = type.text;
	type_loc.loc = type.loc;

	// (
	// should already be checked before calling this function, but just to be sure
	assert((*parser->tokens)->type == TOKENTYPE_LPAREN);
	++parser->tokens;

	Token* token = *parser->tokens;
	Arg* args = malloc(8 * sizeof(Arg));
	size_t size = 0;
	size_t max_size = 8;
	while (token->type != TOKENTYPE_RPAREN && token->type != TOKENTYPE_EOF) {
		Token* arg_type = parser_expect_type(parser, SEVERITY_ERROR, TOKENTYPE_IDENTIFIER, "expected type");
		Token* arg_name = parser_expect_type(parser, SEVERITY_ERROR, TOKENTYPE_IDENTIFIER, "expected identifier");
		token = *parser->tokens;
		if (token->type != TOKENTYPE_RPAREN && token->type != TOKENTYPE_EOF) {
			parser_expect_type(parser, SEVERITY_ERROR, TOKENTYPE_COMMA, "expected ',' or ')'");
		}

		if (size == max_size) {
			max_size += 8;
			args = realloc(args, max_size * sizeof(Arg));
		}

		Arg arg;
		arg.name.text = malloc(arg_name->text_length + 1);
		memcpy(arg.name.text, arg_name->text, arg_name->text_length + 1);
		arg.name.loc = arg_name->loc;
		arg.type.text = malloc(arg_type->text_length + 1);
		memcpy(arg.type.text, arg_type->text, arg_type->text_length + 1);
		arg.type.loc = arg_type->loc;

		args[size++] = arg;
	}

	// )
	parser_expect_type(parser, SEVERITY_ERROR, TOKENTYPE_RPAREN, "expected ')' after function declaration");

	Prototype prototype = {.name = name_loc, .type = type_loc, .arg_count = size, .args = args};

	return prototype;
}

Node* parse_function(Parser* parser, Prototype prototype) {
	++parser->tokens;
	Token* token = *parser->tokens;
	Node** body = malloc(8 * sizeof(Token*));
	size_t max_size = 8;
	size_t size = 0;
	while (token->type != TOKENTYPE_RBRACE && token->type != TOKENTYPE_EOF) {
		if (size == max_size) {
			max_size *= 2;
			body = realloc(body, max_size * sizeof(Node*));
		}
		body[size++] = parser_parse(parser);
		token = *parser->tokens;
	}

	parser_expect_type(parser, SEVERITY_ERROR, TOKENTYPE_RBRACE, "expected '}' after function definition");

	Node* node = node_new(NODE_FUNCTION);
	node->function.prototype = prototype;
	node->function.body = body;
	node->function.body_length = size;

	return node;
}

Node* parse_variable(Parser* parser, Primary type) {
	Token* name = *parser->tokens;
	++parser->tokens;
	Token* next = *parser->tokens;
	if (next->type == TOKENTYPE_LPAREN) {
		Prototype prototype = parse_prototype(parser, *name, type);
		Token* token = *parser->tokens;
		if (token->type == TOKENTYPE_LBRACE) return parse_function(parser, prototype);
		else if (token->type == TOKENTYPE_SEMICOLON) {
			++parser->tokens;
			Node* node = node_new(NODE_FUNCTION_DECL);
			node->function_decl = prototype;
			return node;
		}
		else {
			fprintf(stderr,
					"%s:%zu:%zu: error: expected '{' or ';' after function declaration\n",
					parser->file, token->loc.line, token->loc.column);
			exit(EXIT_FAILURE);
		}
	}
	else if (next->type == TOKENTYPE_EQ) {
		++parser->tokens;
		Primary* value = parse_primary(parser);
		if (!value) {
			fprintf(stderr,
			        "%s:%zu:%zu: error: expected expression\n",
			        parser->file, (*parser->tokens)->loc.line, (*parser->tokens)->loc.column);
			exit(EXIT_FAILURE);
		}
		parser_expect_type(parser, SEVERITY_ERROR, TOKENTYPE_SEMICOLON, "expected ';'");

		Node* node = node_new(NODE_VARIABLE_DECL);
		TextLoc name_loc;
		name_loc.text = malloc(name->text_length + 1);
		memcpy(name_loc.text, name->text, name->text_length + 1);
		name_loc.loc = name->loc;
		node->variable_decl.name = name_loc;
		node->variable_decl.type = type;
		node->variable_decl.value = *value;
		return node;
	}
	else if (next->type == TOKENTYPE_SEMICOLON) {
		++parser->tokens;
		Node* node = node_new(NODE_VARIABLE_DECL);
		TextLoc name_loc;
		name_loc.text = malloc(name->text_length + 1);
		memcpy(name_loc.text, name->text, name->text_length + 1);
		name_loc.loc = name->loc;
		node->variable_decl.name = name_loc;
		node->variable_decl.type = type;
		node->variable_decl.value.type = PRIMARY_NONE;
		return node;
	}
	else {
		fprintf(stderr,
		        "%s:%zu:%zu: error: expected '(', '=' or ';'\n",
		        parser->file, next->loc.line, next->loc.column);
		exit(EXIT_FAILURE);
	}
}

Node* parse_return(Parser* parser, Primary keyword) {
	Primary* value = parse_primary(parser);
	if (!value) {
		fprintf(stderr,
		        "%s:%zu:%zu: error: expected expression after 'return'\n",
		        parser->file, (*parser->tokens)->loc.line, (*parser->tokens)->loc.column);
		exit(EXIT_FAILURE);
	};

	parser_expect_type(parser, SEVERITY_ERROR, TOKENTYPE_SEMICOLON, "expected ';'");

	Node* node = node_new(NODE_RETURN);
	node->ret.value = *value;
	node->ret.loc = keyword.loc;

	return node;
}

Node* parser_parse(Parser* parser) {
	Primary* primary = parse_primary(parser);
	if (!primary) return NULL;

	Token* token = *parser->tokens;
	if (token->type == TOKENTYPE_IDENTIFIER) {
		return parse_variable(parser, *primary);
	}
	else {
		if (primary->type == PRIMARY_IDENTIFIER && strcmp(primary->text, "return") == 0) {
			return parse_return(parser, *primary);
		}
		__builtin_trap();
	}
}

Parser parser_new(Token** tokens, size_t tokens_size, const char* file) {
	Parser parser = {.tokens = tokens, .tokens_size = tokens_size, .file = file};
	return parser;
}

#pragma clang diagnostic pop