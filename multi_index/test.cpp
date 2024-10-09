#include "multi_index.hpp"
#include <vector>
#include <map>

using five_map = map_wrapper<std::map,std::map,std::map,std::map,std::map>;

int main(){
    constexpr std::vector<std::tuple<int,int,double,std::string,std::string>> vec{
        {0,0,1.5,"S","A"},
        {1,2,3.4,"B","C"},
        {4,5,2.5,"Hahaha","ABCDE"}
    };
    constexpr multi_index<std::vector,five_map,int,int,double,std::string,std::string> student(vec);
    
    auto find_1 = student.find<4>("S");
    for (auto i : find_1){
        std::print("{},",*i);
    }
    std::print("\n");

    student.insert(student.begin(),std::tie(3,1,10.25,"CC","Test"));
    auto find_2 = student.find<5>("Test");
    for(auto i : find_2){
        std::print("{},",*i);
    }

    return 0;
}
