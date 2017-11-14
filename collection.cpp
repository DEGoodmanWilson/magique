//
// Created by Don Goodman-Wilson on 13/11/2017.
//

#include "collection.h"
#include <fstream>
#include <sstream>

template<class T>
std::istream &ReadCsv(std::istream &myfile, std::vector<std::vector<T>> &data)
{
    using namespace std;
    string row;
    while (getline(myfile, row))
    {
        data.push_back(vector<T>());
        istringstream tokenS(row);
        string token;
        while (getline(tokenS, token, ','))
        {
            istringstream valueS(token);
            valueS.imbue(myfile.getloc());
            T value;
            if (valueS >> value)
            {
                data.back().push_back(value);
            }
        }
    }

    return myfile;
}

namespace magique
{

collection::collection(std::string filename)
{
    //load up from a CSV
    std::ifstream file{filename};

    std::string row;
    getline(file, row); //throw away header row;
    while (getline(file, row))
    {
        std::vector<std::string> row_v;
        std::istringstream tokenS(row);
        std::string token;
        while (getline(tokenS, token, ','))
        {
            std::istringstream valueS(token);
            valueS.imbue(file.getloc());
            std::string value;
            if (valueS >> value)
            {
                row_v.push_back(value);
            }
        }
        auto qty = std::stoul(row_v[0]);
        while (qty)
        {
            cards_.push_back(row_v[1]);
            --qty;
        }
    }


}

}