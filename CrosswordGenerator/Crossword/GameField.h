#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

typedef std::vector<std::vector<std::string>> dictionary_type;

class GameField
{
public:
	GameField(unsigned width, unsigned height, \
		const dictionary_type* dictionary);
	bool isValid() const;
	// method = 0; 1
	bool startPlacement(dictionary_type& dictionary, int method);
	char& at(unsigned x, unsigned y) \
		{ return wordMap.at(y).at(x); }
	char getResultAt(unsigned x, unsigned y) const \
		{ return tempField.at(y).at(x); }

private:
	enum { horizontal, vertical };
	struct wordPlace
	{
		unsigned leftX, topY, direction, connectivity, length;
	};

	unsigned width, height;
	int bestDepth;
	std::vector<std::vector<char>> wordMap, tempField, savedMap;
	std::vector<wordPlace> wordPlacesList;
	std::vector<std::string> usedWordsList;
	const dictionary_type* dictionary_ptr;

	void obtainWordsList();
	void sortByIntersections();
	bool doWordsIntersect(const wordPlace& w1, const wordPlace& w2);
	bool placeWords(unsigned n = 0);
	bool isWordAlreadyUsed(const std::string& word) const;
};