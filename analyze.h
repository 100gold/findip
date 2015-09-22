#pragma once

#include <string>
#include <vector>
#include <list>

typedef std::list<std::string> DirectoryList;
typedef std::vector<std::pair<std::string,std::string> > IpSubstitution;

void analyze(const DirectoryList& dirs, const IpSubstitution& ip);
