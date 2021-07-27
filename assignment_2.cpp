/* ==============================================================================
   File:        assignment2.cpp
   Date:        Mon Feb  8 01:33:31 2021
   Course:      Computer Science I
   Project:     Assignment 2 - Triangle Area Calculator
   Description: Displays the area of a triangle after prompting the user
                for it's dimensions
   =========================================================================== */

#include <iostream>

using namespace std;


int main() {
    double base, height, area;

    cout << "This program will calculate the area of a triangle" << endl << endl;

    cout << "Enter width of base: ";
    cin >> base;

    cout << "Enter height: ";
    cin >> height;

    area = base * height / 2;

    cout << "Area: " << area << endl
         << "Height: " << height << endl
         << "Base: " << base << endl;

    return 0;
}
