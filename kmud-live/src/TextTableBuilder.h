#ifndef TEXT_TABLE_BUILDER
#define TEXT_TABLE_BUILDER

#include <string>
#include <vector>

class TextTableBuilder
{
private:
protected:
	std::vector<std::vector<std::string>> table;
	std::vector<int> columnWidths;
	std::vector<std::vector<int>> cellWidths;
	int writeRowIndex;
	int tableWidth;

	bool doneWritingHeader() const;
	std::string buildBorder() const;
	std::string buildRow(int rowNumber) const;
public:

	TextTableBuilder();

	TextTableBuilder *startRow();

	TextTableBuilder *put(const std::string &value);
	TextTableBuilder *put(const char *value);
	TextTableBuilder *put(const int value);
	TextTableBuilder *put(const short value);
	TextTableBuilder *put(const char value);

	std::string build() const;

};

#endif