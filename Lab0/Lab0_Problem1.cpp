/*
Author: Joshua Miller
Class: ECE 6122 Q
Last Date Modified: 20250830

Description:

Solve problem 1 of Lab 0:
Write a C++ program using the insertion stream operator and 
escape sequences that outputs the following text to your terminal screen when executed: 
My name is: your first and last name separated by a space 
This (“) is a double quote.  
This (‘) is a single quote.  
This (\) is a backslash. 
This (/) is a forward slash. 
This program is very simple with no user input, command arguments, or file output.  You can 
place all the code in your main() function in a file called Lab0_Problem1.cpp.
*/

#include <iostream>

/**
 * @brief Entry point for problem 1
 *
 * Prints my name and several special character escape sequences.
 * 
 * @return int Exit status code (0 for success)
 */
int main()
{
    std::cout << "My name is: Joshua Miller\n";
    std::cout << "This (\") is a double quote.\n";
    std::cout << "This (\') is a single quote.\n";
    std::cout << "This (\\) is a backslash.\n";
    std::cout << "This (/) is a forward slash.\n";
}