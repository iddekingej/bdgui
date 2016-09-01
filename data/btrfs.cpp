#include <QDir>
#include <QDirIterator>
#include <QSet>
#include <QString>
#include "btrfs.h"


void TBtrfsItem::appendDevice(const QString &p_device)
{
	devices.append(p_device);
}


TBtrfsItem::TBtrfsItem(const QString &p_fs,const QString &p_raidLevel)
{
	fs=p_fs;
	raidLevel=p_raidLevel;
}

int TBtrfsInfo::readDevices(QString p_path,TBtrfsItem *p_info)
{	
	QDirIterator l_iter(p_path+"/devices/",QDirIterator::NoIteratorFlags);
	while(l_iter.hasNext()){
		l_iter.next();
		if(l_iter.fileInfo().isSymLink()){
			p_info->appendDevice(l_iter.fileName());
			btrfsDevices << l_iter.fileName();

		}
	}
	return 0;
}

void TBtrfsInfo::getRaidLevel(const QString &p_path,QString &p_raidLevel)
{		
	QDirIterator l_iter(p_path+"/allocation/system/",QDirIterator::NoIteratorFlags);
	while(l_iter.hasNext()){
		l_iter.next();
		if(l_iter.fileName().startsWith("raid")){
			p_raidLevel=l_iter.fileName();
			return ;
		}
	}
	p_raidLevel="";	
}
int TBtrfsInfo::readInfo(TAlias *p_alias)
{		
	QDirIterator l_iter("/sys/fs/btrfs",QDirIterator::NoIteratorFlags);
	QString l_fileName;
	QString l_deviceLevel;
	TBtrfsItem *l_info;
	while(l_iter.hasNext()){
		l_iter.next();
		l_fileName=l_iter.fileName();
		if(l_iter.fileInfo().isDir() && (l_fileName != "features") && (l_fileName != ".") && (l_fileName != "..")){
			getRaidLevel(l_iter.filePath(),l_deviceLevel);
			l_info=new TBtrfsItem( p_alias->getDeviceNameFromAliasPath("/dev/disk/by-uuid/"+l_fileName),l_deviceLevel);
			readDevices(l_iter.filePath(), l_info);
			btrfsDeviceList.append(l_info);
		}
		
	}
	return 0;
}

int TBtrfsInfo::getNumberMultiDevices()
{
		TLinkListIterator<TBtrfsItem> l_iter(&btrfsDeviceList);
		
		int l_return=0;
		while(l_iter.hasNext()){
			if(l_iter.next()->isMultiDev()) l_return++;
		}
		return l_return;		
}

bool TBtrfsInfo::isBtrfs(const QString &p_name)
{
	return btrfsDevices.contains(p_name);
}

