#include "BatObfuscate.h"
#include "Settings.h"
#include <Windows.h>
#include "encrypt.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <random>

namespace BatObfuscate {
	std::string obfuscateBatchFile(const std::string& batchfile, int pass) {
		std::string obfuscatedContent;

		for (int i = 0; i < pass; ++i) {
			std::string script;

			if (i == 0) {
				std::ifstream file(batchfile);
				std::stringstream buffer;
				buffer << file.rdbuf();
				script = buffer.str();
			}
			else {
				script = obfuscatedContent;
			}

			obfuscatedContent = "";

			std::string charactersToReplace = std::string(x("@ 0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"));
			std::string replacementCharacters = std::string(x("_ÄÅÇÉÑÖÜáàâäãåçéèêëíìîïñóòôöõúùûüabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"));
			std::string lowercaseCharacters = std::string(x("abcdefghijklmnopqrstuvwxyz"));
			std::string additionalCharacters = std::string(x("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_¯-ஐ→あⓛⓞⓥⓔ｡°º¤εïз╬㊗⑪⑫⑬㊀㊁㊂のð"));

			std::string variablePrefix = replacementCharacters.substr(0, std::mt19937{ std::random_device{}() }() % (5 - 3 + 1) + 3); // Value of the variable for the pass
			std::string replacementString;

			std::vector<std::pair<char, std::string>> replacementTable;
			std::string shuffledCharacters = charactersToReplace;
			std::shuffle(shuffledCharacters.begin(), shuffledCharacters.end(), std::mt19937{ std::random_device{}() });

			for (size_t pos = 0; pos < shuffledCharacters.size(); ++pos) {
				replacementTable.push_back(std::make_pair(shuffledCharacters[pos], "%" + variablePrefix + ":~" + std::to_string(pos) + ",1%"));
				replacementString += shuffledCharacters[pos];
			}

			bool waitForVariableEnd = false;
			bool waitForLabelEnd = false;
			bool isLineBreak = true;
			if (i == pass - 1) obfuscatedContent += "\xFF\xFE" "&@cls&";
			obfuscatedContent += "@set \"" + variablePrefix + "=" + replacementString + "\"\n";

			for (char& originalChar : script) {
				if (isLineBreak && originalChar == ':') {
					waitForLabelEnd = true;
				}

				if (originalChar == '\n') {
					isLineBreak = true;
					waitForVariableEnd = false;
					waitForLabelEnd = false;
				}
				else {
					isLineBreak = false;
				}

				if (originalChar == ' ') {
					waitForLabelEnd = false;
				}

				if (!waitForVariableEnd && (originalChar == '%' || originalChar == '!')) {
					waitForVariableEnd = true;
				}
				else if (waitForVariableEnd && (originalChar == '%' || originalChar == '!')) {
					waitForVariableEnd = false;
					waitForLabelEnd = false;
				}

				if (!waitForVariableEnd && !waitForLabelEnd && !isLineBreak) {
					bool converted = false;
					for (const auto& pair : replacementTable) {
						if (originalChar == pair.first) {
							if (std::mt19937{ std::random_device{}() }() % 20 + 1 == 8) {
								obfuscatedContent += pair.second + "%" + replacementCharacters.substr(3, 7) + "%";
							}
							else {
								obfuscatedContent += pair.second;
							}
							converted = true;
							break;
						}
					}

					if (!converted) {
						obfuscatedContent += originalChar;
					}
				}
				else {
					obfuscatedContent += originalChar;
				}
			}
		}

		return obfuscatedContent;
	}

	std::string generateRandomString(int length) {
		static const char alphanum[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);

		std::string randomString;
		randomString.reserve(length);
		for (int i = 0; i < length; ++i) {
			randomString += alphanum[dis(gen)];
		}
		return randomString;
	}

	std::string generateRandomFileType() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 1);

		if (dis(gen) == 0) {
			return std::string(x(".cmd"));
		}
		else {
			return std::string(x(".bat"));
		}
	}

	void createObfuscatedBatchFile(const std::string& batchfile, const std::string& outputfile, int pass) {
		std::string obfuscatedContent = obfuscateBatchFile(batchfile, pass);
		std::ofstream outputFile(outputfile);
		if (outputFile.is_open()) {
			outputFile << obfuscatedContent;
			outputFile.close();
			Settings::status = x("Obfuscated batch file!");
			Beep(1000, 100);
		}
		else {
			Settings::status = x("Failed to obfuscate batch file!");
		}
	}
}