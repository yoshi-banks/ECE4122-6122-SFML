/**
 * Author: Joshua Miller
 * Class: ECE6122 (Q)
 * Last Date Modified: 2025-10-07
 * 
 * @brief: Main function to start the Application Suzanne program
 */

#include <iostream>

#include "Application.hpp"

/**
 * @brief: 
 * 
 * @return int 0 if successful not 0 if not
 */
int main()
{
    Application app = Application();

    app.run();

    return 0;
}