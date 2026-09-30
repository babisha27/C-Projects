#include "lms.h"

#define LINE_LEN 512

/* File formats (one record per line, '|' separated):
     books.dat   : id|title|author|total_copies
     members.dat : id|name|phone
     issues.dat  : no|book_id|member_id|issue_date|due_date|return_date|fine
   Dates are written as DD-MM-YYYY; return_date is "-" if not returned.
   Available copies and books-per-member are worked out from issues.dat,
   so they can never go out of step. */

/* Reads the next non-empty line.
   Returns 1 = got a line, 0 = end of file, -1 = line too long (skipped) */
static int next_line(FILE *fp, char *buf, int size)
{
    int c;

    while (fgets(buf, size, fp) != NULL)
    {
        if (strchr(buf, '\n') == NULL && !feof(fp))
        {
            while ((c = fgetc(fp)) != '\n' && c != EOF)
                ;
            return -1;
        }

        buf[strcspn(buf, "\r\n")] = '\0';
        if (buf[0] != '\0')
            return 1;
    }
    return 0;
}

/* ======================= LOAD ======================= */

static int load_books(LIBRARY *lib, int *skipped)
{
    FILE *fp;
    BOOK *b, *last = NULL;
    char line[LINE_LEN], title[TITLE_LEN], author[AUTHOR_LEN];
    int id, total, used, r, count = 0;

    fp = fopen(BOOK_FILE, "r");
    if (fp == NULL)
        return -1;

    while ((r = next_line(fp, line, sizeof line)) != 0)
    {
        used = 0;
        if (r < 0 ||
            sscanf(line, "%d|%35[^|]|%25[^|]|%d%n", &id, title, author, &total, &used) != 4 ||
            line[used] != '\0' || id < 1 || total < 1 || total > MAX_COPIES ||
            find_book(lib->books, id) != NULL)
        {
            (*skipped)++;
            continue;
        }

        b = (BOOK *)malloc(sizeof(BOOK));
        if (b == NULL)
        {
            printf("Memory Allocation Failed While Loading\n");
            break;
        }

        b->id = id;
        strcpy(b->title, title);
        strcpy(b->author, author);
        b->total = total;
        b->available = total;
        b->next = NULL;

        if (last == NULL)
            lib->books = b;
        else
            last->next = b;
        last = b;
        count++;
    }

    fclose(fp);
    return count;
}

static int load_members(LIBRARY *lib, int *skipped)
{
    FILE *fp;
    MEMBER *m, *last = NULL;
    char line[LINE_LEN], name[NAME_LEN], phone[PHONE_LEN];
    int id, used, r, count = 0;

    fp = fopen(MEMBER_FILE, "r");
    if (fp == NULL)
        return -1;

    while ((r = next_line(fp, line, sizeof line)) != 0)
    {
        used = 0;
        if (r < 0 ||
            sscanf(line, "%d|%25[^|]|%10[^|]%n", &id, name, phone, &used) != 3 ||
            line[used] != '\0' || id < 1 || !valid_phone(phone) ||
            find_member(lib->members, id) != NULL)
        {
            (*skipped)++;
            continue;
        }

        m = (MEMBER *)malloc(sizeof(MEMBER));
        if (m == NULL)
        {
            printf("Memory Allocation Failed While Loading\n");
            break;
        }

        m->id = id;
        strcpy(m->name, name);
        strcpy(m->phone, phone);
        m->issued = 0;
        m->next = NULL;

        if (last == NULL)
            lib->members = m;
        else
            last->next = m;
        last = m;
        count++;
    }

    fclose(fp);
    return count;
}

static int load_issues(LIBRARY *lib, int *skipped)
{
    FILE *fp;
    ISSUE *is, *last = NULL, *p;
    BOOK *b;
    MEMBER *m;
    char line[LINE_LEN], s1[DATE_STR], s2[DATE_STR], s3[DATE_STR];
    int id, bid, mid, idate, ddate, rdate, fine, used, r, dup, count = 0;

    fp = fopen(ISSUE_FILE, "r");
    if (fp == NULL)
        return -1;

    while ((r = next_line(fp, line, sizeof line)) != 0)
    {
        used = 0;
        rdate = 0;
        if (r < 0 ||
            sscanf(line, "%d|%d|%d|%15[^|]|%15[^|]|%15[^|]|%d%n",
                   &id, &bid, &mid, s1, s2, s3, &fine, &used) != 7 ||
            line[used] != '\0' || id < 1 || bid < 1 || mid < 1 || fine < 0 ||
            !parse_date(s1, &idate) || !parse_date(s2, &ddate) ||
            (strcmp(s3, "-") != 0 && (!parse_date(s3, &rdate) || rdate < idate)))
        {
            (*skipped)++;
            continue;
        }

        for (dup = 0, p = lib->issues; p; p = p->next)
            if (p->id == id)
                dup = 1;
        if (dup)
        {
            (*skipped)++;
            continue;
        }

        is = (ISSUE *)malloc(sizeof(ISSUE));
        if (is == NULL)
        {
            printf("Memory Allocation Failed While Loading\n");
            break;
        }

        is->id = id;
        is->book_id = bid;
        is->member_id = mid;
        is->issue_date = idate;
        is->due_date = ddate;
        is->return_date = rdate;
        is->fine = fine;
        is->next = NULL;

        if (last == NULL)
            lib->issues = is;
        else
            last->next = is;
        last = is;
        count++;

        /* Book still out: update available copies and member count */
        if (rdate == 0)
        {
            b = find_book(lib->books, bid);
            if (b != NULL && b->available > 0)
                b->available--;

            m = find_member(lib->members, mid);
            if (m != NULL)
                m->issued++;
        }
    }

    fclose(fp);
    return count;
}

void load_all(LIBRARY *lib)
{
    int skipped = 0, nb, nm, ni;

    nb = load_books(lib, &skipped);
    nm = load_members(lib, &skipped);
    ni = load_issues(lib, &skipped);

    if (nb < 0 && nm < 0 && ni < 0)
    {
        printf("No Saved Data Found. Starting with an Empty Library.\n");
        return;
    }

    printf("Loaded : %d Book(s), %d Member(s), %d Issue Record(s)\n",
           nb < 0 ? 0 : nb, nm < 0 ? 0 : nm, ni < 0 ? 0 : ni);

    if (skipped > 0)
        printf("Warning : %d Invalid Line(s) Skipped\n", skipped);
}

/* ======================= SAVE ======================= */

static void write_books(FILE *fp, const LIBRARY *lib)
{
    const BOOK *b;

    for (b = lib->books; b; b = b->next)
        fprintf(fp, "%d|%s|%s|%d\n", b->id, b->title, b->author, b->total);
}

static void write_members(FILE *fp, const LIBRARY *lib)
{
    const MEMBER *m;

    for (m = lib->members; m; m = m->next)
        fprintf(fp, "%d|%s|%s\n", m->id, m->name, m->phone);
}

static void write_issues(FILE *fp, const LIBRARY *lib)
{
    const ISSUE *is;
    char d1[DATE_STR], d2[DATE_STR], d3[DATE_STR];

    for (is = lib->issues; is; is = is->next)
    {
        format_date(is->issue_date, d1);
        format_date(is->due_date, d2);
        format_date(is->return_date, d3);      /* "-" if not returned */
        fprintf(fp, "%d|%d|%d|%s|%s|%s|%d\n",
                is->id, is->book_id, is->member_id, d1, d2, d3, is->fine);
    }
}

/* Writes to a temporary file first, then replaces the real file,
   so old data is not lost if writing fails. */
static int save_one(const char *file, void (*writer)(FILE *, const LIBRARY *),
                    const LIBRARY *lib)
{
    char tmp[64];
    FILE *fp;

    snprintf(tmp, sizeof tmp, "%s.tmp", file);

    fp = fopen(tmp, "w");
    if (fp == NULL)
    {
        printf("Error : Cannot Write %s\n", file);
        return 0;
    }

    writer(fp, lib);

    if (ferror(fp) || fclose(fp) != 0)
    {
        remove(tmp);
        printf("Error : Failed Writing %s\n", file);
        return 0;
    }

    remove(file);
    if (rename(tmp, file) != 0)
    {
        printf("Error : Could Not Replace %s (data kept in %s)\n", file, tmp);
        return 0;
    }
    return 1;
}

int save_all(const LIBRARY *lib)
{
    if (save_one(BOOK_FILE, write_books, lib) &&
        save_one(MEMBER_FILE, write_members, lib) &&
        save_one(ISSUE_FILE, write_issues, lib))
    {
        printf("All Data Saved Successfully\n");
        return 1;
    }

    printf("Save Failed\n");
    return 0;
}

/* ======================= FREE ======================= */

void free_all(LIBRARY *lib)
{
    BOOK *b;
    MEMBER *m;
    ISSUE *is;

    while (lib->books)
    {
        b = lib->books;
        lib->books = b->next;
        free(b);
    }
    while (lib->members)
    {
        m = lib->members;
        lib->members = m->next;
        free(m);
    }
    while (lib->issues)
    {
        is = lib->issues;
        lib->issues = is->next;
        free(is);
    }
}
