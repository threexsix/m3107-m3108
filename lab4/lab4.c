#include<stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TAGS{
    char version[3];
    char ver[2];
    char flags;
    char size[4];
} TAGS;

typedef union bytesize{
    int value;
    char byte[4];
};

typedef struct FRAMES{
    char name[4];
    char size[4];
    char flags[2];
    char unicode;
} FRAMES;

TAGS tag_header;
FRAMES frame_header;

int string_count(char *str){
    int i = 0;
    while(str[i]!= '\0'){
        i++;
    }
    return i;
}

int synchsafe_to_int(char byte[4])
{
    return byte[0] << 24 | byte[1] << 16 | byte[2] << 8 | byte[3];
}

char *int_to_synchsafe(int val)
{
    union bytesize x;
    x.value = val;
    char byte[4];

    byte[0] = x.byte[0] & 0x7f;
    byte[1] = (x.byte[1] >> 7) & 0x7f;
    byte[2] = (x.byte[2] >> 14) & 0x7f;
    byte[3] = (x.byte[3] >> 21) & 0x7f;

    return byte;
}

int header_size_to_int(char byte[4]) {
    return byte[0] << 21 | byte[1] << 14 | byte[2] << 7 | byte[3];
}

int size_of_file(const char *file_name){

    int file_size = 0;
    FILE *new = fopen(file_name, "rb");

    while(fgetc(new) != EOF)
        file_size++;

    fclose(new);

    return file_size;
}

void is_correct_file(char *file_name){

    if (file_name == NULL){
        printf("Error opening file");
        exit(1);
    }
}

void show(char *file_name){

    FILE *file = fopen(file_name, "rb");
    is_correct_file(file);

    fread(&tag_header, 1, 10, file);
    printf("%sv%d.%d\n", tag_header.version, tag_header.ver[0], tag_header.ver[1]);


    int tag_size = header_size_to_int(tag_header.size);
    printf("tag_size = %d\n", tag_size);

    int file_size = size_of_file(file_name);
    printf("file_size = %d\n", file_size);

    while(ftell(file) < tag_size){

        fread(&frame_header, 1, 11, file);
        if (frame_header.name[0] == 0){
            break;
        }
        int frame_size = synchsafe_to_int(frame_header.size);
        if (frame_header.name[0] == 'A'){
            fseek(file, frame_size, SEEK_CUR);
            continue;
        }
        char *content = calloc(frame_size, 1);
        fgets(content, frame_size, file);
        printf("id: %5s || size: %5d || value: ", frame_header.name, frame_size);
        printf("%s\n", content);
        free(content);
    }
    fclose(file);
}

void get(char *file_name, char *frame){

    FILE *file = fopen(file_name, "rb");
    is_correct_file(file);

    fread(&tag_header, 1, 10, file);
    printf("%sv%d.%d\n", tag_header.version, tag_header.ver[0], tag_header.ver[1]);

    int tag_size = header_size_to_int(tag_header.size);
    printf("tag size = %d\n", tag_size);

    while(ftell(file) < tag_size){

        fread(&frame_header, 1, 11, file);
        int frame_size = synchsafe_to_int(frame_header.size);

        if (strcmp(frame, frame_header.name) == 0){

            int frame_size = synchsafe_to_int(frame_header.size);
            char *content = calloc(frame_size, 1);
            fgets(content, frame_size, file);
            printf("id: %5s || size: %5d || value: ", frame_header.name, frame_size);
            printf("%s\n", content);
            free(content);
            break;
        }

        fseek(file, frame_size - 1, SEEK_CUR);

    }
    fclose(file);
}

void set_value(char *file_name, char *frame, char *value){

    FILE *file = fopen(file_name, "rb");
    is_correct_file(file);
    int file_size = size_of_file(file_name);

    fread(&tag_header, 1, 10, file);
    printf("%sv%d.%d  ", tag_header.version, tag_header.ver[0], tag_header.ver[1]);

    int tag_size = header_size_to_int(tag_header.size);
    printf("tag size = %d\n", tag_size);

    while(ftell(file) < tag_size){

        fread(&frame_header, 1, 11, file);

        if (strcmp(frame, frame_header.name) == 0){

            int frame_size = synchsafe_to_int(frame_header.size);

            int left_ptr = ftell(file) - 7;
            int right_ptr = left_ptr + frame_size + 6;

            int value_size = string_count(value);

            char *left_buf = calloc(1, left_ptr);
            char *right_buf = calloc(1, file_size - right_ptr);

            fseek(file, 0 , 0);
            fread(left_buf, 1, left_ptr, file);

            fseek(file, right_ptr, 0);
            fread(right_buf, 1, file_size - right_ptr, file);

            FILE *new = fopen("newmp3.mp3", "wb");

            fwrite(left_buf, 1, left_ptr, new);

            union bytesize x;
            x.value = value_size + 1;

            char info_buf[7];
            info_buf[0] = x.byte[3];
            info_buf[1] = x.byte[2];
            info_buf[2] = x.byte[1];
            info_buf[3] = x.byte[0];
            info_buf[4] = frame_header.flags[0];
            info_buf[5] = frame_header.flags[1];
            info_buf[6] = frame_header.unicode;

            fwrite(info_buf, 1, 7, new);

            fwrite(value, 1, value_size, new);

            fwrite(right_buf, 1, file_size - right_ptr, new);

            fclose(new);

            printf("\n frame was successfully edited \n");
            break;
        }
    }
    fclose(file);
}

void add_frame(char *file_name, char *frame, char *value){

    FILE *file = fopen(file_name, "rb");
    is_correct_file(file);
    int file_size = size_of_file(file_name);

    fread(&tag_header, 1, 10, file);
    printf("%sv%d.%d\n", tag_header.version, tag_header.ver[0], tag_header.ver[1]);

    int tag_size = header_size_to_int(tag_header.size);
    printf("start size = %d\n", tag_size);

    fseek(file, tag_size, SEEK_SET);

    int pos = ftell(file);   //end of tag section

    char new_tag_header[6];
    char *left_buf = calloc(1, pos - 10);
    char *right_buf = calloc(1, file_size - pos);

    fseek(file, 10, SEEK_SET);
    fread(left_buf, 1, pos - 10, file);

    fseek(file, pos, SEEK_SET);
    fread(right_buf, 1, file_size - pos, file);

    fseek(file, 0 , 0);

    fread(new_tag_header, 1, 6, file);

    int new_tag_size = tag_size + 4 + string_count(value);

    printf("new size = %d\n", new_tag_size);

    char size_buf = tag_header.size;

    FILE *new = fopen("newmp3.mp3", "wb");

    fwrite(new_tag_header, 1, 6, new);
    fwrite(tag_header.size, 1, 4, new);
    fwrite(left_buf, 1, pos - 10, new);

    fwrite(frame, 1, 4, new);

    union bytesize x;
    x.value = string_count(value) + 1;

    char new_header[7];
    new_header[0] = x.byte[3];
    new_header[1] = x.byte[2];
    new_header[2] = x.byte[1];
    new_header[3] = x.byte[0];
    new_header[4] = '0';
    new_header[5] = '0';
    new_header[6] = '0';

    fwrite(new_header, 1, 7, new);

    fwrite(value, 1, string_count(value), new);

    fwrite(right_buf, 1, file_size - pos, new);

    printf("\n frame was successfully aded");
    fclose(new);
    fclose(file);
}

int main(int argc, char *argv[])
{

    char *get_val;
    char *name = strpbrk(argv[1], "=") + 1;

    if(strcmp(argv[argc - 1], "--show") == 0)
        show(name);

    else if(argv[argc - 1][2] == 'g')
        {
        get_val = strpbrk(argv[argc - 1], "=") + 1;
        get(name, get_val);
        }

    else if(argc > 3)
        {
        char *frame = strpbrk(argv[2], "=") + 1;
        char *set_val = strpbrk(argv[3], "=") + 1;
        set_value(name, frame, set_val);
        }
    return 0;
}

