#include <fstream>
#include <vector>
#include <string>
#include <clocale>
#ifdef _WIN32
#	include <Windows.h>
#endif
#include "GameField.h"
#include "GUI.h"


dictionary_type dictionary;

bool createCrossword();

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "rus");
#ifdef _WIN32
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	// Load the dictionary
	std::ifstream dictionaryFile;
	dictionaryFile.open("dict.txt");
	if (!dictionaryFile.is_open())
	{
		GUI::throwError("Ошибка открытия словаря");
		return 1;
	}

	std::string word;

	while (dictionaryFile >> word)
	{
		word.erase(remove(word.begin(), word.end(), '\n'), word.end());

		while (dictionary.size() <= word.length())
		{
			dictionary.push_back(std::vector<std::string>());
		}

		dictionary.at(word.length()).push_back(word);
	}
	dictionaryFile.close();

	GUI::showWindow(createCrossword);
	return 0;
}

bool createCrossword()
{
	unsigned width = GUI::getMapWidth(), \
		height = GUI::getMapHeight();
	
	// Create word template for the future crossword
	GameField wordMap(width, height, &dictionary);
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			wordMap.at(x, y) = GUI::getMapMaskAt(x, y);
		}
	}

	if (!wordMap.isValid())
	{
		GUI::throwError(("Шаблон кроссворда некорректен" \
			" или превышена максимальная длина слова (" \
			+ std::to_string(dictionary.size() - 1) + ")").c_str());
		return false;
	}

	bool result = wordMap.startPlacement(dictionary, 1);
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			GUI::setLetterAt(x, y, wordMap.getResultAt(x, y));
		}
	}

	if (!result)
	{
		GUI::throwError("Не удалось построить кроссворд полностью!" \
			" Будет представлен наиболее полный вариант!");
	}
	return true;
}