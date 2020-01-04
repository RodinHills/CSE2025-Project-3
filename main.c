#include "binomial.h"
#include "fatal.h"
#include <string.h>
#include <math.h>

typedef struct BinNode *Position;

struct BinNode {
    char *processName;
    int constantExeTime;
    int exeTime;
    int arrTime;
    int waitTime;
    int isOnHeap;
    ElementType priority;
    Position    LeftChild;
    Position    NextSibling;
    Position    LLNext; // For linked-list purpose.
};

struct Collection {
    int CurrentSize;
    BinTree TheTrees[ MaxTrees ];
};

BinQueue Initialize( void ) {
    BinQueue H;
    int i;

    H = malloc( sizeof( struct Collection ) );
    if( H == NULL )
        FatalError( "Out of space!!!" );
    H->CurrentSize = 0;
    for( i = 0; i < MaxTrees; i++ )
        H->TheTrees[ i ] = NULL;
    return H;
}

static void DestroyTree( BinTree T ) {
    if( T != NULL ) {
        DestroyTree( T->LeftChild );
        DestroyTree( T->NextSibling );
        free( T );
    }
}

void Destroy( BinQueue H ) {
    int i;

    for( i = 0; i < MaxTrees; i++ )
        DestroyTree( H->TheTrees[ i ] );
}

BinQueue MakeEmpty( BinQueue H ) {
    int i;
    Destroy( H );
    for( i = 0; i < MaxTrees; i++ )
        H->TheTrees[ i ] = NULL;
    H->CurrentSize = 0;
    return H;
}

BinQueue Insert( Position process, BinQueue H ) {
    BinQueue OneItem;
    OneItem = Initialize( );
    OneItem->CurrentSize = 1;
    OneItem->TheTrees[ 0 ] = process;
    return Merge( H, OneItem );
}

Position DeleteMin( BinQueue H ) {
    int i, j;
    int MinTree;
    int MinArrival;
    BinQueue DeletedQueue;
    Position DeletedTree, OldRoot;
    ElementType MinPriority;
    if( IsEmpty( H ))
        Error( "Empty binomial queue" );

    MinPriority = Infinity;
    for( i = 0; i < MaxTrees; i++ ){
        if( H->TheTrees[ i ] && H->TheTrees[ i ]->priority < MinPriority ) {
            MinPriority = H->TheTrees[ i ]->priority;
            MinTree = i;
        }
    }

    DeletedTree = H->TheTrees[ MinTree ];
    OldRoot = DeletedTree;
    DeletedTree = DeletedTree->LeftChild;

    DeletedQueue = Initialize( );
    DeletedQueue->CurrentSize = ( 1 << MinTree ) - 1;
    for( j = MinTree - 1; j >= 0; j-- ) {
        DeletedQueue->TheTrees[ j ] = DeletedTree;
        DeletedTree = DeletedTree->NextSibling;
        DeletedQueue->TheTrees[ j ]->NextSibling = NULL;
    }

    H->TheTrees[ MinTree ] = NULL;
    H->CurrentSize -= DeletedQueue->CurrentSize + 1;

    Merge( H, DeletedQueue );
    return OldRoot;
}

int IsEmpty( BinQueue H ){
    return H->CurrentSize == 0;
}

int IsFull( BinQueue H ){
    return H->CurrentSize == Capacity;
}

BinTree CombineTrees( BinTree T1, BinTree T2 ) {
    if (T1->priority > T2->priority)
        return CombineTrees(T2, T1);
    T2->NextSibling = T1->LeftChild;
    T1->LeftChild = T2;
    return T1;
}

BinQueue Merge( BinQueue H1, BinQueue H2 ) {
    BinTree T1, T2, Carry = NULL;
    int i, j;

    if( H1->CurrentSize + H2->CurrentSize > Capacity )
        Error( "Merge would exceed capacity" );

    H1->CurrentSize += H2->CurrentSize;
    for( i = 0, j = 1; j <= H1->CurrentSize; i++, j *= 2 ) {
        T1 = H1->TheTrees[ i ]; T2 = H2->TheTrees[ i ];
        switch( !!T1 + 2 * !!T2 + 4 * !!Carry ){
            case 0: /* No trees */
            case 1: /* Only H1 */
                break;
            case 2: /* Only H2 */
                H1->TheTrees[ i ] = T2;
                H2->TheTrees[ i ] = NULL;
                break;
            case 4: /* Only Carry */
                H1->TheTrees[ i ] = Carry;
                Carry = NULL;
                break;
            case 3: /* H1 and H2 */
                Carry = CombineTrees( T1, T2 );
                H1->TheTrees[ i ] = H2->TheTrees[ i ] = NULL;
                break;
            case 5: /* H1 and Carry */
                Carry = CombineTrees( T1, Carry );
                H1->TheTrees[ i ] = NULL;
                break;
            case 6: /* H2 and Carry */
                Carry = CombineTrees( T2, Carry );
                H2->TheTrees[ i ] = NULL;
                break;
            case 7: /* All three */
                H1->TheTrees[ i ] = Carry;
                Carry = CombineTrees( T1, T2 );
                H2->TheTrees[ i ] = NULL;
                break;
        }
    }
    return H1;
}

/*------------------------------------------------------------------------------------*/
FILE *openInputFile() {
    FILE *filePtr ;
    if ((filePtr = fopen("input.txt" , "r")) == NULL ) {
        printf("File couldn't open!"); exit(-1);
    }
    return filePtr;
}

Position createNewProcess(char *processName, int exeTime , int arrTime) {
    Position newProcess = malloc(sizeof(struct BinNode));
    newProcess->priority = -1; newProcess->waitTime = 0; newProcess->isOnHeap = 0;
    newProcess->NextSibling = NULL; newProcess->LeftChild = NULL; newProcess->LLNext = NULL;
    newProcess->processName = calloc(8, sizeof(char));
    strcpy(newProcess->processName ,processName);
    newProcess->exeTime = exeTime;
    newProcess->constantExeTime = exeTime;
    newProcess->arrTime = arrTime;
    return newProcess;
}

Position getProcesses() {
    FILE *filePtr = openInputFile();
    char *currentInput = calloc(32, sizeof(char));
    Position processes = NULL;

    while((fgets(currentInput, 32 , filePtr)) != NULL ){
        if (strcmp("\r\n" , currentInput) == 0) break;

        char *processName = calloc(8, sizeof(char)); strcpy(processName, strtok(currentInput, " "));
        int exeTime = atoi(strtok(NULL, " ")) , arrTime = atoi(strtok(NULL , " "));
        Position newProcess = createNewProcess(processName,exeTime,arrTime);

        if (processes != NULL) {
            Position previousProcess = processes;
            for (; previousProcess->LLNext != NULL ; previousProcess = previousProcess->LLNext);
            previousProcess->LLNext =newProcess;
        } else
            processes = newProcess;
    }
    return processes;
}

int getExeTimeMax(Position processes) {
    int exeTimeMax = 0;
    for (Position currentProcess = processes ; currentProcess != NULL ; currentProcess = currentProcess->LLNext) {
        if (currentProcess->exeTime > exeTimeMax)
            exeTimeMax = currentProcess->exeTime;
    } return exeTimeMax;
}

int isProcessExist(Position processes) {
    for (Position currentProcess = processes ; currentProcess != NULL ; currentProcess = currentProcess->LLNext)
        if (currentProcess->exeTime >= 1)
            return 1;
    return 0;
}

ElementType findAWT(Position processes) {
    ElementType AWT = 0, totalProcess;
    for (Position currentProcess = processes ; currentProcess != NULL ; currentProcess = currentProcess->LLNext , totalProcess++)
        AWT += currentProcess->waitTime;
    
    return AWT / totalProcess;
}

Position copyProcesses(Position processes) {
    Position copiedProcesses = NULL, currentCopiedProcess = NULL , previousCopiedProcess = NULL;
    for (Position currentProcess = processes ; currentProcess != NULL ; currentProcess = currentProcess->LLNext, previousCopiedProcess = currentCopiedProcess, 
        currentCopiedProcess = currentCopiedProcess->LLNext) {
        currentCopiedProcess = malloc(sizeof(struct BinNode));
        currentCopiedProcess->exeTime = currentProcess->exeTime;
        currentCopiedProcess->waitTime = currentProcess->waitTime;
        currentCopiedProcess->isOnHeap = currentProcess->isOnHeap;
        currentCopiedProcess->priority = currentProcess->priority;
        currentCopiedProcess->arrTime = currentProcess->arrTime;
        currentCopiedProcess->constantExeTime = currentProcess->constantExeTime;
        currentCopiedProcess->LLNext = NULL;
        currentCopiedProcess->processName = calloc(32 , sizeof(char));
        strcpy(currentCopiedProcess->processName,currentProcess->processName);

        if (previousCopiedProcess != NULL)
            previousCopiedProcess->LLNext = currentCopiedProcess;

        copiedProcesses = currentProcess == processes ? currentCopiedProcess : copiedProcesses;
    }
    return copiedProcesses;
}

int main() {
    BinQueue BH = Initialize();
    Position processes = getProcesses();
    Position currentProcesses = copyProcesses(processes);
    ElementType exeTimeMax = getExeTimeMax(processes);
    ElementType optimizedAWT = INFINITY; int optimizedQuantumSize = 1;

    for (int quantumSize = 1; quantumSize <= 10 ; quantumSize++) {
        for (int time = 0; isProcessExist(currentProcesses);) {

            if (quantumSize== 2 && time == 6) {
                printf("Test");
            }

            for (Position currentProcess = currentProcesses ; currentProcess != NULL ; currentProcess = currentProcess->LLNext){
                if (currentProcess->isOnHeap || currentProcess->exeTime <= 0 || currentProcess->arrTime > time) continue;

                ElementType priority = currentProcess->constantExeTime == currentProcess->exeTime ?
                        currentProcess->constantExeTime : 1 / exp(-1 * pow((float)(2 * currentProcess->exeTime) / (3 * exeTimeMax),3));
                currentProcess->priority = priority;
                currentProcess->isOnHeap = 1;
                Insert(currentProcess, BH);
            }

            if (IsEmpty(BH)) continue;

            Position priorProcess = DeleteMin(BH);
            priorProcess->waitTime += time - priorProcess->arrTime;

            if (priorProcess->exeTime > quantumSize) {
                time += quantumSize;
                priorProcess->arrTime = time;
                priorProcess->exeTime = priorProcess->exeTime - quantumSize;
                priorProcess->priority = 1 / exp(-1 * pow((float)(2 * priorProcess->exeTime) / (3 * exeTimeMax),3));
                Insert(priorProcess , BH);
            } else {
                time += priorProcess->exeTime;
                priorProcess->exeTime = priorProcess->exeTime - quantumSize;
                priorProcess->isOnHeap = 0;
            }
        }

        ElementType currentAWT = findAWT(currentProcesses);
        if (currentAWT < optimizedAWT) {
            optimizedAWT = currentAWT;
            optimizedQuantumSize = quantumSize;
        }

        printf("PID Waiting Time\n");
        for (Position currentProcess = currentProcesses ; currentProcess != NULL ; currentProcess = currentProcess->LLNext)
            printf("%s %d\n" , currentProcess->processName , currentProcess->waitTime);
        printf("Q -> %d AWT -> %lf \n" , quantumSize, currentAWT);

        currentProcesses = copyProcesses(processes);
    }

    printf("\nOptimized Quantum Size -> %d | Optimized AWT -> %lf " , optimizedQuantumSize , optimizedAWT);
}