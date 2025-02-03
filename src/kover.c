/*
 * =====================================================================================
 *
 *       Filename:  kover.c
 *
 *    Description:  Command-line utility for managing communication antenna scenes
 *                  Parses input scenes, validates building and antenna positioning,
 *                  and provides scene analysis through various subcommands
 *
 *        Version:  1.0
 *        Created:  03/02/2025 15:56:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  GUY OLIVIER YANOUBA MILLIMOUNO (Email : millimounou.guy_olivier_yanouba@courrier.uqam.ca, Code P: MILG69360006), 
 *
 * =====================================================================================
 */

#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --------------------------------------------------------
// SECTION: CONSTANTS AND DEFINITIONS
// --------------------------------------------------------

// Size and configuration constants
#define MAX_LINE_LENGTH 51
#define MAX_ID_LENGTH 11
#define MAX_BUILDINGS 100
#define MAX_ANTENNAS 100
#define MAX_ARGS 6
#define MAX_ARG_LENGTH 11

// Return codes
#define SUCCESS 0
#define ERROR 1

// Valid subcommands
const char* VALID_SUBCOMMANDS[] = {
    "bounding-box",  // Calculate and display scene bounding box
    "describe",      // Show detailed scene description
    "help",          // Display help message
    "summarize"      // Show scene summary
};
const int NUM_SUBCOMMANDS = 4;

// --------------------------------------------------------
// SECTION: DATA STRUCTURES
// --------------------------------------------------------

// Building structure
typedef struct {
    char id[MAX_ID_LENGTH];     // Building identifier
    int x;                      // X coordinate
    int y;                      // Y coordinate
    int w;                      // Half-width
    int h;                      // Half-height
} Building;

// Antenna structure
typedef struct {
    char id[MAX_ID_LENGTH];     // Antenna identifier
    int x;                      // X coordinate
    int y;                      // Y coordinate
    int r;                      // Coverage radius
} Antenna;

// Scene structure
typedef struct {
    Building buildings[MAX_BUILDINGS];   // Buildings array
    unsigned int num_buildings;          // Number of buildings
    Antenna antennas[MAX_ANTENNAS];      // Antennas array
    unsigned int num_antennas;           // Number of antennas
} Scene;

// --------------------------------------------------------
// SECTION: UTILITY AND VALIDATION FUNCTIONS
// --------------------------------------------------------

bool is_blank(char c) {
    return c == ' ' || c == '\t';
}

bool is_valid_id_char(char c, bool first) {
    if (first) 
        return isalpha(c) || c == '_';
    return isalnum(c) || c == '_';
}

bool is_valid_id(const char* id) {
    if (!id[0]) return false;
    if (!is_valid_id_char(id[0], true)) return false;
    
    for (int i = 1; id[i]; i++) {
        if (!is_valid_id_char(id[i], false)) return false;
    }
    return true;
}

bool is_valid_integer(const char* str) {
    if (!str[0]) return false;
    if (str[0] == '0' && str[1]) return false;
    
    int start = (str[0] == '-') ? 1 : 0;
    if (start == 1 && !str[1]) return false;
    
    for (int i = start; str[i]; i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

bool is_valid_positive_integer(const char* str) {
    if (!str[0] || str[0] == '-' || str[0] == '0') return false;
    
    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

bool is_valid_subcommand(const char* subcommand) {
    for (int i = 0; i < NUM_SUBCOMMANDS; i++) {
        if (strcmp(subcommand, VALID_SUBCOMMANDS[i]) == 0) return true;
    }
    return false;
}

bool is_begin_scene(const char* line) {
    return strcmp(line, "begin scene") == 0;
}

bool is_end_scene(const char* line) {
    return strcmp(line, "end scene") == 0;
}

void trim_line(char* line) {
    char* end = line + strlen(line) - 1;
    while (end > line && is_blank(*end)) end--;
    *(end + 1) = '\0';
    
    while (*line && is_blank(*line)) line++;
}

// --------------------------------------------------------
// SECTION: ERROR HANDLING FUNCTIONS
// --------------------------------------------------------

void print_error_line(int line_num) {
    fprintf(stderr, "error: unrecognized line (line #%d)\n", line_num);
}

void print_error_mandatory() {
    fprintf(stderr, "error: subcommand is mandatory\n");
}

void print_error_unrecognized(const char* subcommand) {
    fprintf(stderr, "error: subcommand '%s' is not recognized\n", subcommand);
}

void print_help() {
    printf("Usage: kover SUBCOMMAND\n");
    printf("Handles positioning of communication antennas by reading a scene on stdin.\n\n");
    printf("SUBCOMMAND is mandatory and must take one of the following values:\n");
    printf("  bounding-box: returns a bounding box of the loaded scene\n");
    printf("  describe: describes the loaded scene in details\n");
    printf("  help: shows this message\n");
    printf("  summarize: summarizes the loaded scene\n\n");
    printf("A scene is a text stream that must satisfy the following syntax:\n\n");
    printf("  1. The first line must be exactly 'begin scene'\n");
    printf("  2. The last line must be exactly 'end scene'\n");
    printf("  3. Any line between the first and last line must either be a building line\n");
    printf("     or an antenna line\n");
    printf("  4. A building line has the form 'building ID X Y W H' (with any number of\n");
    printf("     blank characters before or after), where\n");
    printf("       ID is the building identifier\n");
    printf("       X is the x-coordinate of the building\n");
    printf("       Y is the y-coordinate of the building\n");
    printf("       W is the half-width of the building\n");
    printf("       H is the half-height of the building\n");
    printf("  5. An antenna line has the form 'antenna ID X Y R' (with any number of\n");
    printf("     blank characters before or after), where\n");
    printf("       ID is the building identifier\n");
    printf("       X is the x-coordinate of the antenna\n");
    printf("       Y is the y-coordinate of the antenna\n");
    printf("       R is the radius scope of the antenna\n");
}

// --------------------------------------------------------
// SECTION: SCENE AND BUILDING VALIDATION FUNCTIONS
// --------------------------------------------------------

bool buildings_overlap(const Building* b1, const Building* b2) {
    return !(b1->x + b1->w <= b2->x - b2->w || 
             b1->x - b1->w >= b2->x + b2->w ||
             b1->y + b1->h <= b2->y - b2->h || 
             b1->y - b1->h >= b2->y + b2->h);
}

bool is_duplicate_building_id(const Scene* scene, const char* id) {
    for (int i = 0; i < scene->num_buildings; i++) {
        if (strcmp(scene->buildings[i].id, id) == 0) return true;
    }
    return false;
}

bool is_duplicate_antenna_id(const Scene* scene, const char* id) {
    for (int i = 0; i < scene->num_antennas; i++) {
        if (strcmp(scene->antennas[i].id, id) == 0) return true;
    }
    return false;
}

bool check_building_overlaps(const Scene* scene, char* id1, char* id2) {
    for (int i = 0; i < scene->num_buildings; i++) {
        for (int j = i + 1; j < scene->num_buildings; j++) {
            if (buildings_overlap(&scene->buildings[i], &scene->buildings[j])) {
                strcpy(id1, scene->buildings[i].id);
                strcpy(id2, scene->buildings[j].id);
                return true;
            }
        }
    }
    return false;
}

bool has_same_position(const Antenna* a1, const Antenna* a2) {
    return (a1->x == a2->x && a1->y == a2->y);
}

bool check_antenna_positions(const Scene* scene, char* id1, char* id2) {
    for (int i = 0; i < scene->num_antennas; i++) {
        for (int j = i + 1; j < scene->num_antennas; j++) {
            if (has_same_position(&scene->antennas[i], &scene->antennas[j])) {
                strcpy(id1, scene->antennas[i].id);
                strcpy(id2, scene->antennas[j].id);
                return true;
            }
        }
    }
    return false;
}

// --------------------------------------------------------
// SECTION: PARSING FUNCTIONS
// --------------------------------------------------------

bool parse_building_line(const char* line, Building* building, int line_num) {
    char id[MAX_ID_LENGTH];
    char x_str[MAX_ARG_LENGTH], y_str[MAX_ARG_LENGTH];
    char w_str[MAX_ARG_LENGTH], h_str[MAX_ARG_LENGTH];
    
    if (sscanf(line, " building %10s %10s %10s %10s %10s ", 
               id, x_str, y_str, w_str, h_str) != 5) {
        fprintf(stderr, "error: building line has wrong number of arguments (line #%d)\n", line_num);
        return false;
    }

    if (!is_valid_id(id)) {
        fprintf(stderr, "error: invalid identifier \"%s\" (line #%d)\n", id, line_num);
        return false;
    }

    if (!is_valid_integer(x_str)) {
        fprintf(stderr, "error: invalid integer \"%s\" (line #%d)\n", x_str, line_num);
        return false;
    }

    if (!is_valid_integer(y_str)) {
        fprintf(stderr, "error: invalid integer \"%s\" (line #%d)\n", y_str, line_num);
        return false;
    }

    if (!is_valid_positive_integer(w_str)) {
        fprintf(stderr, "error: invalid positive integer \"%s\" (line #%d)\n", w_str, line_num);
        return false;
    }

    if (!is_valid_positive_integer(h_str)) {
        fprintf(stderr, "error: invalid positive integer \"%s\" (line #%d)\n", h_str, line_num);
        return false;
    }

    strcpy(building->id, id);
    building->x = atoi(x_str);
    building->y = atoi(y_str);
    building->w = atoi(w_str);
    building->h = atoi(h_str);
    
    return true;
}

bool parse_antenna_line(const char* line, Antenna* antenna, int line_num) {
    char id[MAX_ID_LENGTH];
    char x_str[MAX_ARG_LENGTH], y_str[MAX_ARG_LENGTH], r_str[MAX_ARG_LENGTH];
    
    if (sscanf(line, " antenna %10s %10s %10s %10s ", 
               id, x_str, y_str, r_str) != 4) {
        fprintf(stderr, "error: antenna line has wrong number of arguments (line #%d)\n", line_num);
        return false;
    }

    if (!is_valid_id(id)) {
        fprintf(stderr, "error: invalid identifier \"%s\" (line #%d)\n", id, line_num);
        return false;
    }

    if (!is_valid_integer(x_str)) {
        fprintf(stderr, "error: invalid integer \"%s\" (line #%d)\n", x_str, line_num);
        return false;
    }

    if (!is_valid_integer(y_str)) {
        fprintf(stderr, "error: invalid integer \"%s\" (line #%d)\n", y_str, line_num);
        return false;
    }

    if (!is_valid_positive_integer(r_str)) {
        fprintf(stderr, "error: invalid positive integer \"%s\" (line #%d)\n", r_str, line_num);
        return false;
    }

    strcpy(antenna->id, id);
    antenna->x = atoi(x_str);
    antenna->y = atoi(y_str);
    antenna->r = atoi(r_str);
    
    return true;
}

// --------------------------------------------------------
// SECTION: SCENE PROCESSING FUNCTIONS
// --------------------------------------------------------

void init_scene(Scene* scene) {
    scene->num_buildings = 0;
    scene->num_antennas = 0;
}

bool process_building(Scene* scene, const char* line, int line_num) {
    Building building;
    if (!parse_building_line(line, &building, line_num)) return false;
    
    if (is_duplicate_building_id(scene, building.id)) {
        fprintf(stderr, "error: building identifier %s is non unique\n", building.id);
        return false;
    }
    
    for (int i = 0; i < scene->num_buildings; i++) {
        if (buildings_overlap(&scene->buildings[i], &building)) {
            fprintf(stderr, "error: buildings %s and %s are overlapping\n", scene->buildings[i].id, building.id);
            return false;
        }
    }
    
    scene->buildings[scene->num_buildings++] = building;
    return true;
}

bool process_antenna(Scene* scene, const char* line, int line_num) {
    Antenna antenna;
    if (!parse_antenna_line(line, &antenna, line_num)) return false;
    
    if (is_duplicate_antenna_id(scene, antenna.id)) {
        fprintf(stderr, "error: antenna identifier %s is non unique\n", antenna.id);
        return false;
    }
    
    scene->antennas[scene->num_antennas++] = antenna;
    
    char id1[MAX_ID_LENGTH], id2[MAX_ID_LENGTH];
    if (check_antenna_positions(scene, id1, id2)) {
        fprintf(stderr, "error: antennas %s and %s have the same position\n", id1, id2);
        return false;
    }
    return true;
}

bool process_line(Scene* scene, char* line, int line_num) {
    char type[MAX_ARG_LENGTH];
    if (sscanf(line, " %10s ", type) != 1) {
        print_error_line(line_num);
        return false;
    }

    if (strcmp(type, "building") == 0) 
        return process_building(scene, line, line_num);
    else if (strcmp(type, "antenna") == 0) 
        return process_antenna(scene, line, line_num);
    
    print_error_line(line_num);
    return false;
}

bool read_scene(Scene* scene) {
    char line[MAX_LINE_LENGTH];
    
    if (!fgets(line, MAX_LINE_LENGTH, stdin)) return false;
    line[strcspn(line, "\n")] = 0;
    if (!is_begin_scene(line)) {
        fprintf(stderr, "error: first line must be exactly 'begin scene'\n");
        return false;
    }
    
    int line_num = 1;
    
    while (fgets(line, MAX_LINE_LENGTH, stdin)) {
        line_num++;
        line[strcspn(line, "\n")] = 0;
        
        if (is_end_scene(line)) return true;
        if (!process_line(scene, line, line_num)) return false;
    }
    
    fprintf(stderr, "error: last line must be exactly 'end scene'\n");
    return false;
}

// --------------------------------------------------------
// SECTION: SCENE COMPUTATION FUNCTIONS
// --------------------------------------------------------

void compute_bounding_box(const Scene* scene, int* min_x, int* max_x, int* min_y, int* max_y) {
    *min_x = INT_MAX;
    *max_x = INT_MIN;
    *min_y = INT_MAX;
    *max_y = INT_MIN;

    for (int i = 0; i < scene->num_buildings; i++) {
        const Building* b = &scene->buildings[i];
        if (b->x - b->w < *min_x) *min_x = b->x - b->w;
        if (b->x + b->w > *max_x) *max_x = b->x + b->w;
        if (b->y - b->h < *min_y) *min_y = b->y - b->h;
        if (b->y + b->h > *max_y) *max_y = b->y + b->h;
    }

    for (int i = 0; i < scene->num_antennas; i++) {
        const Antenna* a = &scene->antennas[i];
        if (a->x - a->r < *min_x) *min_x = a->x - a->r;
        if (a->x + a->r > *max_x) *max_x = a->x + a->r;
        if (a->y - a->r < *min_y) *min_y = a->y - a->r;
        if (a->y + a->r > *max_y) *max_y = a->y + a->r;
    }
}

// --------------------------------------------------------
// SECTION: OUTPUT FUNCTIONS
// --------------------------------------------------------

void print_bounding_box(const Scene* scene) {
    if (scene->num_buildings == 0 && scene->num_antennas == 0) {
        printf("undefined (empty scene)\n");
        return;
    }
    int min_x, max_x, min_y, max_y;
    compute_bounding_box(scene, &min_x, &max_x, &min_y, &max_y);
    printf("bounding box [%d, %d] x [%d, %d]\n", min_x, max_x, min_y, max_y);
}

void print_summary(const Scene* scene) {
    if (scene->num_buildings == 0 && scene->num_antennas == 0) {
        printf("An empty scene\n");
        return;
    }
    printf("A scene with ");
    
    if (scene->num_buildings > 0) {
        printf("%d building%s", scene->num_buildings, scene->num_buildings > 1 ? "s" : "");
        if (scene->num_antennas > 0) printf(" and ");
    }
    
    if (scene->num_antennas > 0) {
        printf("%d antenna%s", scene->num_antennas, scene->num_antennas > 1 ? "s" : "");
    }
    printf("\n");
}

void print_building(const Building* b) {
    printf("  building %s at %d %d with dimensions %d %d\n", 
           b->id, b->x, b->y, b->w, b->h);
}

void print_antenna(const Antenna* a) {
    printf("  antenna %s at %d %d with range %d\n", 
           a->id, a->x, a->y, a->r);
}

int compare_ids(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

void print_description(const Scene* scene) {
    print_summary(scene);
    
    // Sort buildings by ID
    const char* building_ids[MAX_BUILDINGS];
    for (int i = 0; i < scene->num_buildings; i++) {
        building_ids[i] = scene->buildings[i].id;
    }
    qsort(building_ids, scene->num_buildings, sizeof(char*), compare_ids);
    
    // Print buildings in sorted order
    for (int i = 0; i < scene->num_buildings; i++) {
        for (int j = 0; j < scene->num_buildings; j++) {
            if (strcmp(building_ids[i], scene->buildings[j].id) == 0) {
                print_building(&scene->buildings[j]);
                break;
            }
        }
    }
    
    // Sort antennas by ID
    const char* antenna_ids[MAX_ANTENNAS];
    for (int i = 0; i < scene->num_antennas; i++) {
        antenna_ids[i] = scene->antennas[i].id;
    }
    qsort(antenna_ids, scene->num_antennas, sizeof(char*), compare_ids);
    
    // Print antennas in sorted order
    for (int i = 0; i < scene->num_antennas; i++) {
        for (int j = 0; j < scene->num_antennas; j++) {
            if (strcmp(antenna_ids[i], scene->antennas[j].id) == 0) {
                print_antenna(&scene->antennas[j]);
                break;
            }
        }
    }
}

// --------------------------------------------------------
// SECTION: MAIN FUNCTION
// --------------------------------------------------------

int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_error_mandatory();
        return ERROR;
    }
    
    const char* subcommand = argv[1];
    
    if (strcmp(subcommand, "help") == 0) {
        print_help();
        return SUCCESS;
    }
    
    if (!is_valid_subcommand(subcommand)) {
        print_error_unrecognized(subcommand);
        return ERROR;
    }
    
    Scene scene;
    init_scene(&scene);
    
    if (!read_scene(&scene)) {
        return ERROR;
    }
    
    if (strcmp(subcommand, "bounding-box") == 0) {
        print_bounding_box(&scene);
    }
    else if (strcmp(subcommand, "describe") == 0) {
        print_description(&scene);
    }
    else if (strcmp(subcommand, "summarize") == 0) {
        print_summary(&scene);
    }
    
    return SUCCESS;
}