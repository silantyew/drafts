#include "GameField.h"

template<typename T>
void shuffleVector(std::vector<T>& vector)
{
	static bool hasBeenExecuted = false; // run code once
	if (!hasBeenExecuted)
	{
		srand(static_cast<unsigned>(time(NULL)));
		hasBeenExecuted = true;
	};

	if (vector.size())
	{
		auto randomOfArray = [&vector]() -> unsigned
		{
			return rand() % vector.size();
		};

		for (unsigned cx = 0; cx < randomOfArray(); ++cx)
		{
			unsigned i = randomOfArray(), j = randomOfArray();

			T buf = vector.at(i);
			vector.at(i) = vector.at(j);
			vector.at(j) = buf;
		}
	}
}

GameField::GameField(unsigned width, unsigned height, \
	const dictionary_type* dictionary) : \
	width(width), height(height), dictionary_ptr(dictionary), \
	bestDepth(-1)
{
	wordMap.resize(height);
	tempField.resize(height);
	savedMap.resize(height);
	for (unsigned x = 0; x < height; ++x)
	{
		wordMap[x].resize(width);
		savedMap.resize(width);
		tempField[x].resize(width);
		for (auto& i : tempField[x])
		{
			i = '\0';
		}
	}
}

bool GameField::isValid() const
{
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			// Check that 2 word are not beside
			if (x < width - 1 && y < height - 1 \
				&& wordMap.at(y).at(x) && wordMap.at(y + 1).at(x) \
				&& wordMap.at(y).at(x + 1) && wordMap.at(y + 1).at(x + 1))
			{
				return false;
			}

			// Check it is not a separate letter
			unsigned neighborhood = \
				static_cast<unsigned>(x && wordMap.at(y).at(x -	1)) \
				+ static_cast<unsigned>(x < width - 1 && wordMap.at(y).at(x + 1)) \
				+ static_cast<unsigned>(y && wordMap.at(y - 1).at(x))
				+ static_cast<unsigned>(y < height - 1 && wordMap.at(y + 1).at(x));

			if (wordMap.at(y).at(x) && !neighborhood)
			{
				return false;
			}
		}
	}

	// Length of a word should be <= maximal length in dictionary
	for (int dir = 0; dir <= 1; ++dir)
	{
		unsigned length1, length2;
		if (dir == vertical)
		{
			length1 = width;
			length2 = height;
		}
		else if (dir == horizontal)
		{
			length1 = height;
			length2 = width;
		}

		for (unsigned a = 0; a < length1; ++a)
		{
			unsigned wordLength = 0;
			unsigned newWordCoord1 = a, newWordCoord2 = 0;

			for (unsigned b = newWordCoord2; b < length2; ++b)
			{
				unsigned& i = dir == vertical ? b : a;
				unsigned& j = dir == vertical ? a : b;

				if (wordMap.at(i).at(j))
				{
					++wordLength;
				}

				if (dir == horizontal && j && !wordMap.at(i).at(j) \
					|| dir == vertical && i && !wordMap.at(i).at(j) \
					|| b == length2 - 1)
				{
					if (wordLength >= dictionary_ptr->size())
					{
						return false;
					}

					wordLength = 0;
				}
			}
		}

	}

	return true;
}

void GameField::obtainWordsList()
{
	for (int dir = 0; dir <= 1; ++dir)
	{
		unsigned length1, length2;
		if (dir == vertical)
		{
			length1 = width;
			length2 = height;
		}
		else if(dir == horizontal)
		{
			length1 = height;
			length2 = width;
		}

		for (unsigned a = 0; a < length1; ++a)
		{
			unsigned newWordLength = 0, newWordConnectivity = 0;
			unsigned newWordCoord1 = a, newWordCoord2 = 0;

			for (unsigned b = newWordCoord2; b < length2; ++b)
			{
				unsigned& i = dir == vertical ? b : a;
				unsigned& j = dir == vertical ? a : b;

				if (wordMap.at(i).at(j))
				{
					if (!newWordLength)
					{
						newWordCoord2 = b;
					}

					++newWordLength;

					if (dir == horizontal && (static_cast<int>(j) - 1 >= 0 && wordMap.at(i).at(j - 1) || j + 1 < length2 && wordMap.at(i).at(j + 1)) \
						|| dir == vertical && (static_cast<int>(i) - 1 >= 0 && wordMap.at(i - 1).at(j) || i + 1 < length2 && wordMap.at(i + 1).at(j)))
					{
						++newWordConnectivity;
					}
				}

				if (dir == horizontal && j && !wordMap.at(i).at(j) \
					|| dir == vertical && i && !wordMap.at(i).at(j) \
					|| b == length2 - 1)
				{
					if (newWordLength >= 2)
					{
						// new word is recognized
						wordPlace newWord;
						newWord.direction = dir;
						newWord.connectivity = newWordConnectivity;
						newWord.length = newWordLength;

						if (dir == vertical)
						{
							newWord.leftX = newWordCoord1;
							newWord.topY = newWordCoord2;
						}
						else if (dir == horizontal)
						{
							newWord.leftX = newWordCoord2;
							newWord.topY = newWordCoord1;
						}

						wordPlacesList.push_back(newWord);
					}

					newWordLength = 0;
					newWordConnectivity = 0;
				}
			}
		}

	}
}

bool GameField::startPlacement(dictionary_type& dictionary, int method)
{
	obtainWordsList();
	
	// sort words list by connectivity
	std::sort(wordPlacesList.begin(), wordPlacesList.end(), \
		[](const wordPlace& el1, const wordPlace& el2) { return el1.connectivity > el2.connectivity; });

	if (1 == method)
	{
		sortByIntersections();
	}

	// shuffle dictionary to get new crossword each time
	for (auto& arr : dictionary)
	{
		//shuffleVector<std::string>(arr);
	}

	// search of suitable words
	return placeWords();
}

bool GameField::placeWords(unsigned n)
{
	if (n < wordPlacesList.size())
	{
		for (const auto& i : dictionary_ptr->at(wordPlacesList.at(n).length))
		{
			unsigned x = wordPlacesList.at(n).leftX,
				y = wordPlacesList.at(n).topY;

			unsigned& axis = wordPlacesList.at(n).direction == vertical ? y : x;

			std::string initialWordPlaceContent = "";

			if (isWordAlreadyUsed(i))
			{
				goto _continue;
			}

			for(unsigned t = 0; t < wordPlacesList.at(n).length; ++t, ++axis)
			{
				initialWordPlaceContent += tempField.at(y).at(x);
				if (!tempField.at(y).at(x) || tempField.at(y).at(x) == i.at(t))
				{
					tempField.at(y).at(x) = i.at(t);
				}
				else
				{
					goto _recover_and_continue;
				}
			}


			usedWordsList.push_back(i);
			if (placeWords(n + 1))
			{
				if (static_cast<int>(n) > bestDepth)
				{
					bestDepth = n;
					savedMap = tempField;
				}
				return true;
			}
			usedWordsList.pop_back();

			// recover map as before placing the word
			_recover_and_continue:
			x = wordPlacesList.at(n).leftX, y = wordPlacesList.at(n).topY;
			for (unsigned t = 0; t < initialWordPlaceContent.length(); ++t, ++axis)
			{
				tempField.at(y).at(x) = initialWordPlaceContent.at(t);
			}

		_continue:;
		}

		return false;
	}
	else
	{
		return true;
	}
}

bool GameField::isWordAlreadyUsed(const std::string& word) const
{
	for (const auto& i : usedWordsList)
	{
		if (word == i)
		{
			return true;
		}
	}

	return false;
}

/*void GameField::sortByIntersections()
{
	for (unsigned compared = 0, curr = 1; \
		compared < wordPlacesList.size() - 1;)
	{
		bool swapped = false;
		for (unsigned i = curr + 1; \
			i < wordPlacesList.size(); \
			++i)
		{
			if (doWordsIntersect(wordPlacesList.at(curr), \
				wordPlacesList.at(i)))
			{
				wordPlace buf = wordPlacesList.at(i);
				wordPlacesList.at(i) = wordPlacesList.at(curr);
				wordPlacesList.at(curr) = buf;
				swapped = true;
				++curr;
				break;
			}
		}
		compared += swapped ? 1 : -1;
	}
}*/

void GameField::sortByIntersections()
{
	for (unsigned compared = 0, curr = 1; \
		compared < wordPlacesList.size() - 1;)
	{
		unsigned max = 0; // will never be 0s

		for (unsigned i = curr + 1; \
			i < wordPlacesList.size(); \
			++i)
		{
			if (doWordsIntersect(wordPlacesList.at(curr), \
				wordPlacesList.at(i)) \
				&& (!max || wordPlacesList.at(i).connectivity \
			> wordPlacesList.at(max).connectivity))
			{
				max = i;
			}
		}

		if (max)
		{
			wordPlace buf = wordPlacesList.at(max);
			wordPlacesList.at(max) = wordPlacesList.at(curr);
			wordPlacesList.at(curr) = buf;
			++curr;
		}

		compared += max ? 1 : -1;
	}
}



bool GameField::doWordsIntersect(const wordPlace& w1, const wordPlace& w2)
{
	if (w1.direction == w2.direction)
	{
		return false;
	}

	for (unsigned delta1 = 0; delta1 < w1.length; ++delta1)
	{
		for (unsigned delta2 = 0; delta2 < w2.length; ++delta2)
		{
			unsigned x1 = w1.leftX, y1 = w1.topY, \
				x2 = w2.leftX, y2 = w2.topY;

			if (w1.direction == horizontal)
			{
				x1 += delta1;
			}
			else if (w1.direction == vertical)
			{
				y1 += delta1;
			}

			if (w2.direction == horizontal)
			{
				x2 += delta2;
			}
			else if (w2.direction == vertical)
			{
				y2 += delta2;
			}

			if (x1 == x2 && y1 == y2)
			{
				return true;
			}
		}
	}

	return false;
}