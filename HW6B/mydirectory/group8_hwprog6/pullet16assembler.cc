#include "pullet16assembler.h"

/***************************************************************************
 *3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456
 * Class 'Assembler' for assembling code.
 *
 * Author/copyright:  Duncan A. Buell.  All rights reserved.
 * Used with permission and modified by: Group 8
 * Date: 18 November 2018
**/

/***************************************************************************
 * Constructor
**/
Assembler::Assembler() {
}

/***************************************************************************
 * Destructor
**/
Assembler::~Assembler() {
}

/***************************************************************************
 * Accessors and Mutators
**/

/***************************************************************************
 * General functions.
**/

/***************************************************************************
 * Function 'Assemble'.
 * This top level function assembles the code.
 *
 * Parameters:
 *   in_scanner - the scanner to read for source code
 *   out_stream - the output stream to write to
**/
void Assembler::Assemble(Scanner& in_scanner, string binary_filename,
                         ofstream& out_stream) {
#ifdef EBUG
  Utils::log_stream << "enter Assemble" << endl;
#endif
  /// ///////////////////////////////////////////////////////////////////////
  // Pass one
  // Produce the symbol table and detect errors in symbols.
  // ////////////////////////////////////////////////////////////////////////
  Assembler::PassOne(in_scanner);
  Utils::log_stream << "\nPASS ONE" << endl;
  this->PrintCodeLines();
  this->PrintSymbolTable();
  // ///////_////////////////////////////////////////////////////i/////////////
  // Pass two
  // Generate the machine code.
  // ////////////////////////////////////////////////////////////////////////
  Assembler::PassTwo();
  Utils::log_stream << "\nPASS TWO" << endl;
  this->PrintCodeLines();
  this->PrintSymbolTable();
  /**
    * Checks to see if the source code has any errors.
    * If it does have an error does not generate machine code.
    * If no errors then it generates machine code
  **/
  if (!has_an_error_) {
    // no errors found
    vector<int> dec;    // holds values to dump to a bin file.

    // due to how Buell has the binary files written
    // we needed to swap the bit order
    // essential we read the first 8 bits into first and the rest into second
    // these are then joined again but flipped into swapped_code
    // from this value the binary file is generated
    string first;
    string second;
    string swapped_code;
    for (auto iter = machinecode_.begin(); iter != machinecode_.end(); ++iter) {
      int i = iter->first;
      first = machinecode_.at(i).substr(0, 8);
      second = machinecode_.at(i).substr(8);
      swapped_code = second + first;
      // takes the machine code and generates a vector of integers.
      dec.push_back(DABnamespace::BitStringToDec(swapped_code));
      // dumping the machine code to a dotout.txt file as binary strings
      out_stream << machinecode_.at(i) << endl;
    }

    // dumping to a bin file
    std::ofstream output(binary_filename, std::ofstream::binary);
    if (output) {
      char* buffer = new char[2];
      for ( int i = 0; i < dec.size(); i++ ) {
        int *p = &dec.at(i);
        buffer = reinterpret_cast<char*>(p);
        output.write(buffer, 2);
      }
      output.close();
    }
    Utils::log_stream << "\n\n\n" << "MACHINE CODE" << endl;
    PrintMachineCode(binary_filename, dec.size());
  } else {
    Utils::log_stream << "\n\n\n";
    Utils::log_stream << "ERRORS EXIST\nNO MACHINE CODE GENERATED" << endl;
  }

#ifdef EBUG
  Utils::log_stream << "leave Assemble" << endl;
#endif
}

/***************************************************************************
 * Function 'GetInvalidMessage'.
 * This creates a "value is invalid" error message.
 *
 * Parameters:
 *   leadingtext - the text of what it is that is invalid
 *   symbol - the symbol that is invalid
**/
string Assembler::GetInvalidMessage(string leadingtext, string symbol) {
  string returnvalue = "";
  returnvalue = "***** ERROR -- " + leadingtext + " " +
                symbol + " IS INVALID\n";
  has_an_error_ = true;
  return returnvalue;
}

/***************************************************************************
 * Function 'GetInvalidMessage'.
 * This creates a "value is invalid" error message.
 *
 * Parameters:
 *   leadingtext - the text of what it is that is invalid
 *   hex - the hex operand that is invalid
**/
string Assembler::GetInvalidMessage(string leadingtext, Hex hex) {
  string returnvalue = "";
  returnvalue = "***** ERROR -- " + leadingtext + " " + hex.GetText()
                + " IS INVALID\n";
  has_an_error_ = true;
  return returnvalue;
}

/***************************************************************************
 * Function 'GetUndefinedMessage'.
 * This creates a "symbol is undefined" error message.
 *
 * Parameters:
 *   badtext - the undefined symbol text
**/
string Assembler::GetUndefinedMessage(string badtext) {
  string returnvalue = "";
  returnvalue = "***** ERROR -- SYMBOL " + badtext + " IS UNDEFINED\n";
  has_an_error_ = true;
  return returnvalue;
}

/***************************************************************************
 * Function 'PassOne'.
 * Produce the symbol table and detect multiply defined symbols.
 *
 * CAVEAT: We have deliberately forced symbols and mnemonics to have
 *         blank spaces at the end and thus to be all the same length.
 *         Symbols are three characters, possibly with one or two blank at end
 *         Mnemonics are three characters, possibly with one blank at end.
 *
 * Parameters:
 *   in_scanner - the input stream from which to read
 *   out-stream - the output stream to which to write
**/
void Assembler::PassOne(Scanner& in_scanner) {
#ifdef EBUG
  Utils::log_stream << "enter PassOne" << endl;
#endif

  // Looping until we read in an empty line
  // because the scanner.HasNext() function was causing
  // the scanner.NextLine() function to strip the beggining whitespaces
  Utils::log_stream << "\nReading machine code" << endl;
    pc_in_assembler_ = 0;
    int line_counter = 0;

    // The while-loop will loop until an empty line is read in
  while (true) {
    // holds a line of text read in from the source code.
    string line = in_scanner.NextLine();

    // setting the default attributes
    string label = "nulllabel";
    string mnemonic = "nullmnemonic";
    string symoperand = "nullsymoperand";
    string comments = "nullcomments";
    string addr = "";
    string hexoperand = "";
    string code = "nullcode";

    CodeLine code_line = CodeLine();

    // Checks if the source code is 4096 lines or longer and breaks if it is.
    if (pc_in_assembler_ == DABnamespace::kMaxMemory) {
      Utils::log_stream << "***** ERROR -- SOURCE CODE EXCEEDS MAXIMUM SIZE"
                        << endl;
      break;
    }
    // Break when we read in an empty line
    if (line == "")
      break;

    // Skip first line if it contains column headers
    if (line.at(0) == '*') {
      code_line.SetCommentsOnly(line_counter, line);
      codelines_.push_back(code_line);
      line_counter++;
      continue;
    }

    // Set attributes according to line length
    if (line.size() >= 2)
      label = line.substr(0, 3);
    if (line.size() >= 6)
      mnemonic = line.substr(4, 3);
    if (line.size() >= 8)
      addr = line.substr(8, 1);
    if (line.size() >= 12)
      symoperand = line.substr(10, 3);
    if (line.size() >= 18)
      hexoperand = line.substr(14, 5);
    if (line.size() >= 20)
      comments = line.substr(20, line.size());

    // Set up new CodeLine and store in codelines vector
    code_line.SetCodeLine(line_counter, pc_in_assembler_, label,
                          mnemonic, addr, symoperand, hexoperand,
                          comments, code);
    codelines_.push_back(code_line);

    // If there is a label within line of code
    if (label != "   ") {
      Symbol symbol = Symbol(label, pc_in_assembler_);
      if (symbol.HasAnError()) {
        string error_message = GetInvalidMessage("SYMBOL", label);
        codelines_.at(line_counter).SetErrorMessages(error_message);
      }

      // This block checks if the symbol has been defined more than once, if
      // not, then insert the symbol into the symboltable_ map.
      map<string, Symbol>::iterator it;
      it = symboltable_.find(label);
      if (it != symboltable_.end()) {
        this->UpdateSymbolTable(pc_in_assembler_, label);
    } else {
      // Insert into symbol table if valid symbol
      symboltable_.insert({label, symbol});
    }
  }

    /**
      * If the mnemonic is an org or ds, call SetNewPC
      * If the mnemonic is an end, decrement pc and set found end to true
      * If the menomic is neither, continue like normal
    **/
    if (mnemonic == "ORG" || mnemonic == "DS ") {
      this->SetNewPC(code_line);
      pc_in_assembler_--;
    } else if (mnemonic == "END") {
      pc_in_assembler_--;
      found_end_statement_ = true;
    }
    line_counter++;
    pc_in_assembler_++;
  }

#ifdef EBUG
  Utils::log_stream << "leave PassOne" << endl;
#endif
}

/***************************************************************************
 * Function 'PassTwo'.
 * This function does pass two of the assembly process.
**/
void Assembler::PassTwo() {
#ifdef EBUG
  Utils::log_stream << "enter PassTwo" << endl;
#endif
  pc_in_assembler_ = 0;
  map<int, string> mach_pre;
  for (int i = 0; i < codelines_.size(); ++i) {
    if (codelines_.at(i).IsAllComment() == false) {
      string mnemonic = codelines_.at(i).GetMnemonic();
      string machine_code_ = "";
      string addr = codelines_.at(i).GetAddr();
      Hex hex = Hex();

      /**
        * Checks if the inputted mnemonic is one handled in the assembler
        * if it is valid mnemonic_exits is true
        * the if statement catches mnemonic_exists being false
        * if it is false, prints an error message
      **/
      bool mnemonic_exists = mnemonics_.find(mnemonic) != mnemonics_.end();
      if (!mnemonic_exists) {
        string error_message = GetInvalidMessage("MNEMONIC", mnemonic);
        codelines_.at(i).SetErrorMessages(error_message);
        pc_in_assembler_++;
      } else {
        // starts generating the machine code.
        // converts the mnemonic to their ASCII representation
        machine_code_ += DABnamespace::GetBitsFromMnemonic(mnemonic);
      }
      string sym = codelines_.at(i).GetSymOperand();
      // If the sym is shorter then three char add spaces until 3 char.
      if (sym.length() < 2)
        sym += "  ";
      else if (sym.length() < 3)
        sym += " ";

      // checks if the symbol exists and is defined
      if ( sym != "   "  && sym != "nullsymoperand" ) {
        map<string, Symbol>::iterator it;
        it = symboltable_.find(sym);
        // if the iterator gets to the end then the symbol was not found
        if (it == symboltable_.end()) {
          string error_message = GetUndefinedMessage(sym);
          codelines_.at(i).SetErrorMessages(error_message);
          pc_in_assembler_++;
          continue;
        }
      }

      // Generating the rest of the machine code
      // Added DS because noticed address flag was always set in log files
      // when dealing with DS.

      // generates the direct or indirect portion of the machine code
      if (addr == "*") {
        machine_code_ += "1";
      } else if (addr == " ") {
          machine_code_ += "0";
      } else if (addr != "") {
        string error_message = GetInvalidMessage("ADDRESS", addr);
        codelines_.at(i).SetErrorMessages(error_message);
      }

      // generates the address portion of the machine code
      if (mnemonic == "RD") {
        machine_code_ = "1110000000000001";
      } else if (mnemonic == "STP") {
        machine_code_ = "1110000000000010";
      } else if (mnemonic == "WRT") {
        machine_code_ = "1110000000000011";
      } else if (mnemonic == "END") {
        machine_code_ = kDummyCodeD;
      } else if (mnemonic == "DS" || mnemonic == "DS ") {
          machine_code_ = kDummyCodeC;
          hex = codelines_.at(i).GetHexObject();
          int hex_value_ = hex.GetValue();
          int updated_pc = pc_in_assembler_ + hex_value_;
          // Checks if the DS allocation is a valid address value.
          if ((updated_pc >= 0) && (updated_pc < DABnamespace::kMaxMemory)) {
            pc_in_assembler_ = updated_pc;
            pc_in_assembler_--;
        } else {
            string error_message = GetInvalidMessage("DS ALLOCATION", hex);
            codelines_.at(i).SetErrorMessages(error_message);
        }
    } else if (mnemonic == "ORG") {
        machine_code_ = kDummyCodeA;
        hex = codelines_.at(i).GetHexObject();
        int hex_value_ = hex.GetValue();
        // Checks if ORG to a legal address
        if ((hex_value_ >= 0) && (hex_value_ < DABnamespace::kMaxMemory)) {
          pc_in_assembler_ = hex_value_;
          pc_in_assembler_--;
      } else {
          string error_message = GetInvalidMessage("PC VALUE", hex);
          codelines_.at(i).SetErrorMessages(error_message);
      }
    } else if (mnemonic == "HEX") {
        machine_code_ = "";
        hex = codelines_.at(i).GetHexObject();
        // checks the validity of the hex
        if (hex.HasAnError()) {
          string error_message = hex.GetErrorMessages();
          codelines_.at(i).SetErrorMessages(error_message);
        }
        int hex_value_ = hex.GetValue();
        string addr_string = DABnamespace::DecToBitString(hex_value_, 16);
        machine_code_ += addr_string;
    } else {
        string sym = codelines_.at(i).GetSymOperand();
        // add spaces to sym until it is three characters long
        if (sym.length() < 2)
          sym += "  ";
        else if (sym.length() < 3)
          sym += " ";

        int loc = symboltable_.at(sym).GetLocation();
        string addr_string = DABnamespace::DecToBitString(loc, 12);
        // Double checks if pc is still within memory.
        if (loc >= 0 && loc < DABnamespace::kMaxMemory) {
          machine_code_ += addr_string;
      } else {
          string error_message;
          error_message = "***** ERROR -- ADDRESS IS OUT OF BOUNDS\n";
          codelines_.at(i).SetErrorMessages(error_message);
          machine_code_ = kDummyCodeA;
      }
    }

    // adds the machine code to the machine code map
    // there is some error when generating machine code.
    codelines_.at(i).SetMachineCode(machine_code_);
    if ( codelines_.at(i).GetMnemonic() != "END" ) {
      mach_pre.insert({stoi(codelines_.at(i).GetPC()), machine_code_});
    } else {
      // mnemonic is end
      pc_in_assembler_--;
    }
    pc_in_assembler_++;
    }
  }

  auto iter = mach_pre.begin();
  int j;
  for (int pc = 0; pc < pc_in_assembler_; ++pc) {
    if ( iter == mach_pre.end() ) {
      iter--;
    }
    j = iter->first;
    if ( j != pc ) {
      machinecode_.insert({pc, kDummyCodeA});
    } else {
      machinecode_.insert({pc, mach_pre.at(j)});
      iter++;
    }
  }
#ifdef EBUG
  Utils::log_stream << "leave PassTwo" << endl;
#endif
}

/***************************************************************************
 * Function 'PrintCodeLines'.
 * This function prints the code lines.
**/
void Assembler::PrintCodeLines() {
#ifdef EBUG
  Utils::log_stream << "enter PrintCodeLines" << endl;
#endif
  string s = "";

  for (auto iter = codelines_.begin(); iter != codelines_.end(); ++iter) {
    s += (*iter).ToString() + '\n';
  }

  if (!found_end_statement_) {
    s += "\n***** ERROR -- NO 'END' STATEMENT\n";
    has_an_error_ = true;
  }
  Utils::log_stream << s << endl;
#ifdef EBUG
  Utils::log_stream << "leave PrintCodeLines" << endl;
#endif
}

/***************************************************************************
 * Function 'PrintMachineCode'.
 * This function prints the machine code
   Binary read code is based off 
   Duncan Buell's posted solution to HW5
**/
void Assembler::PrintMachineCode(string binary_filename, int size) {
#ifdef EBUG
  Utils::log_stream << "enter PrintMachineCode" << " "
                    << binary_filename << endl;
#endif
  string s = "";

  std::ifstream input(binary_filename, std::ifstream::binary);
  if (input) {
    char* inputbuffer = new char[2];

    int16_t value_read;
    string first_bit;
    string second_bit;
    string converted_bin;
    vector<string> machcode_frombin;

    for (int i = 0; i < size; i++) {
      input.read(inputbuffer, 2);
      value_read = *(reinterpret_cast<int16_t*>(inputbuffer));
      converted_bin = DABnamespace::DecToBitString(value_read, 16);
      first_bit = converted_bin.substr(0, 8);
      second_bit = converted_bin.substr(8);
      converted_bin = second_bit + first_bit;
      machcode_frombin.push_back(converted_bin);
    }
    input.close();
    // Dump converted binary
    string s = "";
    for ( int i = 0; i < machcode_frombin.size(); i++ ) {
      s = machcode_frombin.at(i).substr(0, 4) + " ";
      s += machcode_frombin.at(i).substr(4, 4) + " ";
      s += machcode_frombin.at(i).substr(8, 4) + " ";
      s += machcode_frombin.at(i).substr(12, 4);
      Utils::log_stream << i << " " << DABnamespace::DecToBitString(i, 12) <<
        " " << s << endl;
    }
    Utils::log_stream << endl;
  } else {
    Utils::log_stream << "ERROR: " << binary_filename
      << " COULD NOT BE READ" << endl;
  }
#ifdef EBUG
  Utils::log_stream << "leave PrintMachineCode" << endl;
#endif
}

/******************************************************************************
 * Function 'PrintSymbolTable'.
 * This function prints the symbol table.
**/
void Assembler::PrintSymbolTable() {
#ifdef EBUG
  Utils::log_stream << "enter PrintSymbolTable" << endl;
#endif
  Utils::log_stream << endl;
  Utils::log_stream << "SYMBOL TABLE" << endl << "    SYM LOC FLAGS" << endl;
  for (auto it = symboltable_.cbegin(); it != symboltable_.cend(); ++it) {
    Utils::log_stream << "SYM " << " "
                      << it->second.ToString() << endl;
  }
#ifdef EBUG
  Utils::log_stream << "leave PrintSymbolTable" << endl;
#endif
}

/******************************************************************************
 * Function 'SetNewPC'.
 * This function gets a new value for the program counter.
 *
 * No return value--this sets the class's PC variable.
 *
 * Parameters:
 *   codeline - the line of code from which to update
**/
void Assembler::SetNewPC(CodeLine codeline) {
#ifdef EBUG
  Utils::log_stream << "enter SetNewPC" << endl;
#endif
  Hex hex = Hex();
  hex = codeline.GetHexObject();
  int hex_value = hex.GetValue();
  string mnemonic = codeline.GetMnemonic();
  // performs "ORG" or "DS"
  if (mnemonic == "ORG") {
    // sets pc to hex value only if the hex value is within the memory
    if (hex_value >= 0 && hex_value < DABnamespace::kMaxMemory)
      pc_in_assembler_ = hex_value;
} else {
    int updated_pc = hex_value + pc_in_assembler_;
    // checks to make sure the pc is valid
    if (updated_pc >= 0 && updated_pc < DABnamespace::kMaxMemory)
      pc_in_assembler_ = updated_pc;
  }

#ifdef EBUG
  Utils::log_stream << "leave SetNewPC" << endl;
#endif
}

/*****************************************************************************
 * Function 'UpdateSymbolTable'.
 * This function updates the symbol table for a putative symbol.
 * Note that there is a hack here, in that the default value is 0
 * and that would mean we can't store a symbol at location zero.
 * So we add one, and then back that out after the full first pass is done.
**/
void Assembler::UpdateSymbolTable(int pc, string symboltext) {
#ifdef EBUG
  Utils::log_stream << "enter UpdateSymbolTable" << endl;
#endif
  symboltable_.at(symboltext).SetMultiply();
  for ( int i = 0; i < codelines_.size(); ++i ) {
    if (codelines_.at(i).GetLabel() == symboltext) {
      string error_message = "***** ERROR -- SYMBOL " + symboltext +
                             " IS MULTIPLY DEFINED\n";
      has_an_error_ = true;
      codelines_.at(i).SetErrorMessages(error_message);
    }
  }
#ifdef EBUG
  Utils::log_stream << "leave UpdateSymbolTable" << endl;
#endif
}
