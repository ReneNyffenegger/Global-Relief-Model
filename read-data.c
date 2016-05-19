// Records proceed along parallels from West to East, then rows from South to
// North. The first record is the South-West corner (-89.9917 deg latitude, 
// -179.9917 deg longitude), and the last record is the North-East corner 
// (89.9917 deg latitude, 179.9917 deg latitude).

#include <stdio.h>
#include <stdlib.h>

#define data_points_per_degree 60.0
#define data_size               2

const unsigned int data_points_east_west   = (360 * data_points_per_degree);
const unsigned int data_points_south_north = (180 * data_points_per_degree);

const char* data_file = "data/Earth2014.BED2014.1min.geod.bin";

struct coord {
  double lat;
  double lon;
};

int read_height_pos(FILE* fd, unsigned long pos) {

  fseek(fd, pos, SEEK_SET);

  signed char data[data_size];
  fread(data, data_size, 1, fd);

  return data[0]*256 + ((unsigned char) data[1]);
}

int read_height(FILE* fd, struct coord* coord) {

  int x, y;

  x = ( coord -> lon + 180.0) * data_points_per_degree;
  y = ( coord -> lat +  90.0) * data_points_per_degree;


  unsigned long pos = data_size * ( x + y * data_points_east_west);

//printf("Pos %lu\n", pos);

  return read_height_pos(fd, pos);
}


void test_height(FILE* fd, struct coord* coord, int expected) {

  int height = read_height(fd, coord);

  if (height != expected) {
    fprintf(stderr, "Height: %d, but expected: %d\n", height, expected);
  }
  else {
    printf("Height %d ok\n", height);
  }
  printf("\n");
}

void find_deepest_heighest_pos(FILE* fd) {

  int min = 20000, max = -20000;
  unsigned long pos_min, pos_max;
  for (unsigned long pos = 0; pos <= data_points_east_west * data_points_south_north * data_size; pos+=2) {

    int height = read_height_pos(fd, pos);

    if (height > max) { max = height; pos_max = pos;}
    if (height < min) { min = height; pos_min = pos;}
  }

  printf("Max height: %d at position %lu\n", max, pos_max);
  printf("Min height: %d at position %lu\n", min, pos_min);

}

void test(FILE* fd) {


  // Test assumptions about file size
  unsigned long file_size_expected = data_points_east_west * data_points_south_north * data_size;
  fseek(fd, 0, SEEK_END);
  unsigned long file_size = ftell(fd);

  if (file_size != file_size_expected) {
    fprintf(stderr, "File size: %lu, but expected was %lu\n", file_size, file_size_expected);
  }
  else {
    printf("File size: %lu ok\n", file_size);
  }


  struct coord south_west_0  = { .lat =-89.9917           , .lon =-179.9917                                 };
  struct coord south_west_2  = { .lat =-89.9917           , .lon =-179.9917 + 0x151  /data_points_per_degree};
  struct coord south_west_3  = { .lat =-89.9917           , .lon =-179.9917 + 0x152  /data_points_per_degree};
  struct coord mount_everest = { .lat = 27.98805          , .lon = 86.9252777                               };
  struct coord mariane       = { .lat = 11.358472222      , .lon = 142.5557861                              };

  test_height(fd, &south_west_0  ,    49);
  test_height(fd, &south_west_2  ,    49);
  test_height(fd, &south_west_3  ,    50);
  test_height(fd, &mount_everest ,  8212);  // Mt Everest is 8848 M???
  test_height(fd, &mariane       ,-10834);
  
  find_deepest_heighest_pos(fd);
}

int main(/* int argc, char* argv */) {

  FILE* fd;
  fd = fopen(data_file, "r");
  if (fd == NULL) { fprintf(stderr, "Could not open %s\n", data_file); exit (-1); }

  test(fd);

  fclose(fd);
}
