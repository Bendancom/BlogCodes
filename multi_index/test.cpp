import multi_index;
import <print>;
import <map>;
import <vector>;

// 对于five_map应有更简单的形式，如 map_wrapper<std::map>表示全为std::map，读者可自行实现(挖坑)
using five_map = map_wrapper<std::map,std::map,std::map,std::map,std::map>;

int main(){
    // 任意range容器，注意列元素不应重复(因为使用的全是std::map,若使用std::multi_map则没有此顾虑)
    constexpr std::vector<std::tuple<int,int,double,std::string,std::string>> vec{
        {0,0,1.5,"S","A"},
        {1,2,3.4,"B","C"},
        {4,5,2.5,"Hahaha","ABCDE"}
    };
    // 本例只实现了一种初始化，还有更简便的如同std::make_tuple的样式的初始化(挖坑)
    constexpr multi_index<std::vector,five_map,int,int,double,std::string,std::string> student(vec);
    
    auto find_1 = student.find<4>("S");
    // 别忘记 i 是迭代器，要使用得解引用
    for (auto i : find_1){
        // 可自行实现 std::fomatter 与 std::ostream 达到直接输出数据的效果
        std::print("{},",*i);
    }
    std::print("\n");

    student.insert(student.begin(),std::tie(3,1,10.25,"CC","Test"));
    auto find_2 = student.find<5>("Test");
    // 同上
    for(auto i : find_2){
        std::print("{},",*i);
    }

    return 0;
}