#ifndef LMS_H
#define LMS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* Field sizes (maximum characters + 1 for '\0') */
#define TITLE_LEN   36
#define AUTHOR_LEN  26
#define NAME_LEN    26
#define PHONE_LEN   11      /* 10 digits */
#define DATE_STR    16      /* buffer for "DD-MM-YYYY" */

/* Library rules */
#define MAX_COPIES            100
#define MAX_BOOKS_PER_MEMBER  3
#define LOAN_DAYS             14
#define FINE_PER_DAY          2     /* Rs. per day late */

/* Data files */
#define BOOK_FILE    "books.dat"
#define MEMBER_FILE  "members.dat"
#define ISSUE_FILE   "issues.dat"

typedef struct book
{
    int id;
    char title[TITLE_LEN];
    char author[AUTHOR_LEN];
    int total;              /* total copies */
    int available;          /* copies on the shelf */
    struct book *next;
} BOOK;

typedef struct member
{
    int id;
    char name[NAME_LEN];
    char phone[PHONE_LEN];
    int issued;             /* books currently borrowed */
    struct member *next;
} MEMBER;

typedef struct issue
{
    int id;                 /* issue number */
    int book_id;
    int member_id;
    int issue_date;         /* dates are stored as day numbers */
    int due_date;
    int return_date;        /* 0 = not returned yet */
    int fine;
    struct issue *next;
} ISSUE;

/* Everything the program works with, kept in one place */
typedef struct
{
    BOOK *books;
    MEMBER *members;
    ISSUE *issues;
    int unsaved;            /* 1 if there are changes not yet saved */
} LIBRARY;

/* book.c */
void    book_menu(LIBRARY *lib);
BOOK   *find_book(BOOK *head, int id);

/* member.c */
void    member_menu(LIBRARY *lib);
MEMBER *find_member(MEMBER *head, int id);

/* issue.c */
void    issue_book(LIBRARY *lib);
void    return_book(LIBRARY *lib);
void    issued_menu(LIBRARY *lib);
void    show_member_issues(const LIBRARY *lib, int member_id);

/* fileio.c */
void    load_all(LIBRARY *lib);
int     save_all(const LIBRARY *lib);
void    free_all(LIBRARY *lib);

/* util.c - screen */
void    print_menu(const char *title, const char *items[], int n);
void    print_border(const int widths[], int n);

/* util.c - keyboard input */
int     read_choice(const char *prompt);
int     read_int(const char *prompt, int min, int max);
int     read_int_keep(const char *prompt, int min, int max, int current);
void    read_text(const char *prompt, char *buf, int size, const char *what);
void    read_text_keep(const char *prompt, char *buf, int size, const char *what);
void    read_phone(const char *prompt, char *buf, int keep);
int     valid_phone(const char *s);
int     confirm(const char *prompt);

/* util.c - strings */
int     str_icmp(const char *a, const char *b);
int     str_icontains(const char *text, const char *word);

/* util.c - dates */
int     date_to_days(int d, int m, int y);
int     today(void);
void    format_date(int days, char *buf);
int     parse_date(const char *s, int *days);
int     read_date(const char *prompt, int def, int min, int max);

#endif
