#include "TextTableBuilder.h"

#include "StringUtil.h"
#include "MiscUtil.h"
#include "Exception.h"

TextTableBuilder::TextTableBuilder()
{
	writeRowIndex = -1;
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

std::string TextTableBuilder::buildBorder() const
{
	std::string buffer;

	for(int columnWidth : columnWidths)
	{
		buffer += '+' + StringUtil::repeat("-", columnWidth + 2);
	}

	buffer += "+\n";

	return buffer;
}

std::string TextTableBuilder::buildRow(int rowNumber) const
{
	std::string buffer;

	for(std::string::size_type columnIndex = 0;columnIndex < tableWidth;++columnIndex)
	{
		buffer += "| " + table[rowNumber][columnIndex] + StringUtil::repeat(" ", (columnWidths[columnIndex] - cellWidths[rowNumber][columnIndex]) + 1);
	}

	buffer += "|\n";

	return buffer;
}

std::string TextTableBuilder::build() const
{
	if(tableWidth == 0)
		return std::string("");

	std::string buffer;

	buffer.append(buildBorder());
	buffer.append(buildRow(0));
	buffer.append(buildBorder());

	for(int tableRow = 1;tableRow < (int)table.size();++tableRow)
	{
		buffer.append(buildRow(tableRow));
	}

	if(table.size() > 0)
		buffer.append(buildBorder());

	return buffer;
}