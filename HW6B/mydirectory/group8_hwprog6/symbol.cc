#include "symbol.h"
/******************************************************************************
 *3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
 * Class 'Symbol' as a container for one symbol.
 *
 * Author/copyright:  Duncan A. Buell.  All rights reserved.
 * Used with permission and modified by: Group 8
 * Date: 12-3-2018
**/

/******************************************************************************
 * Constructor
**/
Symbol::Symbol() {
}

/******************************************************************************
 * Constructor
**/
Symbol::Symbol(string text, int programcounter) {
  text_ = text;
  location_ = programcounter;
  is_invalid_ = this->CheckInvalid();
}

/******************************************************************************
 * Destructor
**/
Symbol::~Symbol() {
}

/******************************************************************************
 * Accessors and Mutators
**/

/******************************************************************************
 * Accessor for 'error_messages_'.
**/
string Symbol::GetErrorMessages() const {
  bool previouserror = false;

  return error_messages_;
}

/******************************************************************************
 * Accessor for the 'location_'.
**/
int Symbol::GetLocation() const {
  return location_;
}

/******************************************************************************
 * Accessor for the existence of errors.
**/
bool Symbol::HasAnError() const {
  return (is_invalid_ || is_multiply_);
}

/******************************************************************************
 * Mutator 'SetMultiply'.
 * Sets the 'is_multiply' value to 'true'.
**/
void Symbol::SetMultiply() {
  is_multiply_ = true;
}

/******************************************************************************
 * General functions.
**/

/******************************************************************************
 * Function 'CheckInvalid'.
 * Returns the boolean to say whether a symbol is invalid.
**/
bool Symbol::CheckInvalid() const {
  bool returnvalue = false;  // false means no, not invalid
  string new_error = "";
  // check if symbol is one - three characters long
  if (text_.size() > 3 || text_.size() < 1) {
    new_error = "Symbol has invalid length";
    // std::cout << text_ << " " << new_error;
    // error_messages_ += new_error;
    returnvalue = true;
  }

  // check if symbol starts with alpha character
  if (!isalpha(text_.at(0))) {
    new_error = "Symbol does not begin with Alpha Character";
    // std::cout << text_ << " " << new_error;
    // error_messages_ += new_error;
    returnvalue = true;
  }

  // check that all characters are alphanumberic
  for (int i = 0; i < text_.size(); i++) {
    if (text_.at(i) != ' ' && !isalnum(text_.at(i))) {
      new_error = "Symbol has non alphanumeric character";
      // error_messages_ += new_error;
      // std::cout << text_ << " " << new_error;
      returnvalue = true;
    // check if symbol has blank space in middle
    }
  }

  // check for blank space in middle of symbol
  // if there is a space at the first or second char and not a space at end
  if ((text_.at(0) == ' ' || text_.at(1) == ' ') && text_.at(2) != ' ') {
    new_error = "Symbol has a blank space in the middle \n";
    // std::cout << text_ << " " << new_error;
    // error_messages_ += new_error;
    returnvalue = true;
  }

  return returnvalue;
}

/******************************************************************************
 * Function 'ToString'.
 * This function formats an 'Symbol' for prettyprinting.
 *
 * Returns:
 *   the prettyprint string for printing
**/
string Symbol::ToString() const {
#ifdef EBUG
  Utils::log_stream << "enter ToString" << endl;
#endif
  string s = "";

  if (text_ == "nullsymbol") {
    s += Utils::Format("sss", 3);
  } else {
    s += Utils::Format(text_, 3);
  }

  s += Utils::Format(location_, 4);
  if (is_invalid_) {
    s += " INVALID";
  }
  if (is_multiply_) {
    s += " MULTIPLY";
  }

#ifdef EBUG
  Utils::log_stream << "leave ToString" << endl;
#endif
  return s;
}
