#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "analyze.h"

namespace po = boost::program_options;
namespace pt = boost::property_tree;

int main(int argc, char** argv)
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message")
		("configfile,c", po::value<std::string>(), "set config file")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << "\n";
		return 1;
	}

	if (!vm.count("configfile"))
	{
		std::cout << "need a config file\n";
		return 1;
	}

	pt::ptree tree;
	pt::read_json(vm["configfile"].as<std::string>(), tree);

	DirectoryList scandirs;
	for (auto dir : tree.get_child("scan_dirs"))
	{
		scandirs.push_back(dir.second.data());
	}
	IpSubstitution iplist;
	for (auto ipentry : tree.get_child("ip"))
	{
		iplist.push_back(std::make_pair<std::string,std::string>(
			ipentry.second.get<std::string>("from"),
			ipentry.second.get<std::string>("to")
			));
	}

	try
	{
		analyze(scandirs, iplist);
	}
	catch(const std::exception& e)
	{
		std::cout << "unknown exception: " << e.what() << "\n";
		return 1;
	}
	return 0;
}
