/* Talha Bayburtlu 150118066                                                             *
*  This project aims to optimize quantum size by creating an environment that processes  * 
*  goes into processer by their priority.And those processes waits while the most prior  * 
*  processes executes. With the help of changing quantum size optimized quantum size can * 
*  be found with ease.                                                                   */

#include "binomial.h"
#include "fatal.h"
#include <string.h>
#include <math.h>

struct BinNode { // Struct for Binomial Tree
    char *processName;
    int constantExeTime;
    int exeTime;
    int arrTime;
    int waitTime;
    int isOnHeap;
    ElementType priority;
    Position LeftChild; // For binomial tree purpose
    Position NextSibling; // For binomial tree purpose
    Position LLNext; // For linked-list purpose.
};

struct Collection { // Struct for Binomial Heap
    int CurrentSize;
    BinTree TheTrees[ MaxTrees ];
};

int main() {
    BinQueue BH = Initialize(); // Initializing heap.
    Position processes = GetProcesses(); // Getting processes with linked list notation.
    Position currentProcesses = CopyProcesses(processes); // Copying processes for using another run with different quantum size.
    ElementType exeTimeMax = GetExeTimeMax(processes); // Getting maximum execution time along all processes.
    ElementType optimizedAWT = INFINITY; int optimizedQuantumSize = 1;

    for (int quantumSize = 1; quantumSize <= 10 ; quantumSize++, currentProcesses = CopyProcesses(processes)) {// Running processes with differen quantum sizes.
        for (int time = 0; IsProcessExist(currentProcesses);) { // Running processes until there is none.
        
            // Checking processes arrival time for inserting Binomial Heap (BH).
            for (Position currentProcess = currentProcesses ; currentProcess != NULL ; currentProcess = currentProcess->LLNext){
                if (currentProcess->isOnHeap || currentProcess->exeTime <= 0 || currentProcess->arrTime > time) continue;

                currentProcess->priority = currentProcess->exeTime;
                currentProcess->isOnHeap = 1;
                Insert(currentProcess, BH);
            }

            if (IsEmpty(BH)) {time += quantumSize; continue;}

            Position priorProcess = DeleteMin(BH); // Geting most prior process into uP.
            priorProcess->waitTime += time - priorProcess->arrTime;

            if (priorProcess->exeTime > quantumSize) { // Checking processes is going to be done or not after running.
                time += quantumSize;
                priorProcess->arrTime = time;
                priorProcess->exeTime = priorProcess->exeTime - quantumSize;
                priorProcess->priority = (1 / exp(-1 * pow((float)(2 * priorProcess->exeTime) / (3 * exeTimeMax),3)) * priorProcess->exeTime);
                Insert(priorProcess , BH); // Inserting processes with new priority value and remaining execution time.
            } else { 
                // Increasing time by reamining execution time and NOT inserting process to Binomial Heap (BH).
                time += priorProcess->exeTime;
                priorProcess->exeTime = priorProcess->exeTime - quantumSize;
                priorProcess->isOnHeap = 0;
            }
        }

        ElementType currentAWT = FindAWT(currentProcesses); // Finding average waiting time for current quantum size.
        if (currentAWT < optimizedAWT) { // Updating optimizied average waiting time and quantum size.
            optimizedAWT = currentAWT;
            optimizedQuantumSize = quantumSize;
        }


        // Printing individual processes with their waiting time and average waiting time for current quantum size.
        printf("PID Waiting Time\n");
        for (Position currentProcess = currentProcesses ; currentProcess != NULL ; currentProcess = currentProcess->LLNext)
            printf("%-4s %11d\n" , currentProcess->processName , currentProcess->waitTime);
        printf("Q -> %d AWT -> %lf \n------------------------------------\n" , quantumSize, currentAWT);
    }

    // Printing optimized quantum size and average waiting time.
    printf("\nOptimized Quantum Size -> %d | Optimized AWT -> %lf" , optimizedQuantumSize , optimizedAWT);
}

BinQueue Initialize( void ) { // Initializes Binomial Heap
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

static void DestroyTree( BinTree T ) { // Destroys tree with it's left and right subtree.
    if( T != NULL ) {
        DestroyTree( T->LeftChild );
        DestroyTree( T->NextSibling );
        free( T );
    }
}

void Destroy( BinQueue H ) { // Destroys Binomial Heap
    int i;

    for( i = 0; i < MaxTrees; i++ ) // Destroying heap by removing all trees.
        DestroyTree( H->TheTrees[ i ] );
}

BinQueue Insert( Position process, BinQueue H ) { // Inserts process by merging.
    BinQueue OneItem;
    OneItem = Initialize( );
    OneItem->CurrentSize = 1;
    OneItem->TheTrees[ 0 ] = process;
    return Merge( H, OneItem );
}

Position DeleteMin( BinQueue H ) { // Delets process which has minimum priority.
    int i, j;
    int MinTree;
    int MinArrival;
    BinQueue DeletedQueue;
    Position DeletedTree, OldRoot;
    ElementType MinPriority;

    if( IsEmpty( H )) // Checking heap is empty or not.
        Error( "Empty binomial queue" );

    MinPriority = Infinity;
    for( i = 0; i < MaxTrees; i++ ){ // Determining process by looking roots of trees.
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

int IsEmpty( BinQueue H ){ // Checks binomial heap is empty or not. 
    return H->CurrentSize == 0;
}

BinTree CombineTrees( BinTree T1, BinTree T2 ) { // Combines trees by looking their priority value.
    if (T1->priority > T2->priority)
        return CombineTrees(T2, T1);
    T2->NextSibling = T1->LeftChild;
    T1->LeftChild = T2;
    return T1;
}

BinQueue Merge( BinQueue H1, BinQueue H2 ) { // Merges given two binomial heap by looping thourgh each heap's trees.
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

FILE *OpenInputFile() { // Opens input file.
    FILE *filePtr ;
    if ((filePtr = fopen("input.txt" , "r")) == NULL ) {
        printf("File couldn't open!"); exit(-1);
    }
    return filePtr;
}

Position CreateNewProcess(char *processName, int exeTime , int arrTime) { // Creates new process with given information.
    Position newProcess = malloc(sizeof(struct BinNode));
    newProcess->priority = -1; newProcess->waitTime = 0; newProcess->isOnHeap = 0;
    newProcess->NextSibling = NULL; newProcess->LeftChild = NULL; newProcess->LLNext = NULL;
    newProcess->processName = calloc(8, sizeof(char));
    strcpy(newProcess->processName ,processName);
    newProcess->exeTime = exeTime;  newProcess->constantExeTime = exeTime;  newProcess->arrTime = arrTime;
    return newProcess;
}

Position GetProcesses() { // Gets processes by reading input file with linked list notation.
    FILE *filePtr = OpenInputFile();
    char *currentInput = calloc(32, sizeof(char));
    Position processes = NULL;

    while((fgets(currentInput, 32 , filePtr)) != NULL ){
        if (strcmp("\r\n" , currentInput) == 0) break;

        char *processName = calloc(8, sizeof(char)); strcpy(processName, strtok(currentInput, " "));
        int exeTime = atoi(strtok(NULL, " ")) , arrTime = atoi(strtok(NULL , " "));
        Position newProcess = CreateNewProcess(processName,exeTime,arrTime);

        if (processes != NULL) {
            Position previousProcess = processes;
            for (; previousProcess->LLNext != NULL ; previousProcess = previousProcess->LLNext);
            previousProcess->LLNext =newProcess;
        } else
            processes = newProcess;
    }
    return processes;
}

int GetExeTimeMax(Position processes) { // Gets maximum execution time along all processes.
    int exeTimeMax = 0;
    for (Position currentProcess = processes ; currentProcess != NULL ; currentProcess = currentProcess->LLNext) { // Passing through all processes.
        if (currentProcess->exeTime > exeTimeMax)
            exeTimeMax = currentProcess->exeTime;
    } return exeTimeMax;
}

int IsProcessExist(Position processes) { // Determines are there any process to run indepentent of current time.
    for (Position currentProcess = processes ; currentProcess != NULL ; currentProcess = currentProcess->LLNext)
        if (currentProcess->exeTime >= 1)
            return 1;
    return 0;
}

ElementType FindAWT(Position processes) { // Finds average waiting time by dividing sum of waiting times with total processes.
    ElementType AWT = 0, totalProcess;
    for (Position currentProcess = processes ; currentProcess != NULL ; currentProcess = currentProcess->LLNext , totalProcess++)
        AWT += currentProcess->waitTime;
    
    return AWT / totalProcess;
}

Position CopyProcesses(Position processes) { // Copies all processes for using another run with different quantum size.
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

        if (previousCopiedProcess != NULL) // Linking current and previous processes by linked list notation.
            previousCopiedProcess->LLNext = currentCopiedProcess;

        copiedProcesses = currentProcess == processes ? currentCopiedProcess : copiedProcesses; // Storing first copied process.
    }
    return copiedProcesses;
}