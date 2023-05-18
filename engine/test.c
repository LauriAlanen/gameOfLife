#include <stdio.h>
#include <stdlib.h>

FILE *fp;

void read_file();

int main()
{
    read_file();
    return 0;
}


void read_file(){
    char letter;
    fp = fopen("engine/initial_pattern.txt", "r");

    if (fp == NULL)
    {
        exit(EXIT_FAILURE);
    }

    while ((letter = getc(fp)) != EOF)
    {
        printf("%c", letter);
        if (letter == 'O')
        {
            printf("0mg");
        }
        
    }
    
    fclose(fp);
}