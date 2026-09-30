#include "lms.h"

static const int MW[] = {5, 25, 10, 5};     /* column widths */
#define MCOLS 4

static void member_header(void)
{
    printf("\n");
    print_border(MW, MCOLS);
    printf("| %-5s | %-25s | %-10s | %5s |\n", "ID", "Name", "Phone", "Books");
    print_border(MW, MCOLS);
}

static void member_row(const MEMBER *m)
{
    printf("| %-5d | %-25s | %-10s | %5d |\n", m->id, m->name, m->phone, m->issued);
}

static void show_one_member(const MEMBER *m)
{
    member_header();
    member_row(m);
    print_border(MW, MCOLS);
}

MEMBER *find_member(MEMBER *head, int id)
{
    while (head)
    {
        if (head->id == id)
            return head;
        head = head->next;
    }
    return NULL;
}

static MEMBER *find_phone(MEMBER *head, const char *phone)
{
    while (head)
    {
        if (strcmp(head->phone, phone) == 0)
            return head;
        head = head->next;
    }
    return NULL;
}

/* New ID = highest ID ever used + 1 (deleted IDs are not reused) */
static int next_member_id(const LIBRARY *lib)
{
    const MEMBER *m;
    const ISSUE *is;
    int max = 0;

    for (m = lib->members; m; m = m->next)
        if (m->id > max)
            max = m->id;
    for (is = lib->issues; is; is = is->next)
        if (is->member_id > max)
            max = is->member_id;

    return max + 1;
}

static MEMBER *ask_member(MEMBER *head)
{
    MEMBER *m = find_member(head, read_int("Enter Member ID : ", 1, INT_MAX));

    if (m == NULL)
        printf("Member Not Found\n");
    return m;
}

static void add_member(LIBRARY *lib)
{
    MEMBER *m, *last, *other;
    char name[NAME_LEN], phone[PHONE_LEN];

    printf("\n========== ADD MEMBER ==========\n");
    read_text("Enter Name  : ", name, NAME_LEN, "Name");
    read_phone("Enter Phone : ", phone, 0);

    other = find_phone(lib->members, phone);
    if (other != NULL)
    {
        printf("This phone number is already registered to Member ID %d (%s).\n",
               other->id, other->name);
        return;
    }

    m = (MEMBER *)malloc(sizeof(MEMBER));
    if (m == NULL)
    {
        printf("Memory Allocation Failed\n");
        return;
    }

    m->id = next_member_id(lib);
    strcpy(m->name, name);
    strcpy(m->phone, phone);
    m->issued = 0;
    m->next = NULL;

    if (lib->members == NULL)
        lib->members = m;
    else
    {
        for (last = lib->members; last->next; last = last->next)
            ;
        last->next = m;
    }

    lib->unsaved = 1;
    printf("Member Added Successfully. Member ID : %d\n", m->id);
}

static void show_members(const MEMBER *head)
{
    int count = 0;

    if (head == NULL)
    {
        printf("\nNo Members Found\n");
        return;
    }

    member_header();
    for (; head; head = head->next)
    {
        member_row(head);
        count++;
    }
    print_border(MW, MCOLS);
    printf("Total Members : %d\n", count);
}

static void search_members(const MEMBER *head)
{
    char key[NAME_LEN];
    int count = 0;

    if (head == NULL)
    {
        printf("\nNo Members Found\n");
        return;
    }

    read_text("Enter Name (or part of it) : ", key, NAME_LEN, "Search text");

    for (; head; head = head->next)
    {
        if (str_icontains(head->name, key))
        {
            if (count == 0)
                member_header();
            member_row(head);
            count++;
        }
    }

    if (count == 0)
        printf("No Matching Members Found\n");
    else
    {
        print_border(MW, MCOLS);
        printf("%d Member(s) Found\n", count);
    }
}

static void modify_member(LIBRARY *lib)
{
    MEMBER *m, *other;
    char phone[PHONE_LEN];

    if (lib->members == NULL)
    {
        printf("\nNo Members Found\n");
        return;
    }

    printf("\n========== MODIFY MEMBER ==========\n");
    m = ask_member(lib->members);
    if (m == NULL)
        return;

    show_one_member(m);
    printf("Press Enter to keep the current value.\n");

    read_text_keep("Enter New Name  : ", m->name, NAME_LEN, "Name");

    strcpy(phone, m->phone);
    read_phone("Enter New Phone : ", phone, 1);
    other = find_phone(lib->members, phone);

    if (other != NULL && other != m)
        printf("Phone already registered to Member ID %d. Phone not changed.\n", other->id);
    else
        strcpy(m->phone, phone);

    lib->unsaved = 1;
    printf("\nUpdated Record:");
    show_one_member(m);
    printf("Member Modified Successfully\n");
}

static void delete_member(LIBRARY *lib)
{
    MEMBER *m, *prv;

    if (lib->members == NULL)
    {
        printf("\nNo Members Found\n");
        return;
    }

    printf("\n========== DELETE MEMBER ==========\n");
    m = ask_member(lib->members);
    if (m == NULL)
        return;

    show_one_member(m);

    if (m->issued > 0)
    {
        printf("Cannot Delete : Member still has %d book(s). Return them first.\n", m->issued);
        return;
    }

    if (!confirm("Delete this member? (Y/N) : "))
    {
        printf("Deletion Cancelled\n");
        return;
    }

    if (lib->members == m)
        lib->members = m->next;
    else
    {
        for (prv = lib->members; prv->next != m; prv = prv->next)
            ;
        prv->next = m->next;
    }
    free(m);

    lib->unsaved = 1;
    printf("Member Deleted Successfully\n");
}

static void member_books(const LIBRARY *lib)
{
    MEMBER *m;

    if (lib->members == NULL)
    {
        printf("\nNo Members Found\n");
        return;
    }

    m = ask_member(lib->members);
    if (m == NULL)
        return;

    printf("\nBooks issued to %s (Member ID %d):", m->name, m->id);
    show_member_issues(lib, m->id);
}

void member_menu(LIBRARY *lib)
{
    const char *items[] = {
        "1 : Add Member",
        "2 : Show All Members",
        "3 : Search Member by Name",
        "4 : Modify Member",
        "5 : Delete Member",
        "6 : Books Issued to a Member",
        "0 : Back to Main Menu"
    };
    int n = sizeof items / sizeof items[0];

    while (1)
    {
        print_menu("MEMBER MANAGEMENT", items, n);

        switch (read_choice("\nEnter Your Choice : "))
        {
            case 1: add_member(lib);             break;
            case 2: show_members(lib->members);  break;
            case 3: search_members(lib->members); break;
            case 4: modify_member(lib);          break;
            case 5: delete_member(lib);          break;
            case 6: member_books(lib);           break;
            case 0: return;
            default: printf("\nInvalid Choice...\n");
        }
    }
}
