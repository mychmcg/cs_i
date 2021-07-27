# Assignment 3 - File I/O

## Character Equipment
Write a program that will accept as input, from a (save) file, information about your game character. Your program should then calculate your character’s effective stats (base stats plus equipment bonuses) and output the information to the screen. You can format the information to the console in any way you see fit (be creative!). It should make sense to the user who their character is, what equipment they have equipped, what their effective stats are, and how they were affected by their equipment.

When your program runs, it should prompt for a file name. The program should then read the character data from the specified file, compute the results, then output to the console.

The input file will be formatted as follows:
 - Line 1: Character’s full name (all type string)
 - Line 2: The character’s health, stamina, strength, and magic (all type integer)
 - Line 3: The character’s weapon magic bonus and name (type integer and string)
 - Line 4: The character’s head armor strength bonus and name (type string and integer)
 - Line 5: The character’s body armor health bonus and name (type string and integer)
 - Line 6: The character’s leg armor stamina bonus and name (type string and integer)

## Sample Input file:

```
Aggokk the Brutal
70 50 90 10
0 Hammer of Smashing
10 Orcish Helm
30 Orcish Cuirass
20 Orcish Greaves
```
