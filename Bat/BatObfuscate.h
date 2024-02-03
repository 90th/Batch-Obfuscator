#pragma once
#ifndef BATOBFUSCATE_H
#define BATOBFUSCATE_H

#include <string>

namespace BatObfuscate {
	std::string obfuscateBatchFile(const std::string& batchfile, int pass = 1);
	void createObfuscatedBatchFile(const std::string& batchfile, const std::string& outputfile, int pass = 1);
	std::string generateRandomString(int length);
	std::string generateRandomFileType();
}

#endif // BATOBFUSCATE_H
