#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <fstream>

#define N_ERR 7

using namespace std;

// Folder must end with "/", e.g. "D:/images/"
vector<string> get_all_files_full_path_within_folder(string folder)
{
    vector<string> names;
    char search_path[200];
    sprintf(search_path, "%s*.*", folder.c_str());
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(search_path, &fd);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            // read all (real) files in current folder, delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
            {
                names.push_back(folder+fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
    return names;
}

//Correction function which suggests corrections
string correctionFunction(string line, int &err){
    bool correct = false;
    switch (err){
    //Removing Z0
    case 0:
        if (line.find("G53 G0 Z0.") != string::npos){
            line = "";
            correct = true;
        }
    break;
    //Removing three lines
    case 1:
        if (line.find("M9") != string::npos){
            line = "";
            correct = true;
        }
    break;
    case 2:
        if (true){
            line = "";
            correct = true;
        }
    break;
    case 3:
        if (true){
            line = "";
            correct = true;
        }
    break;
    //Adding Z20
    case 4:
        if (line.find("G54") != string::npos){
            line = "G54\nG0 Z20.\n";
            correct = true;
        }
    break;
    //Removing H
    case 5:
        if (line.find("H") != string::npos){
            line = line.substr(0,line.size()-2)+'\n';
            correct = true;
        }
    break;
    //Removing M9
    case 6:
        if (line.find("M9") != string::npos){
            line = "";
            correct = true;
        }
    break;
    //Removing Z0
    case 7:
        if (line.find("G53 Z0.") != string::npos){
            line = "";
            correct = true;
        }
    }

    if (!correct){
        return line + '\n';
    }
    else {
        err++;
        return line;
    }
}

int main(){
    //Getting all the file names within the current directory
    vector<string> files;

    files = get_all_files_full_path_within_folder(".\\");

    //Selecting only the gCode
    string keyString(".ngc");

    vector<string> gCodeNames;

    //Finding the gCodes
    for (int i = 0; i < files.size(); ++i){
        if (files[i].find(keyString) != string::npos){
            gCodeNames.push_back(files[i]);
        }
    }

    files.clear();

    //Creating vector of output names
    vector<string> gCodeOutNames;

    for (int i = 0; i < gCodeNames.size(); ++i){
        string outName = ".\\out" + gCodeNames[i].substr(1,string::npos);
        gCodeOutNames.push_back(outName);
    }

    //Creating output directory
    CreateDirectory(".\\out",NULL);

    //Counting errors
    int nErr = 0;

    //Do the file correction to each gCode in the vector
    for (int i = 0; i < gCodeNames.size(); ++i){
        ifstream gCodeOriginal(gCodeNames[i].c_str());
        ofstream gCodeEdit;
        gCodeEdit.open(gCodeOutNames[i].c_str());

        string line;

        int errorIndex = 0;

        //Going through the files
        while(getline(gCodeOriginal,line)){

            line = correctionFunction(line, errorIndex);
            gCodeEdit <<line;
        }

        if (errorIndex == N_ERR+1){
            cout << "Generated "<< gCodeOutNames[i] << " ok!\n";
        }

        else {
            cout << "Generated "<< gCodeOutNames[i] << N_ERR+1 - errorIndex <<" modifications couldn't be done.\n";
            nErr++;
        }

        gCodeEdit.close();
    }

    cout << "All files processed with "<< nErr << " errors!\n";

    //End of the program
    system("pause");
}
