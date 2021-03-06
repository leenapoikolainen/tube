#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>
#include <cstring>
#include <cctype>
#include <cstdlib>

using namespace std;

/* error codes for Question 3 */
#define ERROR_START_STATION_INVALID -1 
#define ERROR_ROUTE_ENDPOINT_IS_NOT_STATION -2
#define ERROR_LINE_HOPPING_BETWEEN_STATIONS -3
#define ERROR_BACKTRACKING_BETWEEN_STATIONS -4
#define ERROR_INVALID_DIRECTION -5
#define ERROR_OFF_TRACK -6
#define ERROR_OUT_OF_BOUNDS -7
#include "tube.h"

/* You are pre-supplied with the functions below. Add your own 
   function definitions to the end of this file. */

/* internal helper function which allocates a dynamic 2D array */
char **allocate_2D_array(int rows, int columns) {
  char **m = new char *[rows];
  assert(m);
  for (int r=0; r<rows; r++) {
    m[r] = new char[columns];
    assert(m[r]);
  }
  return m;
}

/* internal helper function which deallocates a dynamic 2D array */
void deallocate_2D_array(char **m, int rows) {
  for (int r=0; r<rows; r++)
    delete [] m[r];
  delete [] m;
}

/* internal helper function which gets the dimensions of a map */
bool get_map_dimensions(const char *filename, int &height, int &width) {
  char line[512];
  
  ifstream input(filename);

  height = width = 0;

  input.getline(line,512);  
  while (input) {
    if ( (int) strlen(line) > width)
      width = strlen(line);
    height++;
    input.getline(line,512);  
  }

  if (height > 0)
    return true;
  return false;
}

/* pre-supplied function to load a tube map from a file*/
char **load_map(const char *filename, int &height, int &width) {

  bool success = get_map_dimensions(filename, height, width);
  
  if (!success)
    return NULL;

  char **m = allocate_2D_array(height, width);
  
  ifstream input(filename);

  char line[512];
  char space[] = " ";

  for (int r = 0; r<height; r++) {
    input.getline(line, 512);
    strcpy(m[r], line);
    while ( (int) strlen(m[r]) < width )
      strcat(m[r], space);
  }
  
  return m;
}

/* pre-supplied function to print the tube map */
void print_map(char **m, int height, int width) {
  cout << setw(2) << " " << " ";
  for (int c=0; c<width; c++)
    if (c && (c % 10) == 0) 
      cout << c/10;
    else
      cout << " ";
  cout << endl;

  cout << setw(2) << " " << " ";
  for (int c=0; c<width; c++)
    cout << (c % 10);
  cout << endl;

  for (int r=0; r<height; r++) {
    cout << setw(2) << r << " ";    
    for (int c=0; c<width; c++) 
      cout << m[r][c];
    cout << endl;
  }
}

/* pre-supplied helper function to report the errors encountered in Question 3 */
const char *error_description(int code) {
  switch(code) {
  case ERROR_START_STATION_INVALID: 
    return "Start station invalid";
  case ERROR_ROUTE_ENDPOINT_IS_NOT_STATION:
    return "Route endpoint is not a station";
  case ERROR_LINE_HOPPING_BETWEEN_STATIONS:
    return "Line hopping between stations not possible";
  case ERROR_BACKTRACKING_BETWEEN_STATIONS:
    return "Backtracking along line between stations not possible";
  case ERROR_INVALID_DIRECTION:
    return "Invalid direction";
  case ERROR_OFF_TRACK:
    return "Route goes off track";
  case ERROR_OUT_OF_BOUNDS:
    return "Route goes off map";
  }
  return "Unknown error";
}

/* presupplied helper function for converting string to direction enum */
Direction string_to_direction(const char *token) {
  const char *strings[] = {"N", "S", "W", "E", "NE", "NW", "SE", "SW"};
  for (int n=0; n<8; n++) {
    if (!strcmp(token, strings[n])) 
      return (Direction) n;
  }
  return INVALID_DIRECTION;
}

bool get_symbol_position(char **map, int height, int width, char target, 
                        int &r, int &c) 
{
  for (int row = 0; row < height; row++) {
    for (int column = 0; column < width; column++) {
      if (map[row][column] == target) {
        r = row;
        c = column;
        return true;
      }
    }
  }
  r = c = -1;
  return false;
}

char get_symbol_for_station_or_line(const char *name) {
  char ch = ' ';
  // check for lines
  ifstream in;
  in.open("lines.txt");
  if (in.fail()) {
    cout << "Error in opening the stations file" << endl;
    exit(1);
  }

  // Read the file line by line
  for (string str; getline(in, str); ) {
    const char *line = str.c_str();
    if (!strcmp(name, line + 2)) {
      ch = *line;
    }
  }
  in.close();

  // check for stations
  in.open("stations.txt");
  if (in.fail()) {
    cout << "Error in opening the stations file" << endl;
    exit(1);
  }

  // Read the file line by line
  for (string str; getline(in, str); ) {
    const char *line = str.c_str();
    if (!strcmp(name, line + 2)) {
      ch = *line;
    }
  }
  return ch;
}



int validate_route(char **map, int height, int width, const char *start_station, 
char *route, char *destination)
{
  char start_symbol = ' ';
  if(get_symbol_for_station_or_line(start_station) == ' ')
    return ERROR_START_STATION_INVALID;
  else
    start_symbol = get_symbol_for_station_or_line(start_station);
  
  // Directions array
  Direction directions[50];
  int count = 0;

  // Go through the route separated by comma
  char *pointer;
  pointer = strtok(route, ",");
  while (pointer != NULL) {
    if (string_to_direction(pointer) == INVALID_DIRECTION)
      return ERROR_INVALID_DIRECTION;
    else {
      directions[count++] = string_to_direction(pointer);
    }
    pointer = strtok(NULL,",");
  }
  
  // Set start position
  int row = 0, column = 0;
  get_symbol_position(map, height, width, start_symbol, row, column);

  int line_changes = 0;
  char previous;
  char before_station = ' ';

  // Checking movements
  for (int i = 0; i < count; i++) {
    // Store previous symbol
    previous = map[row][column];
    // make move
    make_move(row, column, directions[i]);
    // Test if in the map
    if (row < 0 || row >= height || column < 0 || column >= width)
      return ERROR_OUT_OF_BOUNDS;
    // check if on track
    if (map[row][column] == ' ')
      return ERROR_OFF_TRACK;
    // Check line hopping between stations
    // Current char is not station && is different that the previous
    if (!is_station(map[row][column]) && !is_station(previous)
        && map[row][column] != previous)
      return ERROR_LINE_HOPPING_BETWEEN_STATIONS;
    // Checking backtrackin
    if (i > 0)  {
      if (abs (directions[i] - directions[i-1]) == 1  &&
        !is_station(previous))
      {
        return ERROR_BACKTRACKING_BETWEEN_STATIONS; 
      }
    }
    // if it is station store the previous
    if (is_station(map[row][column])) {
      before_station = previous;
    }
    if (is_station(previous) && before_station != ' ' && before_station != map[row][column])
     line_changes++;
  }

  // Check end station
  symbol_to_station(map[row][column], destination);
  return line_changes;
}

void symbol_to_station(char symbol, char *station){
  ifstream in;
  in.open("stations.txt");
  if (in.fail()) {
    cout << "Error in opening the stations file" << endl;
    exit(1);
  }
  // Read the file line by line
  for (string str; getline(in, str); ) {
    if (str[0] == symbol) {
      const char *line = str.c_str();
      strcpy(station, line + 2);
    }  
  }
  in.close();
}

void make_move(int &row, int &column, Direction direction) {
  switch (direction) {
    case N:
      row--; break;
    case S:
      row++; break;
    case W:
      column--; break;
    case E:
      column++; break;
    case NE:
      row--; column++; break;
    case NW:
      row--; column--; break;
    case SE:
      row++; column++; break;
    case SW:
      row++; column--; break;
    case INVALID_DIRECTION:
      break;
  }
}


bool is_station(char symbol) {
  ifstream in;
  in.open("stations.txt");
  if (in.fail()) {
    cout << "Error in opening the stations file" << endl;
    exit(1);
  }
  // Read the file line by line
  for (string str; getline(in, str); ) {
    if (str[0] == symbol) {
      in.close();
      return true;
    }  
  }
  in.close();
  return false;
}