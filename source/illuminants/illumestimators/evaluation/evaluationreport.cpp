#include "evaluationreport.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace illumestimators {

EvaluationReport::EvaluationReport()
{
	clear();
}

EvaluationReport::~EvaluationReport()
{

}

void EvaluationReport::clear()
{
	m_data.clear();
	m_rowIndex.clear();
	m_colIndex.clear();
	m_rowName.clear();
	m_colName.clear();
	m_rowHeader.clear();
	m_colHeader.clear();

	m_rows = 0;
	m_cols = 0;
}

void EvaluationReport::set(int row, int col, const std::string& string)
{
	addRow(row);
	addColumn(col);

	m_data[std::pair<int, int>(row, col)] = string;
}

void EvaluationReport::set(int row, int col, double value)
{
	std::stringstream ss;
	ss << value;

	set(row, col, ss.str());
}

void EvaluationReport::set(int row, int col, const Illum& value)
{
	std::stringstream ss;
	ss << value.r << " " << value.g << " " << value.b;

	set(row, col, ss.str());
}

void EvaluationReport::set(const std::string& row, const std::string& col, const std::string& string)
{
	if (m_rowIndex.find(row) == m_rowIndex.end()) {
		addRow(row);
	}

	if (m_colIndex.find(col) == m_colIndex.end()) {
		addColumn(col);
	}

	set(m_rowIndex[row], m_colIndex[col], string);
}

void EvaluationReport::set(const std::string& row, const std::string& col, double value)
{
	std::stringstream ss;
	ss << value;

	set(row, col, ss.str());
}

void EvaluationReport::set(const std::string& row, const std::string& col, const Illum& value)
{
	std::stringstream ss;
	ss << value.r << " " << value.g << " " << value.b;

	set(row, col, ss.str());
}

std::string EvaluationReport::get(int row, int col)
{
	return m_data[std::pair<int, int>(row, col)];
}

void EvaluationReport::setRowName(int row, const std::string& name)
{
	addRow(row);

	m_rowIndex[name] = row;
	m_rowName[row] = name;
}

void EvaluationReport::setColumnName(int col, const std::string& name)
{
	addColumn(col);

	m_colIndex[name] = col;
	m_colName[col] = name;
}

void EvaluationReport::addRow(int row)
{
	m_rows = std::max(row + 1, m_rows);
}

void EvaluationReport::addColumn(int col)
{
	m_cols = std::max(col + 1, m_cols);
}

void EvaluationReport::addRow(const std::string& name)
{
	if (m_rowIndex.find(name) == m_rowIndex.end()) {
		setRowName(m_rows, name);
	}
}

void EvaluationReport::addColumn(const std::string& name)
{
	if (m_colIndex.find(name) == m_colIndex.end()) {
		setColumnName(m_cols, name);
	}
}

void EvaluationReport::setRowHeader(int row, const std::string& header)
{
	addRow(row);

	m_rowHeader[row] = header;
}

void EvaluationReport::setColumnHeader(int col, const std::string& header)
{
	addColumn(col);

	m_colHeader[col] = header;
}

void EvaluationReport::setRowHeader(const std::string& name, const std::string& header)
{
	addRow(name);

	setRowHeader(m_rowIndex[name], header);
}

void EvaluationReport::setColumnHeader(const std::string& name, const std::string& header)
{
	addColumn(name);

	setColumnHeader(m_colIndex[name], header);
}

std::string EvaluationReport::getRowHeader(int row)
{
	if (m_rowHeader.find(row) != m_rowHeader.end()) {
		return m_rowHeader[row];
	} else {
		std::stringstream ss;
		ss << row + 1;
		return ss.str();
	}
}

std::string EvaluationReport::getColumnHeader(int col)
{
	if (m_colHeader.find(col) != m_colHeader.end()) {
		return m_colHeader[col];
	} else {
		std::stringstream ss;
		ss << col + 1;
		return ss.str();
	}
}

void EvaluationReport::save(const std::string& filename)
{
	std::ofstream of(filename.c_str());

	if (of.good()) {
		for (unsigned int col = 0; col < (unsigned int)cols(); col++) {
			of << "," << getColumnHeader(col);
		}

		of << std::endl;

		for (unsigned int row = 0; row < (unsigned int)rows(); row++) {
			of << getRowHeader(row);

			for (unsigned int col = 0; col < (unsigned int)cols(); col++) {
				of << "," << get(row, col);
			}

			of << std::endl;
		}
	}

	of.close();
}

} // namespace illumestimators
