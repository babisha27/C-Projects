#include <ctype.h>
#include <errno.h>
#include <time.h>
#include "lms.h"

#define LINE_LEN 256

/* ======================= SCREEN ======================= */

void print_menu(const char *title, const char *items[], int n)
{
    int i, len = (int)strlen(title);
    int left = (39 - len) / 2, right = 39 - len - left;

    printf("\n    *****************************************\n");
    printf("    *%*s%s%*s*\n", left, "", title, right, "");
    printf("    *****************************************\n");
    for (i = 0; i < n; i++)
        printf("    *   %-35s *\n", items[i]);
    printf("    *****************************************\n");
}

/* Prints a line like +------+--------+ for the given column widths */
void print_border(const int widths[], int n)
{
    int i, j;

    for (i = 0; i < n; i++)
    {
        putchar('+');
        for (j = 0; j < widths[i] + 2; j++)
            putchar('-');
    }
    printf("+\n");
}

/* ======================= INPUT ======================= */

/* Reads one full line, removes the newline, discards extra characters */
static void read_line(char *buf, int size)
{
    int c;

    if (fgets(buf, size, stdin) == NULL)
    {
        printf("\nInput ended. Exiting without saving...\n");
        exit(0);
    }

    if (strchr(buf, '\n') == NULL)
        while ((c = getchar()) != '\n' && c != EOF)
            ;

    buf[strcspn(buf, "\r\n")] = '\0';
}

/* Removes leading and trailing spaces */
static char *trim(char *s)
{
    char *end;

    while (isspace((unsigned char)*s))
        s++;

    if (*s == '\0')
        return s;

    end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end))
        end--;
    end[1] = '\0';

    return s;
}

static char *get_input(const char *prompt, char *line, int size)
{
    printf("%s", prompt);
    fflush(stdout);
    read_line(line, size);
    return trim(line);
}

static int parse_int(const char *s, int min, int max, int *out)
{
    char *end;
    long v;

    errno = 0;
    v = strtol(s, &end, 10);

    if (*s == '\0' || *end != '\0' || errno != 0 || v < min || v > max)
        return 0;

    *out = (int)v;
    return 1;
}

/* Menu choice: returns the number, or -1 if the input is invalid */
int read_choice(const char *prompt)
{
    char line[LINE_LEN];
    int v;

    if (parse_int(get_input(prompt, line, sizeof line), 0, 99, &v))
        return v;
    return -1;
}

/* Keeps asking until a whole number in [min, max] is entered */
int read_int(const char *prompt, int min, int max)
{
    char line[LINE_LEN];
    int v;

    while (1)
    {
        if (parse_int(get_input(prompt, line, sizeof line), min, max, &v))
            return v;
        printf("Invalid input. Enter a whole number between %d and %d.\n", min, max);
    }
}

/* Same as read_int, but pressing Enter keeps the current value */
int read_int_keep(const char *prompt, int min, int max, int current)
{
    char line[LINE_LEN], *s;
    int v;

    while (1)
    {
        s = get_input(prompt, line, sizeof line);
        if (*s == '\0')
            return current;
        if (parse_int(s, min, max, &v))
            return v;
        printf("Invalid input. Enter a whole number between %d and %d.\n", min, max);
    }
}

static int text_ok(const char *s, int size, const char *what)
{
    if (strlen(s) > (size_t)(size - 1))
    {
        printf("%s too long (maximum %d characters).\n", what, size - 1);
        return 0;
    }
    if (strchr(s, '|') != NULL)
    {
        printf("%s cannot contain the '|' character.\n", what);
        return 0;
    }
    return 1;
}

/* Reads a non-empty text (spaces allowed) */
void read_text(const char *prompt, char *buf, int size, const char *what)
{
    char line[LINE_LEN], *s;

    while (1)
    {
        s = get_input(prompt, line, sizeof line);

        if (*s == '\0')
            printf("%s cannot be empty.\n", what);
        else if (text_ok(s, size, what))
        {
            strcpy(buf, s);
            return;
        }
    }
}

/* Reads a text; pressing Enter keeps the current value in buf */
void read_text_keep(const char *prompt, char *buf, int size, const char *what)
{
    char line[LINE_LEN], *s;

    while (1)
    {
        s = get_input(prompt, line, sizeof line);

        if (*s == '\0')
            return;
        if (text_ok(s, size, what))
        {
            strcpy(buf, s);
            return;
        }
    }
}

/* A valid phone number is exactly 10 digits */
int valid_phone(const char *s)
{
    int i;

    if (strlen(s) != 10)
        return 0;
    for (i = 0; i < 10; i++)
        if (!isdigit((unsigned char)s[i]))
            return 0;
    return 1;
}

/* Reads a phone number. If keep is 1, Enter keeps the current value. */
void read_phone(const char *prompt, char *buf, int keep)
{
    char line[LINE_LEN], *s;

    while (1)
    {
        s = get_input(prompt, line, sizeof line);

        if (*s == '\0' && keep)
            return;
        if (valid_phone(s))
        {
            strcpy(buf, s);
            return;
        }
        printf("Invalid phone number. Enter exactly 10 digits.\n");
    }
}

/* Asks a Yes/No question. Returns 1 for Y, 0 for N. */
int confirm(const char *prompt)
{
    char line[LINE_LEN], *s;

    while (1)
    {
        s = get_input(prompt, line, sizeof line);

        if (strlen(s) == 1)
        {
            if (toupper((unsigned char)s[0]) == 'Y')
                return 1;
            if (toupper((unsigned char)s[0]) == 'N')
                return 0;
        }
        printf("Please enter Y or N.\n");
    }
}

/* ======================= STRINGS ======================= */

/* Case-insensitive comparison */
int str_icmp(const char *a, const char *b)
{
    int d;

    while (*a && *b)
    {
        d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0)
            return d;
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

/* Returns 1 if word appears anywhere in text (case-insensitive) */
int str_icontains(const char *text, const char *word)
{
    size_t i, j, n = strlen(word);

    if (n == 0)
        return 1;

    for (i = 0; text[i] != '\0'; i++)
    {
        for (j = 0; j < n && text[i + j] != '\0'; j++)
            if (tolower((unsigned char)text[i + j]) != tolower((unsigned char)word[j]))
                break;
        if (j == n)
            return 1;
    }
    return 0;
}

/* ======================= DATES =======================
   A date is stored as a day number (days counted from 01-01-0001),
   so the difference between two dates is a simple subtraction. */

static int is_leap(int y)
{
    return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
}

static int month_days(int m, int y)
{
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (m == 2 && is_leap(y))
        return 29;
    return days[m - 1];
}

int date_to_days(int d, int m, int y)
{
    int i, days, py = y - 1;

    days = py * 365 + py / 4 - py / 100 + py / 400;   /* full years */
    for (i = 1; i < m; i++)                           /* full months */
        days += month_days(i, y);
    return days + d;
}

static void days_to_date(int days, int *d, int *m, int *y)
{
    int yy = 1, mm = 1, len;

    while (days > (len = is_leap(yy) ? 366 : 365))
    {
        days -= len;
        yy++;
    }
    while (days > (len = month_days(mm, yy)))
    {
        days -= len;
        mm++;
    }

    *d = days;
    *m = mm;
    *y = yy;
}

int today(void)
{
    time_t t = time(NULL);
    struct tm *now = localtime(&t);

    return date_to_days(now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
}

/* Writes the date as DD-MM-YYYY, or "-" for no date */
void format_date(int days, char *buf)
{
    int d, m, y;

    if (days <= 0)
    {
        strcpy(buf, "-");
        return;
    }
    days_to_date(days, &d, &m, &y);
    snprintf(buf, DATE_STR, "%02d-%02d-%04d", d % 100, m % 100, y % 10000);
}

/* Accepts DD-MM-YYYY or DD/MM/YYYY. Returns 1 if valid. */
int parse_date(const char *s, int *days)
{
    int d, m, y;
    char c1, c2, extra;

    if (sscanf(s, "%d%c%d%c%d%c", &d, &c1, &m, &c2, &y, &extra) != 5)
        return 0;
    if (!((c1 == '-' && c2 == '-') || (c1 == '/' && c2 == '/')))
        return 0;
    if (y < 1900 || y > 2999 || m < 1 || m > 12 || d < 1 || d > month_days(m, y))
        return 0;

    *days = date_to_days(d, m, y);
    return 1;
}

/* Reads a date between min and max. Pressing Enter gives def. */
int read_date(const char *prompt, int def, int min, int max)
{
    char line[LINE_LEN], *s, buf[DATE_STR];
    int v;

    while (1)
    {
        s = get_input(prompt, line, sizeof line);

        if (*s == '\0')
            v = def;
        else if (!parse_date(s, &v))
        {
            printf("Invalid date. Use the format DD-MM-YYYY.\n");
            continue;
        }

        if (v < min)
        {
            format_date(min, buf);
            printf("Date cannot be before %s.\n", buf);
        }
        else if (v > max)
        {
            format_date(max, buf);
            printf("Date cannot be after %s.\n", buf);
        }
        else
            return v;
    }
}
