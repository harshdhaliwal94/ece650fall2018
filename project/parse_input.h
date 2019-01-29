#include <iostream>
#include <sstream>
#include <regex>
#include <string>
#include <vector>
#include <list>

//using namespace std;

std::string removeChar(std::string, char);
int parse_vertex(std::string, char *);
bool parse_edge(std::string, std::vector< std::pair<int,int> >&, int, char *);