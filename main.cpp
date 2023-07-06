#include"task_list.h"
#include"tools.h"
#include"cmd_processor.h"
#include<iostream>
#include<algorithm>
#include<thread>
#include<mutex>
#include<condition_variable>
std::mutex mtx;
condition_variable cv;
using namespace std;
TaskList t("task.txt");
void get_cmd() {
	string cmd;
	
	while (1)
	{

		// Wait for a certain period before triggering the remind thread again
		std::cout << "menu:addtask\tdeletetask\tshowtask\tmodifytask\tsearchtask\tquit" << endl;
		string op;
		std::cin >> op;
		to_lower(op);
		if (op == "addtask")
		{
			// 添加退出功能
			std::cout << "Do you want to continue adding tasks? (Y/N)" << endl;
			string answer;
			std::cin >> answer;

			if (answer == "N" || answer == "n")
			{
				continue;// 继续下一轮循环，即回到菜单选择步骤
			}
			
			AddTaskNoOp(t);
				
		}
		else if (op == "deletetask")
		{
			std::cout << "Do you want to continue deleting tasks? (Y/N)" << endl;
			string answer;
			std::cin >> answer;
			if (answer == "N" || answer == "n")
			{
				continue;// 继续下一轮循环，即回到菜单选择步骤
			}
			
			DeleteTaskNoOp(t);
		}
		else if (op == "showtask")
		{
			std::cout << "Do you want to continue showing tasks? (Y/N)" << endl;
			string answer;
			std::cin >> answer;
			if (answer == "N" || answer == "n")
			{
				continue;// 继续下一轮循环，即回到菜单选择步骤
			}
			
			ShowTaskNoOp(t);
		}
		else if (op == "modifytask")
		{
			ModifyTaskNoOp(t);
		}
		else if (op == "searchtask")
		{
			SearchTaskNoOp(t);
		}
		else if (op == "quit")
		{
			exit(666);
			break;
		}
		else
		{
			std::cout << "invalid command" << endl;
			continue;
		}
		cv.notify_one();
	}
}


int main()
{
	
	std::thread get_cmd_Thread(get_cmd);
	unique_lock<mutex> lck(mtx);

	while (1) {
		while (cv.wait_for(lck, std::chrono::seconds(1)) == std::cv_status::timeout) {
			t.Remind();
		}
	}

	get_cmd_Thread.join();
	return 0;
}
