#include <stdio.h>
#include "symbolstack.h"
#include "debug.h"

int main(int argc, char const *argv[])
{
    SStack_t *sstack;
    SStackItem_t sstackItem, *topItem;
    Init_SStack(&sstack, 50);
    DebugFPrintSStack(stdout, sstack);

    sstackItem.type = STACK_ITEM_OPERATION;
    sstackItem.operationType = TO_ADD;
    Push_SStack(&sstack, &sstackItem);

    sstackItem.type = STACK_ITEM_DOUBLE;
    sstackItem.doubleValue = 3.14159;
    Push_SStack(&sstack, &sstackItem);

    sstackItem.type = STACK_ITEM_INTEGER;
    sstackItem.operationType = 42;
    Push_SStack(&sstack, &sstackItem);

    sstackItem.type = STACK_ITEM_OPERATION;
    sstackItem.operationType = TO_LBRACKET;
    Push_SStack(&sstack, &sstackItem);

    sstackItem.type = STACK_ITEM_OPERATION;
    sstackItem.operationType = TO_ADD;
    Push_SStack(&sstack, &sstackItem);

    DebugFPrintSStack(stdout, sstack);

    DebugFPuts("\n~~~ POPING stack ~~~\n\n", stdout);

    Pop_SStack(sstack);
    DebugFPrintSStack(stdout, sstack);

    DebugFPuts("\n~~~ TOP item ~~~\n\n", stdout);

    topItem = Top_SStack(sstack);

    DebugFPrintSStackItem(stdout, topItem);

    DebugFPuts("\n~~~ POPING stack (3x) ~~~\n\n", stdout);

    Pop_SStack(sstack);
    Pop_SStack(sstack);
    Pop_SStack(sstack);
    DebugFPrintSStackItem(stdout, topItem);

    DebugFPuts("\n~~~ PUSH 1 item ~~~\n\n", stdout);

    sstackItem.type = STACK_ITEM_INTEGER;
    sstackItem.operationType = -5;
    Push_SStack(&sstack, &sstackItem);
    DebugFPrintSStackItem(stdout, topItem);

    DebugFPuts("\n~~~ Dispose tack ~~~\n\n", stdout);
    Dispose_SStack(&sstack);
    return 0;
}
