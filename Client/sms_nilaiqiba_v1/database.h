#ifndef DATABASE_H
#define DATABASE_H
#define MAX 110000
#include<string>
#include<set>
#include<map>
#include"sms_file.h"
#include"contact.h"
#include"sms.h"

using namespace std;

class Database {
private:
    set<int> free_space;
    set<int> occu_space;
    map<string,Contact> ip_contact;//the map of Contacts map<ip,contact>
    map<string,string> contact_map;//map<time,ip>
    string number;
    string ip_server;

public:
    Sms_file file;
    /****************************************
    Sms** search (str: Sc);//~~
    boolean delete (Contact: string);//~~
    boolean delete (con: Sms*);//~~
    boolean delete (id: int);//~~
    boolean add (dx: Sms);//~~
    bool add (vector: Sms);
    Contact* getContactList();//~~
    Sms* getSmsList(con: Contact*);//~~
    Sms** getSearch(str: Sc);//
    boolean sending(dx: Sms);
    boolean setIp(Ip: String);//~~
    Contact get_contact();//~~
    *****************************************/
public:

    Database() {
        free_space.clear();
        occu_space.clear();
        file.init();
        for(int i = 0; i < MAX; ++ i)free_space.insert(i);
        number = "empty";
        comeon_ip_contact();
        comeon_contact_map();
        comeon_smss();
        comeon_ip_number();     //update
    }

    int get_sms_number(){
        return occu_space.size();
    }


    void update_contact(Sms ins,string ip) {
        Contact contact(ip,"未命名",ins.get_time());
        ip_contact.insert(pair<string,Contact>(ip,contact));
    }


    bool Delete(int id) { //删除单个短信
        if(occu_space.find(id) == occu_space.end()) {
            return false;//不存在该短信
        }
        int temid = id;
        while(temid != -1){
            free_space.insert(temid);//插入空闲区域
            occu_space.erase(temid);
            temid = file.findfile(temid).is_end();
        }
        Sms tem_sms = file.findfile(id);
        string tem_ip = tem_sms.get_cont();//找到了该短信联系人的ip->tem_ip
        map<string,Contact>::iterator it = ip_contact.find(tem_ip);//获取该联系人
        contact_map.erase((it->second).get_time());

        (it->second).Sms_self.erase(tem_sms.get_time());//updatev1.2  (it->second) -> tem_sms
        if((it->second).Sms_self.size() != 0) (it->second).set_time((it->second).Sms_self.begin()->first);//获取联系人短信树中最新短信时间赋值

        if((it->second).Sms_self.size() != 0) contact_map.insert(pair<string,string>((it->second).get_time(),tem_ip));//更新联系人树
        else (it->second).Sms_self.clear();
        return true;
    }


    bool Delete(string ip) {
        map<string,Contact>::iterator its = ip_contact.find(ip);
        if(its == ip_contact.end()) return false;

        map<string,int>::iterator it;
        for(it = (its->second).Sms_self.begin(); it != (its->second).Sms_self.end(); ++ it) {
            int temid = it->second;
            while(temid != -1){
                free_space.insert(temid);//插入空闲区域
                occu_space.erase(temid);
                temid = file.findfile(temid).is_end();
            }
        }
        //挨个标记短信空间空余

        (its->second).Sms_self.clear();
        contact_map.erase((its->second).get_time());//删除该联系人的短信树中的信息
        return true;
    }


    int Add(Sms ins) {//改动，返回值 是插入的地址id
        if(ins.get_text_length() > 70) {
            return Add_long(ins);
        }
        set<int>::iterator it = free_space.begin();
        int id = *it;
        ins.set_id(id);
        if(!file.insertfile(ins,id)) return -1;
        free_space.erase(it);
        occu_space.insert(*it);
        //插入短信并且删除空闲标记

        string ip = ins.get_cont();
        if(ip_contact.find(ip) == ip_contact.end()) {
            update_contact(ins,ip);
        }
        map<string,Contact>::iterator itc = ip_contact.find(ip);
        contact_map.erase((itc->second).get_time());//删除旧的记录
        (itc->second).Sms_self.insert(pair<string,int>(ins.get_time(),id));//更新联系人短信map
        (itc->second).set_time(ins.get_time());//更新联系人时间
        contact_map.insert(pair<string,string>(ins.get_time(),ip));//更新联系人树
        return id;
    }


    map<string,string> get_contact_list() {
        return contact_map;
    }


    map<string,int> get_sms_list(string ip) {
        Contact tem_con = ip_contact.find(ip)->second;
        return tem_con.Sms_self;
    }


    map<string,int> search(string str) {
        map<string,int> ret;
        set<int>::iterator it;
        Sms tem_sms;
        for(it = occu_space.begin(); it != occu_space.end(); ++ it) {
            tem_sms = file.findfile(*it);
            if(tem_sms.get_text().find(str) != tem_sms.get_text().npos) {
                if(tem_sms.get_start() != -1) tem_sms = file.findfile(tem_sms.get_start());
                ret.insert(pair<string,int>(tem_sms.get_time(),tem_sms.get_id()));
            }
        }

        //cout << ret.size() << endl;
        return ret;
    }


    bool Delete(map<string,int> sms_list) {
        map<string,int>::iterator it;
        for(it = sms_list.begin(); it != sms_list.end(); ++ it) {
            Delete(it->second);
        }
        return true;
    }


    Contact get_contact(string ip) {
        return ip_contact.find(ip)->second;
    }

    Sms get_sms(int id) {
        //return file.findfile(id);
        Sms sms = file.findfile(id);
        Sms tsms = sms;
        while(1){
            //cout << tsms.get_id() << " " << tsms.is_end() << endl;
            tsms = file.findfile(tsms.is_end());
            sms.add_text(tsms.get_text());
            if(tsms.is_end() == -1) break;
        }
        return sms;
    }


    void change_name(string name,string ip) {
        Contact tem = ip_contact.find(ip)->second;
        tem.set_name(name);
        ip_contact.erase(ip);
        ip_contact.insert(pair<string,Contact>(ip,tem));
    }


    bool shut_down() {
       // cout << "hshs" << endl;
        record_ip_contact();
        record_contact_map();
        record_smss();
        record_ip_number();
        return true;
    }

    string get_number(){
        return number;
    }

    string get_ip_server() {        //update
        return ip_server;
    }


    void set_number(string n){
        number = n;
    }

private:

    int insert_sms(Sms ins){
        set<int>::iterator it = free_space.begin();
        ins.set_id(*it);
        if(!file.insertfile(ins,*it)) return -1;
        free_space.erase(it);
        occu_space.insert(*it);
        return *it;
    }

    vector<Sms> cut_sms(Sms long_sms){
        vector<Sms> smss;
        smss.clear();
        string str;
        str.clear();
        string text = long_sms.get_text();
        int ret = 0;
        for(int i = 0; i < (int)text.length(); i++) {
            if(text[i] & 0x80 && (unsigned)text[i] >= 0) {
                str.append(1,text[i]);
                str.append(1,text[i + 1]);
                i += 2;
            }
            str.append(1,text[i]);
            ret++;
            if(!(ret % 70)) {
                Sms newsms(long_sms.get_from(), long_sms.get_to(), long_sms.get_cont(), str, -1);
                str.clear();
                smss.push_back(newsms);
            }
        }
        if(str.size()){
            Sms newsms(long_sms.get_from(), long_sms.get_to(), long_sms.get_cont(), str, -1);
            str.clear();
            smss.push_back(newsms);
        }
        return smss;
    }


    int Add_long(Sms long_sms){
        vector<Sms> smss = cut_sms(long_sms);
        int id;
        vector<Sms>::iterator it = smss.end();
        it --;
        int startid = Add(*(smss.begin()));
        while(it != smss.begin()){
            (*it).set_start(startid);
            id = insert_sms(*it);
            -- it;
            (*it).set_isend(id);
        }
        Sms sms_start = file.findfile(startid);
        sms_start.set_isend(id);
        file.insertfile(sms_start,startid);
        return startid;
    }


    bool record_ip_contact() {
        ofstream("ip_contact_database").close();
        fstream in("ip_contact_database");
        assert(in);
        in.seekg(0,ios::beg);
        in << ip_contact.size() << endl;
        map<string,Contact>::iterator it;
        for(it = ip_contact.begin(); it != ip_contact.end(); it ++) {
            in << it->first << " " << it->second;
        }
        //cout  << number << endl;
        in.close();
        return true;
    }


    bool record_contact_map() {
        ofstream("contact_map_database").close();
        fstream in("contact_map_database");
        assert(in);
        in.seekg(0,ios::beg);
        in << contact_map.size() << endl;
        map<string,string>::iterator it;
        for(it = contact_map.begin(); it != contact_map.end(); it ++) {
            in << it->first << " " << it->second << endl;
        }
        in.close();
        return true;
    }


    bool record_smss() {
        ofstream("smss_database").close();
        fstream in("smss_database");
        assert(in);
        in.seekg(0,ios::beg);
        in << contact_map.size() << endl;
        map<string,string>::iterator it;
        map<string,int>::iterator itc;
        Contact cont;
        string ip;
        for(it = contact_map.begin(); it !=contact_map.end(); ++ it) {
            ip = it->second;
            cont = ip_contact.find(ip)->second;
            in << cont.Sms_self.size() << " " << cont.get_ip() << endl;
            for(itc = cont.Sms_self.begin(); itc != cont.Sms_self.end(); ++ itc) {
                in << itc->first << " " << itc->second << " " << endl;
            }
        }
        in << occu_space.size() << " ";
        set<int>::iterator iti;
        for(iti = occu_space.begin();iti != occu_space.end();iti ++){
            in << (*iti) << " ";
        }
        return true;
    }

    bool record_ip_number() {           //update
        ofstream("ip_number").close();
        fstream in("ip_number");
        assert(in);
        in.seekg(0,ios::beg);
        in << ip_server << endl;
        in << number << endl;
        in.close();
    }

    bool comeon_ip_contact() {
        fstream in("ip_contact_database");
        assert(in);
        in.seekg(0,ios::beg);
        int num;
        in >> num;
        string ip;
        Contact cont;
        for(int i = 0; i < num; ++ i) {
            in >> ip >> cont;
            ip_contact.insert(pair<string,Contact>(ip,cont));
        }

        //  cout << ip_contact.size() << ip_contact.begin()->second << endl;
        in.close();
        return true;
    }


    bool comeon_contact_map() {
        fstream in("contact_map_database");
        assert(in);
        in.seekg(0,ios::beg);
        int num;
        in >> num;
        string ip;
        string time;
        for(int i = 0; i < num; ++ i) {
            in >> time >> ip;
            contact_map.insert(pair<string,string>(time,ip));
        }
        in.close();
        return true;
    }


    bool comeon_smss() {
        fstream in("smss_database");
        assert(in);
        in.seekg(0,ios::beg);
        int num;
        in >> num;
        string ip;
        string tim;
        int id;
        int num0;
        Contact cont;
        for(int i = 0; i < num; ++ i) {
            in >> num0 >> ip;
            cont = ((ip_contact.find(ip))->second);
            ip_contact.erase(ip);
            for(int j = 0; j < num0; ++ j) {
                in >> tim >> id;
                cont.Sms_self.insert(pair<string,int>(tim,id));
            }
            ip_contact.insert(pair<string,Contact>(ip,cont));
        }
        in >> num0;
        int tem;
        for(int i = 0;i < num0;++ i){
            in >> tem;
            occu_space.insert(tem);
            free_space.erase(tem);
        }
        in.close();
        return true;
    }


    bool comeon_ip_number() {       //update
        fstream in("ip_number");
        assert(in);
        in.seekg(0,ios::beg);
        in >> ip_server;
        in >> number;
        in.close();
    }

};

#endif // DATABASE_H
