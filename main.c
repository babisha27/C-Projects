#include "student.h"

static void show_menu(void)
{
    const char *items[] = {
        "A : Add New Record",
        "D : Delete a Record",
        "S : Show the List",
        "M : Modify a Record",
        "V : Save",
        "T : Sort the List",
        "E : Exit"
    };
    int i, n = sizeof items / sizeof items[0];

    printf("\n    *************************************\n");
    printf("    *        STUDENT RECORD MENU        *\n");
    printf("    *************************************\n");
    for (i = 0; i < n; i++)
    {
        printf("    *   %-31s *\n", items[i]);
        if (i < n - 1)
            printf("    *-----------------------------------*\n");
    }
    printf("    *************************************\n");
}

/* Returns 1 if the program should exit */
static int confirm_exit(ST *head, int unsaved)
{
    char opt;

    if (!unsaved)
        return 1;

    printf("\nYou have unsaved changes.\n");
    printf("S : Save and Exit\n");
    printf("E : Exit Without Saving\n");
    printf("C : Cancel\n");

    while (1)
    {
        opt = read_char("Enter Choice : ");

        switch (opt)
        {
            case 'S':
                if (save_file(head))
                    return 1;
                printf("Save Failed. Returning to Menu.\n");
                return 0;

            case 'E':
                return 1;

            case 'C':
                return 0;

            default:
                printf("Invalid Choice\n");
        }
    }
}

int main(void)
{
    ST *head = NULL;
    int unsaved = 0;
    char ch;

    load_file(&head);

    while (1)
    {
        show_menu();
        ch = read_char("\nEnter Your Choice : ");

        switch (ch)
        {
            case 'A':
                if (stud_add(&head)) unsaved = 1;
                break;

            case 'D':
                if (stud_del(&head)) unsaved = 1;
                break;

            case 'S':
                stud_show(head);
                break;

            case 'M':
                if (stud_mod(head)) unsaved = 1;
                break;

            case 'V':
                if (save_file(head)) unsaved = 0;
                break;

            case 'T':
                if (stud_sort(head)) unsaved = 1;
                break;

            case 'E':
                if (confirm_exit(head, unsaved))
                {
                    free_list(&head);
                    printf("\nThank You...\n");
                    return 0;
                }
                break;

            default:
                printf("\nInvalid Choice...\n");
        }
    }
}
