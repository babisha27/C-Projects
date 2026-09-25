# Student Record Management System

A menu-driven console application in C to manage student records using a **singly linked list** and **file handling**. Records are saved to a file and loaded automatically on the next run.

## Features

- Add a student (roll number assigned automatically)
- Delete a record by roll number or name (with confirmation)
- Modify a record, searching by roll number, name or percentage
- Display all records in a table
- Sort by roll number, name, or percentage (highest first)
- Save records to `student.dat` and load them automatically on startup
- Input validation (rejects invalid numbers, empty or too-long names, percentages outside 0–100)
- Names with spaces are supported (e.g. "Ravi Kumar")
- Warns about unsaved changes before exit

## File Structure

| File | Purpose |
|------|---------|
| `main.c` | Menu and program flow |
| `student.h` | Structure definition and function prototypes |
| `student_add.c` | Add a record |
| `student_del.c` | Delete a record |
| `student_mod.c` | Modify a record |
| `student_show.c` | Display records in a table |
| `student_sort.c` | Sort records |
| `student_save.c` | Save records to file |
| `student_load.c` | Load records from file |
| `student_util.c` | Safe input functions and helpers |
| `makefile` | Build instructions |

## How to Compile and Run

Using make:
```
make
./student
```

Or compile manually:
```
gcc *.c -o student -lm
./student
```

To remove compiled files:
```
make clean
```

## Sample Output

```
    *************************************
    *        STUDENT RECORD MENU        *
    *************************************
    *   A : Add New Record              *
    *   D : Delete a Record             *
    *   S : Show the List               *
    *   M : Modify a Record             *
    *   V : Save                        *
    *   T : Sort the List               *
    *   E : Exit                        *
    *************************************

Enter Your Choice : s

+---------+--------------------------------+------------+
| Roll No | Name                           | Percentage |
+---------+--------------------------------+------------+
| 1       | Ravi Kumar                     |      78.50 |
| 2       | Anu                            |      91.00 |
| 3       | Karthik                        |      65.00 |
+---------+--------------------------------+------------+
Total Records : 3
```

## Data File Format

Records are stored in `student.dat`, one per line:
```
roll|name|percentage
1|Ravi Kumar|78.50
```

## Concepts Used

- Structures and `typedef`
- Singly linked list (insert, delete, traverse, sort)
- Dynamic memory allocation (`malloc`, `free`)
- File handling (`fopen`, `fprintf`, `fgets`, `sscanf`)
- Pointers and pointer-to-pointer
- Function pointers (for sorting)
- Modular programming with multiple source files and a makefile