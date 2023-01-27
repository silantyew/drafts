#include "GUI.h"

namespace GUI
{
	void closeWindow(Fl_Widget*, void* window);
	void drawField(Fl_Widget*);
	void clearField(Fl_Widget*);
	void createCrossword(Fl_Widget* self);
	void exportAsSVG(Fl_Widget*);
	void cellClicked(Fl_Widget* cell);
	void allowEditing(Fl_Widget* self);
	void drawDefaultMap();

	std::vector<std::vector<Fl_Button*>> cellsButtons;

	Fl_Window* window;
	Fl_Simple_Counter *widthCounter, *heightCounter;
	Fl_Light_Button* exitButton;
	Fl_Button *createButton, *clearButton, *exportButton, \
		*drawButton, *editButton;

	bool (*createCrosswordMethod_)();

	bool hasBeenFilled = false;
};

void stringReplace(std::string& src, const std::string& sub, \
	const std::string& replace)
{
	int index;
	while ((index = src.find(sub)) != -1)
	{
		src.replace(index, sub.size(), replace);
	}
}

int GUI::showWindow(bool (*createCrosswordMethod)())
{
	createCrosswordMethod_ = createCrosswordMethod;

	window = new Fl_Window(1200, 720, "Кроссворд");
	window->color(FL_LIGHT3);
	//window->fullscreen();

	exitButton = \
		new Fl_Light_Button(10, 10, 70, 25, "Выйти");
	exitButton->callback(closeWindow, window);

	widthCounter = \
		new Fl_Simple_Counter(140, 10, 100, 25, "Ширина поля");
	widthCounter->step(1);
	widthCounter->value(15);
	widthCounter->range(5, 30);

	heightCounter = \
		new Fl_Simple_Counter(250, 10, 100, 25, "Высота поля");
	heightCounter->step(1);
	heightCounter->value(15);
	heightCounter->range(5, 30);

	drawButton = \
		new Fl_Button(360, 10, 130, 25, "Нарисовать поле");
	drawButton->callback(drawField);

	clearButton = \
		new Fl_Button(500, 10, 120, 25, "Очистить поле");
	clearButton->deactivate();
	clearButton->callback(clearField);

	createButton = \
		new Fl_Button(670, 10, 150, 25, "Создать кроссворд");
	createButton->deactivate();
	createButton->callback(createCrossword);

	editButton = \
		new Fl_Button(830, 10, 150, 25, "Редактировать");
	editButton->deactivate();
	editButton->callback(allowEditing);

	exportButton = \
		new Fl_Button(1030, 10, 150, 25, "Сохранить как SVG");
	exportButton->deactivate();
	exportButton->callback(exportAsSVG);

	drawDefaultMap();

	window->end();
	window->show();
	return Fl::run();
}

void GUI::closeWindow(Fl_Widget*, void* window)
{
	if (1 == fl_choice("Вы уверены, что хотите выйти?", \
		"Нет", "Да", 0))
	{
		static_cast<Fl_Window*>(window)->hide();
	}
}

void GUI::drawField(Fl_Widget*)
{
	// Delete previous buttons
	for (auto& row : cellsButtons)
	{
		for (auto& cell : row)
		{
			delete cell;
		}

		row.clear();
	}
	cellsButtons.clear();

	const unsigned fieldLeft = 10, fieldTop = 80;
	unsigned cellSize;
	if (heightCounter->value() >= widthCounter->value())
	{
		cellSize = (static_cast<double>(window->h()) - 100) \
			/ heightCounter->value();
	}
	else
	{
		cellSize = (static_cast<double>(window->w()) - 50) \
			/ widthCounter->value();
	}

	cellsButtons.resize(heightCounter->value());
	for (auto& row : cellsButtons)
	{
		row.resize(widthCounter->value());
	}

	window->begin();
	for (unsigned y = 0; y < heightCounter->value(); ++y)
	{
		for (unsigned x = 0; x < widthCounter->value(); ++x)
		{
			Fl_Button* cell;
			cellsButtons.at(y).at(x) = cell = \
				new Fl_Button(fieldLeft + x * cellSize, \
					fieldTop + y * cellSize, \
					cellSize, cellSize);

			cell->type(FL_TOGGLE_BUTTON);
			cell->color(FL_WHITE);
			cell->callback(cellClicked);
		}
	}

	createButton->activate();
	clearButton->activate();
	window->end();
	window->redraw();
}

void GUI::createCrossword(Fl_Widget* self)
{
	/*std::thread placementThread([&]()
	{
		self->deactivate();
		bool result = createCrosswordMethod_();
		self->activate();
		if (result)
		{
			self->deactivate();
			editButton->activate();
			exportButton->activate();
			hasBeenFilled = true;
		}
	});

	placementThread.detach();*/
	
	if (createCrosswordMethod_())
	{
		self->deactivate();
		editButton->activate();
		exportButton->activate();
		hasBeenFilled = true;
	}
}

void GUI::throwError(const char* message)
{
	Fl::error(message);
}

unsigned GUI::getMapWidth()
{
	return cellsButtons.at(0).size();
}

unsigned GUI::getMapHeight()
{
	return cellsButtons.size();
}

char GUI::getMapMaskAt(unsigned x, unsigned y)
{
	const char* label = cellsButtons.at(y).at(x)->label();

	return cellsButtons.at(y).at(x)->value() \
		|| label && *label;
}

void GUI::setLetterAt(unsigned x, unsigned y, char letter)
{
	window->begin();
	char text[4] = { letter, '\0'};
	fl_utf8from_mb(text, 4, text, strlen(text));
	cellsButtons.at(y).at(x)->value(0);
	cellsButtons.at(y).at(x)->copy_label(text);
	window->end();
	window->show();
}

void GUI::clearField(Fl_Widget*)
{
	for (auto& row : cellsButtons)
	{
		for (auto& cell : row)
		{
			cell->copy_label("");
			cell->value(0);
		}
	}

	editButton->deactivate();
	exportButton->deactivate();
	createButton->activate();
	hasBeenFilled = false;
}

void GUI::exportAsSVG(Fl_Widget*)
{
	std::string content = "";

	for (const auto& row : cellsButtons)
	{
		content += "<tr>";
		for (const auto& cell : row)
		{
			content += "<td>";
			if (cell->label())
			{
				content += cell->label();
			}
			else
			{
				content += " ";
			}
			content += "</td>";
		}
		content += "</tr>";
	}

	std::string svgMarkup = "", buf;
	std::ifstream templateFile("template.svg");
	while (std::getline(templateFile, buf))
	{
		svgMarkup += buf;
	}
	templateFile.close();
	stringReplace(svgMarkup, "[CONTENT]", content);
	unsigned cellSize = 40;
	stringReplace(svgMarkup, "[CELL_SIZE]", \
		std::to_string(cellSize));
	stringReplace(svgMarkup, "[WIDTH]", \
		std::to_string(cellSize * getMapWidth() + 20));
	stringReplace(svgMarkup, "[HEIGHT]", \
		std::to_string(cellSize * getMapHeight() + 20));
	char* outputFilename = fl_file_chooser("Сохранить как", \
		"*.svg", "crossword.svg");
	if (outputFilename)
	{
		std::ofstream outputFile(outputFilename);
		outputFile << svgMarkup;
		outputFile.close();
	}
}

void GUI::cellClicked(Fl_Widget* cell)
{
	if (hasBeenFilled)
	{
		if (1 == fl_choice("Перейти в режим редактирования ? ", \
			"Нет", "Да", 0))
		{
			hasBeenFilled = false;
			allowEditing(editButton);
			exportButton->deactivate();
			static_cast<Fl_Button*>(cell)->value(\
				!static_cast<Fl_Button*>(cell)->value());
		}
		else
		{
			static_cast<Fl_Button*>(cell)->value(0);
		}
	}
}

void GUI::allowEditing(Fl_Widget* self)
{
	self = static_cast<Fl_Button*>(self);

	for (auto& row : cellsButtons)
	{
		for (auto& cell : row)
		{
			cell->value(cell->label() && *(cell->label()));
			cell->copy_label("");
		}
	}

	hasBeenFilled = false;
	self->deactivate();
	createButton->activate();
}

void GUI::drawDefaultMap()
{
	widthCounter->value(defaultMap.at(0).size());
	heightCounter->value(defaultMap.size());
	drawField(drawButton);

	for (unsigned y = 0; y < cellsButtons.size(); ++y)
	{
		for (unsigned x = 0; x < cellsButtons.at(0).size(); ++x)
		{
			cellsButtons.at(y).at(x)->value(\
				defaultMap.at(y).at(x));
		}
	}
}