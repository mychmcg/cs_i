/* ==============================================================================
   File:        assignment_4.cpp
   Date:        Sat Feb 27 2021
   Course:      Computer Science I
   Project:     Assignment 4 - Control Structures I
   Description: Alter the flow of the program based on user input
   =========================================================================== */

/*
 * Psuedocode:
 *
 * Get file name from user  <-----------+
 * - Let  fileName = userInput          |
 * - Open fileName                      |
 * - If   fileName is not open, loop ---+
 *
 * [...]
 *
 * List classes in output
 * Get class choice from user
 * let classChoice = userInput
 *
 * [...]
 *
 * Display player class in prettified output.
 *
 */

#include <ios>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <iterator>
#include <ostream>
#include <string>
#include <regex> // increased my compilation by 2 seconds
#include <vector>
#include <list>

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::left;
using std::right;
using std::setfill;
using std::setw;
using std::string;
using std::to_string;

class character;
class gear;

namespace gameState {
  std::list<character> characters;
  std::list<gear> loot;
}

const string statNames[] =
  { "Magic"
  , "Strength"
  , "Health"
  , "Stamina"
  };

const string equipmentSlots[] =
  { "Weapon"
  , "Head"
  , "Torso"
  , "Leg"
  };

const string classNames[] =
  { "Berserker"
  , "Engineer"
  , "Monk"
  , "Magi"
  , "Primalist"
  };

// These constants are used to keep us from hardcoding array/loop sizes
const int numStats = sizeof(statNames)/sizeof(statNames[0]);
const int numSlots = sizeof(equipmentSlots)/sizeof(equipmentSlots[0]);
const int numClassNames = sizeof(classNames)/sizeof(classNames[0]);

namespace colorCode {
  // Escape sequences to control console output colors.
  // -- Text style
  const string reset = "\e[0m";
  const string bold  = "\e[1m";
  const string uline = "\e[4m";
  // -- Foreground(text) colors
  const string noFg   = "\e[39m";
  const string black  = "\e[30m";
  const string blue   = "\e[34m";
  const string gray   = "\e[37m";
  const string green  = "\e[32m";
  const string purple = "\e[35m";
  const string red    = "\e[31m";
  const string yellow = "\e[33m";
  // -- Background colors
  const string noBg     = "\e[49m";
  const string blueBg   = "\e[44m";
  const string redBg    = "\e[41m";
  const string greenBg  = "\e[42m";
  const string yellowBg = "\e[43m";
  // -- Array allows us to loop through bg-colors, one bar color for each stat.
  const string bgColor[numStats] =
    { blueBg
    , redBg
    , greenBg
    , yellowBg
    };
}
using namespace colorCode;

// Utility function to truncate a string. shamelessly stolen from some stackoverflow
// I made it better though...
// Return: truncated version of the string with optional elipsis
string truncate(string str, size_t width, bool show_ellipsis=true)
{
  if (str.length() > width) {
    if (show_ellipsis) {
      return str.substr(0, width-3) + "...";
    }
    else {
      return str.substr(0, width);
    }
  }
  return str;
}

namespace display
{
  enum MessageType
    { LOADING = 0
    , PROMPT
    , SUCCESS
    , ERROR
    , MENU
    };

  void printMessage(MessageType messageType, string message) {
    switch (messageType) {
      case LOADING:
        cout << yellow << ":: " << noFg << message;
        break;
      case PROMPT:
        cout << green << "=> " << noFg
             << message << endl
             // no space after arrow so we can control input color
             << green << "=>" << noFg;
        break;
      case SUCCESS:
        cout << green << ":: " << noFg << message;
        break;
      case ERROR:
        cout << bold << red << ">< " << message
             << reset << noFg;
        break;
      case MENU:
        cout << blue << ":: "
             << noFg + bold << message << reset;
        break;
    }
    if (messageType != PROMPT) {
      cout << endl;
    }
  }

  enum TextAlign
    { LEFT = 0
    , CENTER
    , RIGHT
    };

  // Print hotizontal line
  void printHorizontalLine(string borderStart, char lineFill,
                                  string borderEnd, int width) {
    cout << borderStart
         << setw(width-borderStart.length()) << setfill(lineFill) << borderEnd
         << setfill(' ') << endl;
  }

  // Print a horizontal line filled with/by whitespace
  void printBlankLine(string borderStart, string borderEnd, int width) {
    printHorizontalLine(borderStart, ' ', borderEnd, width);
  }

  // Print text aligned left, right or center
  void printAlignedText(string borderStart, TextAlign textAlign,
                        std::vector<string> textStyles, string text,
                        string borderEnd, int width) {

    cout << borderStart;

    // Apply all text styles
    for (long i=0; i < textStyles.size(); i++) {
      cout << textStyles[i];
    }

    int contentWidth = (width-borderStart.length()-borderEnd.length());

    // Print left aligned text
    if (textAlign == LEFT) {
      cout << left << setw(contentWidth)
           << text << right << reset; // print text and then remove styles
    }

    // TODO(): cant use the underline style with center/right alignment
    // _________ <-- as all of the blank spacing gets underlined
    // If textAlign is set to center, center the text with setw
    if (textAlign == CENTER) {
      // Display-index/x-coordinate of the last char of the text
      int textEndIndex = contentWidth/2 + text.length()/2;
      cout << setw(textEndIndex)
           << text << reset // print text and then remove styles
           << setw(width - textEndIndex - borderEnd.length());
    }

    if (textAlign == RIGHT) {
      cout << setw(contentWidth)
           << text << reset; // print text and then remove styles
    }

    cout << borderEnd << endl;
  }

  // Prompts the user for an input file name
  // Return: the name of the input file
  string getFileNameFromUser()
  {
    string fileName;
    printMessage(PROMPT, "Enter an input file name:");
    cout << yellow << " ";
    cin >> fileName;
    cout << noFg;
    return fileName;
  }

  // If passed file name is valid        <--------+
  // -- Return file stream with that file opened  |
  // If passed file name is invalid               |
  // -- promt user for the name of a valid file --+
  // Return: a valid file stream
  ifstream getValidFileStream(string passedFileName = "")
  {
    // Open file for reading, loops if the file is invalid
    ifstream inputFile;
    std::regex validFileName("^\\w+$");
    string fileName = passedFileName != "" ? passedFileName : getFileNameFromUser();
    inputFile.open(fileName);
    while (!inputFile.is_open() || !std::regex_match(fileName, validFileName))
    {
      printMessage(ERROR,
                   "Cannot open the input file. Try another file name: (Crtl+C to exit.)");
      cout << endl;
      fileName = getFileNameFromUser();
      inputFile.open(fileName);
    }
    return inputFile;
  }

  // Print menu with numbered items
  void printMenu(string menuTitle, int listLength, const string listItems[]) {
      cout << endl;
      printMessage(MENU, menuTitle);
      for (int i=0; i < listLength; i++) {
        cout << purple << " " << i+1 << noFg << " " << listItems[i] << endl;
      }
      cout << endl;
  }

  // Print menu and prompt for an integer selection
  // Return: integer corresponding to the user's selection
  int getSelectionFromMenu(string menuTitle, int numSelections,
                              const string selections[], string prompt) {
    int selection = 0;
    string strInput;

    while ( !(selection > 0 && selection <= numSelections) )
    {
      printMenu(menuTitle, numSelections, selections);
      printMessage(PROMPT, prompt + " by number: (1-" + to_string(numSelections) + ")");
      cout << purple << " ";
      cin >> strInput;
      cout << noFg;
      try {
        selection = std::stoi(strInput);
      } catch (...) {
        // If user doesn't enter a valid int, prompt & continue to loop
        printMessage(ERROR, "Invalid selection, try again.");
        continue;
      }
      // TODO(): is there a way to not do this comparison/duplicate code?
      if ( !(selection > 0 && selection <= numSelections) ) {
        printMessage(ERROR, "Invalid selection, try again.");
      }
    }

    return selection;
  }
};

class gear
{
  string name;
  int bonus;
  int equipmentSlot;
  character *equippedBy;

  public:
    gear(string nameArg="", int bonusArg=0, int equipmentSlotArg=0, character* characterToEquip=nullptr) {
      name = nameArg;
      bonus = bonusArg;
      equipmentSlot = equipmentSlotArg;
      equippedBy = characterToEquip;
    }

    string getName() {
      return name;
    }

    void setName(string nameArg) {
      name = nameArg;
    }

    int getBonus() {
      return bonus;
    }

    void setBonus(int bonusArg) {
      bonus = bonusArg;
    }

    int getEquipmentSlot() {
      return equipmentSlot;
    }

    void setEquipmentSlot(int slotArg) {
      equipmentSlot = slotArg;
    }

    void unEquip() {
      equippedBy = nullptr;
    }

    void equipTo(character* characterToEquip);

};

class character
{
  string name;
  int baseStats[numStats];
  int stats[numStats];
  public:gear *equipped[numSlots];
  string className;

  public:character() {
    name = "";
    for (int i = 0; i < numStats; i++) {
      baseStats[i] = 0;
      stats[i] = 0;
    }
    for (int i = 0; i < numSlots; i++) {
      equipped[i] = nullptr;
    }
    className = "Knave";
  }

  public:void setName(string nameArg) {
    name = nameArg;
  }

  public:string getName() {
    return name;
  }

  public:void setBaseStat(int stat, int statValue) {
    baseStats[stat] = statValue;
  }

  public:int getBaseStat(int stat) {
    return baseStats[stat];
  }

  public:void setStat(int stat, int statValue) {
    stats[stat] = statValue;
  }

  public:int getStat(int stat) {
    return stats[stat];
  }

  public:gear* getEquipped(int slot){
    return equipped[slot];
  }

  public:void equipGear(int slot, gear* gearArg) {
    if (equipped[slot] == gearArg) {
      display::printMessage(display::ERROR, uline + name + reset + " already has "
                            + uline + equipped[slot]->getName() + reset + "equipped." );
    } else {
      gearArg->equipTo(this);
    }
  }

  public:void unEquipGear(int slot) {
    if (equipped[slot] == nullptr) {
      display::printMessage(display::ERROR, uline + name + reset + " has nothing equipped in their "
                            + equipmentSlots[slot] + " slot." );
    } else {
      equipped[slot]->unEquip();
      equipped[slot] = nullptr;
    }
  }

  // Return the highest stat of the player, after bonuses
  public:int getMaxStat() {
    int max = 0;
    for (int i = 0; i < numStats; i++) {
      max = stats[i] > max ? stats[i] : max;
    }
    return max;
  }

  // Return the string length of the largest bonus value
  public:int getMaxBonusLength() {
    long unsigned int max = 0;
    for (int i = 0; i < numSlots; i++) {
      max = to_string(equipped[i]->getBonus()).length() > max
        ? to_string(equipped[i]->getBonus()).length()
        : max;
    }
    return max;
  }

  // Display class selection menu and prompt user for selection
  public:void setClassFromUser() {
    string menuTitle = "Class Selection";
    string prompt = "Choose a class for " + uline + name + reset;

    className = classNames[
      display::getSelectionFromMenu(menuTitle, numClassNames, classNames, prompt)
      -1];

    display::printMessage(display::SUCCESS, uline + name + reset
                          + " has successfully became a(n) " + className + "!");
  }


  public:string getClass() {
    return className;
  }

  // Create a character object from file data
  // Add said object to the game state
  static void loadCharacterFromFile(string fileName = "") {
    cout << endl;
    display::printMessage(display::LOADING, "Loading character from file...");

    ifstream inputFile;
    inputFile = display::getValidFileStream(fileName);

    character newCharacter;
    gameState::characters.push_back(newCharacter);
    // -- Get character name
    string tempCharName;
    getline(inputFile, tempCharName);
    gameState::characters.back().setName(tempCharName);
    display::printMessage(display::SUCCESS, bold + gameState::characters.back().getName() + reset
                          + " successfully loaded.");
    // -- Get character base stats
    // -- Assumes we get base stats in the same stat order as the gear bonuses
    for (int i = 0; i < numStats; i++)
    {
      int tempStatValue;
      inputFile >> tempStatValue;
      gameState::characters.back().setBaseStat(i, tempStatValue);
    }
    // -- Get character gear and calculate bonuses
    for (int i = 0; i < numSlots; i++)
    {
      // create a new gear and update gamestate
      int tempBonus;
      string tempName;
      inputFile >> tempBonus;
      inputFile.get(); // consume whitespace char
      getline(inputFile, tempName);
      gear tempGear;
      gameState::loot.push_back(tempGear);

      gameState::loot.back().setName(tempName);
      gameState::loot.back().setBonus(tempBonus);
      gameState::loot.back().setEquipmentSlot(i);
      
      // update character members
      gameState::characters.back().setStat(i, gameState::characters.back().getBaseStat(i) + tempBonus);
      gameState::characters.back().equipGear(i, &gameState::loot.back());
    }

    gameState::characters.back().setClassFromUser();

    inputFile.close();
  }


  // Create a new character via a series of user prompts
  static void loadCharacterFromPrompts() {
    // Temporary character
    character newCharacter;
    gameState::characters.push_back(newCharacter);

    // Prompt for character name
    cout << endl;
    display::printMessage(display::PROMPT, "Enter a new name for this character:");
    cout << bold << " ";
    string tempName;
    cin >> tempName;
    gameState::characters.back().setName(tempName);
    cout << reset;

    // Prompt for character class
    gameState::characters.back().setClassFromUser();

    // Prompt for character stats
    for (int i = 0; i < numStats; i++) {
      cout << endl;
      display::printMessage(display::PROMPT,
                            "Enter a value for "
                            + uline + newCharacter.name + reset
                            + "\'s base "
                            + bold + statNames[i] + reset
                            + " stat:");
      cout << bold << " ";
      string value;
      cin >> value;
      cout << reset;
      gameState::characters.back().setStat(i, std::stoi(value));
    }

    // TODO(): add prompt for gear
  }

  // Takes in a character then displays its info in a nice way.
  static void printCharcater(character* person, int width,
                   string corners = "+",
                   string verticalBorderFill = "|",
                   char horizontalBorderFill = '-') {
    cout << endl;

    string bp = verticalBorderFill + " "; // Border then padding
    string pb = " " + verticalBorderFill; // Padding then border

    int contentWidth = width - (bp.length() + pb.length());
    // Formatting: print top border.
    display::printHorizontalLine(corners, horizontalBorderFill, corners, width);

    // Print center aligned character name.
    std::vector<string> textStyles = {bold};
    display::printAlignedText(bp, display::CENTER, textStyles, truncate(person->getName(), contentWidth), pb, width);

    // Print character's class
    printAlignedText(bp, display::LEFT, textStyles, person->getClass(), pb, width);

    // Print a blank line within the character display.
    display::printBlankLine(bp, pb, width);

    // Print character stats with bars to indicate progress.
    for (int i = 0; i < numStats; i++)
    {
      // Line 1: stat name, equipment name & equipment bonus
      string bonusText, sign, bonusTextColor;
      int spaceBetweenGearAndBonus = 3;
      int tempBonus = person->equipped[i]->getBonus();
      // -- Set the sign of the stat bonus
      sign = tempBonus >= 0 ? "+" : "";
      // -- Set the color of the bonus based on whether it is positive or negative
      if (tempBonus < 0) {
        bonusTextColor = red;
      } else if (tempBonus > 0) {
        bonusTextColor = green;
      } else {
        bonusTextColor = noFg;
      }
      bonusText = sign + to_string(tempBonus);
      cout << bp
        // Print stat name
           << statNames[i]
        // Allocate whitespace for gear name
           << setw(contentWidth
                   - statNames[i].length()
                   - (spaceBetweenGearAndBonus + person->getMaxBonusLength()))
        // Print gear name
           << person->equipped[i]->getName()
        // Allocate whitespace for stat bonus value and set color
           << bonusTextColor
           << setw(spaceBetweenGearAndBonus + person->getMaxBonusLength())
        // Print bonus value
           << bonusText
           << noFg
           << pb
           << endl;

      // Line 2: stat value & visual bar representation
      //
      // -- Set our highest stat bar to fill the content width
      // -- and avoid dividing by zero
      int barLength = person->getMaxStat() > 0
        ? contentWidth/static_cast<double>(person->getMaxStat())*person->stats[i]
        : contentWidth/100.0*person->stats[i];
      // -- Prevent us from printing a string longer than our bar
      // -- we would get formatting errors otherwise
      barLength = barLength < static_cast<int>(to_string(person->stats[i]).length())
        ? to_string(person->stats[i]).length()
        : barLength;

      cout << bp
        // Set bar color and print it
           << bgColor[i] << black << std::left << setw(barLength)
        // Print stat value within bar
           << person->stats[i] << std::right << noBg << noFg
        // Print whitespace until display border
           << setw(contentWidth
                   -barLength
                   +pb.length())
           << pb << endl;

      // If the last stat has not been displayed
      // Print a blank line within the character display
      if(i < numStats - 1) {
        display::printBlankLine(bp, pb, width);
      }
    }

    // Formatting: print bottom border
    display::printHorizontalLine(corners, horizontalBorderFill, corners, width);

  } // print character end

  // Prompt user with a menu to change character info
  public:void editCharacterFromMenu() {
    string selections[numStats+3];
    int numSelections = sizeof(selections)/sizeof(selections[0]);
    selections[0] = "Name = " + name;       // branch 1
    selections[1] = "Class = " + className; // branch 2
    for (int i=0; i < numStats; i++) {      // branch 3
      selections[i+2] = statNames[i] + " = " + to_string(baseStats[i]);
    }
    selections[numSelections-1] = "Cancel"; // branch 4
    int selection =
      display::getSelectionFromMenu("Edit Character",
                                    numSelections,
                                    selections,
                                    "Choose a value to update");
    selection--;
    if (selection == 0) { // branch 1
      // Prompt for character name
      cout << endl;
      display::printMessage(display::PROMPT, "Enter a new name for this character:");
      cout << bold << " ";
      cin >> name;
      cout << reset;
    } else if (selection == 1) { // branch 2
      setClassFromUser();
    } else if (selection > 1 && selection < numSelections-1 ) { // branch 3
      // Prompt for character name
      int selectedStat = selection - 2; // correct index for stat arrays
      string selectedStatName = statNames[selectedStat];
      bool notValidInputValue = true;
      while (notValidInputValue) {
        cout << endl;
        display::printMessage(display::PROMPT, "Enter a new integer value for "
                              + name + "'s base "
                              + bold + selectedStatName
                              + reset + " stat:"
        );
        cout << bold << " ";
        string strInput;
        cin >> strInput;
        cout << reset;
        try {
          stats[selectedStat] = std::stoi(strInput);
        } catch (...) {
          // If user doesn't enter a valid int, prompt & continue to loop
          printMessage(display::ERROR, "Invalid integer value, try again.");
          continue;
        }
        notValidInputValue = false;
      }
    } else { // branch 4
      // TODO() Go back to previous menu
    }

  }
};

void gear::equipTo(character* characterToEquip) {
  if (equippedBy != nullptr) {
    display::printMessage(display::ERROR, name + " is already equipped by: " + bold + equippedBy->getName() + reset);
  }
  else {
    if (characterToEquip->getEquipped(equipmentSlot) != nullptr) {
      display::printMessage(display::LOADING, "Unequipping "
                            + characterToEquip->getEquipped(equipmentSlot)->name
                            + " from " + bold + characterToEquip->getName() + reset + "...");
      characterToEquip->getEquipped(equipmentSlot)->unEquip();
      display::printMessage(display::SUCCESS, "Successfully unequipped "
                            + characterToEquip->getEquipped(equipmentSlot)->name
                            + " from " + bold + characterToEquip->getName() + reset + ".");
    }
    characterToEquip->equipped[equipmentSlot] = this;
    equippedBy = characterToEquip;
  }
}

int main(int argc, char *argv[])
{
  // Use passed in filename if it is present
  string fileName = argc > 1 ? argv[1] : "";

  // Get character info
  character::loadCharacterFromFile(fileName);
  int displayWidth = 54;
  character::printCharcater(&gameState::characters.back(), displayWidth);

  string mainMenuOpts[] = {
    "Character Menu",
    "Equipment Menu",
    "Quit"
  };
  int numMainMenuOpts = sizeof(mainMenuOpts)/sizeof(mainMenuOpts[0]);

  bool running = true;
  while (running)
  {
    int menuSelection =
    display::getSelectionFromMenu("Main Menu", numMainMenuOpts, mainMenuOpts,
                                  "Select an action");

    switch (menuSelection) {
      case 1: { // begin character menu

        // crud character
        string characterMenuOpts[] = {
        "Create Character",
        "View Character",
        "Edit Character",
        "Delete Character",
        // TODO so there is something...to do... "Make Battle",
        "Cancel"
        };
        int numCharacterMenuOpts = sizeof(characterMenuOpts)/sizeof(characterMenuOpts[0]);
        int charMenuSelection = 0;

        while (charMenuSelection != numCharacterMenuOpts) {
          charMenuSelection = display::getSelectionFromMenu("Character Menu",
                                                            numCharacterMenuOpts,
                                                            characterMenuOpts,
                                                            "Select an action");
          // character menu logic
          switch (charMenuSelection) {
            // create new character and add to vector
            case 1: {
              string selections[] = {
              "Import Character From File",
              "Create New Character",
              "Cancel"
            };
              int numSelections = sizeof(selections)/sizeof(selections[0]);
              int selection = display::getSelectionFromMenu("Create Character",
                                                            numSelections,
                                                            selections,
                                                            "Choose a character creation method");
              switch (selection) {
                case 1: {
                  character::loadCharacterFromFile();
                  break;
                }
                case 2: {
                  character::loadCharacterFromPrompts();
                  break;
                }
              }
              break;
            }

              // Menu to choose a character to view
            case 2: {
              int numCharacters = gameState::characters.size();
              int numOtherOpts = 2;
              string selections[numCharacters+numOtherOpts];
              int selectionNum = 0;
              // iterate over character list to create menu optiions
              for (character idx : gameState::characters) {
                selections[selectionNum] = idx.getName();
                selectionNum++;
              }
              selections[gameState::characters.size()]   = "Show all";
              selections[gameState::characters.size()+1] = "Cancel";
              int selection = display::getSelectionFromMenu("View Character",
                                                            numCharacters+numOtherOpts,
                                                            selections,
                                                            "Choose a character to view");
              selection--; // correct selection for zero indexing
              if (selection < numCharacters) {
                std::list<character>::iterator it = gameState::characters.begin();
                std::advance(it, selection);
                character::printCharcater(&*it, displayWidth);
              } else if (selection == numCharacters) {
                // Print all chars
                for (std::list<character>::iterator idx = gameState::characters.begin();
                     idx != gameState::characters.end(); idx++) {
                  character::printCharcater(&*idx, displayWidth);
                }
                // TODO() abstract out a print function for both types?
                // -- use composition? share printable function/feature to both types
                // printVector(characters, console.printPlayer);
              }
              break;
            }

              // Menu to choose character to edit
            case 3: {
              int numCharacters = gameState::characters.size();
              int numOtherOpts = 1;
              string selections[numCharacters+numOtherOpts];
              int selectionNum = 0;
              for (character idx : gameState::characters) {
                selections[selectionNum] = idx.getName();
                selectionNum++;
              }
              selections[gameState::characters.size()] = "Cancel";
              int selection = display::getSelectionFromMenu("Edit Character",
                                                            numCharacters+numOtherOpts,
                                                            selections,
                                                            "Choose a character to edit");
              selection--; // correct selection for zero indexing
              if (selection < numCharacters) {
                std::list<character>::iterator it = gameState::characters.begin();
                std::advance(it, selection);
                it->editCharacterFromMenu();
              }
              break;
            }

              // Menu to choose character to delete
            case 4: {
              int numCharacters = gameState::characters.size();
              int numOtherOpts = 1;
              string selections[numCharacters+numOtherOpts];
              int selectionNum = 0;
              for (character idx : gameState::characters) {
                selections[selectionNum] = idx.getName();
                selectionNum++;
              }
              selections[gameState::characters.size()] = "Cancel";
              int selection = display::getSelectionFromMenu("Edit Character",
                                                            numCharacters+numOtherOpts,
                                                            selections,
                                                            "Choose a character to delete");
              selection--; // correct selection for zero indexing
              if (selection < numCharacters) {
                std::list<character>::iterator it = gameState::characters.begin();
                std::advance(it, selection);
                string characterName = it->getName();
                display::printMessage(display::LOADING, "Deleting " + characterName + "...");
                // TODO(): this must also unequip the character's gear. as all pointers to the
                //               character will be invalidated once it is deleted.
                gameState::characters.erase(it);
                display::printMessage(display::SUCCESS, "You monster, you deleted " + characterName + "!");
              }

              break;
            }

            case 5: {
              break;
            }
          }
        }
        break;
      } // end character menu

      case 2: { // begin gear menu

        break;
      }  // end gear menu

      case 3: { // begin quit game
        running = false;
      } // end quit game
    } //end main menu

  }

  return 0;
}
