#ifndef PARASITIC_C_FILE__0000018DC3D37320
#define PARASITIC_C_FILE__0000018DC3D37320
#include <stdlib.h>
#include <string.h>
#include "cheats.h"
struct List {
    struct ListNode* first;
    struct ListNode* last;
};
struct ListNode {
    void* data;
    struct ListNode* next;
};
fn(struct List list_init())
{
struct List list;
list.first = NULL;
list.last = NULL;
return list;
}
fn(void list_push(struct List* list, void* data))
{
struct ListNode* new_node = malloc(sizeof (*new_node));
new_node->data = data;
new_node->next = NULL;
if (list->first && list->last)
{
list->last->next = new_node;
list->last = new_node;
}
else
{
list->first = new_node;
list->last = new_node;
}
}
fn(bool list_is_empty(struct List* list))
{
return list->first == NULL;
}
fn(void* list_pop_front(struct List* list))
{
if (list_is_empty(list))
return NULL;
void* data = list->first->data;
list->first = list->first->next;
if (list->first == NULL)
list->last = NULL;
return data;
}
fn(bool list_contains_cstr(struct List* haystack, char* needle))
{
struct ListNode* node = haystack->first;
while (node)
{
char* element = (char*) node->data;
if (strcmp(element, needle) == 0)
return true;
node = node->next;
}
return false;
}
#endif // PARASITIC_C_FILE__0000000000000000
