# Library Management System (LMS)

A menu-driven console application in C to manage a library's **books**, **members**, and **book issue/return** with automatic **due dates** and **late fines**. All data is stored in linked lists and saved to files, so it is loaded automatically on the next run.

## Features

**Books**
- Add a book (Book ID assigned automatically, starting from 101)
- Adding a book that already exists offers to add more copies instead
- Show all books with total and available copies
- Search by Book ID, title or author (partial, case-insensitive)
- Modify title, author or number of copies (press Enter to keep a value)
- Delete a book (blocked while any copy is issued)
- Sort by ID, title, author, or available copies

**Members**
- Add a member (Member ID assigned automatically) with a 10-digit phone number
- Duplicate phone numbers are rejected
- Show, search by name, modify, delete (blocked while the member has books)
- View the books currently issued to a member

**Issue and Return**
- Issue a book to a member (maximum 3 books per member)
- Due date is set automatically, 14 days after the issue date
- Return a book, found by Issue No or by Member ID
- Late fine is calculated automatically: Rs. 2 per day after the due date
- Issue and return dates default to today, or can be typed as DD-MM-YYYY

**Reports**
- Currently issued books, with On time / Late status
- Overdue books
- Full issue history with fines collected

**Safety**
- All input is validated (numbers, dates, phone numbers, text length)
- Warns about unsaved changes before exit
- Data is saved safely through a temporary file, so a failed save never destroys old data

## File Structure

| File | Purpose |
|------|---------|
| `main.c` | Main menu and program flow |
| `lms.h` | Structures, constants and function prototypes |
| `book.c` | Book menu: add, show, search, modify, delete, sort |
| `member.c` | Member menu: add, show, search, modify, delete |
| `issue.c` | Issue, return, fines and reports |
| `fileio.c` | Load, save and free all data |
| `util.c` | Safe input, menus, tables, string and date functions |
| `makefile` | Build instructions |

## How to Compile and Run

Using make:
```
make
./lms
```

Or compile manually:
```
gcc *.c -o lms
./lms
```

To remove compiled files:
```
make clean
```

## Library Rules

These are defined in `lms.h` and are easy to change:

| Rule | Value |
|------|-------|
| Loan period | 14 days |
| Fine | Rs. 2 per day late |
| Books per member | 3 |
| Copies per title | 100 |

## Sample Output

```
    *****************************************
    *       LIBRARY MANAGEMENT SYSTEM       *
    *****************************************
    *   1 : Book Management                 *
    *   2 : Member Management               *
    *   3 : Issue a Book                    *
    *   4 : Return a Book                   *
    *   5 : Issued Books & History          *
    *   6 : Save                            *
    *   0 : Exit                            *
    *****************************************

Enter Your Choice : 3

========== ISSUE BOOK ==========
Enter Member ID : 1
Member : Priya Sharma (0 of 3 books issued)
Enter Book ID   : 102
Book   : Let Us C by Yashavant Kanetkar (3 of 3 available)
Enter Issue Date (DD-MM-YYYY) [Enter = 30-09-2026] : 01-09-2026
Confirm issue? (Y/N) : y

Book Issued Successfully
Issue No : 1   Due Date : 15-09-2026
```

Showing all books:
```
+-------+-------------------------------------+---------------------------+--------+--------+
| ID    | Title                               | Author                    | Copies |  Avail |
+-------+-------------------------------------+---------------------------+--------+--------+
| 101   | The C Programming Language          | Kernighan & Ritchie       |      3 |      2 |
| 102   | Let Us C                            | Yashavant Kanetkar        |      3 |      2 |
| 103   | Data Structures Using C             | Reema Thareja             |      1 |      0 |
+-------+-------------------------------------+---------------------------+--------+--------+
Titles : 3   Total Copies : 7   Available : 4   Issued : 3
```

Returning a late book:
```
========== RETURN BOOK ==========
1 : Find by Issue No
2 : Find by Member ID
Enter Your Choice : 2
Enter Member ID : 1

Books issued to Priya Sharma:
+------+------------------------+------------------+------------+------------+-----------+
| No   | Book Title             | Member           | Issued     | Due        | Status    |
+------+------------------------+------------------+------------+------------+-----------+
| 1    | Let Us C               | Priya Sharma     | 01-09-2026 | 15-09-2026 | Late 15d  |
| 3    | Data Structures Using  | Priya Sharma     | 30-09-2026 | 14-10-2026 | On time   |
+------+------------------------+------------------+------------+------------+-----------+
Enter Issue No : 1
Enter Return Date (DD-MM-YYYY) [Enter = 30-09-2026] : 
Returned 15 day(s) late. Fine : Rs. 30 (Rs. 2 per day)
Confirm return? (Y/N) : y
Book Returned Successfully
```

Rules being enforced:
```
Member : Priya Sharma (3 of 3 books issued)
Limit reached. The member must return a book first.

Book   : Data Structures Using C by Reema Thareja (0 of 1 available)
No copies available right now.

Cannot Delete : 2 copy(ies) currently issued. Return them first.
```

## Data Files

Created automatically when you save. One record per line, `|` separated:

| File | Format | Example |
|------|--------|---------|
| `books.dat` | id\|title\|author\|copies | `102\|Let Us C\|Yashavant Kanetkar\|3` |
| `members.dat` | id\|name\|phone | `1\|Priya Sharma\|9876543210` |
| `issues.dat` | no\|book\|member\|issued\|due\|returned\|fine | `1\|102\|1\|01-09-2026\|15-09-2026\|30-09-2026\|30` |

A returned date of `-` means the book has not been returned yet. Available copies and books-per-member are not stored; they are worked out from `issues.dat` when the program loads, so they always stay correct.

## Concepts Used

- Structures, nested data and `typedef`
- Three singly linked lists (books, members, issues) linked by IDs
- Dynamic memory allocation (`malloc`, `free`)
- File handling (`fopen`, `fprintf`, `fgets`, `sscanf`, `rename`)
- Date calculations (leap years, days between dates) using `time.h`
- Pointers, pointer-to-structure and function pointers (for sorting)
- Input validation with `fgets` and `strtol`
- Modular programming with multiple source files and a makefile
