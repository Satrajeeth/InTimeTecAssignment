#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    int key;
    char value[101];
    struct Node *prev, *next;
} Node;

typedef struct {
    int capacity;
    int size;
    Node *head, *tail;
    Node *map[10000];
} LRUCache;

LRUCache *cache;

unsigned int hash(int k){
    return (unsigned int)k % 10000;
}

void removeNode(Node *n){
    if(n->prev) n->prev->next = n->next;
    if(n->next) n->next->prev = n->prev;
    if(n == cache->head) cache->head = n->next;
    if(n == cache->tail) cache->tail = n->prev;
}

void insertFront(Node *n){
    n->prev = NULL;
    n->next = cache->head;
    if(cache->head) cache->head->prev = n;
    cache->head = n;
    if(cache->tail == NULL) cache->tail = n;
}

void createCache(int capacity){
    cache = malloc(sizeof(LRUCache));
    cache->capacity = capacity;
    cache->size = 0;
    cache->head = cache->tail = NULL;
    for(int i=0;i<10000;i++) cache->map[i] = NULL;
}

char* get(int key){
    unsigned int h = hash(key);
    Node *n = cache->map[h];
    while(n && n->key != key) n = n->next;
    if(!n) return "NULL";
    removeNode(n);
    insertFront(n);
    return n->value;
}

void put(int key, char *value){
    unsigned int h = hash(key);
    Node *n = cache->map[h];
    Node *prev = NULL;
    while(n && n->key != key){ prev = n; n = n->next; }
    if(n){
        strcpy(n->value, value);
        removeNode(n);
        insertFront(n);
        return;
    }
    if(cache->size == cache->capacity){
        Node *del = cache->tail;
        unsigned int hh = hash(del->key);
        Node *t = cache->map[hh], *tp = NULL;
        while(t && t->key != del->key){ tp = t; t = t->next; }
        if(tp) tp->next = t->next;
        else cache->map[hh] = t->next;
        removeNode(del);
        free(del);
        cache->size--;
    }
    Node *newNode = malloc(sizeof(Node));
    newNode->key = key;
    strcpy(newNode->value, value);
    newNode->prev = newNode->next = NULL;
    insertFront(newNode);
    newNode->next = cache->map[h];
    cache->map[h] = newNode;
    cache->size++;
}

int main(){
    char cmd[20];
    while(scanf("%s", cmd) != EOF){
        if(strcmp(cmd, "createCache") == 0){
            int c; scanf("%d", &c);
            createCache(c);
        }
        else if(strcmp(cmd, "put") == 0){
            int k; char v[101];
            scanf("%d %s", &k, v);
            put(k, v);
        }
        else if(strcmp(cmd, "get") == 0){
            int k; scanf("%d", &k);
            printf("%s\n", get(k));
        }
        else if(strcmp(cmd, "exit") == 0){
            break;
        }
    }
    return 0;
}