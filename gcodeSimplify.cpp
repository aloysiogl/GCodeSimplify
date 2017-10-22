#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>

#define N_ERR 7

using namespace std;

//Check if a file exists
bool is_file_exist(const char *fileName);

//Corrects the gCode
string correctionFunction(string line, int &err);

//Gets files in a folder
vector<string> get_all_files_full_path_within_folder(string folder);

//Generate config file
void generateConfig(string path, string sherlineIp, string destinationDirectory);

int main(){
    //Opening the configurations file
    ifstream config(".//config.txt");

    //Configurations
    string path = "\\Program Files\\PuTTY";
    string sherlineIp = "192.168.0.20";
    string destinationDirectory = "/home/sherline/Documents/SMALL";

    //Getting configurations from file if config file exists
    if (is_file_exist(".\\config.txt")){

        //Sherline info
        string line;
        getline(config,line);
        getline(config,line);
        sherlineIp = line.substr(3,string::npos);
        getline(config,line);
        destinationDirectory = line.substr(10,string::npos);

        //Adding path
        getline(config,line);
        getline(config,line);
        getline(config,line);
        path = line.substr(5,string::npos);

        string setPath;
        setPath+= "set PATH=C:";
        setPath+= path;
        setPath+= ";%PATH%";
        system(setPath.c_str());
    }

    else {
        generateConfig(path, sherlineIp, destinationDirectory);
    }

    //Getting all the file names within the current directory
    vector<string> files;

    files = get_all_files_full_path_within_folder(".\\");

    //Selecting only the gCodes
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

    //Store the maximum size
    int maxSize = 0;

    for (int i = 0; i < gCodeNames.size(); ++i){
        string outName = ".\\out" + gCodeNames[i].substr(1,string::npos);
        gCodeOutNames.push_back(outName);

        if (gCodeNames[i].size() > maxSize)
            maxSize = gCodeNames[i].size();
    }

    //Creating output directory
    CreateDirectory(".\\out",NULL);

    //Counting errors
    int nErr = 0;

    //Do the file correction to each gCode in the vector
    for (int i = 0; i < gCodeNames.size(); ++i){
        ifstream gCodeOriginal(gCodeNames[i].c_str());
        ofstream gCodeEdit(gCodeOutNames[i].c_str());

        string line;

        int errorIndex = 0;

        //Going through the files
        while(getline(gCodeOriginal,line)){

            line = correctionFunction(line, errorIndex);
            gCodeEdit <<line;
        }

        if (errorIndex == N_ERR+1){
            cout << "Generated "<< gCodeOutNames[i] << " ok!\n\n";
        }

        else {
            cout << "Generated "<< setw(maxSize+4) << gCodeOutNames[i] << " " << N_ERR+1 - errorIndex <<" modifications couldn't be done.\n\n";
            nErr++;
        }

        gCodeEdit.close();
    }

    //Final message after generating the codes
    cout << "All files processed with "<< nErr << " errors!\n\n";


    //Prompt user to send files to Sherline
    cout << "Do you want to send the files to Sherline CNC now (Y/n)?";

    cin.putback('n');

    string send;

    getline(cin, send);

    //Clearing the screen
    system("cls");

    //Start routine to send files
    if (send == "Y" || send == "y" || send.empty()){
        cout << "According to the config file the following IP and folder are going to be used:\n";
        cout << "IP: " << sherlineIp << "\n";
        cout << "Destination: " << destinationDirectory << "\n";

        cout << "\nDo you want to change the destination (Y,n)?";

        //comment
        string change;

        getline(cin, change);

        //Generating new config file
        if (change == "Y" || change == "y") {
            //Printing the new path
            cout << "\nType the new path to the archive:\n";




            cin >> destinationDirectory;
            generateConfig(path, sherlineIp, destinationDirectory);

            system("cls");
            cout << "The new settings are:\n";
            cout << "IP: " << sherlineIp << "\n";
            cout << "Destination: " << destinationDirectory << "\n\n";

            //Trying to create directory

            //Opening command file
            ofstream createDirCommand("command.txt");

            //Command do be sent
            string createDir = "mkdir " + destinationDirectory;

            createDirCommand << createDir;
            createDirCommand.close();

            //Opening connection and sending message
            createDir = "plink -ssh ";
            createDir += sherlineIp;
            createDir += " -l sherline -pw sherline -m command.txt";
            system(createDir.c_str());

            system("DEL command.txt");
        }

        else {
            //Clearing screen
            system("cls");
        }

        cout << "Sending files, please wait:\n\n";

        //Sending the codes
        for (int i = 0; i < gCodeOutNames.size(); ++i){
            string command;
            command += "pscp -l sherline -pw sherline ";
            command += gCodeOutNames[i];
            command += " sherline@";
            command += sherlineIp;
            command += ":";
            command += destinationDirectory;

            system(command.c_str());

        }

        //Jumping line
        cout << "\n";

        //Moving the done parts to a dedicate directory
        CreateDirectory(".\\sent",NULL);

        //Moving each file

        cout << "Moving original files to .\\sent directory...\n\n";

        for (int i = 0; i < gCodeNames.size(); ++i){
            string moveCommand;

            moveCommand+= "move ";
            moveCommand+= gCodeNames[i];
            moveCommand+= " .\\sent";

            cout << setw(maxSize) << gCodeNames[i] << " ";
            system(moveCommand.c_str());
        }

        cout << "\n";
    }

    //Do nothing
    else {
        //Moving the done parts to a dedicate directory
        CreateDirectory(".\\done",NULL);

        //Moving each file

        cout << "Moving original files to .\\done directory...\n\n";

        for (int i = 0; i < gCodeNames.size(); ++i){
            string moveCommand;

            moveCommand+= "move ";
            moveCommand+= gCodeNames[i];
            moveCommand+= " .\\done";

            cout << setw(maxSize) << gCodeNames[i] << " ";
            system(moveCommand.c_str());
        }

        cout << "\n";
    }
    //End of the program
    system("pause");
}

//Checking if file already exists
bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

//Generate configuration file
void generateConfig(string path, string sherlineIp, string destinationDirectory){
    ofstream newConfig("config.txt");

    newConfig << "//SHERLINE INFO\n";
    newConfig << "IP:" << sherlineIp << "\n";
    newConfig << "DIRECTORY:" << destinationDirectory << "\n\n";
    newConfig << "//PATH TO PUTTY\n";
    newConfig << "PATH:" << path << "\n";
}

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

//Correction function which indicates the corrections
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
            line = line.substr(0,line.size()-3)+'\n';
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
        if (line.find("G53 Z0.") != string::npos || line.find("G53 G0 Z0.") != string::npos){
            line = "G0 X0 Y0 Z60.\n";
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
