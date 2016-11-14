#include "crossvalidation.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

namespace illumestimators {

CrossValidation::CrossValidation(int size, Method method, int k) :
	m_method(method),
	m_k(k)
{
	if (m_method == HOLDOUT) {
		m_k = 2;
	} else if (m_method == LEAVEONEOUT) {
		m_k = size;
	}

	const double inc = (double) m_k / size;
	m_indices.resize(size);

	for (int i = 0; i < size; i++) {
		m_indices[i] = ceil((i + 0.9) * inc) - 1;
	}

	srand(time(NULL));

	randomize();
}

CrossValidation::~CrossValidation()
{

}

void CrossValidation::repartition()
{
	randomize();
}

std::vector<CrossValidation::Set> CrossValidation::sets()
{
	std::vector<Set> setList;

	if (m_method == HOLDOUT) {
		std::vector<int> training;
		std::vector<int> testing;

		for (unsigned int i = 0; i < m_indices.size(); i++) {
			if (m_indices[i] == 1) {
				training.push_back(i);
			} else {
				testing.push_back(i);
			}
		}

		setList.push_back(Set(training, testing));
	} else if ((m_method == KFOLD) || (m_method == LEAVEONEOUT)) {
		for (int k = 0; k < m_k; k++) {
			std::vector<int> training;
			std::vector<int> testing;

			for (unsigned int i = 0; i < m_indices.size(); i++) {
				if (m_indices[i] == k) {
					testing.push_back(i);
				} else {
					training.push_back(i);
				}
			}

			setList.push_back(Set(training, testing));
		}
	}

	return setList;
}

void CrossValidation::randomize()
{
	for (unsigned int i = 0; i < m_indices.size(); i++) {
		int n = rand() % m_indices.size();

		int temp = m_indices[i];
		m_indices[i] = m_indices[n];
		m_indices[n] = temp;
	}
}

} // namespace illumestimators
