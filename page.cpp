#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Page {  // page class to represent page in a story
 private:
  size_t page_num;         // page number
  vector<string> choices;  // choices on the page
  vector<string> story;    // the story section of the page

 public:
  Page() : page_num(0) {}                           // default constructor
  Page(const string & file_name, size_t page_num);  // constructor
  bool check_errors(const string & file_name);      // check if errors in file
  void print_page();                                // prints the page to stdout
  size_t get_page_num();
  vector<string> get_choices();
};

vector<string> Page::get_choices() {
  return choices;
}
size_t Page::get_page_num() {
  return page_num;
}
void Page::print_page() {
  for (size_t i = 0; i < story.size(); i++) {  // print out story first
    cout << story[i] << "\n";
  }
  cout << "\n";
  if (choices[0] == "WIN") {  // print win condition
    cout << "Congratulations! You have won. Hooray!\n";
    return;
  }
  else if (choices[0] == "LOSE") {  // print lose condition
    cout << "Sorry, you have lost. Better luck next time!\n";
    return;
  }
  cout << "What would you like to do?\n\n";

  for (size_t i = 0; i < choices.size(); i++) {  // prints out choices to user
    cout << " " << i + 1 << ". " << choices[i].substr(choices[i].find(':') + 1) << "\n";
  }
}

Page::Page(const string & file_name, size_t pagenum) {
  page_num = pagenum;
  if (check_errors(file_name) == true) {
    perror("error in file\n");
    exit(1);
  }

  ifstream file(file_name);  // use ifstream to read in file
  string line;
  int check_story = 0;  // flag to check which section of page we are in
  while (getline(file, line)) {
    if (check_story == 1) {
      story.push_back(line);
    }
    else {
      if (line[0] == '#') {  // we are done reading the story part now onto choices
        check_story = 1;
        continue;
      }
      choices.push_back(line);
    }
  }
}

bool Page::check_errors(const string & file_name) {
  int check = 1;            // used to check all three requirements of an input
  int win_loose_count = 0;  // checks to see if win or loose is itself
  int win_loose_flag = 0;
  ifstream file(file_name);  // open ifstream to read in file from file_name
  if (!file.is_open()) {
    perror("cant open file\n");
    exit(1);
  }
  else {
    string line;
    while (getline(file, line)) {  // read line by line
      if (check == 1) {            // first part of the text file
        if (win_loose_count == 1 && win_loose_flag == 1) {
          if (line[0] == '#') {
            check = 2;  // move check to second section of page
            continue;
          }
          else {
            perror("invalid format for #\n");
            return true;  // return that there was an error in the format
          }
        }
        else if (line.compare("WIN") == 0) {
          if (win_loose_count == 0) {
            win_loose_count += 1;  // increase WIN counter
            win_loose_flag = 1;
            continue;
          }
          else {
            perror("too many WIN or LOSE\n");
            return true;
          }
        }
        else if (line.compare("LOSE") == 0) {
          if (win_loose_count == 0) {
            win_loose_count += 1;  // increase LOSE counter
            win_loose_flag = 1;
            continue;
          }
          else {
            perror("too many WIN or LOSE\n");
            return true;  // return that there was an error in the format
          }
        }

        else if ((line[0] < '0' || line[0] > '9') && line[0] != '#') {
          perror("invalid number format\n");
          return true;
        }
        else {  // last part of the page
          win_loose_count += 1;
          for (size_t i = 0; line[i] != '\0'; i++) {
            if (line[i] >= '0' &&
                line[i] <= '9') {  // check if the choice is a valid number
              continue;
            }
            else {
              if (line[i] == ':')  // check for colon
                break;
              else {
                if (line[0] == '#' && win_loose_count >= 1)
                  return false;
                perror("invalid number format and colon\n");
                return true;
              }
            }
          }
          continue;
        }
      }
    }
  }
  return false;
}
