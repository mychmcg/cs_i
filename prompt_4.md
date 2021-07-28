# Assignment 4 - Control Structures

## Game Menu Part 1

This assignment will build on top of Assignment 3. There are two main objectives:

1. Add error handling to opening of the input file 

- This assumes that in the last assignment you were successful in asking the user for the file name and opening the file with the path specified (if you didn’t implement that part, you’ll have to get that working as well). We now want to add error checking to make sure the file successfully opens. If the file exists and can be read, the program should continue on as normal. If the file does not exist, or there is an error opening the file, the program should instead notify the user that the specified file does not exist or cannot be opened and then exit (**you are not required to let the user keep trying new file names**). The exact wording and formatting are left up to you, but make sure to get those main points across to the user.

2. Add the ability for the user to choose the characters class

- After the file is read, let the user choose the character’s class by entering the corresponding number from a list of options (**You can assume for now that user will enter a valid choice**). The chosen class should then be saved and **added to the rest of the character’s displayed stats from the last assignment**. Again, the formatting is left up to you, but make sure it meets the requirements defined above. Below is an example of what the class option **could** look like. Note: bold is input, and the character information being output is coming from the save file.

```
Enter the filename to load your character: mycharacter.txt
Please choose a class for Aggokk the Brutal 
Warrior
Mage
Archer
Berserker
Selection (select a number): 4

…
…
```
 
***The output of the stats and class are omitted here but are still required***

