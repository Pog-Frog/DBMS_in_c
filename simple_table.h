#include <stdio.h>
#define TABLE_MAX_PAGES 100
#define COLUMN_EMAIL_SIZE 255
#define COLUMN_USERNAME_SIZE 32

typedef struct {
    int id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;

typedef struct 
{
    int file_descriptor;
    int file_length;
    void* pages[TABLE_MAX_PAGES];
} Pager;

typedef struct {
    int num_rows;
    Pager* pager;
} Table;
