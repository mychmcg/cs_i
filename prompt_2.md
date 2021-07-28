# Assignemnt 2

Modify the following program to calculate the area of a triangle. The user should be prompted to input the required parameters and the output should be displayed to the console. 

```
#include <iostream>

using namespace std;


int main() {
	double length;
	double width;
	double area, perimeter;

	cout << "This program will calculate the perimeter and “
	     << “area of a rectangle" << endl;

	length = 5;
	width = 7;

	perimeter = 2 * (length + width);
	area = length * width;

	cout << "Length: " << length << endl;
	cout << "Width: " << width << endl;
	cout << "Preimeter: " << perimeter << endl;
	cout << "Area: " << area << endl;

	return 0;
}
```

