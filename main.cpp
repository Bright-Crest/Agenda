
#include<algorithm>
#include<iostream>
using namespace std;
void to_lower( std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
		return std::tolower(c);
		});
}
//把用户输入的转换全部是小写
int main()
{
	while (1)
	{
		string choice;
		cin >> choice;
		to_lower(choice);
		if (choice == "addtask")
		{
			//add()
		}
		else if (choice == "showtask")
		{
			//show()
		}
		else if (choice == "modifytask")
		{
			//modify()
		}
		else if (choice == "deletetask")
		{
			//delete()
		}
		else if (choice == "searchtask")
		{
			//search()
		}
		else if (choice == "quit")
		{
			exit(666);
		}
	}
}
