#include "TextTableBuilder.h"

#include "StringUtil.h"
#include "MiscUtil.h"
#include "Exception.h"
#include "utils.h"

TextTableBuilder::TextTableBuilder()
{
	writeRowIndex = -1;
	tableWidth = 0;
	this->mysqlFormat = false;
}

TextTableBuilder::TextTableBuilder(bool mysqlFormat)
{
	writeRowIndex = -1;
	tableWidth = 0;
	this->mysqlFormat = mysqlFormat;
}

bool TextTableBuilder::doneWritingHeader() const
{
	return writeRowIndex > 0;
}

TextTableBuilder *TextTableBuilder::startRow()
{
	if(doneWritingHeader() && table[writeRowIndex].size() != table[0].size())
	{
		std::stringstream errorBuffer;

		errorBuffer << "TextTableBuilder::startRow : Attempting to start row when previous row(length " << table[writeRowIndex].size()
					<< ") does not match header length(" << table[0].size() << ").";
		throw Exception(errorBuffer.str());
	}
	++writeRowIndex;
	table.push_back(std::vector<std::string>());
	cellWidths.push_back(std::vector<int>());
	
	return this;
}

TextTableBuilder *TextTableBuilder::put(const std::string &value)
{
	int cellWidth = StringUtil::strlenIgnoreColors(value.c_str());
	int columnIndex = (int)table[writeRowIndex].size();

	if(!doneWritingHeader())
	{
		table[writeRowIndex].push_back(value);
		columnWidths.push_back(cellWidth);
		++tableWidth;
	}
	else if((int)table[writeRowIndex].size() > tableWidth)
	{
		throw Exception(std::string("TextTableBuilder : Attempting to write column past table width ") + MiscUtil::toString(tableWidth) + std::string("."));
	}
	else
	{
		table[writeRowIndex].push_back(value);
		columnWidths[columnIndex] = columnWidths[columnIndex] > cellWidth ? columnWidths[columnIndex] : cellWidth;
	}
	
	cellWidths[writeRowIndex].push_back(cellWidth);

	return this;
}

TextTableBuilder *TextTableBuilder::put(const char *value)
{
	return put(std::string(value));
}

TextTableBuilder *TextTableBuilder::put(const int value)
{
	return put(MiscUtil::toString(value));
}

TextTableBuilder *TextTableBuilder::put(const short value)
{
	return put(MiscUtil::toString(value));
}

TextTableBuilder *TextTableBuilder::put(const char value)
{
	return put(MiscUtil::toString(value));
}

TextTableBuilder *TextTableBuilder::append(const std::string &value)
{
	int cellWidth = cellWidths[writeRowIndex].back() + StringUtil::strlenIgnoreColors(value.c_str());
	int columnIndex = (int)table[writeRowIndex].size() - 1;

	cellWidths[writeRowIndex][columnIndex] = cellWidth;
	table[writeRowIndex][columnIndex].append(value);

	return this;
}

TextTableBuilder *TextTableBuilder::append(const char *value)
{
	return append(std::string(value));
}

TextTableBuilder *TextTableBuilder::append(const int value)
{
	return append(MiscUtil::toString(value));
}

TextTableBuilder *TextTableBuilder::append(const short value)
{
	return append(MiscUtil::toString(value));
}

TextTableBuilder *TextTableBuilder::append(const char value)
{
	return append(MiscUtil::toString(value));
}

std::string TextTableBuilder::buildBorder() const
{
	std::string buffer;

	for(int columnWidth : columnWidths)
	{
		buffer.append(mysqlFormat ? "+" : "-");
		buffer += StringUtil::repeat("-", columnWidth + 2);
	}

	buffer.append(mysqlFormat ? "+" : "-");
	buffer.append("\n");

	return buffer;
}

std::string TextTableBuilder::buildRow(int rowNumber) const
{
	if(this->table[rowNumber].size() != this->tableWidth)
	{
		std::stringstream errorBuffer;

		errorBuffer << "TextTableBuilder : Row #" << rowNumber << " has invalid width of " << this->table[rowNumber].size() << " while table width is " << tableWidth;
		throw Exception(errorBuffer.str());
	}
	std::string buffer;

	for(std::string::size_type columnIndex = 0;columnIndex < tableWidth;++columnIndex)
	{
		buffer += std::string(mysqlFormat ? "|" : " ") + " " + table[rowNumber][columnIndex] + StringUtil::repeat(" ", (columnWidths[columnIndex] - cellWidths[rowNumber][columnIndex]) + 1);
	}

	buffer.append(mysqlFormat ? "|" : " ");
	buffer.append("\n");

	return buffer;
}

std::string TextTableBuilder::build() const
{
	if(tableWidth == 0)
		return std::string("");

	std::string buffer;

	if(mysqlFormat)
		buffer.append(buildBorder());

	buffer.append(buildRow(0));
	buffer.append(buildBorder());

	for(int tableRow = 1;tableRow < (int)table.size();++tableRow)
	{
		buffer.append(buildRow(tableRow));
	}

	if(table.size() > 0 && mysqlFormat)
		buffer.append(buildBorder());

	return buffer;
}