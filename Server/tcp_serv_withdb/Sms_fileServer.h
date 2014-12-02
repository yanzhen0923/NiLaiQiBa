#ifndef SMS_FILESERVER_H
#define SMS_FILESERVER_H
#include"Contact.h"
#include"DatabaseServer.h"
#include"Sms.h"
#include <iostream>
#include <fstream>
#include <assert.h>
using namespace std;

class Sms_fileServer
{
    public:
		void init() {
		    fstream in("smsdatabase_server");
		    if(!in) {
		        ofstream ("smsdatabase_server").close();
		        ofstream ("contact_map_database_server").close();
		        ofstream ("ip_contact_database_server").close();
		        ofstream ("smss_database_server").close();

		        fstream in1("contact_map_database_server");
		        fstream in2("ip_contact_database_server");
		        fstream in3("smss_database_server");
		        in1 << 0 << endl;
		        in2 << 0 << endl;
				in2 << 0 << endl;
		        in3 << 0 << endl;
		        in1.close();
		        in2.close();
		        in3.close();
		    }
		    in.close();
		}

        Sms findfile(int id) {
            Sms ret;
            fstream in("smsdatabase_server");
            assert(in);
            in.seekg(id * 300,ios::beg);
            in >> ret;
            in.close();
            return ret;
        }

        bool insertfile(Sms ins,int id) {

            fstream in("smsdatabase_server");
            assert(in);
            in.seekg(id * 300,ios::beg);
            in << ins << endl;
            in.close();
            return true;
        }



};

#endif // SMS_FILESERVER_H
