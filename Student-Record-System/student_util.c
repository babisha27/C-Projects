#include <ctype.h>
#include <errno.h>
#include "student.h"

#define LINE_LEN 256

/* Reads one full line from the keyboard, removes the newline and
   discards any extra characters if the line was too long. */
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

/* Returns a single upper-case character, or '\0' if the input
   was not exactly one character. */
char read_char(const char *prompt)
{
    char line[LINE_LEN], *s;

    printf("%s", prompt);
    fflush(stdout);
    read_line(line, sizeof line);
    s = trim(line);

    if (strlen(s) != 1)
        return '\0';

    return (char)toupper((unsigned char)s[0]);
}

/* Keeps asking until a whole number in [min, max] is entered */
int read_int(const char *prompt, int min, int max)
{
    char line[LINE_LEN], *s, *end;
    long val;

    while (1)
    {
        printf("%s", prompt);
        fflush(stdout);
        read_line(line, sizeof line);
        s = trim(line);

        errno = 0;
        val = strtol(s, &end, 10);

        if (*s != '\0' && *end == '\0' && errno == 0 && val >= min && val <= max)
            return (int)val;

        printf("Invalid input. Enter a whole number between %d and %d.\n", min, max);
    }
}

/* Keeps asking until a number in [min, max] is entered.
   The value is rounded to 2 decimal places. */
float read_float(const char *prompt, float min, float max)
{
    char line[LINE_LEN], *s, *end;
    double val;

    while (1)
    {
        printf("%s", prompt);
        fflush(stdout);
        read_line(line, sizeof line);
        s = trim(line);

        errno = 0;
        val = strtod(s, &end);

        if (*s != '\0' && *end == '\0' && errno == 0 && val >= min && val <= max)
            return (float)(round(val * 100.0) / 100.0);

        printf("Invalid input. Enter a number between %.2f and %.2f.\n", min, max);
    }
}

/* Reads a name (spaces allowed). Rejects empty names, names that are
   too long, and the '|' character (used as separator in the file). */
void read_name(const char *prompt, char *name)
{
    char line[LINE_LEN], *s;

    while (1)
    {
        printf("%s", prompt);
        fflush(stdout);
        read_line(line, sizeof line);
        s = trim(line);

        if (*s == '\0')
            printf("Name cannot be empty.\n");
        else if (strlen(s) > NAME_LEN - 1)
            printf("Name too long (maximum %d characters).\n", NAME_LEN - 1);
        else if (strchr(s, '|') != NULL)
            printf("Name cannot contain the '|' character.\n");
        else
        {
            strcpy(name, s);
            return;
        }
    }
}

/* Asks a Yes/No question. Returns 1 for Y, 0 for N. */
int confirm(const char *prompt)
{
    char ch;

    while (1)
    {
        ch = read_char(prompt);
        if (ch == 'Y')
            return 1;
        if (ch == 'N')
            return 0;
        printf("Please enter Y or N.\n");
    }
}

/* Case-insensitive string comparison */
int name_icmp(const char *a, const char *b)
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

ST *find_by_roll(ST *head, int roll)
{
    while (head)
    {
        if (head->roll == roll)
            return head;
        head = head->next;
    }
    return NULL;
}

static int is_match(const ST *s, char mode, int roll, const char *name, float per)
{
    switch (mode)
    {
        case 'R': return s->roll == roll;
        case 'N': return name_icmp(s->name, name) == 0;
        case 'P': return fabs(s->per - per) < 0.005;
    }
    return 0;
}

/* Searches by Roll (R), Name (N) or Percentage (P), shows the matching
   records and returns the one the user selects (NULL if none). */
ST *select_record(ST *head, char mode)
{
    ST *temp, *found = NULL;
    int roll = 0, pick, count = 0;
    float per = 0.0f;
    char name[NAME_LEN] = "";

    switch (mode)
    {
        case 'R':
            roll = read_int("Enter Roll Number : ", 1, INT_MAX);
            break;
        case 'N':
            read_name("Enter Name : ", name);
            break;
        case 'P':
            per = read_float("Enter Percentage : ", 0.0f, 100.0f);
            break;
        default:
            printf("Invalid Choice\n");
            return NULL;
    }

    for (temp = head; temp; temp = temp->next)
    {
        if (is_match(temp, mode, roll, name, per))
        {
            count++;
            found = temp;
        }
    }

    if (count == 0)
    {
        printf("No Matching Record Found\n");
        return NULL;
    }

    printf("\nMatching Record(s):");
    print_table_header();
    for (temp = head; temp; temp = temp->next)
        if (is_match(temp, mode, roll, name, per))
            print_record(temp);
    print_table_footer();

    if (count == 1)
        return found;

    pick = read_int("Multiple records found. Enter Roll Number to select : ", 1, INT_MAX);
    found = find_by_roll(head, pick);

    if (found == NULL || !is_match(found, mode, roll, name, per))
    {
        printf("That Roll Number is not in the list above\n");
        return NULL;
    }

    return found;
}

void free_list(ST **head)
{
    ST *temp;

    while (*head)
    {
        temp = *head;
        *head = temp->next;
        free(temp);
    }
}
