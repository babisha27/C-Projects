#include "lms.h"

static const int BW[] = {5, 35, 25, 6, 6};     /* column widths */
#define BCOLS 5

static void book_header(void)
{
    printf("\n");
    print_border(BW, BCOLS);
    printf("| %-5s | %-35s | %-25s | %6s | %6s |\n",
           "ID", "Title", "Author", "Copies", "Avail");
    print_border(BW, BCOLS);
}

static void book_row(const BOOK *b)
{
    printf("| %-5d | %-35s | %-25s | %6d | %6d |\n",
           b->id, b->title, b->author, b->total, b->available);
}

static void show_one_book(const BOOK *b)
{
    book_header();
    book_row(b);
    print_border(BW, BCOLS);
}

BOOK *find_book(BOOK *head, int id)
{
    while (head)
    {
        if (head->id == id)
            return head;
        head = head->next;
    }
    return NULL;
}

/* New ID = highest ID ever used + 1 (also checks issue history, so the
   ID of a deleted book is never given to a new one) */
static int next_book_id(const LIBRARY *lib)
{
    const BOOK *b;
    const ISSUE *is;
    int max = 100;

    for (b = lib->books; b; b = b->next)
        if (b->id > max)
            max = b->id;
    for (is = lib->issues; is; is = is->next)
        if (is->book_id > max)
            max = is->book_id;

    return max + 1;
}

static BOOK *ask_book(BOOK *head)
{
    BOOK *b = find_book(head, read_int("Enter Book ID : ", 1, INT_MAX));

    if (b == NULL)
        printf("Book Not Found\n");
    return b;
}

static void add_book(LIBRARY *lib)
{
    BOOK *b, *last;
    char title[TITLE_LEN], author[AUTHOR_LEN];
    int copies;

    printf("\n========== ADD BOOK ==========\n");
    read_text("Enter Title  : ", title, TITLE_LEN, "Title");
    read_text("Enter Author : ", author, AUTHOR_LEN, "Author");

    /* Same title and author already in the library? */
    for (b = lib->books; b; b = b->next)
        if (str_icmp(b->title, title) == 0 && str_icmp(b->author, author) == 0)
            break;

    if (b != NULL)
    {
        printf("This book already exists (Book ID : %d, Copies : %d).\n", b->id, b->total);

        if (b->total >= MAX_COPIES)
        {
            printf("Maximum of %d copies already reached.\n", MAX_COPIES);
            return;
        }
        if (confirm("Add more copies to it? (Y/N) : "))
        {
            copies = read_int("Enter Number of Copies to Add : ", 1, MAX_COPIES - b->total);
            b->total += copies;
            b->available += copies;
            lib->unsaved = 1;
            printf("Copies Updated. Total Copies : %d\n", b->total);
        }
        return;
    }

    copies = read_int("Enter Number of Copies : ", 1, MAX_COPIES);

    b = (BOOK *)malloc(sizeof(BOOK));
    if (b == NULL)
    {
        printf("Memory Allocation Failed\n");
        return;
    }

    b->id = next_book_id(lib);
    strcpy(b->title, title);
    strcpy(b->author, author);
    b->total = copies;
    b->available = copies;
    b->next = NULL;

    if (lib->books == NULL)
        lib->books = b;
    else
    {
        for (last = lib->books; last->next; last = last->next)
            ;
        last->next = b;
    }

    lib->unsaved = 1;
    printf("Book Added Successfully. Book ID : %d\n", b->id);
}

static void show_books(const BOOK *head)
{
    int titles = 0, copies = 0, avail = 0;

    if (head == NULL)
    {
        printf("\nNo Books Found\n");
        return;
    }

    book_header();
    for (; head; head = head->next)
    {
        book_row(head);
        titles++;
        copies += head->total;
        avail += head->available;
    }
    print_border(BW, BCOLS);

    printf("Titles : %d   Total Copies : %d   Available : %d   Issued : %d\n",
           titles, copies, avail, copies - avail);
}

static void search_books(BOOK *head)
{
    BOOK *b;
    char key[TITLE_LEN];
    int ch, count = 0;

    if (head == NULL)
    {
        printf("\nNo Books Found\n");
        return;
    }

    printf("\n========== SEARCH BOOK ==========\n");
    printf("1 : By Book ID\n");
    printf("2 : By Title\n");
    printf("3 : By Author\n");
    ch = read_choice("Enter Your Choice : ");

    if (ch == 1)
    {
        b = ask_book(head);
        if (b)
            show_one_book(b);
        return;
    }
    if (ch != 2 && ch != 3)
    {
        printf("Invalid Choice\n");
        return;
    }

    read_text(ch == 2 ? "Enter Title (or part of it)  : "
                      : "Enter Author (or part of it) : ",
              key, TITLE_LEN, "Search text");

    for (b = head; b; b = b->next)
    {
        if (str_icontains(ch == 2 ? b->title : b->author, key))
        {
            if (count == 0)
                book_header();
            book_row(b);
            count++;
        }
    }

    if (count == 0)
        printf("No Matching Books Found\n");
    else
    {
        print_border(BW, BCOLS);
        printf("%d Book(s) Found\n", count);
    }
}

static void modify_book(LIBRARY *lib)
{
    BOOK *b;
    int issued, new_total;

    if (lib->books == NULL)
    {
        printf("\nNo Books Found\n");
        return;
    }

    printf("\n========== MODIFY BOOK ==========\n");
    b = ask_book(lib->books);
    if (b == NULL)
        return;

    show_one_book(b);
    printf("Press Enter to keep the current value.\n");

    read_text_keep("Enter New Title  : ", b->title, TITLE_LEN, "Title");
    read_text_keep("Enter New Author : ", b->author, AUTHOR_LEN, "Author");

    issued = b->total - b->available;
    if (issued > 0)
        printf("(%d copy(ies) currently issued, so total cannot be less than %d)\n",
               issued, issued);

    new_total = read_int_keep("Enter New Total Copies : ",
                              issued > 0 ? issued : 1, MAX_COPIES, b->total);
    b->available += new_total - b->total;
    b->total = new_total;

    lib->unsaved = 1;
    printf("\nUpdated Record:");
    show_one_book(b);
    printf("Book Modified Successfully\n");
}

static void delete_book(LIBRARY *lib)
{
    BOOK *b, *prv;

    if (lib->books == NULL)
    {
        printf("\nNo Books Found\n");
        return;
    }

    printf("\n========== DELETE BOOK ==========\n");
    b = ask_book(lib->books);
    if (b == NULL)
        return;

    show_one_book(b);

    if (b->available != b->total)
    {
        printf("Cannot Delete : %d copy(ies) currently issued. Return them first.\n",
               b->total - b->available);
        return;
    }

    if (!confirm("Delete this book? (Y/N) : "))
    {
        printf("Deletion Cancelled\n");
        return;
    }

    if (lib->books == b)
        lib->books = b->next;
    else
    {
        for (prv = lib->books; prv->next != b; prv = prv->next)
            ;
        prv->next = b->next;
    }
    free(b);

    lib->unsaved = 1;
    printf("Book Deleted Successfully\n");
}

/* ---------- Sorting ---------- */

static int cmp_id(const BOOK *a, const BOOK *b)
{
    return (a->id > b->id) - (a->id < b->id);
}

static int cmp_title(const BOOK *a, const BOOK *b)
{
    int r = str_icmp(a->title, b->title);
    return r != 0 ? r : cmp_id(a, b);
}

static int cmp_author(const BOOK *a, const BOOK *b)
{
    int r = str_icmp(a->author, b->author);
    return r != 0 ? r : cmp_title(a, b);
}

static int cmp_avail(const BOOK *a, const BOOK *b)      /* most first */
{
    if (a->available != b->available)
        return a->available < b->available ? 1 : -1;
    return cmp_id(a, b);
}

/* Swaps the data of two nodes; the links stay where they are */
static void swap_book(BOOK *a, BOOK *b)
{
    BOOK temp = *a;
    BOOK *a_next = a->next, *b_next = b->next;

    *a = *b;
    *b = temp;
    a->next = a_next;
    b->next = b_next;
}

static void sort_books(LIBRARY *lib)
{
    int (*cmp)(const BOOK *, const BOOK *);
    BOOK *p1, *p2;

    if (lib->books == NULL)
    {
        printf("\nNo Books Found\n");
        return;
    }

    printf("\n========== SORT BOOKS ==========\n");
    printf("1 : By Book ID\n");
    printf("2 : By Title\n");
    printf("3 : By Author\n");
    printf("4 : By Available Copies (Most First)\n");

    switch (read_choice("Enter Your Choice : "))
    {
        case 1: cmp = cmp_id;     break;
        case 2: cmp = cmp_title;  break;
        case 3: cmp = cmp_author; break;
        case 4: cmp = cmp_avail;  break;
        default:
            printf("Invalid Choice\n");
            return;
    }

    for (p1 = lib->books; p1; p1 = p1->next)
        for (p2 = p1->next; p2; p2 = p2->next)
            if (cmp(p1, p2) > 0)
                swap_book(p1, p2);

    lib->unsaved = 1;
    printf("\nBooks Sorted\n");
    show_books(lib->books);
}

void book_menu(LIBRARY *lib)
{
    const char *items[] = {
        "1 : Add Book",
        "2 : Show All Books",
        "3 : Search Book",
        "4 : Modify Book",
        "5 : Delete Book",
        "6 : Sort Books",
        "0 : Back to Main Menu"
    };
    int n = sizeof items / sizeof items[0];

    while (1)
    {
        print_menu("BOOK MANAGEMENT", items, n);

        switch (read_choice("\nEnter Your Choice : "))
        {
            case 1: add_book(lib);         break;
            case 2: show_books(lib->books); break;
            case 3: search_books(lib->books); break;
            case 4: modify_book(lib);      break;
            case 5: delete_book(lib);      break;
            case 6: sort_books(lib);       break;
            case 0: return;
            default: printf("\nInvalid Choice...\n");
        }
    }
}
