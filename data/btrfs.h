#ifndef __BTRFS_H_
#define __BTRFS_H_

#include "base/linklist.h"
#include "alias.h"
#include <QStringList>
#include <QString>
#include <QSet>

class TBtrfsMountItem
{
private:
	QStringList devices;
	QString     fs;
	QString     raidLevel;
public:
	inline QStringList getDevices(){ return devices;}
	inline QString     getFs(){ return fs;}
	inline QString     getDevicesStr(){ return devices.join(",");}
	inline QString     getRaidLevel(){return raidLevel;}
	inline bool        isMultiDev(){ return devices.length() >1;}
	void appendDevice(const QString &p_device);		
	TBtrfsMountItem(const QString &p_fs,const QString &p_raidLevel);
};


class TBtrfsInfo
{
private:
	TLinkList<TBtrfsMountItem> mountItems;
	int readDevices(QString p_path,TBtrfsMountItem *p_info);
	QSet<QString> btrfsDevices;
public:
	TLinkList<TBtrfsMountItem> &getMountItems()
	{
		return mountItems;
	}
	
	TLinkListItem<TBtrfsMountItem> *getStart()
	{
		return mountItems.getStart();
	}
	
	bool isBtrfs(const QString &p_name);
	int readInfo(TAlias *p_aliasses);
	int getNumberMultiDevices();
	QString getRaidLevel(const QString &p_path);
};

#endif