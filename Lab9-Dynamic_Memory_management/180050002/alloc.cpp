#include<bits/stdc++.h>
#include "alloc.h"
using namespace std;

vector<pair<void*,int> >allocated;
pair<vector<void*>,vector<int> > memory;
void* start;

int init_alloc(){
	start = mmap(NULL, PAGESIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	if(start == MAP_FAILED){
		return -1;
	}
	memory.first.push_back(start);
  	memory.second.push_back(PAGESIZE);
  	return 0;
}

int cleanup(){
	munmap(start,PAGESIZE);
	memory.first.clear();
	memory.second.clear();
	return 0;
}

char* alloc(int mem){
	if(mem%MINALLOC!=0){
		return NULL;
	}
	else{
		pair<void*,int> temp;
		temp=make_pair((void*)0,mem);
		int temp1=-1;
    	for(int i=0;i<(memory.second).size();i++){
    		if(mem<=memory.second[i]){
    		temp1=i;
    		break;
    		}
    	}
	if(temp1 == -1){
	return NULL;
	}
	else{
	temp.first=memory.first[temp1];
	allocated.push_back(temp);
		if(mem == memory.second[temp1]){
          memory.first.erase(memory.first.begin()+temp1);
          memory.second.erase(memory.second.begin()+temp1);
		}
		else{
		memory.first[temp1]+=mem;
		memory.second[temp1]-=mem;
		}
	return (char*)temp.first;
	}
	}
}

void dealloc(char* destart){

	for(auto itr=allocated.begin();itr!=allocated.end();itr++){
		if((char*)itr->first == destart){

			int j= lower_bound(memory.first.begin(),memory.first.end(),itr->first)-memory.first.begin();
			if(j>0&&(itr->first == memory.first[j-1]+memory.second[j-1])){
				memory.second[j-1]+=itr->second;
				if(memory.first.size()>j){	
				if((itr->first+itr->second)==memory.first[j]){
					memory.second[j-1]+=memory.second[j];
					memory.first.erase(memory.first.begin()+j);
					memory.second.erase(memory.second.begin()+j);
				}
			}
			}
			else{
				if(memory.first.size()>j){
				if((itr->first+itr->second)==memory.first[j]){
					memory.first[j]=itr->first;
					memory.second[j]+=itr->second;
				
			}
			else{
				memory.first.insert(memory.first.begin()+j,itr->first);
				memory.second.insert(memory.second.begin()+j,itr->second);
			}
		}
				else{
				memory.first.insert(memory.first.begin()+j,itr->first);
				memory.second.insert(memory.second.begin()+j,itr->second);
			}
			}
			itr->first=NULL;
			break;
		}
	}
}
