#include "lms.h"

/* Column widths */
static const int AW[] = {4, 22, 16, 10, 10, 9};   /* active issues */
static const int HW[] = {4, 22, 16, 10, 10, 6};   /* history       */
#define ICOLS 6

static const char *title_of(const LIBRARY *lib, int id)
{
    BOOK *b = find_book(lib->books, id);
    return b ? b->title : "(Deleted Book)";
}

static const char *name_of(const LIBRARY *lib, int id)
{
    MEMBER *m = find_member(lib->members, id);
    return m ? m->name : "(Deleted Member)";
}

static ISSUE *find_issue(ISSUE *head, int id)
{
    while (head)
    {
        if (head->id == id)
            return head;
        head = head->next;
    }
    return NULL;
}

static int next_issue_id(const ISSUE *head)
{
    int max = 0;

    for (; head; head = head->next)
        if (head->id > max)
            max = head->id;
    return max + 1;
}

static int count_active(const ISSUE *head)
{
    int count = 0;

    for (; head; head = head->next)
        if (head->return_date == 0)
            count++;
    return count;
}

/* ---------- Tables ---------- */

static void active_header(void)
{
    printf("\n");
    print_border(AW, ICOLS);
    printf("| %-4s | %-22s | %-16s | %-10s | %-10s | %-9s |\n",
           "No", "Book Title", "Member", "Issued", "Due", "Status");
    print_border(AW, ICOLS);
}

static void active_row(const LIBRARY *lib, const ISSUE *is, int now)
{
    char d1[DATE_STR], d2[DATE_STR], status[24];

    format_date(is->issue_date, d1);
    format_date(is->due_date, d2);

    if (now > is->due_date)
        snprintf(status, sizeof status, "Late %dd", now - is->due_date);
    else
        strcpy(status, "On time");

    printf("| %-4d | %-22.22s | %-16.16s | %-10s | %-10s | %-9s |\n",
           is->id, title_of(lib, is->book_id), name_of(lib, is->member_id),
           d1, d2, status);
}

/* Lists books not yet returned. member_id = 0 means all members.
   Returns the number of records shown. */
static int list_active(const LIBRARY *lib, int member_id, int overdue_only)
{
    const ISSUE *is;
    int now = today(), count = 0;

    for (is = lib->issues; is; is = is->next)
    {
        if (is->return_date != 0)
            continue;
        if (member_id != 0 && is->member_id != member_id)
            continue;
        if (overdue_only && now <= is->due_date)
            continue;

        if (count == 0)
            active_header();
        active_row(lib, is, now);
        count++;
    }

    if (count > 0)
        print_border(AW, ICOLS);
    return count;
}

void show_member_issues(const LIBRARY *lib, int member_id)
{
    int count = list_active(lib, member_id, 0);

    if (count == 0)
        printf("\nNo books currently issued to this member.\n");
    else
        printf("%d of %d allowed book(s) issued\n", count, MAX_BOOKS_PER_MEMBER);
}

static void show_history(const LIBRARY *lib)
{
    const ISSUE *is;
    char d1[DATE_STR], d2[DATE_STR], fine[16];
    int count = 0, total_fine = 0;

    if (lib->issues == NULL)
    {
        printf("\nNo Issue Records Found\n");
        return;
    }

    printf("\n");
    print_border(HW, ICOLS);
    printf("| %-4s | %-22s | %-16s | %-10s | %-10s | %6s |\n",
           "No", "Book Title", "Member", "Issued", "Returned", "Fine");
    print_border(HW, ICOLS);

    for (is = lib->issues; is; is = is->next)
    {
        format_date(is->issue_date, d1);

        if (is->return_date == 0)
        {
            strcpy(d2, "Not yet");
            strcpy(fine, "-");
        }
        else
        {
            format_date(is->return_date, d2);
            snprintf(fine, sizeof fine, "%d", is->fine);
            total_fine += is->fine;
        }

        printf("| %-4d | %-22.22s | %-16.16s | %-10s | %-10s | %6s |\n",
               is->id, title_of(lib, is->book_id), name_of(lib, is->member_id),
               d1, d2, fine);
        count++;
    }
    print_border(HW, ICOLS);

    printf("Total Records : %d   Not Returned : %d   Fine Collected : Rs. %d\n",
           count, count_active(lib->issues), total_fine);
}

/* ---------- Issue ---------- */

void issue_book(LIBRARY *lib)
{
    MEMBER *m;
    BOOK *b;
    ISSUE *is, *last;
    char prompt[100], buf[DATE_STR];
    int now = today(), idate;

    printf("\n========== ISSUE BOOK ==========\n");

    if (lib->books == NULL || lib->members == NULL)
    {
        printf("Add at least one book and one member first.\n");
        return;
    }

    m = find_member(lib->members, read_int("Enter Member ID : ", 1, INT_MAX));
    if (m == NULL)
    {
        printf("Member Not Found\n");
        return;
    }
    printf("Member : %s (%d of %d books issued)\n", m->name, m->issued, MAX_BOOKS_PER_MEMBER);

    if (m->issued >= MAX_BOOKS_PER_MEMBER)
    {
        printf("Limit reached. The member must return a book first.\n");
        return;
    }

    b = find_book(lib->books, read_int("Enter Book ID   : ", 1, INT_MAX));
    if (b == NULL)
    {
        printf("Book Not Found\n");
        return;
    }
    printf("Book   : %s by %s (%d of %d available)\n", b->title, b->author, b->available, b->total);

    if (b->available == 0)
    {
        printf("No copies available right now.\n");
        return;
    }

    for (is = lib->issues; is; is = is->next)
    {
        if (is->return_date == 0 && is->book_id == b->id && is->member_id == m->id)
        {
            printf("This member already has a copy of this book (Issue No %d).\n", is->id);
            return;
        }
    }

    format_date(now, buf);
    snprintf(prompt, sizeof prompt, "Enter Issue Date (DD-MM-YYYY) [Enter = %s] : ", buf);
    idate = read_date(prompt, now, date_to_days(1, 1, 2000), now);

    if (!confirm("Confirm issue? (Y/N) : "))
    {
        printf("Issue Cancelled\n");
        return;
    }

    is = (ISSUE *)malloc(sizeof(ISSUE));
    if (is == NULL)
    {
        printf("Memory Allocation Failed\n");
        return;
    }

    is->id = next_issue_id(lib->issues);
    is->book_id = b->id;
    is->member_id = m->id;
    is->issue_date = idate;
    is->due_date = idate + LOAN_DAYS;
    is->return_date = 0;
    is->fine = 0;
    is->next = NULL;

    if (lib->issues == NULL)
        lib->issues = is;
    else
    {
        for (last = lib->issues; last->next; last = last->next)
            ;
        last->next = is;
    }

    b->available--;
    m->issued++;
    lib->unsaved = 1;

    format_date(is->due_date, buf);
    printf("\nBook Issued Successfully\n");
    printf("Issue No : %d   Due Date : %s\n", is->id, buf);
}

/* ---------- Return ---------- */

void return_book(LIBRARY *lib)
{
    ISSUE *is;
    MEMBER *m;
    BOOK *b;
    char prompt[100], buf[DATE_STR];
    int ch, member_id = 0, now = today(), latest, rdate, late, fine;

    printf("\n========== RETURN BOOK ==========\n");

    if (count_active(lib->issues) == 0)
    {
        printf("No Books Are Currently Issued\n");
        return;
    }

    printf("1 : Find by Issue No\n");
    printf("2 : Find by Member ID\n");
    ch = read_choice("Enter Your Choice : ");

    if (ch == 2)
    {
        m = find_member(lib->members, read_int("Enter Member ID : ", 1, INT_MAX));
        if (m == NULL)
        {
            printf("Member Not Found\n");
            return;
        }
        printf("\nBooks issued to %s:", m->name);
        if (list_active(lib, m->id, 0) == 0)
        {
            printf("\nNo books currently issued to this member.\n");
            return;
        }
        member_id = m->id;
    }
    else if (ch != 1)
    {
        printf("Invalid Choice\n");
        return;
    }

    is = find_issue(lib->issues, read_int("Enter Issue No : ", 1, INT_MAX));

    if (is == NULL || is->return_date != 0 || (member_id != 0 && is->member_id != member_id))
    {
        printf("No Book Currently Issued with that Issue No\n");
        return;
    }

    if (ch == 1)
    {
        active_header();
        active_row(lib, is, now);
        print_border(AW, ICOLS);
    }

    latest = now < is->issue_date ? is->issue_date : now;
    format_date(latest, buf);
    snprintf(prompt, sizeof prompt, "Enter Return Date (DD-MM-YYYY) [Enter = %s] : ", buf);
    rdate = read_date(prompt, latest, is->issue_date, latest);

    late = rdate - is->due_date;
    fine = late > 0 ? late * FINE_PER_DAY : 0;

    if (late > 0)
        printf("Returned %d day(s) late. Fine : Rs. %d (Rs. %d per day)\n", late, fine, FINE_PER_DAY);
    else
        printf("Returned on time. No fine.\n");

    if (!confirm("Confirm return? (Y/N) : "))
    {
        printf("Return Cancelled\n");
        return;
    }

    is->return_date = rdate;
    is->fine = fine;

    b = find_book(lib->books, is->book_id);
    if (b != NULL && b->available < b->total)
        b->available++;

    m = find_member(lib->members, is->member_id);
    if (m != NULL && m->issued > 0)
        m->issued--;

    lib->unsaved = 1;
    printf("Book Returned Successfully\n");
}

/* ---------- Reports menu ---------- */

void issued_menu(LIBRARY *lib)
{
    const char *items[] = {
        "1 : Currently Issued Books",
        "2 : Overdue Books",
        "3 : Full Issue History",
        "0 : Back to Main Menu"
    };
    int n = sizeof items / sizeof items[0], count;

    while (1)
    {
        print_menu("ISSUED BOOKS", items, n);

        switch (read_choice("\nEnter Your Choice : "))
        {
            case 1:
                count = list_active(lib, 0, 0);
                if (count == 0)
                    printf("\nNo Books Are Currently Issued\n");
                else
                    printf("%d Book(s) Currently Issued\n", count);
                break;

            case 2:
                count = list_active(lib, 0, 1);
                if (count == 0)
                    printf("\nNo Overdue Books\n");
                else
                    printf("%d Overdue Book(s). Fine is Rs. %d per day late.\n", count, FINE_PER_DAY);
                break;

            case 3:
                show_history(lib);
                break;

            case 0:
                return;

            default:
                printf("\nInvalid Choice...\n");
        }
    }
}
