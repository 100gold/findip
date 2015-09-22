#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string.hpp>
#include "analyze.h"

namespace fs = boost::filesystem;

const std::string g_apache_keyword("DocumentRoot");

void process_file(const fs::path& path, DirectoryList& dirs_queue, const IpSubstitution& ip)
{
	fs::ifstream file(path);
	std::string line;
	int line_no = 0;
	std::cout << "process file:" << path << "\n";
	while(std::getline(file, line))
	{
		++line_no;
		for (auto ipentry : ip)
		{
			auto pos = line.find(ipentry.first);
			if (pos != std::string::npos)
			{
				std::cout << "Found ip " << ipentry.first <<
					" in file " << path.string() <<
					":" << line_no <<
					". Should be changed to " << ipentry.second << "\n";
			}
		}

		// Check apache config by keyword)
		auto pos = line.find(g_apache_keyword);
		if (pos != std::string::npos)
		{
			auto external_path = line.substr(pos+g_apache_keyword.size(), std::string::npos);
			boost::trim(external_path);
			if ((external_path.substr(0,1)=="\"")&&
				(external_path.substr(external_path.size()-1,1)=="\""))
			{
				external_path = external_path.substr(1,external_path.size()-2);
			}
			if (fs::is_directory(external_path))
			{
				dirs_queue.push_back(external_path);
			}
		}
	}
}

void analyze(const DirectoryList& dirs, const IpSubstitution& ip)
{
	auto dirs_queue = dirs;
	while(dirs_queue.size()>0)
	{
		auto dir = dirs_queue.front();
		dirs_queue.pop_front();
		fs::recursive_directory_iterator it(dir);
		while (it != boost::filesystem::recursive_directory_iterator())
		{
			auto p = it->path();
			if (fs::is_regular_file(p))
			{
				process_file(p, dirs_queue, ip);
			}
			++it;
		}
	}
}
