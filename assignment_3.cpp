/* ==============================================================================
   File:        assignment_3.cpp
   Date:        Sat Feb 20 11:59:05 2021
   Course:      Computer Science I
   Project:     Assignment 3 - File Input
   Description: Manipulate data inputted from file, in this case game char stats
   =========================================================================== */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using std::string;
using std::to_string;
using std::ifstream;
using std::cin;
using std::cout;
using std::endl;
using std::setw;
using std::setfill;

// Prompts the user for an input file name
// Return: the name of the input file
string GetFileNameFromUser()
{
  string fileName;
  cout << "Enter an input file name: ";
  cin >> fileName;
  return fileName;
}

const string statNames[] =
{
  "Magic",
  "Strength",
  "Health",
  "Stamina",
};

const string equipmentSlots[] =
{
  "Weapon",
  "Head",
  "Torso",
  "Leg",
};

// These constants are used to keep us from hardcoding array/loop sizes
const int numStats = sizeof(statNames)/sizeof(statNames[0]);
const int numSlots = sizeof(equipmentSlots)/sizeof(equipmentSlots[0]);
const int displayWidth = 54;
const int paddingPlusBorder = 2;
const int contentWidth = displayWidth - paddingPlusBorder*2;

struct gear
{
  string name;
  int bonus;
};

struct character
{
  string name;
  int baseStats[numStats];
  int stats[numStats];
  gear equipped[numSlots];

  // Return the highest stat of the player, after bonuses
  int getMaxStat() {
    int max = 0;
    for (int i = 0; i < numStats; i++) {
      max = stats[i] > max ? stats[i] : max;
    }
    return max;
  }

  // Return the string length of the largest bonus value
  int getMaxBonusLength() {
    long unsigned int max = 0;
    for (int i = 0; i < numSlots; i++) {
      max = to_string(equipped[i].bonus).length() > max
        ? to_string(equipped[i].bonus).length()
        : max;
    }
    return max;

  }
};

// Takes in a character then displays its info in a nice way.
void display(character pl)
{

  // Escape sequences to control console output colors.
  string noFg = "\e[39m";
  string black = "\e[30m";
  string gray = "\e[37m";
  string green = "\e[32m";
  string red = "\e[31m";

  string noBg = "\e[49m";
  string bgColor[] =
  {
    "\e[44m",
    "\e[41m",
    "\e[42m",
    "\e[43m"
  };

  string reset = "\e[0m";
  string bold = "\e[1m";

  string bp = "| "; // border then padding, used at start of line
  string pb = " |"; // padding then border, used at   end of line

  // Formatting: print top border.
  cout << "+" << setw(displayWidth-1) << setfill('-') << "+" << setfill(' ') << endl;

  // Print center aligned character name.
  int nameEndIndex = contentWidth/2 + pl.name.length()/2;
  cout << bp
       << bold
       << setw(nameEndIndex)
       << pl.name << reset
       << setw(displayWidth - nameEndIndex - paddingPlusBorder)
       << pb
       << endl;

  // Print a blank line within the character display.
  cout << bp << setw(contentWidth+paddingPlusBorder) << pb << endl;

  // Print character stats with bars to indicate progress.
  for (int i = 0; i < numStats; i++)
  {
    // Line 1: stat name, equipment name & equipment bonus
    string bonusText, sign, bonusTextColor;
    int spaceBetweenGearAndBonus = 3;
    // -- Set the sign of the stat bonus
    sign = pl.equipped[i].bonus >= 0 ? "+" : "";
    // -- Set the color of the bonus based on whether it is positive or negative
    if (pl.equipped[i].bonus < 0) {
      bonusTextColor = red;
    } else if (pl.equipped[i].bonus > 0) {
      bonusTextColor = green;
    } else {
      bonusTextColor = noFg;
    }
    bonusText = sign + to_string(pl.equipped[i].bonus);
    cout << bp
         // Print stat name
         << statNames[i]
         // Allocate whitespace for gear name
         << setw(contentWidth
                 - statNames[i].length()
                 - (spaceBetweenGearAndBonus + pl.getMaxBonusLength()))
         // Print gear name
         << pl.equipped[i].name
         // Allocate whitespace for stat bonus value and set color
         << bonusTextColor
         << setw(spaceBetweenGearAndBonus + pl.getMaxBonusLength())
         // Print bonus value
         << bonusText
         << noFg
         << pb
         << endl;

    // Line 2: stat value & visual bar representation
    //
    // -- Set our highest stat bar to fill the content width
    // -- and avoid dividing by zero
    int barLength = pl.getMaxStat() > 0
      ? contentWidth/static_cast<double>(pl.getMaxStat())*pl.stats[i]
      : contentWidth/100.0*pl.stats[i];
    // -- Prevent us from printing a string longer than our bar
    // -- we would get formatting errors otherwise
    barLength = barLength < static_cast<int>(to_string(pl.stats[i]).length())
      ? to_string(pl.stats[i]).length()
      : barLength;

    cout << bp
         // Set bar color and print it
         << bgColor[i] << black << std::left << setw(barLength)
         // Print stat value within bar
         << pl.stats[i] << std::right << noBg << noFg
         // Print whitespace until display border
         << setw(contentWidth
                 -barLength
                 +paddingPlusBorder)
         << pb << endl;

    // If the last stat has not been displayed
    // Print a blank line within the character display
    if(i < numStats - 1) {
      cout << bp << setw(contentWidth+paddingPlusBorder) << pb << endl;
    }
  }

  // Formatting: print bottom border
  cout << "+" << setw(displayWidth-1) << setfill('-') << "+" << endl;

  // Formatting: ending blank line.
  cout << endl;
}

int main(int argc, char *argv[])
{
  // Store input file name from args or user prompt if no args given
  string fileName = ((argc > 1) ? argv[1] : GetFileNameFromUser());

  // Open file for reading, exits if the file does not exist
  ifstream inputFile;
  inputFile.open(fileName);
  if (!inputFile)
  {
    cout << "Cannot open the input file." << endl;
    return 1;
  }

  // Get character info
  character player;
  // -- Get character name, truncate if it overflows the content
  getline(inputFile, player.name);
  if (player.name.size() > contentWidth) {player.name.resize(contentWidth);}
  // -- Get character base stats
  // -- Assumes we get base stats in the same stat order as the gear bonuses
  for (int i = 0; i < numStats; i++)
  {
        inputFile >> player.baseStats[i];
  }
  // -- Get character gear and calculate bonuses
  for (int i = 0; i < numSlots; i++)
  {
    inputFile >> player.equipped[i].bonus;
    player.stats[i] = player.baseStats[i] + player.equipped[i].bonus;
    getline(inputFile, player.equipped[i].name);
  }

  // Display char info in a nice way
  display(player);

  // Remember to close your file!
  inputFile.close();

  return 0;
}
