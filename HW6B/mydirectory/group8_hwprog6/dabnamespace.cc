#include "dabnamespace.h"

/***************************************************************************
 * Code file for the 'DABnamespace' for global constants and
 * functions.
 *
 * Author/copyright:  Duncan A. Buell.  All rights reserved.
 * Used with permission and modified by: Group 8
 * Date: 21 September 2018
 *
 * Documentation for the functions is in the header for those
 * functions.
**/

namespace DABnamespace {
/***************************************************************************
 * Function 'BitStringToDec'.
 * Convert a bit string to a decimal value.
 * This is merely a wrapper for the 'stoi' function.
 *
 * Parameters:
 *   thebits - the ASCII array of "bits" to be converted
 *
 * Returns:
 *   the array 'thebits' converted to an 'int'
**/
int BitStringToDec(const string thebits) {
#ifdef EBUG
  Utils::log_stream << "enter BitStringToDec" << endl;
#endif

  // Remember that the second parameter is for positioning the pointer
  // after doing the conversion (so we don't care here) and the third
  // says we are converting from a string of digits in binary.
  int stoivalue = std::stoi(thebits, nullptr, 2);

#ifdef EBUG
  Utils::log_stream << "leave BitStringToDec" << endl;
#endif

  return stoivalue;
}

/***************************************************************************
 * Function 'DecToBitString'.
 * This function converts a decimal 'int' to a string of 0s and 1s.
 *
 * We only allow conversion to a string of length 12 or of length 16
 * because we only allow an address (lessequal 4096 = 2^12) or a hex
 * operand of 16 bits.
 *
 * This is basically just a wrapper for the 'bitset' function.
 *
 * Parameters:
 *   value - the value to convert
 *   how_many_bits - the length of the result
 *
 * Returns:
 *   the 'string' of bits obtained from the 'value' parameter
**/
string DecToBitString(const int value, const int how_many_bits) {
#ifdef EBUG
  Utils::log_stream << "enter DecToBitString" << endl;
#endif

  string bitsetvalue = "";
  if (how_many_bits == 12) {
    bitsetvalue = std::bitset<12>(value).to_string();
  } else if (how_many_bits == 16) {
    bitsetvalue = std::bitset<16>(value).to_string();
  } else {
    Utils::log_stream << "ERROR DECTOBITSTRING " << value << " "
                      << how_many_bits << endl;
    exit(0);
  }

#ifdef EBUG
  Utils::log_stream << "leave DecToBitString" << endl;
#endif

  return bitsetvalue;
}

/******************************************************************************
 * Function 'GetMnemonicFromBits'.
 * This function retrieves the textual mnemonic from the bitstring code.
string GetMnemonicFromBits(string codebits) {
#ifdef EBUG
  Utils::log_stream << "enter GetMnemonicFromBits" << endl;
#endif
  return code_to_mnemonic_.at(codebits);
#ifdef EBUG
  Utils::log_stream << "leave GetMnemonicFromBits" << endl;
#endif
}
**/

/**************************************************************************
 * Function 'GetBitsFromMnemonic'.
 * This function converts the bit string code from the textual mnemonic
**/
string GetBitsFromMnemonic(string mnemonic) {
#ifdef EBUG
  Utils::log_stream << "enter GetBitsFromMnemonic" << endl;
#endif
  map<string, string> mnem_to_code = { {"BAN", "000"},
                                       {"SUB", "001"},
                                       {"STC", "010"},
                                       {"AND", "011"},
                                       {"ADD", "100"},
                                       {"LD", "101"},
                                       {"LD ", "101"},
                                       {"BR", "110"},
                                       {"BR ", "110"},
                                       {"STP", "111"},
                                       {"RD", "111"},
                                       {"RD ", "111"},
                                       {"WRT", "111"},
                                       {"HEX", "000"},
                                       {"END", "000"},
                                     };

  // mnem_to_code["BAN"] = "000";
  // mnem_to_code["SUB"] = "001";
  // mnem_to_code["STC"] = "010";
  // mnem_to_code["AND"] = "011";
  // mnem_to_code["ADD"] = "100";
  // mnem_to_code["LD "] = "101";
  // mnem_to_code["LD"] = "101";
  // mnem_to_code["BR "] = "110";
  // mnem_to_code["BR"] = "110";
  // mnem_to_code["STP"] = "111";
  // mnem_to_code["RD "] = "111";
  // mnem_to_code["RD"] = "111";
  // mnem_to_code["WRT"] = "111";
  // mnem_to_code["HEX"] = "000";
  // mnem_to_code["END"] = "000";
  return mnem_to_code.at(mnemonic);

#ifdef EBUG
  Utils::log_stream << "leave GetBitsFromMnemonic" << endl;
#endif
}

}  // namespace DABnamespace

