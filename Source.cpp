#include <iostream>
#include <thread>
#include <future>
#include <random>
#include <vector>
#include <cmath>
#include "Timer.hpp"

std::mutex mtx;

void MonteCarlo(std::size_t N, std::size_t& count)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);

	std::size_t a = 0;
	for (auto i = 0; i < N; ++i)
		if (std::pow(dis(gen), 2) + std::pow(dis(gen), 2) <= 1)
		{
			a++;
		}
	std::lock_guard lg(mtx);
	count += a;
}

double SingleMonteCarlo(std::size_t N)
{
	std::size_t count = 0;
	MonteCarlo(N, std::ref(count));
	return 4.0 * count / N;
}

double MultyMonteCarlo(std::size_t N)
{
	if (std::thread::hardware_concurrency() <= 1)
	{
		std::cout << "Need more threads" << std::endl;
		return -1.0;
	}

	std::size_t Nthreads = std::thread::hardware_concurrency();

	std::vector <std::thread> threads;

	std::size_t CountN = N / Nthreads;
	std::size_t count = 0;

	for (auto i = 0U; i < (Nthreads - 1); i++)
		threads.push_back(std::thread(MonteCarlo, CountN, std::ref(count)));

	MonteCarlo(CountN, std::ref(count));

	for (auto i = 0U; i < std::size(threads); i++)
		threads[i].join();

	return 4.0 * count / N;
}

int main()
{
	{
		Timer single("Single Thread");

		std::cout << SingleMonteCarlo(31415926) << std::endl;
	}

	{
		Timer multy("Multy Thread");

		std::cout << MultyMonteCarlo(31415926) << std::endl;
	}

	return 0;
}