#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define ROW 9
#define COLUMN 9
#define true 1

char sudoku[ROW][COLUMN];
int valid_columns, valid_rows;

/* 
    Verifies that all rows are valid

    @return 
        0 if valid 
        -1 otherwise
*/
int verify_rows()
{
    omp_set_nested(true);
    //omp_set_num_threads(9);

    int grid[9];
    int valid = 0;
    //#pragma omp parallel for private(grid)
    for (int i = 0; i < ROW; i++)
    {
        char valid_nums[] = "123456789";
        char *s;
        for (s = &valid_nums[0]; *s != '\0'; s++)
        {
            int found = 0;
            int j = 0;
            while (found == 0 && j < COLUMN)
            {
                if (sudoku[i][j] == *s)
                    found = 1;
                j++;
            }
            if (found == 0)
                valid = -1;
        }
    }
    return valid;
}

int verify_rows_args(char temp[ROW][COLUMN])
{
    omp_set_nested(true);
    //omp_set_num_threads(9);

    int grid[9];
    int valid = 0;
    // #pragma omp parallel for private(grid)

    for (int i = 0; i < ROW; i++)
    {
        char valid_nums[] = "123456789";
        char *s;
        for (s = &valid_nums[0]; *s != '\0'; s++)
        {
            int found = 0;
            int j = 0;
            while (found == 0 && j < COLUMN)
            {
                if (temp[i][j] == *s)
                    found = 1;
                j++;
            }
            if (found == 0)
                valid = -1;
        }
    }
    return valid;
}

/* 
    Verifies that all columns are valid

    @return 
        0 if valid 
        -1 otherwise
*/
int verify_columns()
{
    omp_set_nested(true);
    //omp_set_num_threads(9);

    int grid[9];
    int valid = 0;
    // #pragma omp parallel for private(grid)
    for (int i = 0; i < COLUMN; i++)
    {
        char valid_nums[] = "123456789";
        char *s;
        for (s = &valid_nums[0]; *s != '\0'; s++)
        {
            int found = 0;
            int j = 0;
            while (found == 0 && j < ROW)
            {
                if (sudoku[j][i] == *s)
                    found = 1;
                j++;
            }
            if (found == 0)
                valid = -1;
        }
    }
    return valid;
}

int verify_sub_matrix()
{
    omp_set_nested(true);
    //omp_set_num_threads(3);

    char temp_sudoku[ROW][COLUMN];
    int row = 0, column = 0;

    int grid[9];
    // #pragma omp parallel for private(grid)
    for (int x = 0; x < 3; x++)
    {
        #pragma omp parallel for
        for (int y = 0; y < 3; y++)
        {
            // #pragma omp parallel for
            for (int i = 0; i < 3; i++)
            {
                // #pragma omp parallel for
                for (int j = 0; j < 3; j++)
                {
                    temp_sudoku[row][column] = sudoku[i + (x * 3)][j + (y * 3)];
                    column++;
                }
            }
            column = 0;
            row++;
        }
    }
    return verify_rows_args(temp_sudoku);
}

void *complete_column_verification()
{
    printf("Column Thread ID: %d\n", syscall(SYS_gettid));
    valid_columns = verify_columns();
    pthread_exit(0);
}

void *complete_row_verification()
{
    printf("Row Thread ID: %d\n", syscall(SYS_gettid));
    valid_rows = verify_rows();
    pthread_exit(0);
}


/* Maps Sudoku string to array */
void map_sudoku_from_file(int fd)
{
    omp_set_nested(true);
    //omp_set_num_threads(9);

    struct stat stat_s;
    int status = fstat(fd, &stat_s);
    int size = stat_s.st_size;
    char *ptr = (char *)mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);

    // Data to 9x9 matrix
    int char_pos = 0;
    int grid[9];
    //#pragma omp parallel for private(grid)
    for (int i = 0; i < ROW; i++)
    {
        //#pragma omp parallel for
        for (int j = 0; j < COLUMN; j++)
        {
            sudoku[i][j] = ptr[char_pos];
            char_pos++;
        }
    }
    munmap(ptr,size);
    close(fd);
}

int main(int argc, char *argv[])
{
    // omp_set_num_threads(1);

    if (argc < 2)
    {
        printf("No sudoku file was passed");
        return 1;
    }

    int input;
    if ((input = open(argv[1], O_RDONLY)) < 0)
    {
        perror("Can't open sudoku file");
        return 1;
    }
    else
    {
        map_sudoku_from_file(input);
        // Get pid info
        pid_t parent_pid = getpid();

        int f = fork();
        if (f < 0)
        {
            perror("Fork error");
            return 1;
        }
        else if (f == 0)
        {
            // Child process
            // Convert pid to str
            char p_pid[6];
            sprintf(p_pid, "%d", (int)parent_pid);
            execlp("ps", "ps", "-p", p_pid, "-lLf", NULL);
        }
        else
        {
            // Parent process
            // Create and join column verify thread
            pthread_t col_verification;
            if (pthread_create(&col_verification, NULL, complete_column_verification, NULL))
            {
                perror("Error creating thread");
                return 1;
            }
            if (pthread_join(col_verification, NULL))
            {
                perror("Error joining thread");
                return 1;
            }

            printf("main thread: %d\n", syscall(SYS_gettid));
            usleep(30000);
            printf("child finished\n");

            // Create and join row verify thread
            pthread_t row_verification;
            if (pthread_create(&row_verification, NULL, complete_row_verification, NULL))
            {
                perror("Error creating thread");
                return 1;
            }
            if (pthread_join(row_verification, NULL))
            {
                perror("Error joining thread");
                return 1;
            }

            // Validation
            if (valid_rows == 0 && valid_columns == 0)
            {
                printf("### Valid solution ###\n");
            }
            else
            {
                printf("### Invalid solution ###\n");
            }
        }

        // Second fork
        int f2 = fork();
        if (f2 == 0)
        { // Child process
            // Convert pid to str
            char p_pid[6];
            sprintf(p_pid, "%d", (int)parent_pid);
            execlp("ps", "ps", "-p", p_pid, "-lLf", NULL);
        }
        else
        {
            usleep(30000);
            printf("child finished\n");
            return 0;
        }
    }
}
