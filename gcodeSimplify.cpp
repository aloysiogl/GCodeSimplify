#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>

using namespace std;

// folder must end with "/", e.g. "D:/images/"
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

    //Do the file correction to each gCode in the vector
    for (int i = 0; i < gCodeNames.size(); ++i){

    }

    //Test
    for (int i = 0; i < gCodeNames.size(); ++i){
        cout << gCodeNames[i] << "\n";
    }
}
