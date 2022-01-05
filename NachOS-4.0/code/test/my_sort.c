#include "syscall.h"
#define MAX_SIZE_ARR 100 // n <= 100

void bubbleSort(int arr[], int n, int isIncrease)
{
    int i, j, temp;
    if (isIncrease == 1) // neu isIncrease == 1 thi sap xep tang dan
    {
        for (i = 0; i < n - 1; i++)
            for (j = 0; j < n - i - 1; j++)
                if (arr[j] > arr[j + 1])
                {
                    temp = arr[j + 1];
                    arr[j + 1] = arr[j];
                    arr[j] = temp;
                }
    }
    if (isIncrease == 2) // sap xep giam dan bang bubble sort
    {
        for (i = 0; i < n - 1; i++)
            for (j = 0; j < n - i - 1; j++)
                if (arr[j] < arr[j + 1])
                {
                    temp = arr[j + 1];
                    arr[j + 1] = arr[j];
                    arr[j] = temp;
                }
    }
}
void showArr(int arr[], int n)
{ // print array
    int i;
    i = 0;
    PrintString("Array: \n");
    for (i = 0; i < n; i++)
    {
        PrintNum(arr[i]);
        PrintChar(' ');
        if (i % 10 == 9)
            PrintChar('\n'); // 1 dong toi da 10 phan tu cho de~ nhin
    }
    PrintChar('\n');
}
int main()
{
    int n;
    int Arr[MAX_SIZE_ARR];
    int i, isIncrease;
    i = 0;
    // Input n
    PrintString("Input the amount of array: ");
    n = ReadNum();
    
    // Kiem tra dieu kien n
    while (n > 100 || n <= 0)
    {
        PrintString("Range of the amount [1,100]. Please input again: ");
        n = ReadNum();
    }

    // Input cac phan tu array
    PrintString("Input array: \n");
    while (i < n)
    {
        PrintString("A[");
        PrintNum(i);
        PrintString("] :");
        Arr[i] = ReadNum();
        i++;
    }
    PrintString("Array before Sorting: \n");
    showArr(Arr, n);

    //Lua chon sap xep tang/giam, 1 hoac 2
    do
    {
        PrintString("Input 1 to sort this array in ascending, or 2 to sort it in descending: \n");
        isIncrease = ReadNum();
    } while (isIncrease != 1 && isIncrease != 2);

    
    bubbleSort(Arr, n, isIncrease);

    PrintString("Array after Sorting: \n");
    showArr(Arr, n);
    //return 0;
    Halt();
}