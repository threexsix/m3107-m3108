#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char *calendar[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

typedef struct data {
    char *remote_addr;
    char *local_time;
    char *time_zone;
    char *request;
    char *status;
    char *bytes_sent;
} data;

int get_time(char *request_time) {

    char month[4];
    int time_value;
    struct tm local_time = {};

    sscanf(request_time, "%d/%3s/%d:%d:%d:%d", &local_time.tm_mday, month, &local_time.tm_year, &local_time.tm_hour, &local_time.tm_min, &local_time.tm_sec);
    local_time.tm_year -= 1900;

    for (int i = 0; i < 12; i++){
        if (strcmp(month, calendar[i])){
            local_time.tm_mon = i;
            break;
        }
    }

    time_value = mktime(&local_time);

    //printf("time value = %d\n", time_value);
    return time_value;
}

data get_data(char *request){

    data result;

    result.remote_addr = strtok(request, "-");

    strtok(NULL, "[");
    result.local_time = strtok(NULL, " ");

    result.time_zone = strtok(NULL, "]");

    strtok(NULL, "\"");
    result.request = strtok(NULL, "\"");

    result.status = strtok(NULL, " ");
    result.bytes_sent = strtok(NULL, "\n");

    return result;

}

int data_correct(data request_data){

    if(request_data.local_time == NULL || request_data.request == NULL || request_data.status == NULL || request_data.time_zone == NULL || request_data.remote_addr == NULL || request_data.bytes_sent == NULL){
        return 0;
    }
    return 1;
}

int main(int argc, char** argv){

    if (argc != 3){
        printf("too many or too less arguments (file name ; time window) \n");
        exit(-1);
    }

    char *file_name = argv[1];

    int tm_window = atoi(argv[2]);

    if (tm_window < 1){
        printf("time window error");
        exit(-1);
    }

    unsigned long string_counter = 0;
    unsigned long error_counter = 0;
    int max_string = 256;
    char *cur_request = calloc(4096, sizeof(char));
    int *time_array = calloc(max_string, sizeof(long long));

    FILE *input = fopen(file_name, "r");

    if (input == NULL) {
        printf("file path error");
        exit(-1);
    }

    while (!feof(input)) {

        fgets(cur_request, 4096, input);
        data request_data = get_data(cur_request);

        if (data_correct(request_data)){

            if (request_data.status[0] == '5'){
                printf("%s\n", request_data.request);
                error_counter = error_counter + 1;
            }

            if (string_counter == max_string){
                max_string = max_string * 2;
                time_array = realloc(time_array, max_string * sizeof(long long));
            }

            time_array[string_counter] = get_time(request_data.local_time);
            string_counter++;
        }
    }

    int left = 0;
    int right = 0;
    int max_request = 0;
    int maxl, maxr;

    for (int left = 0; left < string_counter; left++){
        right = left;

        while (right < string_counter && time_array[right] - time_array[left] <= tm_window) {
            right++;
        }

        if ((right - left) > max_request){
            max_request = right - left;
            maxl = time_array[left];
            maxr = time_array[right] - 1;
        }
    }

    printf("======================================================\n");
    printf("max request : [%d] in time window : [%d] from time gap : [%d] to : [%d]\n", max_request, tm_window, maxl, maxr);
    printf("total [5xx] errors: [%d]\n", error_counter);

    return 0;
}

