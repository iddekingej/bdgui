#include "alias.h"
#include <QDir>
#include <QDirIterator>
#include <QMap>
#include <QFile>

/*
 * Start with a file follows symlinks until in reaches a non symlink (endpoint)
 * All symlinks as stored (symlink,endpoint)
 **/
void TAlias::processAlias(const QString &p_file)
{
	QString l_currentFile=p_file;
	QString l_target;
	QStringList l_paths;	
	bool l_linked=false;
	while(true){			
			if(aliasses.contains(l_currentFile)){
				l_currentFile=aliasses.value(l_currentFile);
				break;
			}
			l_target=QFile::symLinkTarget(l_currentFile);
			if(l_target=="") break;
			l_paths << l_currentFile;
			l_currentFile=l_target;
			l_linked=true;
	}
	if(l_linked){
		foreach(QString l_found, l_paths){
			aliasses.insert(l_found,l_currentFile);
		}
			
	}
}
/*Scans all files in a folder "p_path" , if it is a symlink the 
 * links are followed until a non symlink is found(endpoint)
 *
*/
void TAlias::procesAliasses(const QString &p_path)
{	
	QDirIterator l_iter(p_path,QDirIterator::NoIteratorFlags);
	while(l_iter.hasNext()){
		l_iter.next();
		if(l_iter.fileInfo().isSymLink()){
			processAlias(l_iter.filePath());
		}
	}
}
/*
 * 
 */

const QString TAlias::getDeviceNameFromAliasPath(const QString &p_path){
	QString l_path=aliasses.value(p_path);
	QFileInfo l_info(l_path);
	return l_info.fileName();
}

void TAlias::readInfo()
{
	procesAliasses(QString("/dev/disk/by-id"));
	procesAliasses(QString("/dev/disk/by-label"));
	procesAliasses(QString("/dev/disk/by-partlabel"));
	procesAliasses(QString("/dev/disk/by-partuid"));
	procesAliasses(QString("/dev/disk/by-path"));
	procesAliasses(QString("/dev/disk/by-uuid"));
	procesAliasses(QString("/dev/mapper"));
}