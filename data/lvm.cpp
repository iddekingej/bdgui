#include "lvm.h"
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdio.h>
#include <QString>
#include <iostream>
#include "base/stringlineiterator.h"
#include "base/utils.h"
#include <QFile>

using namespace std;

TPhysicalVolume::TPhysicalVolume()
{
	volumeGroup=nullptr;
}


void TPhysicalVolumeList::processList(TVolumeGroupList* p_vgList)
{
	TLinkListIterator<TPhysicalVolume> l_iter(this);
	TPhysicalVolume *l_item;
	TVolumeGroup    *l_vg;
	while(l_iter.hasNext()){
		l_item=l_iter.next();
		l_vg=p_vgList->getVolumeGroupById(l_item->getVgId());
		l_item->setVolumeGroup(l_vg);
		if(l_item->getRealDevice() !=nullptr){
			l_item->getRealDevice()->setVGName(l_item->getVgName());
		}		
	}
}


TLVMResponseParser::TLVMResponseParser(QString& p_text)
{
	text=p_text;
	sectionType=st_top;
	iter=new TStringLineIterator(p_text);
}

TLVMResponseParser::~TLVMResponseParser()
{
	delete iter;
}


bool TLVMResponseParser::chapter(QString p_item)
{
	return false;
}

void TLVMResponseParser::setVar(QString &p_name, QString &p_value)
{
}


void TLVMResponseParser::parseChapter()
{
	QString l_oldPrefix;
	TSectionType l_oldType;
	bool l_namespace;
	while(iter->hasNext()){
		QString l_str=iter->next();
		
		if(l_str.endsWith("{")){				
			l_oldPrefix=prefix;
			l_oldType=sectionType;
			QString l_key=l_str.mid(0,l_str.length()-1).trimmed();
			l_namespace=chapter(l_key);
			if(l_namespace){
				if(prefix.length()>0) prefix += "_";
				prefix += l_key;
			}
			parseChapter();
			prefix=l_oldPrefix;
			sectionType=l_oldType;
		} else 	if(l_str.trimmed()=="}"){
			return;
		} else {
			int l_split=l_str.indexOf("=");
			if(l_split>=0){
				QString l_pre=l_str.mid(0,l_split).trimmed();
				if(prefix.length()>0) l_pre=prefix+"_"+l_pre;
				QString l_after=l_str.mid(l_split+1).trimmed();
				if(l_after.startsWith('"') && l_after.endsWith('"')){
					l_after=l_after.mid(1,l_after.length()-2);
				}
				setVar(l_pre,l_after);
			}
				
		}
	}
}


void TLVMResponseParser::parse()
{
	parseChapter();
}


TPVParser::TPVParser(TDeviceList* p_devList, QString& p_text):TLVMResponseParser(p_text)
{
	items=new TPhysicalVolumeList();
	current=nullptr;
	devList=p_devList;
}

bool TPVParser::chapter(QString p_item)
{
	bool l_namespace=false;
	if(sectionType==st_top){
		sectionType=st_data;
	} else if(sectionType==st_data){
		sectionType=st_pv;
		current=new TPhysicalVolume();
		current->setKey(p_item);
		items->append(current);		
	} else if(sectionType==st_pv){
		sectionType=st_da0;
		l_namespace=true;
	}
	return l_namespace;
}

void TPVParser::setVar(QString& p_name, QString& p_value)
{	
	if(p_name=="id") current->setId(p_value);
	else if(p_name=="device"){
		TDeviceBase *l_db=devList->getDeviceByDeviceNo(p_value.toLong());
		current->setDevice(p_value.toLong());
		current->setRealDevice(l_db);
	}
	else if(p_name=="dev_size") current->setDevSize(p_value.toLongLong());
	else if(p_name=="vgname") current->setVgName(p_value);
	else if(p_name=="vgid") current->setVgId(p_value);
	else if(p_name=="dao_size") current->setDaoSize(p_value.toLongLong());
	else if(p_name=="dao_offset") current->setDaoOffset(p_value.toLongLong());
	else if(p_name=="mdao_freesectors") current->setMdaoFreeSectors(p_value.toLongLong());
	else if(p_name=="mdao_size") current->setMdaoSize(p_value.toLongLong());
	else if(p_name=="mdao_start") current->setMdaoStart(p_value.toLongLong());
	else if(p_name=="mdao_ignore") current->setMdaoIgnore(p_value.toLongLong());
	else if(p_name=="id") current->setId(p_value);
	else if(p_name=="label_sector") current->setLabelSector(p_value.toLongLong());
	else if(p_name=="format")  current->setFormat(p_value);
	
}



TVGMainParser::TVGMainParser(QString& p_text):TLVMResponseParser(p_text)
{
	items=new TVolumeGroupList();
}


bool TVGMainParser::chapter(QString p_item)
{
	if(sectionType==st_top){
		sectionType=st_data;
	} else if(sectionType==st_data){
		sectionType=st_vg;
		current=new TVolumeGroup();
		items->append(current);
		current->setKey(p_item);
		cout <<qstr(p_item) <<endl;
  	}
	return false;
}


void TVGMainParser::setVar(QString& p_name, QString& p_value)
{
	if(p_name=="name")current->setName(p_value);
}



bool TVGParser::chapter(class QString p_item)
{
	if(sectionType==st_top){
		sectionType=st_vg;
	} else if(sectionType==st_vg){
		if(p_item=="logical_volumes"){
			sectionType=st_lvsection;
		}
	} else if(sectionType==st_lvsection){
		currentLv=current->addLv(p_item);
		sectionType=st_lv;
	}
	return false;
}




void TVGParser::setVar(class QString& p_name, class QString& p_value)
{
	if(sectionType==st_lv){
		if(p_name=="id") currentLv->setId(p_value);
	}
}


TVGParser::TVGParser(TVolumeGroup* p_item, QString& p_text):TLVMResponseParser(p_text)
{
	current=p_item;
}


TLogicalVolume * TVolumeGroup::getLVByName(QString& p_name)
{
	QString l_name=getName();
	TLinkListIterator<TLogicalVolume> l_iter(&logicalVolumns);
	TLogicalVolume *l_lv;
	while(l_iter.hasNext()){
		l_lv=l_iter.next();
		if(l_name+"-"+l_lv->getName()==p_name) return l_lv;
	}
	return nullptr;
}


TVolumeGroup::TVolumeGroup()
{
	key="";
	name="";
}

TLogicalVolume * TVolumeGroup::addLv(QString& p_name)
{
	TLogicalVolume *l_logicalVolume=new TLogicalVolume(p_name,this);
	logicalVolumns.append(l_logicalVolume);
	return l_logicalVolume;
}


void TVolumeGroupList::processInfo(TDeviceList* p_list)
{
	TLinkListIterator<TVolumeGroup> l_iter(this);
	TVolumeGroup   *l_vg;
	TLogicalVolume *l_lv;
	QString l_target;
	TDeviceBase *l_deviceBase;
	TDevice *l_device;
	while(l_iter.hasNext()){
		l_vg=l_iter.next();
		TLinkListIterator<TLogicalVolume> l_lvIter(l_vg->getLogicalVolumns());
		while(l_lvIter.hasNext()){
			l_lv=l_lvIter.next();
			l_target=QFile::symLinkTarget(QString("/dev/")+l_vg->getName()+"/"+l_lv->getName());
			l_deviceBase=p_list->findDeviceByDevPath(l_target);
			if((l_device=dynamic_cast<TDevice *>(l_deviceBase))!=nullptr){
				l_device->setVGName(l_vg->getName());				
				l_lv->setRealDevice(l_device);
			}
		}
	}
}

TVolumeGroup * TVolumeGroupList::getVolumeGroupById(QString& p_id)
{
	TLinkListIterator<TVolumeGroup> l_iter(this);
	TVolumeGroup *l_item;
	while(l_iter.hasNext()){
		l_item=l_iter.next();
		if(l_item->getKey()==p_id) return l_item;
	}
	return nullptr;
}



TLogicalVolume::TLogicalVolume(QString& p_name,TVolumeGroup *p_volumeGroup)
{
	name=p_name;
	volumeGroup=p_volumeGroup;
	realDevice=nullptr;
}







bool TLVMHandler::writeSocket(const char* p_str)
{
	int l_return=write(readSocket,p_str,strlen(p_str));
	return l_return >=0;
}

bool TLVMHandler::openLVMSocket()
{
	struct sockaddr_un l_addr;
	readSocket=socket(PF_LOCAL,SOCK_STREAM,0);
	if(readSocket<0) return false;
	l_addr.sun_family=AF_LOCAL;
	strcpy(l_addr.sun_path,"/run/lvm/lvmetad.socket");
	int l_return=connect(readSocket,(const struct sockaddr *)&l_addr,sizeof(l_addr));
	if(l_return<0){
		close(readSocket);
		readSocket=-1;
		return false;
	}
	return true;
}

bool TLVMHandler::sendMessage(const char *p_message,QString &p_return)
{
	if(!writeSocket(p_message)) return false;
	char l_buffer[1024];
	ssize_t l_num;
	p_return="";
	while(true){
		l_num=read(readSocket,l_buffer,sizeof(l_buffer)-1);
		l_buffer[l_num]=0;
		p_return += l_buffer;
		cout << l_buffer <<endl;
		if(l_num+1<sizeof(l_buffer)) break;
	}
	return true;
}
 
TPhysicalVolumeList* TLVMHandler::pvList(TDeviceList* p_devList)
{
	QString l_return;
	bool l_ok=sendMessage("request = \"pv_list\"\ntoken = \"filter:0\"\n\n##\n",l_return);
	if(l_ok){
		TPVParser l_parser(p_devList,l_return);
		l_parser.parse();
		return l_parser.getItems();		
	}
	return nullptr;
}

TVolumeGroupList *TLVMHandler::vgList()
{
	QString l_return;
	TVolumeGroupList *l_items=nullptr;
	bool l_ok=sendMessage("request = \"vg_list\"\ntoken = \"filter:0\"\n\n##\n",l_return);
	if(l_ok){
		TVGMainParser l_parser(l_return);
		l_parser.parse();
		l_items=l_parser.getItems();
		TLinkListIterator<TVolumeGroup> l_iter(l_items);
		TVolumeGroup *l_item;
		QString l_format="request = \"vg_lookup\"\nuuid = \"%1\"\ntoken=\"filter:0\"\n\n##\n";
		while(l_iter.hasNext()){
			l_item=l_iter.next();
			l_ok=sendMessage(qstr(l_format.arg(l_item->getKey())),l_return);
			if(l_ok){
				TVGParser l_parser(l_item,l_return);
				l_parser.parse();
			}
		}
	}
	return l_items;
}


TLVMHandler::TLVMHandler()
{
	readSocket=-1;
}

TLVMHandler::~TLVMHandler()
{
	closeLVMSocket();
}


void TLVMHandler::closeLVMSocket()
{
	if(readSocket !=-1) close(readSocket);
	readSocket=-1;
}




void TLVM::processInfo(TDeviceList* p_devList)
{	
	pvList=nullptr;
	vgList=nullptr;
	TLVMHandler l_handler;
	if(l_handler.openLVMSocket()){
		vgList=l_handler.vgList();
		pvList=l_handler.pvList(p_devList);
		vgList->processInfo(p_devList);
		pvList->processList(vgList);
		l_handler.closeLVMSocket();
	}
	
}
