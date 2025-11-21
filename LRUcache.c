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

LRUCache *gCache;

unsigned int hash(int k){
    return (unsigned int)k % 10000;
}

void removeNode(Node *node){
    if(node->prev) {
        node->prev->next = node->next;
    }
    if(node->next) {
        node->next->prev = node->prev;
    }
    if(node == gCache->head) {
        gCache->head = node->next;
    }
    if(node == gCache->tail) {
        gCache->tail = node->prev;
    }
}

void insertFront(Node *node){
    node->prev = NULL;
    node->next = gCache->head;
    if(gCache->head) {
        gCache->head->prev = node;
    }
    gCache->head = node;
    if(gCache->tail == NULL) {
        gCache->tail = node;
    }
}

void createCache(int capacity){
    gCache = malloc(sizeof(LRUCache));
    gCache->capacity = capacity;
    gCache->size = 0;
    gCache->head = gCache->tail = NULL;
    for(int i = 0; i < 10000; i++) {
        gCache->map[i] = NULL;
    }
}

char* get(int key){
    unsigned int hashIndex = hash(key);
    Node *node = gCache->map[hashIndex];
    while(node && node->key != key) {
        node = node->next;
    }
    if(!node) {
        return "NULL";
    }
    removeNode(node);
    insertFront(node);
    return node->value;
}

void put(int key, char *value){
    unsigned int hashIndex = hash(key);
    Node *node = gCache->map[hashIndex];
    while(node && node->key != key) {
        node = node->next;
    }
    if(node){
        strcpy(node->value, value);
        removeNode(node);
        insertFront(node);
        return;
    }
    if(gCache->size == gCache->capacity){
        Node *nodeToDelete = gCache->tail;
        unsigned int delHash = hash(nodeToDelete->key);
        Node *bucketNode = gCache->map[delHash];
        Node *prevBucketNode = NULL;
        while(bucketNode && bucketNode->key != nodeToDelete->key) {
            prevBucketNode = bucketNode;
            bucketNode = bucketNode->next;
        }
        if(prevBucketNode) {
            prevBucketNode->next = bucketNode->next;
        } else {
            gCache->map[delHash] = bucketNode->next;
        }
        removeNode(nodeToDelete);
        free(nodeToDelete);
        gCache->size--;
    }
    Node *newNode = malloc(sizeof(Node));
    newNode->key = key;
    strcpy(newNode->value, value);
    newNode->prev = newNode->next = NULL;
    insertFront(newNode);
    newNode->next = gCache->map[hashIndex];
    gCache->map[hashIndex] = newNode;
    gCache->size++;
}

int main(){
    char cmd[20];
    while(scanf("%s", cmd) != EOF){
        if(strcmp(cmd, "createCache") == 0){
            int capacity; scanf("%d", &capacity);
            createCache(capacity);
        }
        else if(strcmp(cmd, "put") == 0){
            int key; char valueBuf[101];
            scanf("%d %s", &key, valueBuf);
            put(key, valueBuf);
        }
        else if(strcmp(cmd, "get") == 0){
            int key; scanf("%d", &key);
            printf("%s\n", get(key));
        }
        else if(strcmp(cmd, "exit") == 0){
            break;
        }
    }
    return 0;
}