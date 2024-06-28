/*****************************************************************************
Author: Krrish Raj
Roll Number: 2201CS41
Declaration of Authorship
This file emu.cpp is created by me and belongs to course code CS210
*****************************************************************************/

#include <bits/stdc++.h>
using namespace std;

// data structures used
vector<pair<int, int>> process;
vector<int> mem_locations;
vector<int> storeA;
vector<int> storeB;
vector<int> storePC;
vector<int> storeSP;
int memory[1 << 24];

// variables used
int regA;
int regB;
int PC;
int SP;
int termina = 0;
long long  aa,bb;

/* 

The function "inputBinaryFile" divides the binary code into operands and opcodes and
stores it into the vector named process

*/

void inputBinaryFile(string fileName)
{

    ifstream file;
    file.open(fileName, ios::binary);
    int programCounter = 0;
    int cnt = 0;
    string code = "";
    char x;

    while (file.read((char *)&x, sizeof(x)))
    {
        cnt++;
        code += x;
        if (cnt == 34)
        {

            string operand;
            string opcode;
            string nonNum;

            int pointer = 0;
            while (pointer != 24)
            {
                operand += code[pointer];
                pointer++;
            }
            while (pointer != 32)
            {
                opcode += code[pointer];
                pointer++;
            }
            while (pointer != 34)
            {
                nonNum += code[pointer];
                pointer++;
            }
            code.clear();

            if (nonNum == "00")
            {
                if (operand[0] == '1')
                {
                    for (int i = 0; i < 24; i++)
                    {
                        if (operand[i] == '1')
                            operand[i] = '0';
                        else
                            operand[i] = '1';
                    }
                    
                    aa=(-1) * (stol(operand, 0, 2) + 1);
                    bb=stol(opcode, 0, 2);

                    process.push_back({aa,bb});
                }
                else
                {
                    aa=stol(operand, 0, 2);
                    bb=stol(opcode, 0, 2);
                    process.push_back({aa,bb});
                }
            }
            else
            {
                mem_locations.push_back(programCounter);
                operand += opcode;
                if (operand[0] == '1')
                {
                    for (int i = 0; i < 24; i++)
                    {
                        if (operand[i] == '0')
                            operand[i] = '1';
                        else
                            operand[i] = '0';
                    }
                    if (nonNum == "01")
                    {
                        memory[programCounter] = (-1) * (stol(operand, 0, 2) + 1);
                    }
                    aa = (-1) * (stol(operand, 0, 2) + 1);
                    bb = stol(opcode, 0, 2);
                    process.push_back({aa,bb});
                }
                else
                {
                    aa = stol(operand, 0, 2);
                    bb = stol(opcode, 0, 2);
                    process.push_back({aa,bb});
                    if (nonNum == "01")
                        memory[programCounter] = stol(operand, 0, 2);
                }
            }
            programCounter++;
            cnt = 0;
        }
    }
}

int main()
{

    cout << "Enter file name: " << endl;
    string fileName;
    cin >> fileName;

    inputBinaryFile(fileName);

    /*
        This while runs until we reach the end og line of code or HALT is encountered
    */

    while (termina == 0)
    {

        int operand = process[PC].first;
        int opcode = process[PC].second;

        if (opcode == 0)
        {
            //ldc
            regB = regA;
            regA = operand;
        }
        else if (opcode == 1)
        {
            //adc
            regA += operand;
        }
        else if (opcode == 2)
        {
            //ldl
            regB = regA;
            regA = memory[SP + operand];
        }
        else if (opcode == 3)
        {
            //stl
            memory[SP + operand] = regA;
            mem_locations.push_back(SP + operand);
            regA = regB;
        }
        else if (opcode == 4)
        {
            //ldnl
            regA = memory[regA + operand];
        }
        else if (opcode == 5)
        {
            //stnl
            memory[regA + operand] = regB;
            mem_locations.push_back(regA + operand);
        }
        else if (opcode == 6)
        {
            //add
            regA += regB;
        }
        else if (opcode == 7)
        {
            //sub
            regA = regB - regA;
        }
        else if (opcode == 8)
        {
            //shl
            regA = regB << regA;
        }
        else if (opcode == 9)
        {
            //shr
            regA = regB >> regA;
        }
        else if (opcode == 10)
        {
            //adj
            SP = SP + operand;
        }
        else if (opcode == 11)
        {
            //a2sp
            SP = regA;
            regA = regB;
        }
        else if (opcode == 12)
        {
            //sp2a
            regB = regA;
            regA = SP;
        }
        else if (opcode == 13)
        {

            //call
            regB = regA;
            regA = PC;
            PC += operand;
        }
        else if (opcode == 14)
        {
            //ret
            PC = regA;
            regA = regB;
        }
        else if (opcode == 15)
        {
            //brz
            if (regA == 0)
                PC = PC + operand;
        }
        else if (opcode == 16)
        {
            //brlz
            if (regA < 0)
                PC = PC + operand;
        }
        else if (opcode == 17)
        {
            //br
            PC = PC + operand;
        }
        else if (opcode == 18)
        {
            //HALT
            termina = 1;
        }
        /*
            storing the values of regA , regB, PC , SP
        */
        storeA.push_back(regA);
        storeB.push_back(regB);
        storePC.push_back(PC);
        storeSP.push_back(SP);

        PC++;

        if (PC >= process.size())
            break;
    }

    if (termina == 0)
    {
        cout << "WARNING: No HALT, exit program due to end of file" << endl;
    }

    while (1)
    {
        //Choices available to user
        cout << "To Trace line-by-line : PRESS 1 " << endl;
        cout << "To see current Memory Dump : PRESS 2" << endl;
        cout << "To exit : PRESS 3" << endl;

        int select;
        cin >> select;

    /*
    
    To create trace file
    */

        if (select == 1)
        {
            cout << "Creating trace file..." << endl;
            string newFile;
            for (int i = 0; i < fileName.size(); i++)
            {
                newFile += fileName[i];
                if (fileName[i] == '.')
                    break;
            }

            newFile += "trace";
            ofstream file;
            file.open(newFile);
            int pt = 0;
            while (pt != storeA.size())
            {
                file << "Line = " << pt + 1 << " A = " << storeA[pt] << " B = " << storeB[pt] << " PC = " << storePC[pt] << " SP = " << storeSP[pt] << endl;
                pt++;
            }
        }

        /*
            To generate the memdump file
        */
        if (select == 2)
        {
            cout << "Creating memory dump file..." << endl;
            string newFile;
            for (int i = 0; i < fileName.size(); i++)
            {
                newFile += fileName[i];
                if (fileName[i] == '.')
                    break;
            }
            newFile += "memdump";

            ofstream file;
            file.open(newFile);
            int ptr = 0;
            while (ptr < mem_locations.size())
            {
                stringstream ss, s;
                ss << hex << mem_locations[ptr];
                s << hex << memory[mem_locations[ptr]];
                string loc, val;

                loc = ss.str();
                val = s.str();
                while (loc.size() < 8)
                {
                    loc = "0" + loc;
                }
                while (val.size() < 8)
                {
                    val = "0" + val;
                }

                file << loc << " " << val << endl;
                ptr++;
            }
        }

    /*
        To exit from the loop
    */
        if (select == 3)
        {
            break;
        }

        if (select != 1 && select != 2 && select != 3)
        {
            cout << "Please enter a valid number" << endl;
        }
    }
}



/***********************************************************END OF EMULATOR CODE*************************************************************/
