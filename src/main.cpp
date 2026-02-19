#include <iostream>

int main() {



	return 0;
}

// class employee {
//
// 	char* name;
// 	short department;
// 	employee* next;
// 	static employee* list;
//
// 	public:
// 		employee(char* n, short d) : department(d) {
// 			name = new char[strlen(n) + 1];
// 			strcpy(name, n);
// 			next = list;
// 			list = this;
// 		}
//
// 		virtual ~employee() { delete[] name; }
//
// 		static void print_list() {
// 			for (employee* p = list; p; p=p->next) p->print();
// 		}
//
// 		virtual void print() const {
// 			std::cout << name << ' ' << department << '\n';
// 		}
// };
//
// employee* employee::list = nullptr;
