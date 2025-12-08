#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PROCESSES 100
#define MAX_NAME_LENGTH 50

typedef enum
{
    STATE_READY,
    STATE_RUNNING,
    STATE_WAITING,
    STATE_TERMINATED
} ProcessState;

typedef struct
{
    int pid;
    char name[MAX_NAME_LENGTH];
    int burstTime;
    int ioStartTime;
    int ioDuration;
    int timeExecuted;
    int ioTimeLeft;
    int waitingTime;
    int turnaroundTime;
    int completionTime;
    ProcessState state;
    int isKilled;
} ProcessControlBlock;

typedef struct QueueNode
{
    int pid;
    struct QueueNode* next;
} QueueNode;

typedef struct
{
    QueueNode* front;
    QueueNode* rear;
} ProcessQueue;

ProcessControlBlock* gProcessMap[MAX_PROCESSES];
int gTotalProcesses = 0;

ProcessQueue gReadyQueue;
ProcessQueue gWaitingQueue;
ProcessQueue gTerminatedQueue;

int gKillProcessIds[MAX_PROCESSES];
int gKillTimes[MAX_PROCESSES];
int gTotalKills = 0;

void initializeQueue(ProcessQueue* queue)
{
    queue->front = NULL;
    queue->rear = NULL;
}

void enqueueProcess(ProcessQueue* queue, int pid)
{
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    newNode->pid = pid;
    newNode->next = NULL;
    
    if (queue->rear == NULL)
    {
        queue->front = newNode;
        queue->rear = newNode;
    }
    else
    {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

int dequeueProcess(ProcessQueue* queue)
{
    if (queue->front == NULL)
    {
        return -1;
    }
    
    QueueNode* tempNode = queue->front;
    int pid = tempNode->pid;
    queue->front = queue->front->next;
    
    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }
    
    free(tempNode);
    return pid;
}

int isQueueEmpty(ProcessQueue* queue)
{
    return (queue->front == NULL);
}

ProcessControlBlock* findProcessByPid(int pid)
{
    int i;
    for (i = 0; i < gTotalProcesses; i = i + 1)
    {
        if (gProcessMap[i]->pid == pid)
        {
            return gProcessMap[i];
        }
    }
    return NULL;
}

void removeProcessFromQueue(ProcessQueue* queue, int pid)
{
    if (isQueueEmpty(queue))
    {
        return;
    }
    
    QueueNode* currentNode = queue->front;
    QueueNode* previousNode = NULL;
    
    while (currentNode != NULL)
    {
        if (currentNode->pid == pid)
        {
            if (previousNode == NULL)
            {
                queue->front = currentNode->next;
                if (queue->front == NULL)
                {
                    queue->rear = NULL;
                }
            }
            else
            {
                previousNode->next = currentNode->next;
                if (currentNode->next == NULL)
                {
                    queue->rear = previousNode;
                }
            }
            free(currentNode);
            return;
        }
        previousNode = currentNode;
        currentNode = currentNode->next;
    }
}

void handleKillEvents(int currentTime, int* runningPid)
{
    int i;
    for (i = 0; i < gTotalKills; i = i + 1)
    {
        if (gKillTimes[i] == currentTime)
        {
            int targetPid = gKillProcessIds[i];
            ProcessControlBlock* process = findProcessByPid(targetPid);
            
            if (process != NULL && process->isKilled == 0)
            {
                process->isKilled = 1;
                process->completionTime = currentTime;
                
                if (*runningPid == targetPid)
                {
                    *runningPid = -1;
                }
                
                removeProcessFromQueue(&gReadyQueue, targetPid);
                removeProcessFromQueue(&gWaitingQueue, targetPid);
                
                process->state = STATE_TERMINATED;
                enqueueProcess(&gTerminatedQueue, targetPid);
            }
        }
    }
}

void updateIoOperations(int currentTime)
{
    QueueNode* node = gWaitingQueue.front;
    int completedIoPids[MAX_PROCESSES];
    int completedCount = 0;
    
    while (node != NULL)
    {
        ProcessControlBlock* process = findProcessByPid(node->pid);
        if (process != NULL && process->isKilled == 0)
        {
            process->ioTimeLeft = process->ioTimeLeft - 1;
            
            if (process->ioTimeLeft <= 0)
            {
                completedIoPids[completedCount] = node->pid;
                completedCount = completedCount + 1;
            }
        }
        node = node->next;
    }
    
    int i;
    for (i = 0; i < completedCount; i = i + 1)
    {
        ProcessControlBlock* process = findProcessByPid(completedIoPids[i]);
        removeProcessFromQueue(&gWaitingQueue, completedIoPids[i]);
        process->state = STATE_READY;
        enqueueProcess(&gReadyQueue, completedIoPids[i]);
    }
}

void runScheduler()
{
    int systemClock = 0;
    int currentRunningPid = -1;
    
    while (isQueueEmpty(&gReadyQueue) == 0 || 
           isQueueEmpty(&gWaitingQueue) == 0 || 
           currentRunningPid != -1)
    {
        handleKillEvents(systemClock, &currentRunningPid);
        
        if (currentRunningPid == -1 && isQueueEmpty(&gReadyQueue) == 0)
        {
            currentRunningPid = dequeueProcess(&gReadyQueue);
            ProcessControlBlock* process = findProcessByPid(currentRunningPid);
            
            if (process != NULL && process->isKilled == 0)
            {
                process->state = STATE_RUNNING;
            }
            else
            {
                currentRunningPid = -1;
            }
        }
        
        if (currentRunningPid != -1)
        {
            ProcessControlBlock* process = findProcessByPid(currentRunningPid);
            
            if (process != NULL && process->isKilled == 0)
            {
                process->timeExecuted = process->timeExecuted + 1;
                
                if (process->ioStartTime > 0 && 
                    process->timeExecuted == process->ioStartTime && 
                    process->ioDuration > 0)
                {
                    process->state = STATE_WAITING;
                    process->ioTimeLeft = process->ioDuration;
                    enqueueProcess(&gWaitingQueue, currentRunningPid);
                    currentRunningPid = -1;
                }
                else if (process->timeExecuted >= process->burstTime)
                {
                    process->state = STATE_TERMINATED;
                    process->completionTime = systemClock + 1;
                    process->turnaroundTime = process->completionTime;
                    enqueueProcess(&gTerminatedQueue, currentRunningPid);
                    currentRunningPid = -1;
                }
            }
            else
            {
                currentRunningPid = -1;
            }
        }
        
        QueueNode* readyNode = gReadyQueue.front;
        while (readyNode != NULL)
        {
            ProcessControlBlock* process = findProcessByPid(readyNode->pid);
            if (process != NULL && process->isKilled == 0)
            {
                process->waitingTime = process->waitingTime + 1;
            }
            readyNode = readyNode->next;
        }
        
        updateIoOperations(systemClock + 1);
        
        sleep(1);
        systemClock = systemClock + 1;
        
        if (systemClock > 1000)
        {
            break;
        }
    }
}

void displayResults()
{
    int i;
    
    printf("\n%-6s %-15s %-6s %-6s %-12s %-10s %-10s\n",
           "PID", "Name", "CPU", "IO", "Status", "Turnaround", "Waiting");
    printf("-------------------------------------------------------------------\n");
    
    for (i = 0; i < gTotalProcesses; i = i + 1)
    {
        ProcessControlBlock* process = gProcessMap[i];
        
        if (process->isKilled == 1)
        {
            printf("%-6d %-15s %-6d %-6d KILLED at %-2d %-10s %-10s\n",
                   process->pid,
                   process->name,
                   process->burstTime,
                   process->ioDuration,
                   process->completionTime,
                   "-",
                   "-");
        }
        else
        {
            printf("%-6d %-15s %-6d %-6d %-12s %-10d %-10d\n",
                   process->pid,
                   process->name,
                   process->burstTime,
                   process->ioDuration,
                   "OK",
                   process->turnaroundTime,
                   process->waitingTime);
        }
    }
}

int main()
{
    initializeQueue(&gReadyQueue);
    initializeQueue(&gWaitingQueue);
    initializeQueue(&gTerminatedQueue);
    
    int numberOfProcesses;
    printf("Enter number of processes: ");
    scanf("%d", &numberOfProcesses);
    
    printf("Enter process details (name pid burst_time io_start_time io_duration):\n");
    
    int i;
    for (i = 0; i < numberOfProcesses; i = i + 1)
    {
        ProcessControlBlock* newProcess = (ProcessControlBlock*)malloc(sizeof(ProcessControlBlock));
        
        scanf("%s %d %d %d %d",
              newProcess->name,
              &newProcess->pid,
              &newProcess->burstTime,
              &newProcess->ioStartTime,
              &newProcess->ioDuration);
        
        newProcess->timeExecuted = 0;
        newProcess->ioTimeLeft = 0;
        newProcess->waitingTime = 0;
        newProcess->turnaroundTime = 0;
        newProcess->completionTime = 0;
        newProcess->state = STATE_READY;
        newProcess->isKilled = 0;
        
        gProcessMap[gTotalProcesses] = newProcess;
        gTotalProcesses = gTotalProcesses + 1;
        
        enqueueProcess(&gReadyQueue, newProcess->pid);
    }
    
    printf("Enter number of KILL events (0 if none): ");
    scanf("%d", &gTotalKills);
    
    if (gTotalKills > 0)
    {
        printf("Enter KILL events (pid kill_time):\n");
        for (i = 0; i < gTotalKills; i = i + 1)
        {
            scanf("%d %d", &gKillProcessIds[i], &gKillTimes[i]);
        }
    }
    
    runScheduler();
    displayResults();
    
    for (i = 0; i < gTotalProcesses; i = i + 1)
    {
        free(gProcessMap[i]);
    }
    
    return 0;
}