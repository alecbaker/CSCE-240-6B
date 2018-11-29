#include "pullet16assembler.h"

/***************************************************************************
 *3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456
 * Class 'Assembler' for assembling code.
 *
 * Author/copyright:  Duncan A. Buell.  All rights reserved.
 * Used with permission and modified by: Alec Baker
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
  //////////////////////////////////////////////////////////////////////////
  // Pass one
  // Produce the symbol table and detect errors in symbols.
  //this->PassOne(in_scanner);

  // HW6A Read ASCII input data and dump
  // Read input data into vector

  Assembler::PassOne(in_scanner);
  Utils::log_stream << "\nDumping mach. code" << endl;
  for ( int i = 0; i < codelines_.size(); i++ ) {
    Utils::log_stream << codelines_.at(i).ToString() << "\n";
    out_stream << codelines_.at(i).ToString() << "\n";
  }

  //////////////////////////////////////////////////////////////////////////
  // Pass two
  // Generate the machine code.
  //////////////////////////////////////////////////////////////////////////
  // Read binary, set up map of machine code and dump machine code
//  this->PrintMachineCode(binary_filename, out_stream);
  // Dump the results.

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

  return returnvalue;
}

/***************************************************************************
 * Function 'PassOne'.
 * Produce the symbol table and detect multiply defined symbols.
 *
 * CAVEAT: We have deliberately forced symbols and mnemonics to have
 *         blank spaces at the end and thus to be all the same length.
 *         Symbols are three characters, possibly with one or two blank at end.
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

  // The below code is for 6b
  // Looping until we read in an empty line
  // because the scanner.HasNext() function was causing
  // the scanner.NextLine() function to strip the beggining whitespaces
 
  Utils::log_stream << "\nReading machine code" << endl;
    pc_in_assembler_ = 0;
    int line_counter = 0;
  while(true) {
    string line = in_scanner.NextLine();
    line_counter++;
    // Break when we read in an empty line
    if (line == "")
      break;
    // Skip first line if it contains column headers
    if (line.at(0) == '*') {
      CodeLine comment_line = CodeLine();
      comment_line.SetCommentsOnly(line_counter-1, line);
      codelines_.push_back(comment_line);
      continue;
    }
    // Set attribute defaults
    string label = "nulllabel";
    string mnemonic = "nullmnemonic";
    string symoperand = "nullsymoperand";
    string comments = "nullcomments";
    string addr = "";
    string hexoperand = "";
    string code = "";
    // Get machinecode according to line number
    code = machinecode_.find(line_counter-1)->second;
    if (code == "")
      code = "nullcode";
    // Set attributes according to line length
    if (line.size() >= 2) 
      label = line.substr(0,3);
    if (line.size() >= 6) 
      mnemonic = line.substr(4,3);
    if (line.size() >= 8) 
      addr = line.substr(8,1);
    if (line.size() >= 12) 
      symoperand = line.substr(10,3);
    if (line.size() >= 18) 
      hexoperand = line.substr(14,5);
    if (line.size() >= 20) 
      comments = line.substr(20, line.size()); 
    
    // Set up new CodeLine and store in codelines vector
    CodeLine code_line = CodeLine();
    code_line.SetCodeLine(line_counter, pc_in_assembler_, label,
                          mnemonic, addr, symoperand, hexoperand,
                          comments, code);
    codelines_.push_back(code_line);
    pc_in_assembler_++;
  }
  // Dump code lines
  cout << endl << "DUMPING CODE LINES" << endl;
  this->PrintCodeLines();
  

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
  cout << s << endl;
#ifdef EBUG
  Utils::log_stream << "leave PrintCodeLines" << endl;
#endif
  Utils::log_stream << s << endl;
}

/***************************************************************************
 * Function 'PrintMachineCode'.
 * This function prints the machine code
   Binary read code is based off 
   Duncan Buell's posted solution to HW5
**/
void Assembler::PrintMachineCode(string binary_filename,
                                 ofstream& out_stream) {
#ifdef EBUG
  Utils::log_stream << "enter PrintMachineCode" << " "
                    << binary_filename << endl;
#endif
  string s = "";

  std::ifstream input(binary_filename, std::ifstream::binary);
  input.seekg(0, input.end);
  int length = input.tellg();
  input.seekg(0, input.beg);
  char* inputbuffer = new char[2];
  // Read in binary
  for (int i = 0; i < length/2; i++) {
    input.read(inputbuffer, 2);
    // Convert binary to ASCII and store in machinecode_ with line number
    int16_t valueread = *(reinterpret_cast<int16_t*>(inputbuffer));
    string converted_binary = DABnamespace::DecToBitString(valueread, 16);
    machinecode_.insert(std::pair<int,string>(i,converted_binary));
  }
  input.close();

  // Dump converted binary
  cout << endl << "DUMPING CONVERTED BINARY" << endl;
  for (int i = 0; i < machinecode_.size(); i++) {
    out_stream << machinecode_.at(i) << endl;
    cout << machinecode_.at(i) << endl;
  }
  cout << endl;

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
  string s = "";
#ifdef EBUG
  Utils::log_stream << "leave PrintSymbolTable" << endl;
#endif
  Utils::log_stream << s << endl;
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

#ifdef EBUG
  Utils::log_stream << "leave SetNewPC" << endl;
#endif
}

/******************************************************************************
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

#ifdef EBUG
  Utils::log_stream << "leave UpdateSymbolTable" << endl;
#endif
}
