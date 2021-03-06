#ifndef SMS_FILE_H
#define SMS_FILE_H
#include"contact.h"
#include"sms.h"
#include <iostream>
#include <fstream>
#include <assert.h>
using namespace std;

class Sms_file {
public:

    Sms findfile(int id) {
        Sms ret;
        fstream in("smsdatabase");
        assert(in);
        in.seekg(id * 300,ios::beg);
        in >> ret;
        in.close();
        return ret;
    }

    bool insertfile(Sms ins,int id) {

        fstream in("smsdatabase");
        assert(in);
        in.seekg(id * 300,ios::beg);
        in << ins << endl;
        in.close();
        return true;
    }

    void init() {
        fstream in("smsdatabase");
        if(!in) {
            ofstream ("smsdatabase").close();
            ofstream ("contact_map_database").close();
            ofstream ("ip_contact_database").close();
            ofstream ("smss_database").close();
            ofstream ("ip_number").close();     //update

            fstream in1("contact_map_database");
            fstream in2("ip_contact_database");
            fstream in3("smss_database");
            fstream in4("ip_number");           //update
            in1 << 0;
            in2 << 0;
            in3 << 0 << endl << 0;
            in4 << "127.0.0.1" << endl;         //update
            in4 << "empty" << endl;
            in1.close();
            in2.close();
            in3.close();
            in4.close();                        //update
        }
        in.close();
    }

};

#endif // SMS_FILE_H
