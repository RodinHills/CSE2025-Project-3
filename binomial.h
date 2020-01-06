#include <stdio.h>

typedef double ElementType;
        #define Infinity (6000000000L)

        #ifndef _BinHeap_H
		
		#define MaxTrees (12)   /* Stores 2^9 -1 items */
        #define Capacity (4095)

        struct BinNode;
        typedef struct BinNode *BinTree;
        struct Collection;
        typedef struct Collection *BinQueue;
        typedef struct BinNode *Position;

        BinQueue Initialize( void );
        void Destroy( BinQueue H );
        BinQueue MakeEmpty( BinQueue H );
        BinQueue Insert( struct BinNode *process, BinQueue H );
        struct BinNode *DeleteMin( BinQueue H );
        BinQueue Merge( BinQueue H1, BinQueue H2 );
        ElementType FindMin( BinQueue H );
        int IsEmpty( BinQueue H );
        int IsFull( BinQueue H );
        FILE *OpenInputFile();
        Position CreateNewProcess(char *processName, int exeTime , int arrTime);
        Position GetProcesses();
        int GetExeTimeMax(Position processes);
        int IsProcessExist(Position processes);
        ElementType FindAWT(Position processes);
        Position CopyProcesses(Position processes);
        #endif
/* END */