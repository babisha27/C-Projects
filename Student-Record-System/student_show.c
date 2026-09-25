#include "student.h"

static void print_line(void)
{
    int i;

    putchar('+');
    for (i = 0; i < 9; i++)  putchar('-');
    putchar('+');
    for (i = 0; i < 32; i++) putchar('-');
    putchar('+');
    for (i = 0; i < 12; i++) putchar('-');
    printf("+\n");
}

void print_table_header(void)
{
    printf("\n");
    print_line();
    printf("| %-7s | %-30s | %10s |\n", "Roll No", "Name", "Percentage");
    print_line();
}

void print_record(const ST *s)
{
    printf("| %-7d | %-30s | %10.2f |\n", s->roll, s->name, s->per);
}

void print_table_footer(void)
{
    print_line();
}

void stud_show(ST *ptr)
{
    int count = 0;

    if (ptr == NULL)
    {
        printf("\nNo Records Found...\n");
        return;
    }

    print_table_header();
    while (ptr)
    {
        print_record(ptr);
        count++;
        ptr = ptr->next;
    }
    print_table_footer();

    printf("Total Records : %d\n", count);
}
