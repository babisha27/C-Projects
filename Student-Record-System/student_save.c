#include "student.h"

#define TEMP_FILE DATA_FILE ".tmp"

/* File format, one record per line:  roll|name|percentage
   Data is written to a temporary file first, so the old file is
   not lost if writing fails. */
int save_file(ST *ptr)
{
    FILE *fp;
    int count = 0;

    fp = fopen(TEMP_FILE, "w");
    if (fp == NULL)
    {
        printf("Error: Cannot Open File for Writing\n");
        return 0;
    }

    while (ptr)
    {
        fprintf(fp, "%d|%s|%.2f\n", ptr->roll, ptr->name, ptr->per);
        count++;
        ptr = ptr->next;
    }

    if (ferror(fp))
    {
        fclose(fp);
        remove(TEMP_FILE);
        printf("Error: Failed to Write Records\n");
        return 0;
    }

    if (fclose(fp) != 0)
    {
        remove(TEMP_FILE);
        printf("Error: Failed to Write Records\n");
        return 0;
    }

    remove(DATA_FILE);
    if (rename(TEMP_FILE, DATA_FILE) != 0)
    {
        printf("Error: Could Not Replace %s (data kept in %s)\n", DATA_FILE, TEMP_FILE);
        return 0;
    }

    printf("%d Record(s) Saved Successfully\n", count);
    return 1;
}
