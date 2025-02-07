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
// SECTION: FUNCTION PROTOTYPES AND DOCUMENTATION
// --------------------------------------------------------

/**
 * @brief Checks if a character is blank (space or tab)
 * @param c Character to check
 * @return true if character is blank, false otherwise
 */
bool is_blank(char c);

/**
 * @brief Validates a character for use in an identifier
 * @param c Character to validate
 * @param first True if this is the first character of the identifier
 * @return true if character is valid for the position, false otherwise
 */
bool is_valid_id_char(char c, bool first);

/**
 * @brief Validates a complete identifier string
 * @param id String to validate
 * @return true if identifier is valid, false otherwise
 */
bool is_valid_id(const char* id);

/**
 * @brief Validates an integer string, allowing negatives
 * @param str String to validate
 * @return true if string represents a valid integer, false otherwise
 */
bool is_valid_integer(const char* str);

/**
 * @brief Validates a positive integer string
 * @param str String to validate
 * @return true if string represents a valid positive integer, false otherwise
 */
bool is_valid_positive_integer(const char* str);

/**
 * @brief Checks if a subcommand is valid
 * @param subcommand String to check
 * @return true if subcommand is valid, false otherwise
 */
bool is_valid_subcommand(const char* subcommand);

/**
 * @brief Checks if a line is the begin scene marker
 * @param line String to check
 * @return true if line is "begin scene", false otherwise
 */
bool is_begin_scene(const char* line);

/**
 * @brief Checks if a line is the end scene marker
 * @param line String to check
 * @return true if line is "end scene", false otherwise
 */
bool is_end_scene(const char* line);

/**
 * @brief Removes leading and trailing whitespace from a line
 * @param line String to trim
 */
void trim_line(char* line);

/**
 * @brief Prints an error message for an unrecognized line
 * @param line_num Line number where error occurred
 */
void print_error_line(int line_num);

/**
 * @brief Prints error message when subcommand is missing
 */
void print_error_mandatory(void);

/**
 * @brief Prints error message for unrecognized subcommand
 * @param subcommand The invalid subcommand
 */
void print_error_unrecognized(const char* subcommand);

/**
 * @brief Prints help message with usage instructions
 */
void print_help(void);

/**
 * @brief Checks if two buildings overlap
 * @param b1 First building
 * @param b2 Second building
 * @return true if buildings overlap, false otherwise
 */
bool buildings_overlap(const Building* b1, const Building* b2);

/**
 * @brief Checks for duplicate building ID in scene
 * @param scene Current scene
 * @param id ID to check
 * @return true if ID already exists, false otherwise
 */
bool is_duplicate_building_id(const Scene* scene, const char* id);

/**
 * @brief Checks for duplicate antenna ID in scene
 * @param scene Current scene
 * @param id ID to check
 * @return true if ID already exists, false otherwise
 */
bool is_duplicate_antenna_id(const Scene* scene, const char* id);

/**
 * @brief Checks for overlapping buildings in scene
 * @param scene Current scene
 * @param id1 Output parameter for first overlapping building ID
 * @param id2 Output parameter for second overlapping building ID
 * @return true if overlapping buildings found, false otherwise
 */
bool check_building_overlaps(const Scene* scene, char* id1, char* id2);

/**
 * @brief Checks if two antennas have same position
 * @param a1 First antenna
 * @param a2 Second antenna
 * @return true if antennas have same position, false otherwise
 */
bool has_same_position(const Antenna* a1, const Antenna* a2);

/**
 * @brief Checks for antennas at same position in scene
 * @param scene Current scene
 * @param id1 Output parameter for first antenna ID
 * @param id2 Output parameter for second antenna ID
 * @return true if antennas at same position found, false otherwise
 */
bool check_antenna_positions(const Scene* scene, char* id1, char* id2);

/**
 * @brief Validates all arguments of a building line
 * @param id Building identifier
 * @param x_str X coordinate string
 * @param y_str Y coordinate string
 * @param w_str Width string
 * @param h_str Height string
 * @param line_num Line number for error reporting
 * @return true if all arguments are valid, false otherwise
 */
bool validate_building_args(const char* id, const char* x_str, const char* y_str, 
                          const char* w_str, const char* h_str, int line_num);

/**
 * @brief Extracts building arguments from a line
 * @param line Input line
 * @param id Output for building ID
 * @param x_str Output for x coordinate string
 * @param y_str Output for y coordinate string
 * @param w_str Output for width string
 * @param h_str Output for height string
 * @param line_num Line number for error reporting
 * @return true if extraction successful, false otherwise
 */
bool extract_building_args(const char* line, char* id, char* x_str, char* y_str,
                         char* w_str, char* h_str, int line_num);

/**
 * @brief Constructs a Building structure from validated arguments
 * @param building Output Building structure
 * @param id Building ID
 * @param x_str X coordinate string
 * @param y_str Y coordinate string
 * @param w_str Width string
 * @param h_str Height string
 */
void construct_building(Building* building, const char* id, const char* x_str,
                      const char* y_str, const char* w_str, const char* h_str);

/**
 * @brief Validates all arguments of an antenna line
 * @param id Antenna identifier
 * @param x_str X coordinate string
 * @param y_str Y coordinate string
 * @param r_str Radius string
 * @param line_num Line number for error reporting
 * @return true if all arguments are valid, false otherwise
 */
bool validate_antenna_args(const char* id, const char* x_str, const char* y_str,
                         const char* r_str, int line_num);

/**
 * @brief Extracts antenna arguments from a line
 * @param line Input line
 * @param id Output for antenna ID
 * @param x_str Output for x coordinate string
 * @param y_str Output for y coordinate string
 * @param r_str Output for radius string
 * @param line_num Line number for error reporting
 * @return true if extraction successful, false otherwise
 */
bool extract_antenna_args(const char* line, char* id, char* x_str, char* y_str,
                        char* r_str, int line_num);

/**
 * @brief Constructs an Antenna structure from validated arguments
 * @param antenna Output Antenna structure
 * @param id Antenna ID
 * @param x_str X coordinate string
 * @param y_str Y coordinate string
 * @param r_str Radius string
 */
void construct_antenna(Antenna* antenna, const char* id, const char* x_str,
                      const char* y_str, const char* r_str);

/**
 * @brief Prints buildings in sorted order
 * @param scene Scene containing buildings
 */
void print_sorted_buildings(const Scene* scene);

/**
 * @brief Prints antennas in sorted order
 * @param scene Scene containing antennas
 */
void print_sorted_antennas(const Scene* scene);

/**
 * @brief Initializes an empty scene
 * @param scene Scene to initialize
 */
void init_scene(Scene* scene);

/**
 * @brief Processes a building line and adds to scene
 * @param scene Current scene
 * @param line Line to process
 * @param line_num Current line number for error reporting
 * @return true if processing successful, false otherwise
 */
bool process_building(Scene* scene, const char* line, int line_num);

/**
 * @brief Processes an antenna line and adds to scene
 * @param scene Current scene
 * @param line Line to process
 * @param line_num Current line number for error reporting
 * @return true if processing successful, false otherwise
 */
bool process_antenna(Scene* scene, const char* line, int line_num);

/**
 * @brief Processes any input line
 * @param scene Current scene
 * @param line Line to process
 * @param line_num Current line number for error reporting
 * @return true if processing successful, false otherwise
 */
bool process_line(Scene* scene, char* line, int line_num);

/**
 * @brief Reads complete scene from stdin
 * @param scene Output parameter for read scene
 * @return true if reading successful, false otherwise
 */
bool read_scene(Scene* scene);

/**
 * @brief Computes bounding box for scene
 * @param scene Scene to analyze
 * @param min_x Output parameter for minimum x coordinate
 * @param max_x Output parameter for maximum x coordinate
 * @param min_y Output parameter for minimum y coordinate
 * @param max_y Output parameter for maximum y coordinate
 */
void compute_bounding_box(const Scene* scene, int* min_x, int* max_x, int* min_y, int* max_y);

/**
 * @brief Prints scene bounding box
 * @param scene Scene to analyze
 */
void print_bounding_box(const Scene* scene);

/**
 * @brief Prints scene summary
 * @param scene Scene to summarize
 */
void print_summary(const Scene* scene);

/**
 * @brief Prints building details
 * @param b Building to print
 */
void print_building(const Building* b);

/**
 * @brief Prints antenna details
 * @param a Antenna to print
 */
void print_antenna(const Antenna* a);

/**
 * @brief Comparison function for sorting IDs
 * @param a First ID to compare
 * @param b Second ID to compare
 * @return Negative if a<b, 0 if equal, positive if a>b
 */
int compare_ids(const void* a, const void* b);

/**
 * @brief Prints detailed scene description
 * @param scene Scene to describe
 */
void print_description(const Scene* scene);

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


bool validate_building_args(const char* id, const char* x_str, const char* y_str,
                          const char* w_str, const char* h_str, int line_num) {
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
    return true;
}

bool extract_building_args(const char* line, char* id, char* x_str, char* y_str,
                         char* w_str, char* h_str, int line_num) {
    if (sscanf(line, " building %10s %10s %10s %10s %10s ",
               id, x_str, y_str, w_str, h_str) != 5) {
        fprintf(stderr, "error: building line has wrong number of arguments (line #%d)\n", line_num);
        return false;
    }
    return true;
}

void construct_building(Building* building, const char* id, const char* x_str,
                      const char* y_str, const char* w_str, const char* h_str) {
    strcpy(building->id, id);
    building->x = atoi(x_str);
    building->y = atoi(y_str);
    building->w = atoi(w_str);
    building->h = atoi(h_str);
}

bool parse_building_line(const char* line, Building* building, int line_num) {
    char id[MAX_ID_LENGTH];
    char x_str[MAX_ARG_LENGTH], y_str[MAX_ARG_LENGTH];
    char w_str[MAX_ARG_LENGTH], h_str[MAX_ARG_LENGTH];
    
    if (!extract_building_args(line, id, x_str, y_str, w_str, h_str, line_num)) {
        return false;
    }
    
    if (!validate_building_args(id, x_str, y_str, w_str, h_str, line_num)) {
        return false;
    }
    
    construct_building(building, id, x_str, y_str, w_str, h_str);
    return true;
}

bool validate_antenna_args(const char* id, const char* x_str, const char* y_str,
                         const char* r_str, int line_num) {
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
    return true;
}

bool extract_antenna_args(const char* line, char* id, char* x_str, char* y_str,
                        char* r_str, int line_num) {
    if (sscanf(line, " antenna %10s %10s %10s %10s ",
               id, x_str, y_str, r_str) != 4) {
        fprintf(stderr, "error: antenna line has wrong number of arguments (line #%d)\n", line_num);
        return false;
    }
    return true;
}

void construct_antenna(Antenna* antenna, const char* id, const char* x_str,
                      const char* y_str, const char* r_str) {
    strcpy(antenna->id, id);
    antenna->x = atoi(x_str);
    antenna->y = atoi(y_str);
    antenna->r = atoi(r_str);
}

bool parse_antenna_line(const char* line, Antenna* antenna, int line_num) {
    char id[MAX_ID_LENGTH];
    char x_str[MAX_ARG_LENGTH], y_str[MAX_ARG_LENGTH], r_str[MAX_ARG_LENGTH];
    
    if (!extract_antenna_args(line, id, x_str, y_str, r_str, line_num)) {
        return false;
    }
    
    if (!validate_antenna_args(id, x_str, y_str, r_str, line_num)) {
        return false;
    }
    
    construct_antenna(antenna, id, x_str, y_str, r_str);
    return true;
}

void print_sorted_buildings(const Scene* scene) {
    const char* building_ids[MAX_BUILDINGS];
    for (int i = 0; i < scene->num_buildings; i++) {
        building_ids[i] = scene->buildings[i].id;
    }
    qsort(building_ids, scene->num_buildings, sizeof(char*), compare_ids);
    
    for (int i = 0; i < scene->num_buildings; i++) {
        for (int j = 0; j < scene->num_buildings; j++) {
            if (strcmp(building_ids[i], scene->buildings[j].id) == 0) {
                print_building(&scene->buildings[j]);
                break;
            }
        }
    }
}

void print_sorted_antennas(const Scene* scene) {
    const char* antenna_ids[MAX_ANTENNAS];
    for (int i = 0; i < scene->num_antennas; i++) {
        antenna_ids[i] = scene->antennas[i].id;
    }
    qsort(antenna_ids, scene->num_antennas, sizeof(char*), compare_ids);
    
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
    print_sorted_buildings(scene);
    print_sorted_antennas(scene);
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