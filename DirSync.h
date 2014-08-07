
#ifndef DIRSYNC_H_
#define DIRSYNC_H_

#include <string>
#include <tr1/memory>
#include <vector>
#include <boost/crc.hpp>

class DirSync {
	typedef std::tr1::shared_ptr<std::istream> FstrPtr;
	static std::string CODER;
	static std::string VERSION;
	static std::string PROGRAM;
	static std::string FILE;
	static std::string FOLDER;
	std::vector<std::string> list;

	void copyFile(std::string const&, std::string const&) const;
	void readFile();
	bool compareCRC(std::string&, std::string&);
	int getChecksum(std::ifstream&);
	void init() const;
	void showHelp() const;
public:
	DirSync();
	void sync(std::string const&, std::string const&);
};

#endif /* DIRSYNC_H_ */
