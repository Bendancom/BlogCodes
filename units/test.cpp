#include <unordered_set>
#include <print>
import units;

template<>
struct std::hash<basic_dimension>{
    static constexpr size_t operator()(const std::convertible_to<basic_dimension> auto& dimen){
       return dimen.getId().hash_code();
    }
};

std::unordered_set<basic_dimension> unit;

using namespace units;

int main(){
    unit.insert(gram);
    unit.insert(meter);
    auto magni = unit.find(meter);
    std::print("{}",magni == unit.end());
    return 0;
}