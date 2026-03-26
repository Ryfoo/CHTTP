#include "../include/utils.h"

void load_env(const char *filename) 
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open .env file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n') continue;

        // Remove trailing newline
        line[strcspn(line, "\n")] = 0;

        // Split on '='
        char *eq = strchr(line, '=');
        if (!eq) continue;

        *eq = '\0';              // split into key and value
        char *key   = line;
        char *value = eq + 1;

        if(!setenv(key, value, 1)) continue;  // load into process environment
    }

    fclose(file);
}
