#include "saveParams.h"
#include <bits/types/FILE.h>
#include <dirent.h>
#include <err.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "matrix.h"

int remove_directory(const char *path)
{
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int ret = -1;

    if (d) {
        struct dirent *p;
        ret = 0;
        while (!ret && (p = readdir(d))) {
            int is_current_or_parent = strcmp(p->d_name, ".") == 0 || strcmp(p->d_name, "..") == 0;
            if (!is_current_or_parent) {
                size_t len = path_len + strlen(p->d_name) + 2;
                char *buf = malloc(len);
                if (buf) {
                    struct stat statbuf;
                    snprintf(buf, len, "%s/%s", path, p->d_name);
                    if (!stat(buf, &statbuf)) {
                        if (S_ISDIR(statbuf.st_mode)) {
                            ret = remove_directory(buf);
                        } else {
                            ret = unlink(buf);
                        }
                    }
                    free(buf);
                }
            }
        }
        closedir(d);
    }

    if (!ret) {
        ret = rmdir(path);
    }
    return ret;
}

void matrixToFile(matrix *m, char *path)
{
    //Save matrix m to path
    //Line 1 : row
    //Line 2 : col
    //Other line : datas

    // Open file in writing mode
    FILE *file = fopen(path, "w");

    if(file == NULL)
        errx(0, "Error while opening file.");

    fprintf(file, "%zu\n", m->row);
    fprintf(file, "%zu\n", m->col);

    for(size_t i = 0; i < m->row*m->col; i++)
        fprintf(file, "%f\n", m->data[i]);
    
    //Close file
    fclose(file);
}

void SaveParameters(parameters *p, char *path)
{
    //Save parameters p in path

    //Get current repo
    char current_repo[1024];
    getcwd(current_repo, sizeof(current_repo));

    // Delete old repo if it exists
    struct stat st;
    if (stat(path, &st) == 0)
    {
        if (S_ISDIR(st.st_mode))
        {
            if (remove_directory(path) != 0) 
                errx(0, "Error while removing already existing repository.");
        }
    }

    //Create repo
    if (mkdir(path, 0777) != 0) 
        errx(0, "Error while creating repository.");

    //Go to created repo
    chdir(path);

    matrixToFile(p->W1, "./W1");
    matrixToFile(p->b1, "./b1");
    matrixToFile(p->W2, "./W2");
    matrixToFile(p->b2, "./b2");
    matrixToFile(p->W3, "./W3");
    matrixToFile(p->b3, "./b3");

    //Return in origin repo
    chdir(current_repo);
}

size_t charToSizeT(char *str)
{
    //Convert a char to a size_t value and return it

    size_t result = 0;

    size_t i = 0;
    while(str[i] != 0 && str[i] != '\n')
    {
        result = 10*result + str[i] - 48;
        i++;
    }

    return result;
}

matrix *fileToMatrix(char *path)
{
    //Open file at path
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        errx(0, "Error while opening file");
        return NULL;
    }

    //Get dimensions of matrix
    char* line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, file);
    if (read == -1) {
        perror("Error while reading file");
        fclose(file);
        free(line);
        return NULL;
    }
    size_t row = charToSizeT(line);

    read = getline(&line, &len, file);
    if (read == -1) {
        perror("Error while reading file");
        fclose(file);
        free(line);
        return NULL;
    }
    size_t col = charToSizeT(line);

    //Create result matrix
    matrix *result = Matrix(row, col);
    if (result == NULL) {
        perror("Error while creating matrix");
        fclose(file);
        free(line);
        return NULL;
    }

    //Read data from file
    for(size_t i = 0; i < row*col; i++)
    {
        read = getline(&line, &len, file);
        if (read == -1) {
            perror("Error while reading file");
            freeMatrix(result);
            fclose(file);
            free(line);
            return NULL;
        }
        result->data[i] = atof(line);
    }

    //Clean up and return result
    fclose(file);
    free(line);
    return result;
}

parameters *LoadParameters(char *path)
{
    //Load parameters from path and return them

    //Get current directory
    char current_dir[1024];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        perror("getcwd() error");
        return NULL;
    }

    //Go to directory
    if (chdir(path) == -1) {
        perror("chdir() error");
        return NULL;
    }

    parameters *p = malloc(sizeof(parameters));
    if (p == NULL) {
        perror("malloc() error");
        return NULL;
    }

    //Load matrices from files
    if (access("W1", F_OK) != -1) {
        p->W1 = fileToMatrix("W1");
    } else {
        fprintf(stderr, "File W1 does not exist\n");
        return NULL;
    }

    if (access("b1", F_OK) != -1) {
        p->b1 = fileToMatrix("b1");
    } else {
        fprintf(stderr, "File b1 does not exist\n");
        return NULL;
    }

    if (access("W2", F_OK) != -1) {
        p->W2 = fileToMatrix("W2");
    } else {
        fprintf(stderr, "File W2 does not exist\n");
        return NULL;
    }

    if (access("b2", F_OK) != -1) {
        p->b2 = fileToMatrix("b2");
    } else {
        fprintf(stderr, "File b2 does not exist\n");
        return NULL;
    }

    if (access("W3", F_OK) != -1) {
        p->W3 = fileToMatrix("W3");
    } else {
        fprintf(stderr, "File W3 does not exist\n");
        return NULL;
    }

    if (access("b3", F_OK) != -1) {
        p->b3 = fileToMatrix("b3");
    } else {
        fprintf(stderr, "File b3 does not exist\n");
        return NULL;
    }

    //Return to original directory
    if (chdir(current_dir) == -1) {
        perror("chdir() error");
        return NULL;
    }

    return p;
}