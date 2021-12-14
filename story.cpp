#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "page.cpp"

using namespace std;

class Story {  // story class which consists of pages, and story methods
 private:
  vector<Page> pages;      // vector of pages
  size_t num_pages;        // total number of pages
  vector<size_t> visited;  // check which pages are visited (used in graph)
  size_t current_weight;

 public:
  Story() : num_pages(0), current_weight(0) {}  // default constructor
  Story(const string & directory_name);         // constructor
  bool valid_page(size_t page_number);          // check if page exists in story
  bool check_choice_reference(
      const size_t & page_num);  // check if choice reference is valid
  void play_story();             // reads story
  bool check_choice_valid_input(Page & p,
                                size_t page_choice);  // check user input choice valid
  size_t get_choice_num(const string & s);  // converts choice string into size_t
  void show_depths();                       // shows depths of each page (levels)
  size_t find_choice_num(vector<string> & choices, size_t & next);
  bool is_visited(const size_t & page_num);  // checks if page_num has been visited
  size_t find_depth(Page & p,
                    const size_t & page_num);  // finds the depth of one page in story
  void show_paths();                           // shows all winnable, non-cyclic paths
};

class Graph : public Story {  // using graph traversal to find shortest path from AoP
 public:
  size_t num_nodes;      // total number of pages/nodes
  vector<size_t> * adj;  // adjacency list
  size_t * level;        // array of levels for each page
  vector<size_t> path;   // a winning path
  bool * visited;
  vector<vector<size_t> > paths;  // all winning paths for step 4

 public:
  Graph(size_t num_nodes) : num_nodes(num_nodes) {  // constructor
    adj = new vector<size_t>[num_nodes + 1];        // create adjacency list
    level = new size_t[num_nodes + 1];              // create level array
    visited = new bool[num_nodes + 1];              // create a visited array of nodes
  }
  ~Graph() {  // destructor deleting dynamic memory
    delete[] visited;
    delete[] adj;
    delete[] level;
  }
  void addEdge(const size_t & from, const size_t & to) {  // adds to graph, adjacency list
    adj[from].push_back(to - 1);                          // directed graph
  }

  size_t depth(const size_t & source, const size_t & dest, size_t size) {
    // BFS that returns the level from source to destination node/page
    for (size_t i = 0; i < size; i++) {  // set all visited array to false
      visited[i] = false;
    }

    queue<size_t> Queue;  // switch from stack to queue for BFS from AoP

    visited[source] = true;
    level[source] = 0;   // first page, 0th level/depth
    Queue.push(source);  // push page onto queue

    while (!Queue.empty()) {
      size_t current = Queue.front();  // get page, then pop
      Queue.pop();
      for (size_t i = 0; i < adj[current].size(); i++) {  // cycle through adjacency list
        if (visited[adj[current][i]] == false) {
          visited[adj[current][i]] = true;
          level[adj[current][i]] = level[current] + 1;  // add distance to node
          Queue.push(adj[current][i]);                  // push current page from adj
          if (adj[current][i] == dest) {  // return the level of dest page if found
            delete[] visited;
            visited = new bool[num_nodes + 1];  // delete and refresh visited array
            return level[dest];
          }
        }
      }
    }
    delete[] visited;
    visited = new bool[num_nodes + 1];
    return 0;  // else return nothing
  }
  void add_paths(const size_t & current_page, const size_t & dest) {
    // find and place all path(s) into paths
    visited[current_page] = true;
    if (current_page == dest) {  // once page is destination, add the path to paths
      paths.push_back(path);
    }
    else {
      vector<size_t> adj_node = adj[current_page];
      for (size_t i = 0; i < adj_node.size(); i++) {  // cycle through adj list
        //path.push_back(adj_node[i]); // add adj_node to path
        if (visited[adj_node[i]] == false) {
          path.push_back(adj_node[i]);
          add_paths(adj_node[i], dest);  // if not visited yet, DFS recurisve
          //visited[current_page] = false;
          path.pop_back();
        }
        //path.pop_back();
      }
    }
    visited[current_page] = false;
  }

  int find_wins(const size_t & source, vector<Page> Pages, size_t size) {
    // use of DFS instead of BFS to find winning paths
    vector<size_t> winning_pages;  // find all "WIN" pages and put them into array
    for (size_t i = 0; i < size; i++) {
      visited[i] = false;  // set all paths to not visited
    }

    vector<Page>::iterator it = Pages.begin();  // this section finds all WINing pages
    while (it != Pages.end()) {
      vector<string> choices = (*it).get_choices();
      vector<string>::iterator choice_it = choices.begin();
      if (*choice_it == "WIN") {
        winning_pages.push_back((*it).get_page_num());
      }
      ++it;
    }

    vector<size_t>::iterator win_it = winning_pages.begin();
    vector<size_t>::iterator win_end = winning_pages.end();
    if (win_it == win_end) {  // if win_it has nothing in it, cant win!
      cout << "This story is unwinnable!\n";
      return 0;
    }

    size_t current_page = source;  // start at page 1
    while (win_it != win_end) {
      // add all "path"s to paths vector given the WIN page node in *win_it
      add_paths(current_page, (*win_it) - 1);
      path.clear();       // after adding, clear path
      path.push_back(0);  // start at page 1 and add it
      ++win_it;
      delete[] visited;  // reset visited for new visited in the next *win)it
      visited = new bool[size];
      for (size_t i = 0; i < size; i++) {
        visited[i] = false;  // set all paths to not visited
      }
    }
    return 1;  // found a winnable path
  }
};

size_t Story::find_depth(Page & p, const size_t & page_num) {  // find depth of pages
  if (p.get_page_num() == page_num)                            // first page is always 0
    return 0;
  Graph new_graph = Graph(pages.size());  // create graph and import pages into adj list
  vector<Page>::iterator page_it = pages.begin();
  while (page_it != pages.end()) {
    vector<string> choices = (*page_it).get_choices();
    vector<string>::iterator choice_it = choices.begin();
    if (*choice_it == "WIN" ||
        *choice_it == "LOSE") {  // win/lose pages have no adj pages to
      ++page_it;
      continue;
    }
    while (choice_it != choices.end()) {  // add choices to adj list
      new_graph.addEdge((*page_it).get_page_num() - 1, get_choice_num(*choice_it));
      ++choice_it;
    }
    ++page_it;
  }

  if (new_graph.depth(0, page_num - 1, pages.size()) == 0) {
    cout << "Page " << page_num << " is not reachable\n";  // if not reachable...
    return 100000;  // arbitrary number to return to signal a error
  }

  return new_graph.depth(0, page_num - 1, pages.size());  // return the depth of page
}

size_t Story::find_choice_num(vector<string> & choices,
                              size_t & next) {  // 1 2 3 to page num
  vector<string>::iterator it = choices.begin();
  size_t choice_num = 1;
  while (it != choices.end()) {
    if (get_choice_num(*it) == next) {
      return choice_num;
    }
    choice_num += 1;
    ++it;
  }
  return 0;
}

bool Story::is_visited(
    const size_t & page_num) {  // check if page_num is in visited vector
  vector<size_t>::iterator it = visited.begin();
  while (it != visited.end()) {
    if (page_num == (*it))
      return true;
    ++it;
  }
  return false;
}

void Story::show_paths() {                // print all paths winnable, noncyclic
  Graph new_graph = Graph(pages.size());  //new graph to hold all page data and adj list
  vector<Page>::iterator page_it = pages.begin();
  while (page_it != pages.end()) {
    vector<string> choices = (*page_it).get_choices();
    vector<string>::iterator choice_it = choices.begin();
    if (*choice_it == "WIN" || *choice_it == "LOSE") {
      ++page_it;
      continue;
    }
    while (choice_it != choices.end()) {  // add adj list
      new_graph.addEdge((*page_it).get_page_num() - 1, get_choice_num(*choice_it));
      ++choice_it;
    }
    ++page_it;
  }
  (new_graph.path).push_back(0);
  int win_cond =
      new_graph.find_wins(0, pages, pages.size());  // see if story is unwinnable
  if (win_cond == 0)
    return;
  vector<vector<size_t> >::iterator paths_it = (new_graph.paths).begin();

  if (paths_it ==
      (new_graph.paths).end()) {  // unwinable if there are no paths in paths vector
    cout << "This story is unwinnable!\n";
  }
  while (paths_it != (new_graph.paths).end()) {  // iterate through all win conditions
    vector<size_t>::iterator path_it;
    path_it = (*paths_it).begin();
    while (path_it != (*paths_it).end()) {
      if (path_it + 1 == (*paths_it).end()) {
        cout << (*path_it) + 1 << "(win)";  // print final win statement
        path_it += 1;                       // force iterator to end
        break;
      }
      else {
        vector<string> choices = pages[*path_it].get_choices();
        size_t next_page = *(path_it + 1) + 1;
        size_t choice_num = find_choice_num(choices, next_page);
        cout << (*path_it) + 1 << "(" << choice_num << ")"
             << ",";
      }
      ++path_it;
    }
    ++paths_it;
    cout << "\n";
  }
}

void Story::show_depths() {  // display the depths
  vector<Page>::iterator it = pages.begin();
  while (it != pages.end()) {
    if (find_depth(pages[0], (*it).get_page_num()) ==
        100000) {       // if 100000, means not reachable
      visited.clear();  // clear visited pages from BFS
    }
    else {
      cout << "Page " << (*it).get_page_num() << ":"
           << find_depth(pages[0], (*it).get_page_num()) << "\n";
      visited.clear();
    }
    ++it;
  }
}

size_t Story::get_choice_num(const string & s) {  // convert choice to a number
  size_t ans;
  stringstream ss;
  string value = s.substr(0, s.find(":"));
  ss << value;
  ss >> ans;
  return ans;
}

bool Story::check_choice_valid_input(Page & p,
                                     size_t page_choice) {  // validate choice is a page
  Page temp = p;
  vector<string>::iterator it = (temp.get_choices()).begin();
  while (it != (temp.get_choices()).end()) {
    if (get_choice_num(*it) == page_choice) {
      return true;
    }
    ++it;
  }
  return false;
}

void Story::play_story() {
  size_t i = 0;          // iterator for all valid pages
  int check_finish = 0;  // check when finished with story
  while (check_finish == 0) {
    pages[i].print_page();  // print page
    if ((pages[i].get_choices())[0] == "WIN" || (pages[i].get_choices())[0] == "LOSE") {
      check_finish = 1;  // checks if page is a win or lose page
      continue;
    }
    int check_validity = 0;  // if not a win/lose page, we will check for valid input
    while (check_validity == 0) {
      string input;
      size_t answer_input;
      stringstream ss;
      getline(cin, input);  // recieve stdin into input
      check_validity = 1;
      string::iterator strit = input.begin();
      while (strit != input.end()) {
        if (!isdigit(*strit)) {  // iterator through inputed string
          check_validity = 0;
        }
        ++strit;
      }                  // finished checking if input is an integer
      if (input == "0")  // input must be a valid choice
        check_validity = 0;
      if (check_validity == 0) {
        cout << "That is not a valid choice, please try again\n";
        continue;
      }
      ss << input;
      ss >> answer_input;
      vector<string> choices_temp = (pages[i]).get_choices();
      vector<string>::iterator choiceit = choices_temp.begin();
      int check_on_page = 0;  // flag to check validity as well
      size_t count = 0;
      while (choiceit != choices_temp.end()) {  // cycle through choices of the page
        count += 1;                             // count how many choices there are
        ++choiceit;
      }
      if (answer_input >
          count) {  // if input is greater than total choices (1, 2, 3... etc.)
        check_on_page = 0;
      }
      else
        check_on_page = 1;
      if (check_on_page == 0)
        check_validity = 0;
      if (check_validity == 0) {
        cout << "That is not a valid choice, please try again\n";
        continue;
      }
      if (check_validity == 1)
        i = get_choice_num((pages[i].get_choices())[answer_input - 1]) -
            1;  // update page num
    }
  }
}

bool Story::check_choice_reference(const size_t & page_num) {  // check if page is refed
  vector<Page>::iterator it = pages.begin();
  while (it != pages.end()) {                // cycle through pages
    if ((*it).get_page_num() == page_num) {  // ignore its own page
      ++it;
      continue;
    }
    vector<string> choices_temp = (*it).get_choices();
    vector<string>::iterator choices_it = choices_temp.begin();
    while (choices_it != choices_temp.end()) {  // cycle through current page's choices
      string temp = (*choices_it).substr(0, (*choices_it).find(":"));
      if (temp == "WIN" || temp == "LOSE") {
        ++choices_it;
        continue;
      }
      stringstream ss;
      size_t temp_page_num;
      ss << temp;
      ss >> temp_page_num;  // get choice number in size_t
      if (temp_page_num == page_num)
        return true;  // prove page is refed by at least one other page's choices
      ++choices_it;
    }
    ++it;
  }
  return false;  // page is NOT refed by at least one other page's choices
}

bool Story::valid_page(
    size_t page_number) {  // checks if choice num is an actual story page
  vector<Page>::iterator it = pages.begin();
  while (it != pages.end()) {
    if ((*it).get_page_num() == page_number)
      return true;
    ++it;
  }
  return false;
}

Story::Story(const string & directory_name) {  // constructor
  num_pages = 0;                               // set total number of pages
  current_weight = 0;
  string file_name;
  string file_preset = directory_name + "/page";  // all files should have page#
  size_t current_page_num = 1;                    // set current page to first
  int done_yet = 0;  // flag to see if we are done reading the story
  while (done_yet != 1) {
    file_name = file_preset;
    file_name += to_string(current_page_num);  // add the # in page#
    file_name += ".txt";
    ifstream file(file_name);  // after, create an ifstream
    if (!file.is_open()) {     // if the file does not exist, we are done reading story!
      done_yet = 1;
      continue;
    }
    Page temp(file_name, current_page_num);  // else create a page out of our file
    pages.push_back(temp);                   // add page to pages
    num_pages += 1;                          // icnrease total page num count
    current_page_num += 1;
  }
  if (current_page_num == 1) {  // if exited at page 1, error
    perror("can't find page 1\n");
    exit(1);
  }

  size_t win_check = 0;                       // win flag
  size_t lose_check = 0;                      // lose flag
  vector<Page>::iterator it = pages.begin();  // iterator at begining of story
  while (it != pages.end()) {
    vector<string> choices_temp = (*it).get_choices();
    vector<string>::iterator choices_it = choices_temp.begin();
    while (choices_it != choices_temp.end()) {  // cycle through choices on current page
      string temp = (*choices_it).substr(0, (*choices_it).find(":"));
      if (temp == "WIN" || temp == "LOSE") {  // check if choice is a win or lose choice
        if (temp == "WIN")
          win_check += 1;
        if (temp == "LOSE")
          lose_check += 1;
        ++choices_it;
        continue;
      }
      size_t choice;  // turn choice from string to number
      stringstream ss;
      ss << temp;
      ss >> choice;
      if (valid_page(choice) == false) {  // check if the choice is a actual page in story
        perror("invalid choice\n");
        exit(1);
      }
      ++choices_it;
    }
    // after checking valid choices on page, then check if it is referenced
    if ((*it).get_page_num() == 1) {
      ++it;
      continue;
    }
    if (check_choice_reference((*it).get_page_num()) ==
        false) {  // reference validation check
      perror("this page is not ref'ed by another page\n");
      exit(1);
    }
    ++it;
  }
  if (win_check == 0 || lose_check == 0) {  // verify at least one win and lose page
    perror("need at least one of both win/lose\n");
    exit(1);
  }
}
