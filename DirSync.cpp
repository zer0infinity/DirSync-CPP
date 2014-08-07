#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/crc.hpp>
#include "DirSync.h"

std::string DirSync::PROGRAM = "DirSync";
std::string DirSync::VERSION = "0.1";
std::string DirSync::CODER = "David Tran";
std::string DirSync::FILE = "DirSync.txt";
std::string DirSync::FOLDER = "SyncedFiles";

DirSync::DirSync() {
	readFile();
	init();

	std::cout << "Sync in progress..." << std::endl;
	for (unsigned int i = 0; i < list.size(); ++i) {
		sync(list.at(i), FOLDER);
	}
	std::cout << "Sync finished" << std::endl;
}

void DirSync::showHelp() const {
	std::cout << "\n" << PROGRAM << " " << VERSION << " - " << CODER
		<< "\n\n" << "  " << PROGRAM
		<< ".exe [-file] [-FOLDER] [-log] [-copy] [-help]" << "\n\n"
		<< "  * [-file yourfile.txt] (default) " << FILE << "\n"
		<< "  * [-folder SyncPath] (default) " << FOLDER << " FOLDER"
		<< std::endl;
}

void DirSync::init() const {
	mkdir(FOLDER.c_str(), 0777);
	if(opendir(FOLDER.c_str()) == NULL) {
		std::cerr << FILE  << " does not exists" << std::endl;
		exit(1);
	}
	if(opendir(FOLDER.c_str()) == NULL) {
		std::cerr << FOLDER << " is not valid" << std::endl;
		exit(1);
	}
	if(list.size() == 0) {
		std::cerr << FILE << " is Empty" << std::endl;
		exit(1);
	}
}

int DirSync::getChecksum(std::ifstream &ifs) {
	std::streamsize const buffer_size = 1024;
	boost::crc_32_type result;
	do {
		char buffer[buffer_size];
		ifs.read(buffer, buffer_size);
		result.process_bytes(buffer, ifs.gcount());
	} while (ifs);
	return result.checksum();
}

bool DirSync::compareCRC(std::string &src, std::string &dest) {
	try {
		std::ifstream ifs_src(src.c_str(), std::ios_base::binary);
		if(!ifs_src) {
			std::cerr << "Failed to open " << src << std::endl;
			return false;
		}
		std::ifstream ifs_dest(dest.c_str(), std::ios_base::binary);
		if(!ifs_dest) {
			std::cerr << "Failed to open " << dest << std::endl;
			return false;
		}
		bool result = getChecksum(ifs_src) == getChecksum(ifs_dest);
		if(result == false) {
			std::cerr << "CRC Wrong! " << src << " " << dest << std::endl;
			exit(1);
		}
		std::cout << "CRC OK: " << dest << std::endl;
		return true;
	} catch (std::exception &e) {
		std::cerr << "Found an exception with " << e.what() << std::endl;
	} catch (...) {
		std::cerr << "Found an unknown exception" << std::endl;
	}
	return false;
}

void DirSync::readFile() {
	FstrPtr file(new std::ifstream(FILE.c_str(), std::ios::in));
	std::string t;
	while ((*file).good()) {
		std::getline((*file), t);
		boost::trim(t);
		if(t.size() == 0)
			continue;
		list.push_back(t);
	}
}

void DirSync::copyFile(std::string const &from, std::string const &to) const {
	std::ifstream f1(from.c_str(), std::fstream::binary);
	std::ofstream f2(to.c_str(), std::fstream::trunc | std::fstream::binary);
	f2 << f1.rdbuf();
}

void DirSync::sync(std::string const &from, std::string const &to) {
	DIR *dirp;
	dirent *entry;
	if ((dirp = opendir(from.c_str())) != NULL) {
		while ((entry = readdir(dirp)) != NULL) {
			std::string filename(entry->d_name);
			if (filename == "." || filename == "..")
				continue;
			std::string from_temp = from + "/" + filename;
			std::string to_temp = to + "/" + filename;
			if (opendir(from_temp.c_str()) != NULL) {
				mkdir(to_temp.c_str(), 0777);
				sync(from_temp, to_temp);
				continue;
			}
			copyFile(from_temp, to_temp);
			compareCRC(from_temp, to_temp);
		}
		closedir(dirp);
	} else {
		std::cout << from << " is not a directory" << std::endl;
		exit(1);
	}
}

int main() {
	DirSync ds;
}
