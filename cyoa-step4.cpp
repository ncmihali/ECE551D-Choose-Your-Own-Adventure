#include <string>
#include <vector>

#include "story.cpp"
using namespace std;

int main(int argc, char ** argv) {
  if (argc != 2) {
    perror("not enough arguments\n");
    exit(1);
  }
  Story new_story(argv[1]);
  new_story.show_paths();
  return EXIT_SUCCESS;
}
