#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define declare_list_element(type)                                             \
    typedef struct ListElement_##type ListElement_##type;                      \
    struct ListElement_##type {                                                \
        type data;                                                             \
        struct ListElement_##type* next;                                       \
        struct ListElement_##type* prev;                                       \
    }
#define declare_list(type)                                                     \
    declare_list_element(type);                                                \
    typedef struct List_##type List_##type;                                    \
    struct List_##type {                                                       \
        struct ListElement_##type* first;                                      \
        struct ListElement_##type* last;                                       \
        size_t size;                                                           \
    }

#define List(type)                                                             \
    declare_list(type);                                                        \
    List_##type list_create_##type(void);                                      \
    void list_destroy_##type(List_##type* self);                               \
    bool list_append_##type(List_##type* self, type element);                  \
    bool list_insert_##type(List_##type* self, size_t index, type element);    \
    type list_pop_##type(List_##type* self);                                   \
    bool list_remove_##type(List_##type* self, type* result, size_t index);

#define ListImplementation(type)                                               \
    List_##type list_create_##type(void) {                                     \
        List_##type list = { NULL, NULL, 0 };                                  \
        return list;                                                           \
    }                                                                          \
    void list_destroy_##type(List_##type* self) {                              \
        self->size = 0;                                                        \
        self->last = NULL;                                                     \
        ListElement_##type* current = self->first;                             \
        while (current) {                                                      \
            ListElement_##type* next = current->next;                          \
            free(current);                                                     \
            current = next;                                                    \
        }                                                                      \
    }                                                                          \
    bool list_append_##type(List_##type* self, type element) {                 \
        ListElement_##type* next = malloc(sizeof(ListElement_##type));         \
        if (!next) {                                                           \
            return false;                                                      \
        }                                                                      \
        next->data = element;                                                  \
        next->prev = self->last;                                               \
        next->next = NULL;                                                     \
        if (next->prev) {                                                      \
            next->prev->next = next;                                           \
            self->last = next;                                                 \
        } else {                                                               \
            self->first = next;                                                \
            self->last = next;                                                 \
        }                                                                      \
        self->size += 1;                                                       \
        return true;                                                           \
    }                                                                          \
    bool list_insert_##type(List_##type* self, size_t index, type element) {   \
        if (self->size < index) {                                              \
            return false;                                                      \
        };                                                                     \
        ListElement_##type* next = malloc(sizeof(ListElement_##type));         \
        if (!next) {                                                           \
            return false;                                                      \
        }                                                                      \
        next->data = element;                                                  \
        next->prev = self->last;                                               \
        next->next = NULL;                                                     \
        ListElement_##type* current = self->first;                             \
        while (index) {                                                        \
            current = current->next;                                           \
            index--;                                                           \
        }                                                                      \
        next->next = current;                                                  \
        if (next->next == NULL) {                                              \
            self->last->next = next;                                           \
            next->prev = self->last;                                           \
            self->last = next;                                                 \
        } else {                                                               \
            next->prev = current->prev;                                        \
            current->prev = next;                                              \
        }                                                                      \
        if (next->prev == NULL) {                                              \
            self->first = next;                                                \
        } else {                                                               \
            next->prev->next = next;                                           \
        }                                                                      \
        self->size++;                                                          \
        return true;                                                           \
    }                                                                          \
    type list_pop_##type(List_##type* self) {                                  \
        type res = self->last->data;                                           \
        ListElement_##type* current = self->last;                              \
        self->last = current->prev;                                            \
        self->last->next = NULL;                                               \
        free(current);                                                         \
        self->size--;                                                          \
        return res;                                                            \
    }                                                                          \
    bool list_remove_##type(List_##type* self, type* result, size_t index) {   \
        if (self->size <= index) {                                             \
            return false;                                                      \
        }                                                                      \
        ListElement_##type* current = self->first;                             \
        while (index) {                                                        \
            current = current->next;                                           \
            index--;                                                           \
        }                                                                      \
        if (current->prev) {                                                   \
            current->prev->next = current->next;                               \
        } else {                                                               \
            self->first = current->next;                                       \
        }                                                                      \
        if (current->next) {                                                   \
            current->next->prev = current->prev;                               \
        } else {                                                               \
            self->last = current->prev;                                        \
        }                                                                      \
        *result = current->data;                                               \
        free(current);                                                         \
        self->size--;                                                          \
        return true;                                                           \
    }

// TODO remove the current element in the remove function
