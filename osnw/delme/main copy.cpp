#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main()
{
    ifstream file;
    file.open("Box.txt");

    string s;
    while (true)
    {
        file.get(s);
        getline(file, s);
        cout << s << endl;
    }
    file.close();

    ofstream ofile("Box.txt");
    string str = "write Box";
    if (ofile.is_open())
    {
        ofile << "write Box";
        ofile.close();
    }
    return 0;
}
