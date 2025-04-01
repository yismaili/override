#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void log_wrapper(FILE *log_file, const char *message, const char *filename) {
    char log_entry[256];

    // create the log entry
    snprintf(log_entry, sizeof(log_entry), "%s: %s", message, filename);

    // write the log entry to the log file
    fprintf(log_file, "%s\n", log_entry);
}

int main(int argc, char *argv[]) {
    char c;
    FILE *log_fd, *input_fd;
    int backup_fd = -1;
    
    if (argc != 2) {
        printf("Usage: %s filename\n", argv[0]);
        exit(1);
    }

    log_fd = fopen("./backups/.log", "w");
    if (!log_fd) {
        printf("ERROR: Failed to open %s\n", "./backups/.log");
        exit(1);
    }

    log_wrapper(log_fd, "Starting back up: ", argv[1]);

    input_fd = fopen(argv[1], "r");
    if (!input_fd) {
        printf("ERROR: Failed to open %s\n", argv[1]);
        exit(1);
    }

    char backup_path[100] = "./backups/";
    strncat(backup_path, argv[1], 99);

    backup_fd = open(backup_path, O_WRONLY|O_CREAT, 0600);
    if (backup_fd < 0) {
        printf("ERROR: Failed to open %s%s\n", "./backups/", argv[1]);
        exit(1);
    }

    while ((c = fgetc(input_fd)) != EOF)
        write(backup_fd, &c, 1);

    log_wrapper(log_fd, "Finished back up ", argv[1]);

    fclose(input_fd);
    close(backup_fd);
    return 0;
}