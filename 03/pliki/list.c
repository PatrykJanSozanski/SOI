#include <stdlib.h>
#include <stdio.h>

typedef struct node {
    char* val;
    struct node * next;
} node_t;

node_t* init(char* msg){
    node_t * head = NULL;
    head = malloc(sizeof(node_t));

    head->val = msg;
    head->next = NULL;
    return head;
}

void print_list(node_t * head) {
    node_t * current = head;

    while (current != NULL) {
        printf(" ### %s\n", current->val);
        current = current->next;
    }
}

void push_end(node_t * head, char* val) {
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = malloc(sizeof(node_t));
    current->next->val = val;
    current->next->next = NULL;
}

void push_begin(node_t ** head, char* val) {
    node_t * new_node;
    new_node = malloc(sizeof(node_t));

    new_node->val = val;
    new_node->next = *head;
    *head = new_node;
}

char* pop(node_t** head) {
    char* retval;
    node_t * next_node = NULL;

    if (*head == NULL) {
        return "__ERROR__ Wiadomość usunięta przez Admina.";
    }

    next_node = (*head)->next;
    retval = (*head)->val;
    free(*head);
    *head = next_node;

    return retval;
}

char* readHead(node_t** head) {
    char* retval;
    node_t * next_node = NULL;

    if (*head == NULL) {
        return "__ERROR__ queue empty";
    }

    retval = (*head)->val;

    return retval;
}

char* remove_last(node_t * head) {
    char* retval;
    /* if there is only one item in the list, remove it */
    if (head->next == NULL) {
        retval = head->val;
        free(head);
        return retval;
    }

    /* get to the second to last node in the list */
    node_t * current = head;
    while (current->next->next != NULL) {
        current = current->next;
    }

    /* now current points to the second to last item of the list, so let's remove current->next */
    retval = current->next->val;
    free(current->next);
    current->next = NULL;
    return retval;

}
