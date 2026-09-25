#include "student.h"

/* Unlinks target from the list and frees it */
static void remove_node(ST **ptr, ST *target)
{
    ST *prv;

    if (*ptr == target)
    {
        *ptr = target->next;
    }
    else
    {
        prv = *ptr;
        while (prv->next != target)
            prv = prv->next;
        prv->next = target->next;
    }

    free(target);
}

int stud_del(ST **ptr)
{
    ST *target;
    char ch, question[100];

    if (*ptr == NULL)
    {
        printf("\nList is Empty\n");
        return 0;
    }

    printf("\n========== DELETE RECORD ==========\n");
    printf("R : Delete by Roll Number\n");
    printf("N : Delete by Name\n");
    ch = read_char("Enter Your Choice : ");

    if (ch != 'R' && ch != 'N')
    {
        printf("Invalid Choice\n");
        return 0;
    }

    target = select_record(*ptr, ch);
    if (target == NULL)
        return 0;

    snprintf(question, sizeof question,
             "Delete Roll %d (%s)? (Y/N) : ", target->roll, target->name);

    if (!confirm(question))
    {
        printf("Deletion Cancelled\n");
        return 0;
    }

    remove_node(ptr, target);
    printf("Record Deleted Successfully\n");
    return 1;
}
