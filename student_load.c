#include "student.h"

/* Reads records from the data file. Accepts the new format
   (roll|name|percentage) and the old format (roll name percentage).
   Invalid or duplicate lines are skipped. */
void load_file(ST **ptr)
{
    FILE *fp;
    ST *temp, *last;
    char line[256], name[NAME_LEN];
    int roll, c, count = 0, skipped = 0;
    float per;

    fp = fopen(DATA_FILE, "r");
    if (fp == NULL)
    {
        printf("No Saved Records Found. Starting with an Empty List.\n");
        return;
    }

    last = *ptr;
    while (last && last->next)
        last = last->next;

    while (fgets(line, sizeof line, fp) != NULL)
    {
        /* Line too long for the buffer: skip the rest of it */
        if (strchr(line, '\n') == NULL && !feof(fp))
        {
            while ((c = fgetc(fp)) != '\n' && c != EOF)
                ;
            skipped++;
            continue;
        }

        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
            continue;

        if (sscanf(line, "%d|%29[^|]|%f", &roll, name, &per) != 3 &&
            sscanf(line, "%d %29s %f", &roll, name, &per) != 3)
        {
            skipped++;
            continue;
        }

        if (roll < 1 || per < 0.0f || per > 100.0f || find_by_roll(*ptr, roll) != NULL)
        {
            skipped++;
            continue;
        }

        temp = (ST *)malloc(sizeof(ST));
        if (temp == NULL)
        {
            printf("Memory Allocation Failed While Loading\n");
            break;
        }

        temp->roll = roll;
        strcpy(temp->name, name);
        temp->per = (float)(round(per * 100.0) / 100.0);
        temp->next = NULL;

        if (last == NULL)
            *ptr = temp;
        else
            last->next = temp;
        last = temp;

        count++;
    }

    fclose(fp);

    printf("%d Record(s) Loaded Successfully\n", count);
    if (skipped > 0)
        printf("Warning: %d Invalid Line(s) Skipped\n", skipped);
}
