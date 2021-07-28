/* ==============================================================================
   File:        assignment5.cpp
   Date:        <2021-04-10 Sat>
   Course:      Computer Science I
   Project:     Assignment 4 - Control Structures I
   Description: Alter the flow of the program based on user input
   =========================================================================== */

#include <cctype>
#include <ios>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <iterator>
#include <ostream>
#include <string>
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
  // -- Array allows up to loop for fg-colors for stats
  const string fgColor[numStats] =
    { blue
    , red
    , green
    , yellow
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
    for (unsigned long i=0; i < textStyles.size(); i++) {
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
    // TODO() change to getline to enable filenames with spaces
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
    string fileName = passedFileName != "" ? passedFileName : getFileNameFromUser();
    inputFile.open(fileName);
    while (!inputFile.is_open())
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
  void printMenu(string menuTitle, int numMenuItems, std::vector<string> menuItems) {
      cout << endl;
      printMessage(MENU, menuTitle);
      for (int i=0; i < numMenuItems; i++) {
        cout << purple << " " << i+1 << noFg << " " << menuItems[i] << endl;
      }
      cout << endl;
  }

  // Print menu and prompt for an integer selection
  // Return: integer corresponding to the user's selection
  int getSelectionFromMenu(string menuTitle, std::vector<string> menuItems, string prompt) {
    int selection = 0;
    string strInput;
    bool validInput = false;
    int numMenuItems = menuItems.size();

    while ( !validInput )
    {
      printMenu(menuTitle, numMenuItems, menuItems);
      printMessage(PROMPT, prompt + " by number: (1-" + to_string(numMenuItems) + ")");
      cout << purple << " ";
      cin >> strInput;
      cout << noFg;

      // check for valid integer input
      try {
        selection = std::stoi(strInput);
      } catch (...) {
        // If user doesn't enter a valid int, prompt & continue to loop
        printMessage(ERROR, "Invalid selection, try again.");
        continue;
      }

      // check for integer input in valid range
      if ( !(selection > 0 && selection <= numMenuItems) ) {
        printMessage(ERROR, "Invalid selection, try again.");
      } else {
        validInput = true;
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

    string getBonusText() {
      // -- Set the sign of the stat bonus
      string sign = bonus >= 0 ? "+" : "";
      string bonusText = sign + to_string(bonus);
      return bonusText;
    }

    string getBonusTextColor() {
      // -- Set the color of the bonus based on whether it is positive or negative
      string bonusTextColor;
      if (bonus < 0) {
        bonusTextColor = red;
      } else if (bonus > 0) {
        bonusTextColor = purple;
      } else {
        bonusTextColor = noFg;
      }
      return bonusTextColor;
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

    void printGear(int width, string corners = "+") {
      string bp = "| ";
      string pb = " |";
      string name = getName();
      string bonusText = getBonusText();
      int equipmentSlot = getEquipmentSlot();

      display::printHorizontalLine(corners, '-', corners, width);
      // display::printBlankLine("| ", " |", width);
      display::printAlignedText(bp, display::CENTER, {bold}, equipmentSlots[equipmentSlot], pb, width);
      // display::printBlankLine("| ", " |", width);
      cout << bp
           << name
           << fgColor[equipmentSlot]
           << setw(width-bonusText.length()-name.length()-5)
           << statNames[equipmentSlot]
           << getBonusTextColor()
           << " "
           << getBonusText()
           << noFg
           << pb
           << endl;
      // display::printBlankLine(bp, pb, width);
      display::printHorizontalLine(corners, '-', corners, width);
    }
};

class character
{
  string name;
  int baseStats[numStats];
  gear *equipped[numSlots];
  string className;

  public:
    character() {
      name = "";
      for (int i = 0; i < numStats; i++) {
        baseStats[i] = 0;
      }
      for (int i = 0; i < numSlots; i++) {
        equipped[i] = nullptr;
      }
      className = "Knave";
    }

    void setName(string nameArg) {
      // ensure first letter is capitalized
      char first = std::toupper(nameArg[0]);
      string capitalizedName = first + nameArg.substr(1);
      name = capitalizedName;
    }

    string getName() {
      return name;
    }

    void setBaseStat(int stat, int statValue) {
      baseStats[stat] = statValue;
    }

    int getBaseStat(int stat) {
      return baseStats[stat];
    }

    // calculated field
    int getStat(int stat) {
      int baseStat = getBaseStat(stat);
      if (equipped[stat] == nullptr) {
        return baseStat;
      } else {
        return baseStat + equipped[stat]->getBonus();
      }
    }

    bool hasGearEquipped() {
      bool hasGearEquipped = false;
      for (int i = 0; i < numSlots; i++) {
        if (equipped[i] != nullptr) {
          hasGearEquipped = true;
        }
      }
      return hasGearEquipped;
    }

    gear* getEquipped(int slot){
      return equipped[slot];
    }

    void setEquipped(int slot, gear* gearArg) {
      equipped[slot] = gearArg;
    }

    void equipGear(int slot, gear* gearArg) {
      if (gearArg == nullptr) {
        equipped[slot] = gearArg;
      } else if (equipped[slot] == gearArg) {
        display::printMessage(display::ERROR, uline + name + reset + " already has "
                              + uline + equipped[slot]->getName() + reset + "equipped." );
      } else {
        gearArg->equipTo(this);
      }
    }

    void unEquipGear(int slot) {
      if (equipped[slot] == nullptr) {
        display::printMessage(display::ERROR, uline + name + reset + " has nothing equipped in their "
                              + equipmentSlots[slot] + " slot." );
      } else {
        equipped[slot]->unEquip();
        equipped[slot] = nullptr;
      }
    }

    // Return the highest stat of the player, after bonuses
    int getMaxStatValue() {
      int max = 0;
      for (int i = 0; i < numStats; i++) {
        max = getStat(i) > max ? getStat(i) : max;
      }
      return max;
    }

    // Return the string length of the largest bonus value
    int getMaxBonusTextLength() {
      long unsigned int max = 2; // minimum length including sign
      for (int i = 0; i < numSlots; i++) {
        if (equipped[i] == nullptr) {
          continue;
        }
        max = (equipped[i]->getBonusText()).length() > max
          ? (equipped[i]->getBonusText()).length()
          : max;
      }
      return max;
    }

    // Display class selection menu and prompt user for selection
    void setClassFromUser() {
      string menuTitle = "Class Selection";
      string prompt = "Choose a class for " + uline + bold + name + reset;

      std::vector<string> classNamesVec;
      for (int i = 0; i < numClassNames; i++) {
        classNamesVec.push_back(classNames[i]);
      }
      className = classNames[
        display::getSelectionFromMenu(menuTitle, classNamesVec, prompt)
        -1];

      display::printMessage(display::SUCCESS, uline + bold + name + reset
                            + " has successfully became a(n) "
                            + bold + className + reset + "!");
    }

    string getClass() {
      return className;
    }

    // Takes in a character then displays its info in a nice way.
    void printCharcater(int width, string corners = "+", string verticalBorderFill = "|", char horizontalBorderFill = '-') {
      cout << endl;

      string bp = verticalBorderFill + " "; // Border then padding
      string pb = " " + verticalBorderFill; // Padding then border

      int contentWidth = width - (bp.length() + pb.length());
      // Formatting: print top border.
      display::printHorizontalLine(corners, horizontalBorderFill, corners, width);

      // Print center aligned character name.
      std::vector<string> textStyles = {bold};
      display::printAlignedText(bp, display::CENTER, textStyles, truncate(getName(), contentWidth), pb, width);

      // Print character's class
      printAlignedText(bp, display::LEFT, textStyles, getClass(), pb, width);

      // Print a blank line within the character display.
      display::printBlankLine(bp, pb, width);

      // Print character stats with bars to indicate progress.
      for (int i = 0; i < numStats; i++) {
        // Line 1: stat name, equipment name & equipment bonus
        int spaceBetweenGearAndBonus = 2;
        gear *currentGear = getEquipped(i);
        string bonusText, bonusTextColor, currentGearName;
        int maxBonusLength = getMaxBonusTextLength();

        if (currentGear == nullptr) {
          bonusText = "+0";
          bonusTextColor = "";
          currentGearName = "Empty Slot";
        } else {
          bonusText = currentGear->getBonusText();
          bonusTextColor = currentGear->getBonusTextColor();
          currentGearName = currentGear->getName();
        }

        cout << bp
          // Print stat name
             << statNames[i]
          // Allocate whitespace for gear name
             << setw(contentWidth
                     - statNames[i].length()
                     - (spaceBetweenGearAndBonus + maxBonusLength))
          // Print gear name
             << currentGearName
          // Allocate whitespace for stat bonus value and set color
             << bonusTextColor
             << setw(spaceBetweenGearAndBonus + maxBonusLength)
          // Print bonus values
             << bonusText
             << noFg
             << pb
             << endl;

        // Line 2: stat value & visual bar representation
        //
        // -- Set our highest stat bar to fill the content width
        // -- and avoid dividing by zero
        int barLength = getMaxStatValue() > 0
          ? contentWidth/static_cast<double>(getMaxStatValue()) * getStat(i)
          : contentWidth/100.0 * getStat(i);
        // -- Prevent us from printing a string longer than our bar
        // -- we would get formatting errors otherwise
        barLength = barLength < static_cast<int>(to_string(getStat(i)).length())
          ? to_string(getStat(i)).length()
          : barLength;

        cout << bp
          // Set bar color and print it
             << bgColor[i] << black << std::left << setw(barLength)
          // Print stat value within bar
             << getStat(i) << std::right << noBg << noFg
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
    characterToEquip->setEquipped(equipmentSlot, this);
    equippedBy = characterToEquip;
  }
}

class gameState {
  public:
    std::list<character> characters;
    std::list<gear> loot;

    // Create a character object from file data
    // Add said object to the game state
    static void addCharacterToStateFromFile(gameState *state, string fileName = "") {

      ifstream inputFile;
      string tempCharName;
      int tempStats[numStats];
      gear tempGear[numSlots];
      bool fileIsInValid = true;

      do {
        cout << endl;
        display::printMessage(display::LOADING, "Loading character from file...");
        inputFile = display::getValidFileStream(fileName);

        // -- Get character name
        getline(inputFile, tempCharName);
        // -- Get character base stats & gear
        // -- Assumes we get base stats in the same stat order as the gear bonuses
        for (int i = 0; i < numStats; i++) { inputFile >> tempStats[i]; }
        for (int i = 0; i < numSlots; i++)
        {
          // create a new gear
          int tempBonus;
          string tempGearName;
          inputFile >> tempBonus;
          inputFile.get(); // consume whitespace char
          getline(inputFile, tempGearName);

          // need to get these values out of this for loop scope
          tempGear[i].setName(tempGearName);
          tempGear[i].setBonus(tempBonus);
          tempGear[i].setEquipmentSlot(i);

        }
        fileIsInValid = inputFile.bad() || inputFile.fail();
        if (fileIsInValid) {
          printMessage(display::ERROR,
                       "Invalid file contents. Try a different file.");
          inputFile.clear();
          fileName = "";
        }
      // Ensure that the file contained correct data format
      } while (fileIsInValid);

      // Init new character
      character newChar;
      state->characters.push_back(newChar);
      character &currentChar = state->characters.back();
      currentChar.setName(tempCharName);
      for (int i = 0; i < numStats; i++)
      {
        currentChar.setBaseStat(i, tempStats[i]);
        state->loot.push_back(tempGear[i]);
        currentChar.equipGear(i, &state->loot.back());
      }
      display::printMessage(display::SUCCESS, bold + currentChar.getName() + reset
                            + " successfully loaded.");

      currentChar.setClassFromUser();

      inputFile.close();
    }

    // Create a new character via a series of user prompts
    static void addCharacterToStateFromPrompts(gameState *state) {

      // Prompt for character name
      cout << endl;
      display::printMessage(display::LOADING, "Creating new character...");
      display::printMessage(display::PROMPT, "Enter a name for this character:");
      cout << bold << " ";
      string tempName;
      cin.ignore();
      getline(cin, tempName);
      cout << reset;

      // Temporary character
      character newCharacter;
      state->characters.push_back(newCharacter);
      character &currentChar = state->characters.back();
      currentChar.setName(tempName);

      // Prompt for character class
      currentChar.setClassFromUser();

      // Prompt for character stats
      for (int i = 0; i < numStats; i++) {
        bool validInput = false;
        int tempStat = 777777;
        while (!validInput) {
          cout << endl;
          display::printMessage(display::PROMPT,
                                "Enter a value for "
                                + uline + tempName + reset
                                + "\'s base "
                                + bold + statNames[i] + reset
                                + " stat:");
          cout << bold << " ";
          string strInput;
          cin >> strInput;
          cout << reset;
          try {
            tempStat = std::stoi(strInput);
          } catch (...) {
            // If user doesn't enter a valid int, prompt & continue to loop
            printMessage(display::ERROR, "Invalid integer value, try again.");
            continue;
          }
          validInput = true;
        }
        currentChar.setBaseStat(i, tempStat);
      }

      // TODO(): add prompt for gear
      for (int i = 0; i < numSlots; i++) {
        currentChar.equipGear(i, nullptr);
      }
     
      display::printMessage(display::SUCCESS, bold + currentChar.getName() + reset
                            + " has been successfully created.");
    }

    // Prompt user with a menu to change character info
    static void editCharacterFromMenu(gameState *state, int index) {
      std::list<character>::iterator it = state->characters.begin();
      std::advance(it, index);

      string name;
      string className;

      int numSelections;
      int selection;
      do
      {
        name = it->getName();
        className = it->getClass();

        std::vector<string> selections;
        selections.push_back("Name = " + name);       // branch 1
        selections.push_back("Class = " + className); // branch 2
        for (int i=0; i < numStats; i++) {            // branch 3
          selections.push_back(statNames[i] + " = "
                               + to_string(it->getBaseStat(i)));
        }
        selections.push_back("Cancel");               // branch 4
        numSelections = selections.size();

        selection = display::getSelectionFromMenu("Edit Character: " + uline + name,
                                                  selections,
                                                  "Choose a value to update");
        selection--;
        if (selection == 0) { // branch 1
          // Prompt for character name
          cout << endl;
          display::printMessage(display::PROMPT, "Enter a new name for this character:");
          cout << bold << " ";
          string tempName;
          cin.ignore();
          getline(cin, tempName);
          cout << reset;
          it->setName(tempName);
          display::printMessage(display::SUCCESS,
                                bold + uline + name + reset
                                + " has been renamed to "
                                + bold + uline + it->getName() + reset );
        } else if (selection == 1) { // branch 2
          it->setClassFromUser();
        } else if (selection > 1 && selection < numSelections-1 ) { // branch 3
          // Prompt for character name
          int selectedStat = selection - 2; // correct index for stat arrays
          string selectedStatName = statNames[selectedStat];
          bool validInputValue = false;
          while (!validInputValue) {
            cout << endl;
            display::printMessage(display::PROMPT, "Enter a new integer value for "
                                  + bold + uline + name + reset + "'s base "
                                  + bold + selectedStatName + reset + " stat:"
            );
            cout << bold << " ";
            string strInput;
            cin >> strInput;
            cout << reset;
            try {
              it->setBaseStat(selectedStat, std::stoi(strInput));
              selections[selectedStat+2] =
                statNames[selectedStat] + " = " + to_string(it->getBaseStat(selectedStat));
            } catch (...) {
              // If user doesn't enter a valid int, prompt & continue to loop
              printMessage(display::ERROR, "Invalid integer value, try again.");
              continue;
            }
            validInputValue = true;
          }
        }
        selection++; // fix while loop
      } while (selection != numSelections);
    }
};

int main(int argc, char *argv[])
{
  gameState state;
  // Use passed in filename if it is present
  string fileName = argc > 1 ? argv[1] : "";

  // Get character info
  gameState::addCharacterToStateFromFile(&state, fileName);
  int displayWidth = 54;
  state.characters.back().printCharcater(displayWidth);

  std::vector<string> mainMenuOpts = {
    "Character Menu",
    "Equipment Menu",
    "Quit"
  };

  bool running = true;
  while (running)
  {
    int menuSelection =
    display::getSelectionFromMenu("Main Menu", mainMenuOpts, "Select an action");

    switch (menuSelection) {
      case 1: { // begin character menu

        // crud character
        std::vector<string> characterMenuOptsWithAtLeastOneCharacter =
          {
          "Create Character",
          "View Character",
          "Edit Character",
          "Delete Character",
          // TODO so there is something...to do... "Make Battle",
          "Cancel"
        };


        std::vector<string> characterMenuOptsWithZeroCharacters =
          {
          "Create Character",
          "Cancel"
        };

        int charMenuSelection;
        int numCharacterMenuOpts;
        do {
          int numCharacters = state.characters.size();

          std::vector<string> *characterMenuOpts =
            numCharacters == 0
            ? &characterMenuOptsWithZeroCharacters
            : &characterMenuOptsWithAtLeastOneCharacter;
          numCharacterMenuOpts = characterMenuOpts->size();
          charMenuSelection = display::getSelectionFromMenu("Character Menu",
                                                            *characterMenuOpts,
                                                            "Select an action");
          // character menu logic
          switch (charMenuSelection) {
            // create new character and add to vector
            case 1: {
              std::vector<string> selections =
                {
                "Import Character From File",
                "Create New Character",
                "Cancel"
              };
              int selection;
              int numSelections = selections.size();
              do {
                selection = display::getSelectionFromMenu("Create Character",
                                                          selections,
                                                          "Choose a character creation method");
                switch (selection) {
                  case 1: {
                    gameState::addCharacterToStateFromFile(&state);
                    break;
                  }
                  case 2: {
                    gameState::addCharacterToStateFromPrompts(&state);
                    break;
                  }
                }
              } while (selection != numSelections);
              break;
            }

              // Menu to choose a character to view
            case 2: {
              int numCharacters = state.characters.size();
              int selection;
              int numSelections;
              do {

                std::vector<string> selections;
                // iterate over character list to create menu options
                for (character idx : state.characters) {
                  selections.push_back(idx.getName());
                }
                selections.push_back("Show all");
                selections.push_back("Cancel");
                numSelections = selections.size();

                selection = display::getSelectionFromMenu("View Character",
                                                          selections,
                                                          "Choose a character to view");
                selection--; // correct selection for zero indexing
                if (selection < numCharacters) {
                  std::list<character>::iterator it = state.characters.begin();
                  std::advance(it, selection);
                  it->printCharcater(displayWidth);
                } else if (selection == numCharacters) {
                  // Print all chars
                  for (std::list<character>::iterator idx = state.characters.begin();
                       idx != state.characters.end(); idx++) {
                    idx->printCharcater(displayWidth);
                  }
                  // TODO() abstract out a print function for both types?
                  // -- use composition? share printable function/feature to both types
                  // printVector(characters, character::printCharacter);
                }
                selection++;
              } while (selection != numSelections);
              break;
            }

              // Menu to choose character to edit
            case 3: {
              int selection;
              int numSelections;
              do {
                std::vector<string> selections;
                for (character idx : state.characters) {
                  selections.push_back(idx.getName());
                }
                selections.push_back("Cancel");
                numSelections = selections.size();

                selection = display::getSelectionFromMenu("Edit Character",
                                                              selections,
                                                              "Choose a character to edit");
                selection--; // correct selection for zero indexing
                if (selection < state.characters.size()) {
                  gameState::editCharacterFromMenu(&state, selection);
                }
                selection++;
              } while (selection != numSelections);
              break;
            }

              // Menu to choose character to delete
            case 4: {
              int numCharacters;
              int selection;
              int numSelections;
              do {
                numCharacters = state.characters.size();
                if (numCharacters == 0) {
                  break;
                }

                std::vector<string> selections;
                for (character idx : state.characters) {
                  selections.push_back(idx.getName());
                }
                selections.push_back("Cancel");
                numSelections = selections.size();

                selection = display::getSelectionFromMenu("Edit Character",
                                                              selections,
                                                              "Choose a character to delete");
                selection--; // correct selection for zero indexing
                if (selection < numCharacters) {
                  std::list<character>::iterator it = state.characters.begin();
                  std::advance(it, selection);
                  string characterName = it->getName();
                  display::printMessage(display::LOADING, "Deleting "
                                        + uline + bold + characterName + reset + "...");
                  // TODO(): this must also unequip the character's gear. as all pointers to the
                  //               character will be invalidated once it is deleted.
                  state.characters.erase(it);
                  display::printMessage(display::SUCCESS, "You monster, you deleted "
                                        + uline + bold + characterName + reset + "!");
                }
                selection++;

              } while (selection != numSelections);
              break;
            }

            case 5: {
              break;
            }
          }
        } while (charMenuSelection != numCharacterMenuOpts);
        break;
      } // end character menu

      case 2: { // begin gear menu
        // check if there are any characters to even choose from,
        // if not, notify user and return to previous menu
        // TODO(): this check will be obsolete once crud gear is here,
        // -- and this menu is in view character menus
        int numCharacters = state.characters.size();
        if (numCharacters == 0) {
          display::printMessage(display::ERROR, "There are no characters with gear to view.");
          break;
        }

        // Menu to choose a character who's gear to view
        std::vector<string> gearMenuOpts;
        std::vector<character*> charactersWithEquipment;

        // iterate over character list to create menu optiions
        for (std::list<character>::iterator idx = state.characters.begin();
             idx != state.characters.end();
             idx++)
        {
          if (idx->hasGearEquipped()) {
            gearMenuOpts.push_back(idx->getName());
            charactersWithEquipment.push_back(&*idx);
          }
        }

        // we only care about the ones with gear equipped
        int numCharactersWithEquipment = charactersWithEquipment.size();

        gearMenuOpts.push_back("Cancel");

        int numGearMenuOpts = gearMenuOpts.size();

        if (numCharactersWithEquipment == 0) {
          display::printMessage(display::ERROR, "There are no characters with gear to view.");
          break;
        }

        int selection;
        do {
          selection = display::getSelectionFromMenu("View Character Equipment",
                                                    gearMenuOpts,
                                                    "Choose a character who's gear you'd like to view");

          selection--; // correct selection for zero indexing
          if (selection < numCharactersWithEquipment) {
            // Menu to choose a piece of selected character's equipment to view
            character *selectedCharacter = charactersWithEquipment[selection];

            // -- Create equipment selection menu from character's equipment
            std::vector<string> equipmentMenuSelections;
            for (int i = 0; i < numSlots; i++) {
              gear *currentGear = selectedCharacter->getEquipped(i);
              if (currentGear == nullptr) {continue;}
              equipmentMenuSelections.push_back(currentGear->getName());
            }
            equipmentMenuSelections.push_back("Show all");
            equipmentMenuSelections.push_back("Cancel");

            int equipmentMenuSelection;
            int numEquipmentMenuSelections = equipmentMenuSelections.size();
            do {
              equipmentMenuSelection =
                display::getSelectionFromMenu("View " + uline
                                              + selectedCharacter->getName()
                                              + "\'s"  + reset + bold + " Equipment",
                                              equipmentMenuSelections,
                                              "Select a piece of gear to inspect");
              equipmentMenuSelection--; // correct selection for zero indexing
              if (equipmentMenuSelection < numSlots) {
                selectedCharacter->getEquipped(equipmentMenuSelection)->printGear(displayWidth);
              } else if (equipmentMenuSelection == numSlots) {
                for (int i=0; i < numSlots; i++) {
                  selectedCharacter->getEquipped(i)->printGear(displayWidth);
                }
              }
              equipmentMenuSelection++; // to make the while loop work
            } while (equipmentMenuSelection != numEquipmentMenuSelections);
          }
          selection++; // to make the while loop work
        } while (selection != numGearMenuOpts);
        break;
      }  // end gear menu

      case 3: { // begin quit game
        running = false;
      } // end quit game
    } //end main menu

  }
  return 0;
}

