/*---------------------------------------------------------------------------
Project-Name: Two pass assembler
Name- Krrish Raj
Roll: 2201CS41
Declaration of Authorship
This file, asm.cpp is part of the project of CS210 at IIT Patna , Computer Science and Engineering
---------------------------------------------------------------------------
-------------------------------------------------------------------------
*/


#include <bits/stdc++.h>
using namespace std;
#define mpsp map<string, pair<int, int>>
#define mpis map<int, string>
#define mpsi map<string, int>
#define vpi vector<pair<int, int>>
#define vps vector<pair<string, string>>
#define vi vector<int>
#define vs vector<string>
#define vpis vector<pair<int, string>>
#define all(x) x.begin(), x.end()

// intialise All_errors and other variables
bool extra_operand = false;
int prog_counter = 0;
int max_line = 0;
bool check_halt = false, check_infinity = false; // for warnings

// vector declarations
string my_opcode, my_operand, my_label;
vs code_lines;
vi all_SET_labels;
vps nibble_codes;
vpis All_errors; // for storing differnt All_errors
vpi not_num, not_number_values;

// maps declarations
mpsp operations;                                   // for storing mnemonics and their operand type
mpis original_operands, opcodes, operands, labels; // map to separate_label_opc {PC,labels/opcodes/operans}
mpsi label_val;                                    // to separate_label_opc values of label to use them as operands

set<string> temporary_label;
map<string, bool> label_without_opcode; // to stpore if label is without opcode or not
int line_number = 1;


void read_input_file(string file_name)
{
    string input_file, input_copy;
    ifstream myfile;
    myfile.open(file_name);
    while (myfile)
    {
        // take input from user
        getline(myfile, input_file);
        input_copy = input_file;

        input_copy.erase(remove_if(all(input_copy), ::isspace), input_copy.end());
        if (input_copy != "")
        {
            // removing empty lines
            code_lines.push_back(input_file);
        }
    }
}

// this function will if the operand is numeric or label
// return true for int ,  false for string
bool check_operand(string s)
{
    int n = s.length();
    char chk[n];
    int i = 0;
    while (i < n)
    {
        chk[i] = s[i];
        i++;
    }
    bool f = true;
    unsigned long long int l = strtol(chk, NULL, 0); // separate_label_opc integer value of string to l
    if (l == 0 && s != "0")
        f = false; // will be 0 for any string
    return f;      // otherwise return true(=int);
}

// this function will separate labels, opcodes and operands
void separate_label_opc(string code_line, int Line_no)
{
// Initialize iterators for the beginning and end of the code line
auto ptr = code_line.begin();
auto ending = find(code_line.begin(), code_line.end(), ';'); // Find the end of the line

// Find the position of the colon (if any) to separate the label
auto temp = find(code_line.begin(), ending, ':');

// Extract label if it exists
if (temp != ending) 
{ 
    // If a colon is found before the end of the line
    copy_if(ptr, temp, back_inserter(my_label), [](char c) { return c != ' '; });
    ptr = temp + 1; // Move past the colon
}


    for(;ptr!=ending ; ptr++)
    {
        if((*ptr) !=' ') break;
    }
    // while(ptr!=ending&&(*ptr)==' ') // skip blank spaces
    //     ptr++; 
    while(ptr!=ending&&(*ptr)!=' ') // separate the copcode
    {
        my_opcode+=(*ptr);
        ptr++;
    }
    for(;ptr!=ending ; ptr++)
    {
        if((*ptr) !=' ')break;
    }
    while(ptr!=ending&&(*ptr)!=' ') //separate the operand 
    {
        my_operand+=(*ptr);
        ptr++;
    }
    for(;ptr!=ending ; ptr++)
    {
        if((*ptr) !=' ')break;
    }

    if (ptr!=ending) // check if any extra operand is present
    {
        extra_operand=true;
    }
    for(int i=0;i<my_label.length();i++)
    {
        if(my_label[i]=='\t'|| my_label[i]==' ')
        {
            my_label.erase(i,1);
            i--;
        }
    }
    for(int i=0;i<my_opcode.length();i++)
    {
        if(my_opcode[i]=='\t'|| my_opcode[i]==' ')
        {
            my_opcode.erase(i,1);
            i--;
        }
    }
    for(int i=0;i<my_operand.length();i++)
    {
        if(my_operand[i]=='\t'|| my_operand[i]==' ')
        {
            my_operand.erase(i,1);
            i--;
        }
    }

    // this part will manage all errors found
    if (temporary_label.find(my_label) != temporary_label.end())//check for duplicate labels
    {
        if (my_label != "")  
            All_errors.push_back({Line_no, "Duplicate label found  "});
    }
    else if ((my_label[0] >= '0' && my_label[0] <= '9')) //check if label name starts with a  number
    {
        if (my_label != "")
            All_errors.push_back({Line_no, "bogus label name found "});
    }
    if (operations.find(my_opcode) == operations.end()) //check if opcode is a valid opcode or not 
    {
        if (my_opcode != "")
            All_errors.push_back({Line_no, "bogus mnemonic found"});
    }
    else if (my_opcode != "")
    {
        // if operand is offset 
        if (operations[my_opcode].second == 2)
        {
            char chs = my_operand[0];
            if(my_operand.compare("") == 0)//if no operand is found
                All_errors.push_back({Line_no, " missing operand "});
            else if (!check_operand(my_operand))
            {
                if ((chs >= '0' && chs <= '9'))
                {
                    // if label name starts with a number 
                    All_errors.push_back({Line_no, "bogus label name"});
                }
                else
                    not_num.push_back({prog_counter, Line_no});
            }
        }
        // if operand is value
        if (operations[my_opcode].second == 1)
        {
            if (my_operand.compare("") == 0)
                All_errors.push_back({Line_no, "missing operand"});
            else if (!check_operand(my_operand))//checking if operand is numeric
            { 
                not_number_values.push_back({prog_counter, Line_no});
            }
            //check for set and data type opcode
            if (!check_operand(my_operand) && (my_opcode == "SET" || my_opcode == "data")) 
            {
                All_errors.push_back({Line_no, "not a number"});
            }
        }
        // if no operand is present
        if (operations[my_opcode].second == 0)
        {
            if (my_operand.compare("") != 0) //if any operand is found 
                All_errors.push_back({Line_no, "unexpected operand found "}); 
        }
    }
    if (extra_operand)//if extra operand is found 
        All_errors.push_back({Line_no, "extra on end of line"});
}

// this function will printAll_errors
void print_all_errors(string file_name)
{
    // Find the position of the . in the file_name
    auto it = find(file_name.begin(), file_name.end(), '.');

    // Calculate the index of the period
    int ind = it - file_name.begin();

    // Remove the extension from the file_name
    string file_name_without_extension = file_name.substr(0, ind);

    // Append ".log" to the file_name
    file_name_without_extension += ".log";

    // Open the file for writing
    ofstream file(file_name_without_extension);

    // Sort theAll_errors by line number
    sort(all(All_errors));

    // WriteAll_errors to the log file
    for (const auto &itr : All_errors)
    {
        file << "Error on line " << itr.first << " : " << itr.second << endl;
    }
}

// Print binary representation to a file
void print_machine_code(string file_name)
{
    // Find the position of the first period in the file_name
    auto it = find(all(file_name), '.');

    // Calculate the index of the period
    size_t ind = it - file_name.begin();

    // Remove the extension from the file_name
    string file_name_without_extension = file_name.substr(0, ind);

    // Append ".o" to the file_name
    file_name_without_extension += ".o";

    // Open the file for writing in binary mode
    ofstream file(file_name_without_extension, ios::binary);

    // Convert hexadecimal values to binary and write to file
    for (const auto &itr : nibble_codes)
    {

        string copy=itr.first, final_binary="";
      for (char c : copy) //change to binary from hexadecimal form
      {
        int num = 0;
        if (isdigit(c)) 
        {
            num = c - '0';
        } 
        else if (isalpha(c)) 
        {
            num = toupper(c) - 'A' + 10;
        } 
        final_binary += std::bitset<4>(num).to_string();
      }


        //"SET" and "data" check bits addition
        if (itr.second == "data")  
            final_binary += "01"; // for data
        else if (itr.second == "SET")
            final_binary += "10"; // for SET
        else
            final_binary += "00"; // for  no set/data

        // Write each character of the binary string to the file
        auto it = final_binary.begin();
        for (; it != final_binary.end(); it++)
        {
            char tt = *it;
            file.write((char *)&tt, sizeof(tt)); // Write to the binary file
        }
        // Each instruction is represented by a 34-bit binary number
    }
}

void print_listing(string File_name)
{

    File_name.erase(find(all(File_name), '.')- File_name.begin());
    File_name += string(".l");
    ofstream file;
    file.open(File_name);

    for (int i = 0; i < prog_counter; i++)
    {
        
        std::string opcode_temporary = opcodes[i],operand_temporary = operands[i];
        std::string pc,Leading_zeroes;
        std::string label_temporary = labels[i];
        pc.clear();
        Leading_zeroes.clear();

         //check for halt 
        if (opcode_temporary == "HALT") check_halt = true;
        

        //check for infinity loop is present
        //  cout<<label_temporary<<"-"<<original_operands[i]<<endl;
        if (label_temporary == original_operands[i])
        {
            
            if(label_temporary.compare("")!=0)
            {
            check_infinity = true;
           
            }
        }
        stringstream s;
        s << hex << i; //store hexadecimal of string s in pc
        pc = s.str();
        for (int k=8; k>pc.length();k--) Leading_zeroes += "0"; //ass leading zeroes to make it of length 8
        
        stringstream opcodee, oprnd; // for converting opcodes and operand to hexadecimal
        pc = Leading_zeroes+pc; // to put leading zeroes
        
        if (opcode_temporary != "SET" && opcode_temporary != "data")
        {
            int ops = operations[opcode_temporary].first;
            opcodee << hex << ops;
        }

        std::string oper=operand_temporary;

        //if operand is numeric but not label
        if (!check_operand(operand_temporary))
        { 
            long long  tmpo = label_val[operand_temporary];
            oprnd<<hex<<tmpo;
        }
        else
        {
            long long tmpo = stol(oper, NULL, 0);
            oprnd<<hex<<tmpo;
        }

        std:: string code;
        code.clear();
        if (opcode_temporary != "data" && opcode_temporary != "SET")
        {
            std::string htp = oprnd.str();
            int os1=oprnd.str().size();
            int ops=opcodee.str().length();
            if (os1 > 6)  htp = oprnd.str().substr(os1 - 6, 6);//take only last 6 digits 
            if (ops != 2) code += htp + "0" + opcodee.str(); 
            else   code += htp + opcodee.str();   
        }
        else
        //no opcode for set and data type
            code += oprnd.str(); 
        

        Leading_zeroes.clear();
        //add leading zeroes
        for (int k=8; k>code.length();k--)    Leading_zeroes += "0";

        code = Leading_zeroes+code; 

        if (label_without_opcode[label_temporary])
        {
            file<<pc<<"          "<<label_temporary + ":"<<endl;
            file<<pc<<" "<<code<<" "<<opcode_temporary<<" "<<original_operands[i]<<endl;
        }
        else
        {
            if (label_temporary.compare("") !=0)
                file<<pc<<" "<<code<<" "<<label_temporary + ":"<<" "<<opcode_temporary<<" "<<original_operands[i] << endl;
            else
                file<<pc<<" "<<code<<" "<<opcode_temporary<<" "<<original_operands[i]<<endl;
        }

        nibble_codes.push_back({code, opcode_temporary}); // storing codes and mnemonic
    }
    //check if halt is present or not
    if (check_halt==false)
        file << "Warning : HALT is missing !!" << endl;
    if (check_infinity==true)
        file << "Warning : Infinity LOOP detected !!" << endl;//check for infinity loop
}

// this is first pass function , this will separate label, operand and opcode of each line
// it will also check forAll_errors
void first_pass()
{
    for (auto code_line : code_lines)
    {
        // taking one line at a time
        my_label.clear();
        my_opcode.clear();
        my_operand.clear();
        extra_operand = false;

        separate_label_opc(code_line, line_number);

        if (my_label.compare("") != 0)
        {
            temporary_label.insert(my_label);
            labels[prog_counter] = my_label;
            label_val[my_label] = prog_counter; 

            if (my_opcode.compare("") != 0)  //if opcode is not empty i.e is present 
                label_without_opcode[my_label] = 0;
            else
                label_without_opcode[my_label] = 1;
        }

        // this part will handle SET and data only
        if (my_opcode.compare("") != 0)
        {
            opcodes[prog_counter] = my_opcode;
            operands[prog_counter] = my_operand;
            original_operands[prog_counter] = my_operand;

            if (my_opcode == "SET" || my_opcode == "data") all_SET_labels.push_back(prog_counter);
            prog_counter+=1; // pc increment, if opcode is found
        } 
         line_number++;
    }

}

void second_pass()
{
    for (auto v : not_num )
    {
        string operand_temporary = operands[v.first];
        string temp_string = "";
        if (find(all(temporary_label), operand_temporary) == temporary_label.end())
            All_errors.push_back({line_number, "no such label"});
        else // for jump , it will store the label value as pc+1
            operands[v.first] = to_string(label_val[operand_temporary] - v.first - 1);
    }
    int es = int(All_errors.size());
    if (es == 0)
    {
        int i=0,j=0;
        while (i < all_SET_labels.size())
        {
            auto tmpp= all_SET_labels[i];
            if (opcodes[tmpp] == "SET")
                label_val[labels[tmpp]] = stoi(operands[tmpp]);
            i++;
        }
        while(j < not_number_values.size())
        {
            if (label_val.find(operands[not_number_values[j].first]) == label_val.end())
            {
                All_errors.push_back({line_number, "not a number"});
            }
            j++;
        }
    }
}

int main()
{

    string file_name;
    printf("Enter name of asm file ! \n");
    cin >> file_name;

    read_input_file(file_name);
    

    // initialisation of mnemonics
    //stored in pair ( opcode, no of operands )
    operations["data"] = {-1, 1};
    operations["SET"] =  {-2, 1};
    operations["ldc"] =  {0, 1};
    operations["adc"] =  {1, 1};
    operations["ldl"] =  {2, 2};
    operations["stl"] =  {3, 2};
    operations["ldnl"] = {4, 2};
    operations["stnl"] = {5, 2};
    operations["add"] =  {6, 0};
    operations["sub"] =  {7, 0};
    operations["shl"] =  {8, 0};
    operations["shr"] =  {9, 0};
    operations["adj"] =  {10, 1};
    operations["a2sp"] = {11, 0};
    operations["sp2a"] = {12, 0};
    operations["call"] = {13, 2}; 
    operations["return"]= {14, 0};
    operations["brz"] =  {15, 2};
    operations["brlz"] = {16, 2};
    operations["br"] =   {17, 2};
    operations["HALT"] = {18, 0};
    
     
    first_pass();
    second_pass();

    // print the error file and machine code
    int es = int(All_errors.size());
    
    if (es == 0) // if there is no error then print the listing and binary files
    {
        cout << "binary and listing file created !!" << endl;
        print_listing(file_name);
        print_machine_code(file_name);
    }
    else // print the log file orAll_errors
    {
        cout << " log file created  !!" << endl;
        print_all_errors(file_name);
    }
    return 0;
}