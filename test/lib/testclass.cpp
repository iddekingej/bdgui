#include "testclass.h"
#include <iostream>
#include "base/utils.h"
#include "../../data/device.h"
#include "../../data/partition.h"

void TTestClass::error(const char * p_file,const char *p_method,const char* p_message)
{
	std::cout << p_file << ":" << p_method <<":" << p_message <<std::endl;
	failed=true;
}

void TTestClass::errorv(const char* p_file, const char* p_method, const char* p_message, const char *p_result)
{
	std::cout << p_file << ":" << p_method <<":" << p_message <<" returned:" << p_result <<std::endl;
	failed=true;

}

void TTestClass::errorv(const char* p_file, const char* p_method, const char* p_message, QString p_value)
{
	errorv(p_file,p_method,p_message,qstr(p_value));
}


void TTestClass::errorv(const char* p_file, const char* p_method, const char* p_message, long p_value)
{
	std::cout << p_file << ":" << p_method <<":" << p_message <<" returned:" << p_value <<std::endl;
	failed=true;
}


bool TTestClass::fexpect(const char* p_file, const char* p_method, const char* p_message, long p_expected, long p_found)
{
	if(p_expected != p_found){
		std::cout << p_file << ":" <<p_method <<":" << p_message <<": expected " << p_expected << " returned "<<p_found <<std::endl;
		failed=true;
		return true;
	} 
	return false;
}


bool TTestClass::run()
{

	doRun();	
	return failed;
}

void TTestClass::doRun(){
	error(__FILE__,__FUNCTION__,"Called TTestClass::doRun");
}

TAlias* TTestClass::createAliasList()
{
	return new TAlias();
}

TDeviceList * TTestClass::createDeviceList(TAlias *p_alias)
{
	TDeviceList *l_deviceList =new TDeviceList(p_alias);
	TDevice *l_sda=new TDevice("sda","m1",1024*1024);
	l_sda->addParition("sda1",1024*5,5);
 	l_sda->addParition("sda2",1024*10,1024*6);
	TDevice *l_sdb=new TDevice("sda","m1",1024*1024);
	l_sdb->addParition("sdb",1024*5,5); 	

	l_deviceList->append(l_sda);
	l_deviceList->append(l_sdb);
	return l_deviceList;
}

QString TTestClass::getFilePath(const QString& p_path)
{
	return QStringLiteral("../../test/")+p_path;
}

