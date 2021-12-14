#include <string>
#include <vector>

#include "page.cpp"
using namespace std;

int main(int argc, char ** argv) {
  if (argc != 2) {
    perror("not enough argument\n");
    exit(1);
  }

  Page new_page(argv[1], 1);
  new_page.print_page();

  return EXIT_SUCCESS;
}
