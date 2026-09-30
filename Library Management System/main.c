#include "lms.h"

/* Returns 1 if the program should exit */
static int confirm_exit(LIBRARY *lib)
{
    if (!lib->unsaved)
        return 1;

    printf("\nYou have unsaved changes.\n");
    printf("1 : Save and Exit\n");
    printf("2 : Exit Without Saving\n");
    printf("0 : Cancel\n");

    while (1)
    {
        switch (read_choice("Enter Your Choice : "))
        {
            case 1:
                if (save_all(lib))
                    return 1;
                printf("Returning to Menu.\n");
                return 0;

            case 2:
                return 1;

            case 0:
                return 0;

            default:
                printf("Invalid Choice\n");
        }
    }
}

int main(void)
{
    LIBRARY lib = {NULL, NULL, NULL, 0};
    const char *items[] = {
        "1 : Book Management",
        "2 : Member Management",
        "3 : Issue a Book",
        "4 : Return a Book",
        "5 : Issued Books & History",
        "6 : Save",
        "0 : Exit"
    };
    int n = sizeof items / sizeof items[0];

    load_all(&lib);

    while (1)
    {
        print_menu("LIBRARY MANAGEMENT SYSTEM", items, n);

        switch (read_choice("\nEnter Your Choice : "))
        {
            case 1: book_menu(&lib);   break;
            case 2: member_menu(&lib); break;
            case 3: issue_book(&lib);  break;
            case 4: return_book(&lib); break;
            case 5: issued_menu(&lib); break;

            case 6:
                if (save_all(&lib))
                    lib.unsaved = 0;
                break;

            case 0:
                if (confirm_exit(&lib))
                {
                    free_all(&lib);
                    printf("\nThank You...\n");
                    return 0;
                }
                break;

            default:
                printf("\nInvalid Choice...\n");
        }
    }
}
