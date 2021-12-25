#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>

union filesize {
    unsigned int uint_size;
    unsigned char byte[4];
} filesize;

void create(int argc, char *argv[], char *archive_name) {

    FILE *archive = fopen(archive_name, "wb");
    FILE *file;

    for (int i = 5; i < argc; i++) {
        file = fopen(argv[i], "rb");

        fseek(file, 0, SEEK_END);
        unsigned int file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        if (file_size > UINT_MAX) {
            printf("file is too large");
            break;
        }

        unsigned char file_name_length = strlen(argv[i]) + 1;
        if (file_name_length > UCHAR_MAX) {
            printf("file name is too large");
            break;
        }

        fputc(file_name_length, archive);

        filesize.uint_size = file_size;
        fputc(filesize.byte[3], archive);
        fputc(filesize.byte[2], archive);
        fputc(filesize.byte[1], archive);
        fputc(filesize.byte[0], archive);

        fwrite(argv[i], sizeof(unsigned char), file_name_length, archive);

        unsigned char *buf = calloc(file_size, sizeof(unsigned char));
        fread(buf, sizeof(unsigned char), file_size, file);
        fwrite(buf, sizeof(unsigned char), file_size, archive);
        free(buf);
        fclose(file);
    }
    printf("%s - was successfully created \n", archive_name);
    fclose(archive);
}

void extract(char *archive_name) {
    FILE *archive = fopen(archive_name, "rb");
    int name_length;

    while ((name_length = fgetc(archive)) != EOF) {
        char file_length[4];
        fread(file_length, sizeof(unsigned char), 4, archive);
        filesize.byte[3] = file_length[0];
        filesize.byte[2] = file_length[1];
        filesize.byte[1] = file_length[2];
        filesize.byte[0] = file_length[3];

        unsigned char *file_name = calloc(name_length, sizeof(unsigned char));
        for (unsigned char i = 0; i < name_length; i++) {
            file_name[i] = fgetc(archive);
        }

        FILE *file = fopen(file_name, "wb");

        unsigned char *buf = calloc(sizeof(unsigned char), filesize.uint_size);
        fread(buf, sizeof(unsigned char), filesize.uint_size, archive);
        fwrite(buf, sizeof(unsigned char), filesize.uint_size, file);

        printf("%s - was successfully exctracted \n", file_name);
        free(file_name);
        free(buf);
        fclose(file);
    }
    fclose(archive);
}

void list(char *archive_name) {
    FILE *archive = fopen(archive_name, "rb");
    unsigned int name_length;

    int file_counter = 0;

    while ((name_length = getc(archive)) != EOF) {   //EOF
        char file_length[4];
        fread(file_length, sizeof(unsigned char), 4, archive);
        filesize.byte[3] = file_length[0];
        filesize.byte[2] = file_length[1];
        filesize.byte[1] = file_length[2];
        filesize.byte[0] = file_length[3];



        unsigned char *file_name = calloc(name_length, sizeof(unsigned char));
        fread(file_name, sizeof(unsigned char), name_length, archive);

        fseek(archive, filesize.uint_size, SEEK_CUR);

        printf("%s ", file_name);

        printf(" %d  bytes\n", filesize.uint_size);

        file_counter++;

        free(file_name);
    }

    printf("total files : %d  \n", file_counter);

    fclose(archive);
}

int main(int argc, char *argv[]) {

    char *archive_name = argv[3];

    if (strncmp(argv[4], "--create", 8) == 0)
        create(argc, argv, archive_name);
    if (strncmp(argv[4], "--extract", 9) == 0)
        extract(archive_name);
    if (strncmp(argv[4], "--list", 6) == 0)
        list(archive_name);

    return 0;
}

