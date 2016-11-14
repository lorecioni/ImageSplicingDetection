#ifndef ILLUMESTIMATORS_EVALUATION_EVALUATIONREPORT_H
#define ILLUMESTIMATORS_EVALUATION_EVALUATIONREPORT_H

#include <map>
#include <string>
#include "illum.h"

namespace illumestimators {

class EvaluationReport
{
public:
	EvaluationReport();
	~EvaluationReport();

public:
	void set(int row, int col, const std::string& string);
	void set(int row, int col, double value);
	void set(int row, int col, const Illum& value);
	void set(const std::string& row, const std::string& col, const std::string& string);
	void set(const std::string& row, const std::string& col, double value);
	void set(const std::string& row, const std::string& col, const Illum& value);
	std::string get(int row, int col);

public:
	void setRowName(int row, const std::string& name);
	void setColumnName(int col, const std::string& name);
	void addRow(int row);
	void addColumn(int col);
	void addRow(const std::string& name);
	void addColumn(const std::string& name);

public:
	void setRowHeader(int row, const std::string& header);
	void setColumnHeader(int col, const std::string& header);
	void setRowHeader(const std::string& name, const std::string& header);
	void setColumnHeader(const std::string& name, const std::string& header);
	std::string getRowHeader(int row);
	std::string getColumnHeader(int col);

public:
	void clear();
	void save(const std::string& filename);

public:
	int rows() const { return m_rows; }
	int cols() const { return m_cols; }

private:
	int m_rows;
	int m_cols;
	std::map<std::pair<int, int>, std::string> m_data;
	std::map<std::string, int> m_rowIndex;
	std::map<std::string, int> m_colIndex;
	std::map<int, std::string> m_rowName;
	std::map<int, std::string> m_colName;
	std::map<int, std::string> m_rowHeader;
	std::map<int, std::string> m_colHeader;
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_EVALUATION_EVALUATIONREPORT_H
