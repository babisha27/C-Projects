#ifndef STUDENT_H
#define STUDENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#define NAME_LEN  30              /* max 29 characters + '\0' */
#define DATA_FILE "student.dat"

typedef struct student
{
    int roll;
    char name[NAME_LEN];
    float per;
    struct student *next;
} ST;

/* Operations - functions returning int give 1 if the list was changed */
int  stud_add(ST **);
int  stud_del(ST **);
void stud_show(ST *);
int  stud_mod(ST *);
int  stud_sort(ST *);
int  save_file(ST *);
void load_file(ST **);

/* Display helpers (student_show.c) */
void print_table_header(void);
void print_record(const ST *);
void print_table_footer(void);

/* Input and list helpers (student_util.c) */
char  read_char(const char *prompt);
int   read_int(const char *prompt, int min, int max);
float read_float(const char *prompt, float min, float max);
void  read_name(const char *prompt, char *name);
int   confirm(const char *prompt);
int   name_icmp(const char *a, const char *b);
ST   *find_by_roll(ST *head, int roll);
ST   *select_record(ST *head, char mode);
void  free_list(ST **head);

#endif
