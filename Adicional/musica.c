#include<stdlib.h>
#include<stdio.h>
#include<string.h>
int main(int argc, char* argv[])
{
    char input[255];    
    int i = 0;

    char dir[100] = {""};
    printf("Arriba\n");
    strcpy(dir, getenv("USERPROFILE"));
    strcat(dir, "/Documents/ReproductorConfig.txt");
    FILE* fp = fopen(dir, "r");
    printf("USERPROFILE: %s\n", getenv("USERPROFILE"));
    if(fp == NULL)
    {
        printf("No se ha encontrado el archivo de configuracion\n");
        return 1;
    }
    dir[0] = '\0';
    while(fgets(dir, 100, fp) != NULL && strcmp(dir, "REPRODUCTOR\n") != 0)
    {
        printf("leido: %s\n", dir);
    }

    if(strcmp(dir, "REPRODUCTOR\n") != 0 || fgets(dir, 100, fp) == NULL)
    {
        printf("No se ha encontrado la direccion del reproductor en el archivo de configuracion\n");
        return 1;
    }
    
    dir[strlen(dir)-1] = '\0';

    fclose(fp);

    if(argc != 1)
    {
        i = 2;

        sprintf(input, "start %s \"%s", dir, argv[1]);    

        while(i != argc && argv[i][0] != '-')
        {
            sprintf(input, "%s %s", input, argv[i]);
            i++;
        }

        sprintf(input, "%s\"", input);

        for (i; i < argc; i++)
        {
            sprintf(input, "%s %s", input, argv[i]);
        }
        (void)system(input);
    }
    else
    {
        char extra[255] = {""};
        printf("Tienes que introducir el directorio de la carpeta en donde están guardadas las canciones que se van a reproducir como minimo.\n");
        printf("Introduce los parámetros: ");
        fgets(input, 255, stdin);
        
        if(input[1] == ':')
        {
            sprintf(extra, "start %s %s", dir, input);
        }
        else
        {
            sprintf(extra, "start %s \"", dir);

            while(i+1 != strlen(input) && input[i+1] != '-')
                sprintf(extra, "%s%c", extra, input[i++]);

            strcat(extra, "\" ");
            
            i++;
            if(i != strlen(input) && input[i] =='-')
                strcat(extra, &input[i]);
        }
        (void)system(extra);
    }
    return 0;
}