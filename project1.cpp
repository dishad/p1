
#include <iostream>
#include <sys/types.h>
#include <list>
#include <utility>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <cstdlib>
#include <mutex>
#include <map>
#include <time.h>

/*TODO:
	outline of sending messages w/o actually sending message
	map of users to (map of users to int) = knowVect
	mutex for knowVect, eLog, dic
	load config file
	user input

*/

//std::map<std::string, std::map<std::string, int> > know; -Dyanmic memory?

class event{//class to contain an event in the log
public:
	event(std::string t, std::string u, std::string a, time_t eT){
		type = t;
		user = u;
		arg = a;
		eTime = eT;
	}
	const std::string getType(){return type;};
	const std::string getUser(){return user;};
	const std::string getArg(){return arg;};
	const time_t getRawTime(){return eTime;};

private:
	std::string type;// "tweet", "block" or "unblock"
	std::string user;// username of the site that made the event
	std::string arg;//message for tweet / blocked or unblocked user
	time_t eTime; //Raw
};

std::mutex mtx;

bool compareEvent(event& first, event& second){
	//compares two events, sorting them based on the time in UTC, regardless of base time
	struct tm* fir;
	struct tm* sec;
	time_t firRaw = first.getRawTime();
	time_t secRaw = second.getRawTime();
	fir = gmtime(&firRaw);
	sec = gmtime(&secRaw);
	return difftime(mktime(fir),mktime(sec)) <0;
}

void loadLogKnowDic(std::list<event> *eLog, std::list<std::pair<std::string, std::string> > *dic, std::map<std::string, std::map<std::string, int> > *know){
	//arguments initalized, but empty

	std::fstream logFi;
	std::fstream knowFi;
	std::fstream dicFi;

	logFi.open("log.txt", std::fstream::in);
	if(!logFi){
	}
	else{
		while (logFi.peek()!=EOF){
			std::string type;
			std::string user;
			std::string arg;
			std::string timeTmp1;
			int timeTmp2;
			time_t timeT;
			std::getline(logFi, type, '|');
			std::getline(logFi, user, '|');
			std::getline(logFi, arg, '|');
			std::getline(logFi, timeTmp1);

			timeTmp2 = atoi(timeTmp1.c_str());
			timeT = (time_t) timeTmp2;

			eLog->push_back(event(type, user, arg, timeT));
			eLog->sort(compareEvent);
		}
		logFi.close();
	}

	dicFi.open("dic.txt", std::fstream::in);
	if(!dicFi){
	}
	else{
		while (dicFi.peek()!=EOF){
			std::string fir;
			std::string sec;
			std::getline(dicFi, fir, '|');
			std::getline(dicFi, sec);
			dic->push_back(std::pair<std::string, std::string>(fir, sec));
		}
		dicFi.close();
	}

	knowFi.open("know.txt", std::fstream::in);
	if(!knowFi){
	}
	else{
		while(knowFi.peek()!=EOF){
			std::string fir;
			std::string sec;
			std::string tmpInt;
			std::getline(knowFi, fir, '|');
			std::getline(knowFi, sec, '|');
			std::getline(knowFi, tmpInt);
			(*know)[fir][sec] = atoi(tmpInt.c_str());
		}
		knowFi.close();
	}

}

void updateLogKnowDic(std::list<event> *eLog, std::list<std::pair<std::string, std::string> > *dic, std::map<std::string, std::map<std::string, int> > *know){
	//truncate log
	std::list<event>::iterator it = eLog->begin();
	for(; it!= eLog->end(); it++){
		if(it->getType().compare("unblock")==0){
			
			int etime = (int)it->getRawTime();
			bool knows = true;
			std::map<std::string, std::map<std::string, int> >::iterator knowIt = know->begin();
			for(; knowIt!= know->end(); knowIt++){
				if(knowIt->second[it->getUser()]>=etime)
					knows = false;
			}
			if(knows){
				bool foundBlock = false;
				std::list<event>::iterator itr = eLog->begin();
				for(; itr!=eLog->end(); itr++){
					if(itr->getType().compare("block")==0 && 
						itr->getUser().compare(it->getUser())==0 &&
						itr->getArg().compare(it->getArg())==0){
						foundBlock = true;
						break;
					}
				}
				if(foundBlock){
					
					it = eLog->erase(it);
					eLog->erase(itr);
					it--;
				}
			}
		}
	}
/*
	//write to log and dic and know file
	std::fstream logFi;
	std::fstream knowFi;
	std::fstream dicFi;

	logFi.open("log.txt", std::fstream::out | std::fstream::trunc);
	std::list<event>::iterator itOut = eLog->begin();
	std::cout<<"ENTERING LOG WRITING"<<std::endl;
	for(; itOut!= eLog->end(); itOut++){
		std::cout<<it->getType()<<std::endl;
		logFi<<it->getType()<<"|"<<it->getUser()<<"|"<<it->getArg()<<"|"<<(int)it->getRawTime()<<std::endl;
	}
	logFi.close();

	dicFi.open("dic.txt", std::fstream::out | std::fstream::trunc);
	std::list<std::pair<std::string, std::string> >::iterator it2 = dic->begin();
	std::cout<<"ENTERING DIC WRITING"<<std::endl;
	for(; it2!= dic->end(); it2++){
		dicFi<<it2->first<<"|"<<it2->second<<std::endl;
	}
	dicFi.close();

	knowFi.open("know.txt", std::fstream::out | std::fstream::trunc);
	std::map<std::string, std::map<std::string, int> >::iterator knowIt1 = know->begin();
	std::cout<<"ENTERING KNOW WRITING"<<std::endl;
	for(; knowIt1!= know->end(); knowIt1++){
		std::map<std::string, int>::iterator knowIt2 = knowIt1->second.begin();
		for(; knowIt2 != knowIt1->second.end(); knowIt2++){
			knowFi<<knowIt1->first<<"|"<<knowIt2->first<<"|"<<knowIt2->second<<std::endl;
		}
	}
	knowFi.close();

*/
}

bool addEvent(std::list<event> *eLog, std::list<std::pair<std::string, std::string> > *dic, event e){
	std::string eType = e.getType();
	if(eType.compare("tweet")==0){//if the event to be added is a tweet
		//the tweet in question is assumed to already have been confirmed to not be in the log
		eLog->push_back(e);
		eLog->sort(compareEvent);
		//TODO: Send message funtion call here
	}

	else if(eType.compare("block")==0){//if the event to be added is a block
		std::list<std::pair<std::string, std::string> >::iterator it = dic->begin();
		bool found = false;
		for (; it!=dic->end(); it++){
			if(it->first.compare(e.getUser())==0){
				if (it->second.compare(e.getArg())==0){//found a block instance already in dictionary
					found = true;
					break;
				}
			}	
		}
		if (!found){//there is not a block in the dictionary
			//if block already exists, further blocking is unnecessary
			eLog->push_back(e);//add the new event
			eLog->sort(compareEvent);
			std::pair<std::string, std::string> tmpPare (e.getUser(), e.getArg());
			dic->push_back(tmpPare);
		}
	}

	else if(eType.compare("unblock")==0){//the event ot be added is an unblock
		std::list<std::pair<std::string, std::string> >::iterator it = dic->begin();
		bool found = false;
		for (; it!=dic->end(); it++){//search for corresponding block event.
			//if no block is found, no action is necessary
			if(it->first.compare(e.getUser())==0){
				if (it->second.compare(e.getArg())==0){
					found = true;
					break;
				}
			}	
		}
		if (found){//found a corresponding block, so remove it
			dic->erase(it);//remove block from the dictionary
			eLog->push_back(e);//add unblock to log
			eLog->sort(compareEvent);
		}
	}
	else{
		return false;//event type not recognized. Send soft error message
	}
	return true;
}

void viewEvents(std::list<event> *eLog, std::list<std::pair<std::string, std::string> > *dic, std::string current){
	std::list<event>::iterator it = eLog->begin();
	for(; it!=eLog->end(); it++){//iterate through the events, looking for tweets
		if(it->getType().compare("tweet")==0){
			std::string user = it->getUser();
			bool found = false;//check to see if the tweeter is blocking the user
			std::list<std::pair<std::string, std::string> >::iterator itr = dic->begin();
			for(; itr!=dic->end(); itr++){
				if(itr->first.compare(user)==0 && itr->second.compare(current)==0){
					found = true;
					break;
				}
			}
			if(!found){//the user is not blocked, so display the tweet
				time_t tmp = (it->getRawTime());
				struct tm* timeOut = gmtime(&tmp);
				std::cout<<user<<" at "<<(timeOut->tm_hour)%24<<":"<<timeOut->tm_min
					<<" tweeted:"<<std::endl;
				std::cout<<it->getArg()<<std::endl<<std::endl;
			}
		}
	}
}

void viewLog(std::list<event> *eLog){
	std::list<event>::iterator it = eLog->begin();
	for(; it!= eLog->end(); it++){
		std::cout<<it->getType()<<" | "<<it->getUser()<<" | "<<it->getArg()<<" | "<<it->getRawTime()<<std::endl;
	}
}

void viewDic(std::list<std::pair<std::string, std::string> > *dic){
	std::list<std::pair<std::string, std::string> >::iterator it = dic->begin();
	for(; it!=dic->end(); it++){
		std::cout<<it->first<<" | "<<it->second<<std::endl;
	}
}

bool userInput(std::list<event> *eLog, std::list<std::pair<std::string, std::string> > *dic, std::map<std::string, std::map<std::string, int> > *know, std::string user){
	while(true){
		std::cout<<"Please input command:"<<std::endl;
		std::string command;
		std::getline(std::cin, command);
		if(command.length()>=4){
			if(command.substr(0,4).compare("quit")==0){
				return false;
			}
			else{
				mtx.lock();
				if(command.substr(0,4).compare("view")==0&&command.length()==4){
					viewEvents(eLog, dic, user);
				}
				else if (command.substr(0,7).compare("viewlog")==0){
					viewLog(eLog);
				}
				else if(command.substr(0,7).compare("viewdic")==0){
					viewDic(dic);
				}
				else if (command.substr(0, 5).compare("tweet")==0){
					time_t currTime;
					time(&currTime);
					if(addEvent(eLog, dic, event(command.substr(0,5), user, command.substr(6), currTime))){

						(*know)[user][user] = (int) currTime;
					}

					updateLogKnowDic(eLog, dic, know);
				}
				else if (command.substr(0, 5).compare("block")==0){
					time_t currTime;
					time(&currTime); 
					if(addEvent(eLog, dic, event(command.substr(0,5), user, command.substr(6), currTime))){
						(*know)[user][user] = (int) currTime;
					}
					updateLogKnowDic(eLog, dic, know);
				}
				else if (command.substr(0, 7).compare("unblock")==0){
					time_t currTime;
					time(&currTime); 
					if(addEvent(eLog, dic, event(command.substr(0,7), user, command.substr(8), currTime))){
						(*know)[user][user] = (int) currTime;
					}
					updateLogKnowDic(eLog, dic, know);
				}
				else{
					std::cout<<"Invalid command"<<std::endl;
				}
				mtx.unlock();
			}
		}
		else
			std::cout<<"Invalid command"<<std::endl;
	}
	return false;
}



int main(){
	/*
	pid_t pid;
	pid = fork();
	if(pid==0){
		std::cout<<"Child "<<pid<<std::endl;
	}
	else
		std::cout<<"Parent "<<pid<<std::endl;
	/*
	time_t temp;
	time(&temp);
	std::cout<<(int)temp<<std::endl;*/

	std::list<event> *eLog = new std::list<event>;
	std::list<std::pair<std::string, std::string> > *dic = new std::list<std::pair<std::string, std::string> >;
	std::map<std::string, std::map<std::string, int> > *know = new std::map<std::string, std::map<std::string, int> >;
	std::string* user = new std::string;
	bool * Arunning = new bool;
	bool * Brunning = new bool;
	*Brunning = true;
	*Arunning = true;
	loadLogKnowDic(eLog, dic, know);

	//run comm thread
	*user = "TMP";
	if(!userInput(eLog, dic, know, *user)){
		*Arunning = false;
	}
	while(*Brunning){
		*Brunning = false;
	}
	delete eLog;
	delete dic;
	delete know;
	delete Arunning;
	delete Brunning;


}

