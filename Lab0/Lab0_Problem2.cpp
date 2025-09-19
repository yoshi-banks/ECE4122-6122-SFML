/*
Author: Joshua Miller
Class: ECE 6122 Q
Last Date Modified: 20250823

Description:

Solve lab 0 problem 2:

The prime 41, can be written as the sum of six consecutive primes: 
41 = 2 + 3 + 5 + 7 + 11 + 13 
This is the longest sum of consecutive primes that adds to a prime below 100. 
The longest sum of consecutive primes below 1000 that adds to a prime, contains 21 terms, and is 
equal to 953. 
Then writes a console program that continuously takes in a natural number ( < 232) from the console 
and outputs to the console the value of the longest sum of all the prime numbers whose sum is less 
than or equal to the entered number and displays the elements in the sum.   
Entering a 0 ends the program. 
Use a separate function to determine if a number is prime. 
Make sure code checks for valid input values. Entries must be positive and be made up of numeric 
characters (0, 1, 2, 3, 4, 5, 6, 7, 8, 9).  A sample sequence is shown below and your output should 
match the formatting shown in the sample.
*/

#include <iostream>
#include <limits>
#include <cmath>
#include <vector>

const int MAX_SEGMENT_SIZE = 500 * 1024 * 1024; // max size in MiB

/**
 * @brief Overload the << operator for vectors for utility when outputing vectors
 */
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) 
{
    for (const T& elem : vec)
    {
        os << elem << " ";
    }
    return os;
}

/**
 * @brief Handle user input by repeatadly asking for a number until receiving a natural number
 *        Will prompt the user to keep entering numbers until a correct number is received.
 * 
 * @return double The number the user enters
 */
double getUserInput()
{
    double number;

    // Ask user for a natural number. Repeat if incorrect.
    while (true) 
    {
        std::cout << "Please enter a natural number (0 to quit): ";
        std::cin >> number;

        // make sure the input is a number and that number is less than >= 0
        if (std::cin.fail() || number < 0)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Error! Invalid input!" << std::endl;
        }
        else
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
    }

    return number;
}

/**
 * @brief check if a number is prime
 *
 * @return true if prime, false if not
 */
bool isPrime(const double& num)
{
    if (num <= 1)
    {
        return false;
    }
    if (num == 2)
    {
        return true;
    }
    if (std::fmod(num, 2.0) == 0)
    {
        return false;
    }

    // keep iterating until reaching the sqrt(num) checking if the mod(num, i) == 0
    for (int i = 3; i * i <= num; i += 2) 
    {
        if (std::fmod(num, i) == 0)
        {
            return false;
        }
    }

    return true;
}

/**
 * @brief Generate a vector of all primes less than limit
 * 
 * @param limit the upper bound for prime numbers to generate
 * 
 * @return std::vector<double> Vector of doubles of all primes less than limit
 */
std::vector<double> generatePrimesSegmented(const double& limit)
{
    // Handle the case if limit is less than 2
    if (limit < 2)
    {
        return {};
    }

    std::vector<double> allPrimes;

    // only need to check all the numbers up to the sqrt(limit)
    // Proof:
    // let compositie number n only have two factors a and b such that n = a * b
    // If a and b were greater than sqrt(n) then a * b > n which is a contradiction
    // Therefore either a or b must be less than sqrt(n)
    // So in our algorithm we only check numbers up to the sqrt(n) and mark their 
    // multiples for the larger numbers 

    // sqrtLimit is the sqrt of limit which is how far to check primes up to, see above
    double sqrtLimit = static_cast<double>(std::sqrt(limit)) + 1;
    // allocate memory to store primes
    std::vector<double> smallPrimes;

    // Initial sieve algorithm
    // Based on Sieve of Eratosthenes
    // make sure the sqrtLimit is greater than 2
    if (sqrtLimit >= 2)
    {
        // create a vector up to and including the sqrtLimit
        std::vector<bool> sieve(sqrtLimit + 1, true);
        // mark 0 and 1 as composites
        sieve[0] = sieve[1] = false;

        // iterate through all the vector marking all multiples of iterator i
        for (int i = 2; i * i <= sqrtLimit; ++i) 
        {
            // only check if sieve[i] is still marked as prime, otherwise skip
            if (sieve[i])
            {
                // if sieve[i] is a prime then mark all multiples (2i, 3i, etc) 
                // in the vector as not prime
                for (int j = i * i; j <= sqrtLimit; j += i)
                {
                    sieve[j] = false;
                }
            }
        }

        // save numbers still marked as prime to smallPrimes vector
        for (int i = 2; i <= sqrtLimit; ++i)
        {
            if (sieve[i]) 
            {
                smallPrimes.push_back(i);
            }
        }
    }

    // Use the segmented sieve method to get the rest of the primes
    // The segmented method helps by prevent stack overflows for large limit numbers
    double segmentStart = 2;
    while (segmentStart <= limit)
    {
        double segmentEnd = std::min(segmentStart + MAX_SEGMENT_SIZE - 1, limit);
        double segmentSize = static_cast<double>(segmentEnd - segmentStart + 1);

        std::vector<bool> segment(segmentSize, true);
        
        for (const double& prime : smallPrimes)
        {
            // only check up to the sqrt, or in this case prime^2
            if (prime * prime > segmentEnd)
            {
                break;
            }

            // Find the first multiple, starting after prime^2
            double startMultiple = std::max(prime * prime, ((segmentStart + prime - 1) / prime) * prime);

            // Mark the multiples in the segment
            for (double multiple = startMultiple; multiple <= segmentEnd; multiple += prime)
            {
                segment[multiple - segmentStart] = false;
            }
        }

        // Put primes together
        for (int i = 0; i < segmentSize; ++i)
        {
            if (segment[i] && segmentStart + 1 >= 2)
            {
                allPrimes.push_back(segmentStart + i);
            }
        }

        // Iterate to the next segment
        segmentStart = segmentEnd + 1;
    }

    return allPrimes;
}

/**
 * @brief Find a consecutive Prime sum using the supplied primes vector 
 *
 * @return std::pair<double, std::vector<double>> a pair of doulbe and vector of doubles
 * The double is the summed prime from consecutive vector of primes
 * The vector of doubles is the collection of consecutive primes
 */
std::pair<double, std::vector<double>> findConsecutivePrimeSum(const std::vector<double>& primes, const double& limit)
{
    // runningSum and runningSequence track the current sum and sequence
    double runningSum = 0;
    std::vector<double> runningSequence;
    // largestSum and sequence track the best sum and sequence
    double largestPrimeSum = 0;
    std::vector<double> largestSequence;

    // iterate through the prime list
    for (int i = 0; i < primes.size(); ++i)
    {
        double prime = primes[i];

        // if adding the prime doesn't push sum over the limit add it to the sequence
        if (runningSum + prime <= limit)
        {
            runningSum += prime;
            runningSequence.push_back(prime);

            // if the new sequence size is larger than the largestSequence size make 
            // runningSequence largestSequence
            if (runningSequence.size() > largestSequence.size())
            {
                largestPrimeSum = runningSum;
                largestSequence = runningSequence;
            }
        }
        // if adding the prime was too large then break out the loop
        else 
        {
            break;
        }
    }

    std::pair<double, std::vector<double>> longestSum;
    longestSum.first = largestPrimeSum;
    longestSum.second = largestSequence;

    return longestSum;
}

/**
 * @brief Solve the Consecutive Prime Sum problem utilizing the segmented sieve function
 * 
 * @return a pair(double, vector<double>) that represents the sum and primes of that 
 * sum for the largest consecutive prime sum
 */
std::pair<double, std::vector<double>> findLongestConsecutivePrimeSum(const double& limit)
{
    if (limit < 2)
    {
        return {0, {}};
    }

    // Get an orderd list, from smallest to greatest, of all primes smaller than or equal to limit
    std::vector<double> allPrimes = generatePrimesSegmented(limit);

    // pair to hold the best consecutive prime sum pair
    std::pair<double, std::vector<double>> bestPair({0, {}});

    // start at each number less than the limit and compute the consecutive prime sum for each starting point i
    // keep the best consecutive prime sum and return
    for (int i = 0; i <= allPrimes.size(); ++i)
    {
        std::vector<double> subVector(allPrimes.begin() + i, allPrimes.end());
        std::pair<double, std::vector<double>> subPair = findConsecutivePrimeSum(subVector, limit);
        if (subPair.second.size() > bestPair.second.size())
        {
            bestPair = subPair;
        }
    }

    // TODO remove checks
    // added checks just in case
    if (!isPrime(bestPair.first))
    {
        std::cout << "WARNING!!! summed value is not prime!!!" << std::endl;
    }

    return bestPair;
}

/**
 * @brief Entry point for problem 2
 * 
 * Solve the Consecutive Prime Sum problem. 
 *
 * Ask user for number.
 *
 * Handle user input
 * 
 * Find the sum of all primes that is prime and less than than the inputted number.
 *
 * @return int Exit status code (0 for success) 
 */
int main()
{
    double number;

    while (true)
    {
        number = getUserInput();

        if (number == 0)
        {
            std::cout << "Program terminated.\nHave a nice day!" << std::endl;
            return 0;
        }

        // get all primes less than number
        std::pair<double, std::vector<double>> p = findLongestConsecutivePrimeSum(number);

        std::cout << "The answer is " << p.first << " with " << p.second.size() << " terms: ";
        for (int i = 0; i < p.second.size(); ++i)
        {
            std::cout << p.second.at(i);
            if (i != p.second.size() - 1)
            {
                std::cout << " + ";
            }
        }
        std::cout << std::endl;
    }
}