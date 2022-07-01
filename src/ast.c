#include "ast.h"
#include <stdlib.h>

Node* node_new(NodeType type) {
	Node* node = malloc(sizeof(Node));
	node->type = type;
	return node;
}