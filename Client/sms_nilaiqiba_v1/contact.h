#ifndef CONSTACT_H
#define CONSTACT_H
#include<map>
#include<string>
#include<iostream>
#include<fstream>
using namespace std;

class Contact {
    //
private:
    string ip;//该联系人的绑定ip
    string name;//昵称
    string time;//联系人最近一条信息的时间


public:

    map<string,int> Sms_self;//联系人的map<time,id>

    Contact () {
        Sms_self.clear();
    }

    Contact(string _ip,string _name,string _time): ip(_ip), name(_name), time(_time){
    }

    string get_ip() {
        return ip;
    }

    string get_time(){
        return time;
    }

    string get_name(){
        return name;
    }


    void set_name(string _name){
        name = _name;
    }

    void set_time(string _time){
        time = _time;
    }

    friend ostream& operator << (ostream &o, const Contact &s) {
        o << s.ip << " " << s.name << " " << s.time << endl;
        return o;
    }

    friend fstream& operator << (fstream &o, const Contact &s) {
        o << s.ip << " " << s.name << " " << s.time << endl;
        return o;
    }

    friend istream& operator >> (istream &i, Contact &s) {
        i >> s.ip >> s.name >> s.time;
        return i;
    }

    friend fstream& operator >> (fstream &i, Contact &s) {
        i >> s.ip >> s.name >> s.time;
        return i;
    }
};

#endif // Contact_H
