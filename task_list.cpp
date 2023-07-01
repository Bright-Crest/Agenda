#include "task_list.h"
#include <ctime>
#include <vector>
#include <string>

using namespace std;

void TaskList::Remind() {
  // TODO Remind
}

vector<pair<int, Other>>::iterator TaskList::FindTask(int id) {
  for (auto it = task_list_.begin(); it != task_list_.end(); it++) {
    if (it->first == id) {
      return it;
    }
  }
  return task_list_.end();
}

vector<pair<int, Other>>::iterator TaskList::FindTask(string name) {
  for (auto it = task_list_.begin(); it != task_list_.end(); it++) {
    if (it->second.name.compare(name) == 0) {
      return it;
    }
  }
  return task_list_.end();
}

void TaskList::ShowTask(vector<pair<int, Other>>::iterator it) {
  // TODO ShowTask
}
void TaskList::ShowHead() {
  // TODO ShowHead
}

void TaskList::Show(int start = 0, int end = pow(2, 31) - 1, int priority_range = 7, 
            bool (*Compare)(pair<int, Other> task1, pair<int, Other> task2) = LessBegin){
            	
            	//创建副本并按begin_time排序 
        		vector< pair<int, Other> > tmp(task_list_);
        		
        		tmp.sort(tmp.begin(),tmp.end(),Compare);
        		
        		//调用私有函数展示tmp 
        		Show(start, end, priority_range, tmp);
			}

void TaskList::Show(string type, int start = 0, int end = pow(2, 31) - 1, int priority_range = 7,
            bool (*Compare)(pair<int, Other> task1, pair<int, Other> task2) = LessBegin){
            	
            	//创建只含type类型的时间的副本并排序 
            	vector< pair<int, Other> > tmp;
            	for(int i=0;i<task_list_.size();i++){
            		if(type.compare(task_list_[i].second.type)==0)
            			tmp.push_back(task_list_[i]);
				}
				
				tmp.sort(tmp.begin(),tmp.end(),Compare);
				
				//调用私有函数展示tmp 
				Show(start, end, priority_range, tmp);
			}

//私有函数show，展示vec中begin_time介于start与end之间，priority在priority_range内的事件 
void TaskList::Show(int start = 0, int end = pow(2, 31) - 1, int priority_range = 7,  
			vector<pair<int, Other> > &vec){
            	
            	//根据priority进行区分 
        		switch(priority_range){
        			case 4:{
        				Show_with_one_priority(start, end, 4, vec);
						break;
					}
        			case 2:{
        				Show_with_one_priority(start, end, 2, vec);
						break;
					}
        			case 1:{
        				Show_with_one_priority(start, end, 1, vec);
						break;
					}
        			case 3:{
        				Show_with_two_priority(start, end, 1, 2, vec);
						break;
					}
        			case 5:{
        				Show_with_two_priority(start, end, 1, 4, vec);
						break;
					}
        			case 6:{
        				Show_with_two_priority(start, end, 2, 4, vec);
						break;
					}
        			case 7:{
        				Show_with_all_priority(start, end, vec);
						break;
					}
				}
			}

//私有函数，根据priority的代码返回对应的优先级字符串 
string TaskList::get_priority_string(int Priority){
	switch(Priority){
		case 4:{
			return "High";
			break;
		}
		case 2:{
			return "Medium";
			break;
		}
		case 1:{
			return "Low";
			break;
		}
	}
}

//私有函数，处理展示一个优先级的事件的要求
void TaskList::Show_with_one_priority(int start, int end, int Priority, vector<pair<int, Other> > &vec){
	
	string sPriority = get_priority_string(Priority);
	
	int i=0;
	for(; i<vec.size() && vec[i].second.begin_time<start; i++);
	for(;vec[i].second.begin_time<=end;i++){
		if(vec[i].second.priority==Priority)
			cout << "Name: "<<vec[i].second.name << endl << "Begin time: " << vec[i].second.begin_time 
				<< endl << "Priority: " << sPriority << endl << "Type: " << vec[i].second.type << endl << 
				"Remind time: " << vec[i].second.remind_time << endl;
	}
	
	cout << endl;
}

//私有函数，处理展示两个优先级的事件的要求
void TaskList::Show_with_two_priority(int start, int end, int Priority1, int Priority2, vector<pair<int, Other> > &vec){
	
	string sPriority1 = get_priority_string(Priority1);
	string sPriority2 = get_priority_string(Priority2);
	
	int i=0;
	for(; i<vec.size() && vec[i].second.begin_time<start; i++);
	for(;vec[i].second.begin_time<=end;i++){
		if(vec[i].second.priority==Priority1)
			cout << "Name: "<<vec[i].second.name << endl << "Begin time: " << vec[i].second.begin_time 
				<< endl << "Priority: " << sPriority1 << endl << "Type: " << vec[i].second.type << endl << 
				"Remind time: " << vec[i].second.remind_time << endl;
		else if(vec[i].second.priority==Priority2)
			cout << "Name: "<<vec[i].second.name << endl << "Begin time: " << vec[i].second.begin_time 
				<< endl << "Priority: " << sPriority2 << endl << "Type: " << vec[i].second.type << endl << 
				"Remind time: " << vec[i].second.remind_time << endl;
	}
	
	cout << endl;
}

//私有函数，处理展示全部优先级的事件的要求
void TaskList::Show_with_all_priority(int start, int end, vector< pair<int, Other> > &vec){
	
	int i=0;
	for(; i<vec.size() && vec[i].second.begin_time<start; i++);
	for(;vec[i].second.begin_time<=end;i++){
		if(vec[i].second.priority==4)
			cout << "Name: "<<vec[i].second.name << endl << "Begin time: " << vec[i].second.begin_time 
				<< endl << "Priority: " << "High" << endl << "Type: " << vec[i].second.type << endl << 
				"Remind time: " << vec[i].second.remind_time << endl;
		else if(vec[i].second.priority==2)
			cout << "Name: "<<vec[i].second.name << endl << "Begin time: " << vec[i].second.begin_time 
				<< endl << "Priority: " << "Medium" << endl << "Type: " << vec[i].second.type << endl << 
				"Remind time: " << vec[i].second.remind_time << endl;
		else if(vec[i].second.priority==1)
			cout << "Name: "<<vec[i].second.name << endl << "Begin time: " << vec[i].second.begin_time 
				<< endl << "Priority: " << "Low" << endl << "Type: " << vec[i].second.type << endl << 
				"Remind time: " << vec[i].second.remind_time << endl;
	}
	
	cout << endl;
}
