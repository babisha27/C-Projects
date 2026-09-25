#include "student.h"

int stud_add(ST **ptr)
{
    ST *temp, *last;
    int roll = 1;

    /* Assign the smallest roll number not already in use */
    while (find_by_roll(*ptr, roll) != NULL)
        roll++;

    temp = (ST *)malloc(sizeof(ST));
    if (temp == NULL)
    {
        printf("Memory Allocation Failed\n");
        return 0;
    }

    temp->roll = roll;
    printf("\nRoll Number Assigned : %d\n", roll);

    read_name("Enter Name : ", temp->name);
    temp->per = read_float("Enter Percentage : ", 0.0f, 100.0f);
    temp->next = NULL;

    /* Add at the end of the list */
    if (*ptr == NULL)
    {
        *ptr = temp;
    }
    else
    {
        last = *ptr;
        while (last->next)
            last = last->next;
        last->next = temp;
    }

    printf("Record Added Successfully\n");
    return 1;
}
