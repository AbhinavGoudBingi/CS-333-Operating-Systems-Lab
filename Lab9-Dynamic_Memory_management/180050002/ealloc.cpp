#include<bits/stdc++.h>
#include "ealloc.h"
using namespace std;

vector<tuple<void*,int,int> >allocated;
pair<vector<void*>,vector<int> > memory[4];
void* start[4];
int page_number = 0;

void init_alloc(){
	for(int i=0;i<4;i++){
	memory[i].first.push_back((void*)0);
  	memory[i].second.push_back(0);
	}
}

void cleanup(){
	for(int i=0;i<4;i++){
	memory[i].first.clear();
  	memory[i].second.clear();
	}
}

char* alloc(int mem){
	if(mem%MINALLOC!=0){
		return NULL;
	}
	else{
		tuple<void*,int,int> temp;
		temp=make_tuple((void*)0,mem,-1);
		int temp1=-1;
		int temp2 = -1;
		for(int k=0;k<4;k++){
    	for(int i=0;i<(memory[k].second).size();i++){
    		if(mem<=memory[k].second[i]){
    		temp1=i;
    		temp2 = k;
    		break;
    		}
    	}
    }
	if(temp1 == -1){
	start[page_number] = mmap(NULL, PAGESIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	memory[page_number].first[0] = start[page_number];
	memory[page_number].second[0] = PAGESIZE;
	temp2 = page_number;
	page_number++;
	temp1 = 0;
	}
	
	get<0>(temp)=memory[temp2].first[temp1];
	get<2>(temp)=temp2;
	allocated.push_back(temp);
		if(mem == memory[temp2].second[temp1]){
          memory[temp2].first.erase(memory[temp2].first.begin()+temp1);
          memory[temp2].second.erase(memory[temp2].second.begin()+temp1);
		}
		else{
		memory[temp2].first[temp1]+=mem;
		memory[temp2].second[temp1]-=mem;
		}
	return (char*)get<0>(temp);

	}
}

void dealloc(char* destart){

	for(auto itr=allocated.begin();itr!=allocated.end();itr++){
		if((char*)get<0>(*itr) == destart){
			int page = get<2>(*itr);
			int j= lower_bound(memory[page].first.begin(),memory[page].first.end(),get<0>(*itr))-memory[page].first.begin();
			if(j>0&&(get<0>(*itr) == memory[page].first[j-1]+memory[page].second[j-1])){
				memory[page].second[j-1]+=get<1>(*itr);
				if(memory[page].first.size()>j){	
				if((get<0>(*itr)+get<1>(*itr))==memory[page].first[j]){
					memory[page].second[j-1]+=memory[page].second[j];
					memory[page].first.erase(memory[page].first.begin()+j);
					memory[page].second.erase(memory[page].second.begin()+j);
				}
			}
			}
			else{
				if(memory[page].first.size()>j){
				if((get<0>(*itr)+get<1>(*itr))==memory[page].first[j]){
					memory[page].first[j]=get<0>(*itr);
					memory[page].second[j]+=get<1>(*itr);
				
			}
			else{
				memory[page].first.insert(memory[page].first.begin()+j,get<0>(*itr));
				memory[page].second.insert(memory[page].second.begin()+j,get<1>(*itr));
			}
		}
				else{
				memory[page].first.insert(memory[page].first.begin()+j,get<0>(*itr));
				memory[page].second.insert(memory[page].second.begin()+j,get<1>(*itr));
			}
			}
			get<0>(*itr)=(void*)0;
			break;
		}
	}
}
