#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *byte_color(unsigned char byte) {
  if (byte >= 32 && byte <= 126) {
    return "\033[1;32m";
  }
  if (byte == '\t' || byte == '\n' || byte == '\r') {
    return "\033[1;33m";
  }
  if (byte == 0) {
    return "\033[1;37m";
  }
  if (byte == 255) {
    return "\033[1;34m";
  }
  return "\033[1;31m";
}

static int use_color(void) {
  const char *no_color = getenv("NO_COLOR");
  const char *term = getenv("TERM");

  return (no_color == NULL || no_color[0] == '\0') && term != NULL &&
         term[0] != '\0' && strcmp(term, "dumb") != 0 && isatty(STDOUT_FILENO);
}

static void print_line(const unsigned char group[16], size_t used,
                       size_t offset, int color) {
  printf("%08zx: ", offset);

  for (size_t i = 0; i < 16; i++) {
    if (i < used) {
      if (color) {
        fputs(byte_color(group[i]), stdout);
      }
      printf("%02x", (unsigned int)group[i]);
      if (color) {
        fputs("\033[0m", stdout);
      }
    } else {
      printf("  ");
    }
    if (i % 2 == 1) {
      putchar(' ');
    }
  }

  putchar(' ');
  for (size_t i = 0; i < used; i++) {
    if (color) {
      fputs(byte_color(group[i]), stdout);
    }
    if (isprint(group[i])) {
      putchar(group[i]);
    } else {
      putchar('.');
    }
    if (color) {
      fputs("\033[0m", stdout);
    }
  }
  putchar('\n');
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Error: expected exactly one file.\nUsage: %s <file>\n",
            argv[0]);
    return 1;
  } else {
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
      fprintf(stderr, "Error opening '%s': %s\n", argv[1], strerror(errno));
      return 1;
    }

    unsigned char group[16];
    size_t used = 0;
    size_t offset = 0;
    int color = use_color();
    int read = fgetc(file);
    while (read != EOF) {
      group[used] = (unsigned char)read;
      used += 1;

      if (used == 16) {
        print_line(group, used, offset, color);
        offset += used;
        used = 0;
      }

      read = fgetc(file);
    }
    if (ferror(file)) {
      int read_errno = errno;
      fclose(file);
      fprintf(stderr, "Error reading '%s': %s\n", argv[1], strerror(read_errno));
      return 1;
    }
    if (used > 0) {
      print_line(group, used, offset, color);
    }
    fclose(file);
  }
}
