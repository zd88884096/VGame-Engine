#include "../../../../include/core/template_core.h"
#include "../../../utility/timer.h"
#include <string>
#include <iostream>
using namespace std;
using namespace vmath_test;
using namespace vmath_template_core;
/*
 *  serialize implementation
 */

/*
namespace tester{
        template <typename TypeList>
        struct serialize_res;

        template <template <typename...> class TypeList>
        struct serialize_res<TypeList<>>{
        	const static string res = "";
        };

        template <template <typename...> class TypeList, typename U, typename... Ts>
        struct serialize_res<TypeList<U,Ts...>>{
        	const static string res = std::type_info::name(U)+" "+serialize_res<TypeList<Ts...>>::res;
        };
}

template <typename TypeList>
using serialize = typename tester::serialize_res<TypeList>::res;

*/

int main(){
	cout << std::boolalpha;
	
	std::size_t sz = vmath_template_core::typelist_size<typelist<>>;
	
	cout << "size<typelist<>> result: " << sz << endl;
	
	cout << "correct answer: 0" << endl;

	sz = vmath_template_core::typelist_size<typelist<char, bool, double>>;
        
	cout << "size<typelist<>> result: " << sz << endl;
        
	cout << "correct answer: 3" << endl;

	sz = vmath_template_core::typelist_size<typelist<char, bool, bool, bool, bool, double>>;
        
	cout << "size<typelist<>> result: " << sz << endl;
        
	cout << "correct answer: 6" << endl;
		
	cout << "front<typelist<>> result: " << std::is_same<front<typelist<>>, EmptyType>::value << endl;
	
	cout << "front<typelist<bool>> result: " << std::is_same<front<typelist<bool>>, bool>::value << endl;

	cout << "front<typelist<int, char, char, double>> result: " << std::is_same<front<typelist<int, char, char, double>>, int>::value << endl;

	cout << "back<typelist<>> result: " << std::is_same<back<typelist<>>, EmptyType>::value << endl;

        cout << "back<typelist<bool>> result: " << std::is_same<back<typelist<bool>>, bool>::value << endl;

        cout << "back<typelist<int, char, char, double>> result: " << std::is_same<back<typelist<int, char, char, double>>, double>::value << endl;

	cout << "push_front<typelist<>, char> result: " << std::is_same<push_front<typelist<>, char>, typelist<char>>::value << endl;

	cout << "push_front<typelist<int, int, double>, char> result: " << std::is_same<push_front<typelist<int, int, double>, char>, typelist<char, int, int, double>>::value << endl;

	cout << "push_back<typelist<>, char> result: " << std::is_same<push_back<typelist<>, char>, typelist<char>>::value << endl;

        cout << "push_back<typelist<int, int, double>, char> result: " << std::is_same<push_back<typelist<int, int, double>, char>, typelist<int, int, double, char>>::value << endl;

	cout << "pop_front<typelist<>> result: " << std::is_same<pop_front<typelist<>>, EmptyType>::value << endl;
	
	cout << "pop_front<typelist<char>> result: " << std::is_same<pop_front<typelist<char>>, typelist<>>::value << endl;
	
	cout << "pop_front<typelist<int, int, double>> result: " << std::is_same<pop_front<typelist<int, int, double>>, typelist<int, double>>::value << endl;

	cout << "pop_back<typelist<>> result: " << std::is_same<pop_back<typelist<>>, EmptyType>::value << endl;

	cout << "pop_back<typelist<char>> result: " << std::is_same<pop_back<typelist<char>>, typelist<>>::value << endl;

	cout << "pop_back<typelist<int, int, double>> result: " << std::is_same<pop_back<typelist<int, int, double>>, typelist<int, int>>::value << endl;

	cout << "at<typelist<>, 0> result: " << std::is_same<typelist_at<typelist<>, 0>, EmptyType>::value << endl;

	cout << "at<typelist<char, bool, double>, 0> result: " << std::is_same<typelist_at<typelist<char, bool, double>, 0>, char>::value << endl;

	cout << "resize<typelist<>, 0> result: " << std::is_same<resize<typelist<>, 0>, typelist<>>::value << endl;

        cout << "resize<typelist<char, bool, double>, 0> result: " << std::is_same<resize<typelist<char, bool, double>, 2>, typelist<char, bool>>::value << endl;

	cout << "erase<typelist<>, 0> result: " << std::is_same<vmath_template_core::erase<typelist<>, 0>, typelist<>>::value << endl;

        cout << "erase<typelist<char, bool, double>, 0> result: " << std::is_same<vmath_template_core::erase<typelist<char, bool, double>, static_cast<int>(1)>, typelist<char, double>>::value << endl;
	
	cout << "assign<typelist<>, 0, char> result: " << std::is_same<assign<typelist<>, 0, char>, typelist<>>::value << endl;

        cout << "assign<typelist<char, bool, double>, 4, char> result: " << std::is_same<assign<typelist<char, bool, double>, 4, char>, typelist<char, bool, double>>::value << endl;

	cout << "assign<typelist<char, bool, double>, 1, char> result: " << std::is_same<assign<typelist<char, bool, double>, 1, char>, typelist<char, char, double>>::value << endl;
	
	cout << "insert<typelist<>, 0, char> result: " << std::is_same<insert<typelist<>, 0, char>, typelist<char>>::value << endl;

        cout << "insert<typelist<char, bool, double>, 3, char> result: " << std::is_same<insert<typelist<char, bool, double>, 3, char>, typelist<char, bool, double, char>>::value << endl;

	cout << "insert<typelist<char, bool, double>, 1, char> result: " << std::is_same<insert<typelist<char, bool, double>, 1, char>, typelist<char, char, bool, double>>::value << endl;

	cout << "Correctness of empty<typelist<>> result: " << vmath_template_core::empty<typelist<>> << endl;
	
	cout << "Correctness of empty<typelist<bool>> result: " << !vmath_template_core::empty<typelist<bool>> << endl;
	
	return 0;
}
