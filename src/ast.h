#pragma once

#include "lexer.h"

typedef enum {
	PRIMARY_NONE,
	PRIMARY_IDENTIFIER,
	PRIMARY_INT_LITERAL,
	PRIMARY_STRING_LITERAL,
	PRIMARY_CHAR_LITERAL
} PrimaryType;

typedef struct {
	PrimaryType type;
	union {
		int int_value;
		char* text;
	};
	CodeLoc loc;
} Primary;

typedef struct {
	CodeLoc loc;
	char* text;
} TextLoc;

typedef struct {
	TextLoc name;
	TextLoc type;
} Arg;

typedef struct {
	TextLoc name;
	TextLoc type;
	Arg* args;
	size_t arg_count;
} Prototype;

typedef struct Node Node;

typedef struct {
	Prototype prototype;
	Node** body;
	size_t body_length;
} Function;

typedef struct {
	Primary value;
	CodeLoc loc;
} Return;

typedef struct {
	TextLoc name;
	Primary type;
	Primary value;
} VariableDecl;

typedef enum {
	NODE_PRIMARY,
	NODE_FUNCTION,
	NODE_RETURN,
	NODE_FUNCTION_DECL,
	NODE_VARIABLE_DECL
} NodeType;

typedef struct Node {
	NodeType type;
	union {
		Primary primary;
		Function function;
		Return ret;
		Prototype function_decl;
		VariableDecl variable_decl;
	};
} Node;

Node* node_new(NodeType type);