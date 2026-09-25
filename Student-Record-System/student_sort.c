#include "student.h"

/* Comparison functions: return > 0 if a should come after b */

static int cmp_roll(const ST *a, const ST *b)
{
    return (a->roll > b->roll) - (a->roll < b->roll);
}

static int cmp_name(const ST *a, const ST *b)
{
    int r = name_icmp(a->name, b->name);
    return r != 0 ? r : cmp_roll(a, b);      /* same name: by roll */
}

static int cmp_per(const ST *a, const ST *b)  /* descending */
{
    if (a->per < b->per) return 1;
    if (a->per > b->per) return -1;
    return cmp_roll(a, b);                    /* same %: by roll */
}

/* Swaps the data of two nodes (the links stay the same) */
static void swap_data(ST *a, ST *b)
{
    int t_roll;
    float t_per;
    char t_name[NAME_LEN];

    t_roll = a->roll;
    a->roll = b->roll;
    b->roll = t_roll;

    strcpy(t_name, a->name);
    strcpy(a->name, b->name);
    strcpy(b->name, t_name);

    t_per = a->per;
    a->per = b->per;
    b->per = t_per;
}

static void sort_list(ST *head, int (*cmp)(const ST *, const ST *))
{
    ST *p1, *p2;

    for (p1 = head; p1; p1 = p1->next)
        for (p2 = p1->next; p2; p2 = p2->next)
            if (cmp(p1, p2) > 0)
                swap_data(p1, p2);
}

int stud_sort(ST *ptr)
{
    char ch;

    if (ptr == NULL)
    {
        printf("\nList is Empty\n");
        return 0;
    }

    printf("\n========== SORT RECORDS ==========\n");
    printf("R : Sort by Roll Number\n");
    printf("N : Sort by Name\n");
    printf("P : Sort by Percentage (Highest First)\n");
    ch = read_char("Enter Your Choice : ");

    switch (ch)
    {
        case 'R':
            sort_list(ptr, cmp_roll);
            printf("\nSorted by Roll Number\n");
            break;

        case 'N':
            sort_list(ptr, cmp_name);
            printf("\nSorted by Name\n");
            break;

        case 'P':
            sort_list(ptr, cmp_per);
            printf("\nSorted by Percentage\n");
            break;

        default:
            printf("Invalid Choice\n");
            return 0;
    }

    stud_show(ptr);
    return 1;
}
