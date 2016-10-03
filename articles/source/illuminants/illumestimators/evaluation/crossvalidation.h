#ifndef ILLUMESTIMATORS_EVALUATION_CROSSVALIDATION_H
#define ILLUMESTIMATORS_EVALUATION_CROSSVALIDATION_H

#include <vector>

namespace illumestimators {

class CrossValidation
{
public:
	enum Method { HOLDOUT, KFOLD, LEAVEONEOUT };

public:
	class Set {
		public:
			Set() {}
			Set(const std::vector<int>& training, const std::vector<int>& testing) : m_training(training), m_testing(testing) {}

		public:
			std::vector<int> training() { return m_training; }
			std::vector<int> testing() { return m_testing; }

		private:
			std::vector<int> m_training;
			std::vector<int> m_testing;
	};

public:
	CrossValidation(int size, Method method, int k = 3);
	~CrossValidation();

public:
	void repartition();
	std::vector<Set> sets();

private:
	void randomize();

private:
	Method m_method;
	int m_k;
	std::vector<int> m_indices;
};

} // namespace illumestimators

#endif // ILLUMESTIMATORS_EVALUATION_CROSSVALIDATION_H
