#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 128
#define MAX_NAME 50

typedef struct FreeBlock 
{
    int index;
    struct FreeBlock *next;
} FreeBlock;

typedef struct File 
{
    char name[MAX_NAME];
    char *data;
    int size;
    struct File *next;
} File;

typedef struct Directory 
{
    char name[MAX_NAME];
    struct Directory *parent;
    struct Directory *subdirs;
    struct Directory *next;
    File *files;
} Directory;

char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];
FreeBlock *gFreeList = NULL;
Directory *gRoot = NULL;
Directory *gCwd = NULL;
int gFreeCount = 0;

void initVfs() 
{
    gFreeList = NULL;
    int i;
    for (i = NUM_BLOCKS - 1; i >= 0; i--) 
    {
        FreeBlock *newBlock = malloc(sizeof(FreeBlock));
        if (newBlock == NULL) 
        {
            return;
        }
        newBlock->index = i;
        newBlock->next = gFreeList;
        gFreeList = newBlock;
    }
    gFreeCount = NUM_BLOCKS;

    gRoot = malloc(sizeof(Directory));
    if (gRoot == NULL) 
    {
        return;
    }
    strcpy(gRoot->name, "/");
    gRoot->parent = NULL;
    gRoot->subdirs = NULL;
    gRoot->files = NULL;
    gRoot->next = NULL;
    gCwd = gRoot;

    printf("Simple VFS initialized.Type exit to quit.\n");
}

FreeBlock* allocateBlock() 
{
    if (!gFreeList) return NULL;
    FreeBlock *blk = gFreeList;
    gFreeList = gFreeList->next;
    gFreeCount--;
    return blk;
}

void freeBlock(int index) 
{
    FreeBlock *blk = malloc(sizeof(FreeBlock));
    if (blk == NULL) 
    {
        return;
    }
    blk->index = index;
    blk->next = gFreeList;
    gFreeList = blk;
    gFreeCount++;
}

void cmdPwd() 
{
    char path[512] = "";
    Directory *cur = gCwd;
    while (cur != NULL && cur != gRoot) 
    {
        char temp[512];
        sprintf(temp, "/%s%s", cur->name, path);
        strcpy(path, temp);
        cur = cur->parent;
    }
    printf("%s\n", strlen(path) ? path : "/");
}

void cmdLs() 
{
    Directory *d = gCwd->subdirs;
    File *f = gCwd->files;
    if (!d && !f) 
    {
        printf("(empty)\n");
        return;
    }
    while (d) 
    {
        printf("%s/\n", d->name);
        d = d->next;
    }
    while (f) 
    {
        printf("%s\n", f->name);
        f = f->next;
    }
}

void cmdMkdir(const char *name) 
{
    if (name == NULL || strlen(name) == 0) 
    {
        printf("mkdir:invalid name\n");
        return;
    }
    
    Directory *newDir = malloc(sizeof(Directory));
    if (newDir == NULL) 
    {
        printf("mkdir: memory allocation failed\n");
        return;
    }
    strcpy(newDir->name, name);
    newDir->subdirs = NULL;
    newDir->files = NULL;
    newDir->next = NULL;
    newDir->parent = gCwd;

    if (!gCwd->subdirs)
        gCwd->subdirs = newDir;
    else 
    {
        Directory *temp = gCwd->subdirs;
        while (temp->next) temp = temp->next;
        temp->next = newDir;
    }
    printf("Directory'%s' created.\n", name);
}

void cmdCd(const char *name) 
{
    if (name == NULL) 
    {
        printf("cd: invalid name\n");
        return;
    }
    
    if (strcmp(name, "..") == 0) 
    {
        if (gCwd->parent) 
            gCwd = gCwd->parent;
        return;
    }
    Directory *temp = gCwd->subdirs;
    while (temp) 
    {
        if (strcmp(temp->name, name) == 0) 
        {
            gCwd = temp;
            return;
        }
        temp = temp->next;
    }
    printf("cd:no such directory: %s\n", name);
}

void cmdCreate(const char *name) 
{
    if (name == NULL || strlen(name) == 0) 
    {
        printf("create: invalid name\n");
        return;
    }
    
    File *newFile = malloc(sizeof(File));
    if (newFile == NULL) 
    {
        printf("create:memory allocation failed\n");
        return;
    }
    strcpy(newFile->name, name);
    newFile->data = NULL;
    newFile->size = 0;
    newFile->next = NULL;

    if (!gCwd->files)
        gCwd->files = newFile;
    else 
    {
        File *temp = gCwd->files;
        while (temp->next) 
            temp = temp->next;
        temp->next = newFile;
    }
    printf("File'%s' created.\n", name);
}

void cmdWrite(const char *name, const char *content) 
{
    if (name == NULL || content == NULL) 
    {
        printf("write: invalid arguments\n");
        return;
    }
    
    File *f = gCwd->files;
    while (f) 
    {
        if (strcmp(f->name, name) == 0) 
        {
            int len = strlen(content);
            f->size = len;
            f->data = malloc(f->size + 1);
            if (f->data == NULL) 
            {
                printf("write:memory allocation failed\n");
                return;
            }
            strcpy(f->data, content);
            printf("Data written to'%s'.\n", name);
            return;
        }
        f = f->next;
    }
    printf("write: file not found\n");
}

void cmdRead(const char *name) 
{
    if (name == NULL) 
    {
        printf("read: invalid name\n");
        return;
    }
    
    File *f = gCwd->files;
    while (f) 
    {
        if (strcmp(f->name, name) == 0) 
        {
            if(f->data)
                printf("%s\n", f->data);
            else
                printf("(empty)\n");
            return;
        }
        f = f->next;
    }
    printf("read:file not found\n");
}

void cmdDelete(const char *name) 
{
    if (name == NULL) 
    {
        printf("delete: invalid name\n");
        return;
    }
    
    File *f = gCwd->files;
    File *prev = NULL;
    while (f) 
    {
        if (strcmp(f->name, name) == 0) 
        {
            if (prev) 
                prev->next = f->next;
            else 
                gCwd->files = f->next;
            if(f->data != NULL)
                free(f->data);
            free(f);
            printf("File '%s'deleted.\n", name);
            return;
        }
        prev = f;
        f = f->next;
    }
    printf("delete:file not found\n");
}

void cmdRmdir(const char *name) 
{
    if (name == NULL || strlen(name) == 0) 
    {
        printf("rmdir: invalid name\n");
        return;
    }
    
    Directory *d = gCwd->subdirs;
    Directory *prev = NULL;
    while (d) 
    {
        if (strcmp(d->name, name) == 0) 
        {
            if (d->subdirs != NULL || d->files != NULL) 
            {
                printf("rmdir:directory not empty\n");
                return;
            }
            if (prev) 
                prev->next = d->next;
            else 
                gCwd->subdirs = d->next;
            free(d);
            printf("Directory'%s' removed.\n", name);
            return;
        }
        prev = d;
        d = d->next;
    }
    printf("rmdir: directory not found\n");
}

void cmdDf() 
{
    int used = NUM_BLOCKS - gFreeCount;
    printf("Total Blocks: %d\nFree Blocks: %d\nUsed Blocks: %d\n", NUM_BLOCKS, gFreeCount, used);
}

int main() 
{
    initVfs();
    char cmd[100];
    char arg1[50];
    char arg2[200];

    while (1) 
    {
        if(gCwd == gRoot)
            printf("/> ");
        else
            printf("%s> ", gCwd->name);
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = 0;

        if (strncmp(cmd, "exit", 4) == 0) 
            break;
        else if (strncmp(cmd, "ls", 2) == 0) 
            cmdLs();
        else if (strncmp(cmd, "pwd", 3) == 0) 
            cmdPwd();
        else if (sscanf(cmd, "mkdir %s", arg1) == 1) 
            cmdMkdir(arg1);
        else if (sscanf(cmd, "cd %s", arg1) == 1) 
            cmdCd(arg1);
        else if (sscanf(cmd, "create %s", arg1) == 1) 
            cmdCreate(arg1);
        else if (sscanf(cmd, "write %s %[^\n]", arg1, arg2) == 2) 
            cmdWrite(arg1, arg2);
        else if (sscanf(cmd, "read %s", arg1) == 1) 
            cmdRead(arg1);
        else if (sscanf(cmd, "delete %s", arg1) == 1) 
            cmdDelete(arg1);
        else if (sscanf(cmd, "rmdir %s", arg1) == 1) 
            cmdRmdir(arg1);
        else if (strncmp(cmd, "df", 2) == 0) 
            cmdDf();
        else 
            printf("Unknown command:%s\n", cmd);
    }

    printf("Exiting VFS...\n");
    return 0;
}