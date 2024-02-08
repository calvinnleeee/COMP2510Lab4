// A00922317
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_MSG     "Error"
#define IN_FNAME    "input.txt"
#define OUT_FNAME   "output.txt"
#define EOF_CHAR    'E'

// function prototypes
void printError();
int readLines(char *fname);
void readCoords(char *fname, int *xptr, int *yptr, int lines);
int findMax(int *ptr, int lines);
int findMin(int *ptr, int lines);
void normalize(int *ptr, int min, int lines);
void drawShape(int *xptr, int *yptr, int lineCount, int *arr, int rows, int cols);
void printShape(int *ptr, int rows, int cols, char *fname);

int main(int argc, char *argv[]) {
  // check argument count, error if incorrect
  if (argc != 3) {
    printError();
    return 1;
  }

  // check argument filenames, error if incorrect
  char *inFname = argv[1];
  char *outFname = argv[2];
  if (strcmp(inFname, IN_FNAME) != 0) {
    printError();
    return 1;
  }
  if (strcmp(outFname, OUT_FNAME) != 0) {
    printError();
    return 1;
  }

  // read linecount and create 2 arrays to store x's and y's
  int lineCount = readLines(IN_FNAME);
  int *xArr = malloc(sizeof(int) * lineCount);
  int *yArr = malloc(sizeof(int) * lineCount);
  readCoords(IN_FNAME, xArr, yArr, lineCount);

  // find max and min values
  int maxX = findMax(xArr, lineCount);
  int maxY = findMax(yArr, lineCount);
  int minX = findMin(xArr, lineCount);
  int minY = findMin(yArr, lineCount);
  
  // normalize the coordinates (add minimum value to all values to make minimum = 0)
  normalize(xArr, minX, lineCount);
  normalize(yArr, minY, lineCount);

  // calculate distance between max and mins
  // create array that will hold the drawing, initialize to all 0s
  // minimum size for malloc = rows * cols
  int distX = maxX - minX + 1;    // ex. to draw from 2 to 4 -> (4 - 2) + 1 = 3
  int distY = maxY - minY + 1;    // 2, 3, 4 = 3 points
  int *drawing = malloc(distX * distY * sizeof(int));
  memset(drawing, 0, distX * distY * sizeof(int));

  // draw the shape to the array
  drawShape(xArr, yArr, lineCount, drawing, distY, distX);

  // print the shape to the output file
  printShape(drawing, distY, distX, OUT_FNAME);

  free(xArr);
  free(yArr);
  free(drawing);
  return 0;
}

/*
*   printError: prints the error message to the output file
*/
void printError() {
  FILE *out = fopen(OUT_FNAME, "w");
  fprintf(out, ERR_MSG);
  fclose(out);
}

/*
*   readLines: Get the number of lines in the input file
*   param fname = name of the input file to read
*   return = the number of lines in the input file
*/
int readLines(char *fname) {
  FILE *input = fopen(fname, "r");
  if (input == NULL) {
    printError();
    exit(1);
  }
  
  // count the number of lines in the file
  int lineCount = 0;
  while (1) {
    char check = fgetc(input);
    if (check == '\n') {
      lineCount++;
    }
    else if (check == EOF_CHAR || check == '\0') {
      break;
    }
  }
  return lineCount;
}

/*
*   readCoords: reads the coordinates in the input file
*   param fname = input filename to open
*   param xptr = pointer to memory that will store x coordinates
*   param yptr = pointer to memory that will store y coordinates
*   param lines = number of lines read from the input file
*/
void readCoords(char *fname, int *xptr, int *yptr, int lines) {
  FILE *input = fopen(fname, "r");
  if (input == NULL) {
    printError();
    exit(1);
  }
  
  int offset = 0;
  int isNeg = 0;
  int isX = 1;
  // read coordinates, alternating between storing x and y
  // handle negatives, ignore if ',', and move to next line if hitting a newline char
  while (lines > 0) {
    char check = fgetc(input);
    if (check == ',' || check == '\r') {
      continue;
    }
    else if (check == '\n') {
      lines--;
      offset++;
      continue;
    }
    else if (check == EOF_CHAR) {
      break;
    }
    else {
      if (check == '-') {
        isNeg = 1;
        continue;
      }
      if (isX == 1) {
        *(xptr + offset) = (int) (check - '0');
        if (isNeg == 1) {
          *(xptr + offset) = -*(xptr + offset);
          isNeg = 0;
        }
        isX = 0;
      }
      else {
        *(yptr + offset) = (int) (check - '0');
        if (isNeg == 1) {
          *(yptr + offset) = -*(yptr + offset);
          isNeg = 0;
        }
        isX = 1;
      }
    }
  }
  
  fclose(input);
}

/*
*   findMax: Find the maximum value from the x or y coordinates
*   param ptr = pointer to area memory storing the coordinates
*   param lines = number of lines/coordinates to read
*   return = the maximum value
*/
int findMax(int *ptr, int lines) {
  int max = 0x80000000;
  for (int i = 0; i < lines; i++) {
    int check = *(ptr + i);
    if (check > max) max = check;
  }
  return max;
}

/*
*   findMin: Find the minimum value from the x or y coordinates
*   param ptr = pointer to area memory storing the coordinates
*   param lines = number of lines/coordinates to read
*   return = the minimum value
*/
int findMin(int *ptr, int lines) {
  int min = 0x7FFFFFFF;
  for (int i = 0; i < lines; i++) {
    int check = *(ptr + i);
    if (check < min) min = check;
  }
  return min;
}

/*
*   normalize: add the minimum value to coordinates so the minimum = 0
*   param ptr = pointer to area memory storing the coordinates
*   param min = minimum value to add to all coordinates
*   param lines = number of lines/coordinates to read
*/
void normalize(int *ptr, int min, int lines) {
  for (int i = 0; i < lines; i++) {
    *(ptr + i) = *(ptr + i) - min;
  }
}

/*
*   drawShape: places the asterisks in the array to draw the shape
*   param xptr = pointer to the x coordinates
*   param yptr = pointer to the y coordinates
*   param lineCount = number of coordinates stored in each memory locations for x and y
*   param *arr = location in memory to draw shape to (2d array) (row*col spaces)
*   param rows = number of rows to draw
*   param cols = number of cols to draw
*/
void drawShape(int *xptr, int *yptr, int lineCount, int *arr, int rows, int cols) {
  int lines = lineCount;
  int offset = 0;
  while (lines > 0) {
    int x1 = *(xptr + offset);
    int y1 = *(yptr + offset);
    int x2 = *(xptr + offset + 1);
    int y2 = *(yptr + offset + 1);
    // wrap x2 and y2 around to beginning if end of the allocated memory is reached
    if (offset == lineCount - 1) {
      x2 = *xptr;
      y2 = *yptr;
    }
    if (x1 != x2 && y1 != y2) {       // if x and y both different, diagonal
      while (x1 != x2 && y1 != y2) {
        *(arr + (cols * (rows - y1 - 1)) + x1) = 1;
        if (y1 < y2) y1++;
        else y1--;
        if (x1 < x2) x1++;
        else x1--;
      }
    }
    else if (x1 == x2 && y1 != y2) {  // if x's equal, draw vertical line
      while (y1 != y2) {
        *(arr + (cols * (rows - y1 - 1)) + x1) = 1;
        if (y1 < y2) y1++;
        else y1--;
      }
    }
    else if (y1 == y2 && x1 != x2) {  // if y's equal, draw horizontal line
      while (x1 != x2) {
        *(arr + (cols * (rows - y1 - 1)) + x1) = 1;
        if (x1 < x2) x1++;
        else x1--;
      }
    }
    offset++;
    lines--;
  }

  // fill the shape
  int *start, *end;
  for (int i = 0; i < rows; i++) {
    start = arr + (cols * i);
    end = arr + (cols * (i + 1) - 1);
    int fill = 0;
    // set end index to first occurence of 1 from the end of the row
    while (*end == 0) {
      end--;
    }
    // move the start, once it encounters a 1, start filling until it reaches end index
    while (start <= end) {
      if (*start == 1) {
        fill = 1;
        start++;
        continue;
      }
      if (fill == 1) {
        *start = 1;
        start++;
      }
      else {
        start++;
      }
    }
  }
}

/*
*   printShape: outputs the shape to the output file
*   param ptr = pointer to the drawing (2d array of row*col size)
*   param rows = number of rows in the drawing
*   param cols = number of cols in the drawing
*   param fname = the name of the output file
*/
void printShape(int *ptr, int rows, int cols, char *fname) {
  FILE *output = fopen(fname, "w");
  if (output == NULL) {
    printError();
    exit(1);
  }

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (*(ptr + (cols * i) + j) == 1) {
        fputc('*', output);
      }
      else fputc(' ', output);
    }
    if (i < rows - 1) fputc('\n', output);
  }
  fclose(output);
}