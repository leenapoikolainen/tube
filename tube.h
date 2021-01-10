enum Direction {N, S, W, E, NE, NW, SE, SW, INVALID_DIRECTION};


/* pre-supplied function to load a tube map from a file*/
char **load_map(const char *filename, int &height, int &width);

/* pre-supplied function to print the tube map */
void print_map(char **m, int height, int width);

/* pre-supplied helper function to describe errors for Question 3 */
const char *error_description(int code);

/* presupplied helper function for converting string to Direction enum */
Direction string_to_direction(const char *token);

bool get_symbol_position(char **map, int height, int width, char target, 
                        int &r, int &c); 

char get_symbol_for_station_or_line(const char *name);

int validate_route(char **map, int height, int width, const char *start_station, 
char *route, char *destination);

void make_move(int &row, int &column, Direction direction);

bool is_station(char symbol);
void symbol_to_station(char symbol, char *station);
