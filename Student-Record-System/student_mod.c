#include "student.h"

int stud_mod(ST *ptr)
{
    ST *found;
    int new_roll;
    char ch;

    if (ptr == NULL)
    {
        printf("\nList is Empty\n");
        return 0;
    }

    printf("\n========== MODIFY RECORD ==========\n");
    printf("Search By:\n");
    printf("R : Roll Number\n");
    printf("N : Name\n");
    printf("P : Percentage\n");
    ch = read_char("Enter Your Choice : ");

    found = select_record(ptr, ch);
    if (found == NULL)
        return 0;

    printf("\nModify Field\n");
    printf("R : Roll Number\n");
    printf("N : Name\n");
    printf("P : Percentage\n");
    ch = read_char("Enter Your Choice : ");

    switch (ch)
    {
        case 'R':
            new_roll = read_int("Enter New Roll Number : ", 1, INT_MAX);

            if (new_roll == found->roll)
            {
                printf("Roll Number Unchanged\n");
                return 0;
            }
            if (find_by_roll(ptr, new_roll) != NULL)
            {
                printf("Roll Number Already Exists\n");
                return 0;
            }
            found->roll = new_roll;
            break;

        case 'N':
            read_name("Enter New Name : ", found->name);
            break;

        case 'P':
            found->per = read_float("Enter New Percentage : ", 0.0f, 100.0f);
            break;

        default:
            printf("Invalid Choice\n");
            return 0;
    }

    printf("\nUpdated Record:");
    print_table_header();
    print_record(found);
    print_table_footer();
    printf("Record Modified Successfully...\n");
    return 1;
}
