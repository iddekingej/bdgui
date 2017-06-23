#include <iostream>
#include "scanner.h"

TScanner::TScanner(const QString& p_condition)
{
	condition=p_condition;
}


bool TScanner::parseString(QChar p_end)
{
	current++;
	int l_begin=current;
	while(current < condition.length()){
		QChar l_ch=condition.at(current);
		if(l_ch==p_end){
			tokenText=condition.mid(l_begin,current-l_begin);
			token=TToken::STRING;
			current++;
			return false;
		}
		current++;
	}
	errorText="Missing end quote";
	token=TToken::ERROR;
	return true;
}

bool TScanner::parseIdent()
{
	QChar l_ch;
	int l_begin=current;
	while(current <condition.length()){
		l_ch=condition.at(current);
		if((l_ch<'a'||l_ch>'z') && (l_ch <'A' || l_ch>'Z') && (l_ch<'0' || l_ch>'9') && (l_ch != '_')){
			break;
		}
		current++;
	}
	tokenText=condition.mid(l_begin,current-l_begin).toLower();
	
	if("and"==tokenText){
		token=TToken::OP_AND;
	} else if("or"==tokenText){
		token=TToken::OP_OR;
	} else {
		token=TToken::IDENT;
	}
	return false;
}


bool TScanner::nextToken()
{	
	skipSpace();	
	tokenCol=col;
	tokenLine=line;
	if(current>=condition.length()){
		token=TToken::FILE_END;
		tokenText="";
		return false;
	}	

	QChar l_ch=condition.at(current);
	
	if(l_ch=='"'||l_ch=='\''){		
		return parseString(l_ch);
	} else if((l_ch>='a' && l_ch <='z') ||(l_ch>='A' && l_ch<='Z') || l_ch=='_'){
		return parseIdent();
	} 
	token=TToken::CHAR;
	tokenText=l_ch;
	current++;
	if(l_ch=='='){
		token=TToken::COND_EQUAL;
	} else if(l_ch=='>'){
		token=TToken::COND_BIGGER;		
		if(current<condition.length()){			
			if(condition.at(current)=='='){			
				token=TToken::COND_BE;
				tokenText=">=";
				current++;
			}
		}
	} else if(l_ch=='<'){
		token=TToken::COND_SMALLER;		
		if(current<condition.length()){
			if(condition.at(current)=='='){
				token=TToken::COND_SE;
				tokenText="<=";
				current++;
			}
		}
		
	} else if(l_ch=='!'){
		token=TToken::COND_NOT;		
		if(current<condition.length()){
			if(condition.at(current)=='='){
				token=TToken::COND_NE;
				tokenText="!=";
				current++;
			}
		}
		
	} else if(l_ch=='('){
		token=TToken::HOOK_L;		
	} else if(l_ch==')'){
		token=TToken::HOOK_R;		
	}
	return false;
}

void TScanner::skipSpace(){
	QChar l_ch;
	while(current<condition.length()){
		l_ch=condition.at(current);		
		if(l_ch==13){
			current++;
			line++;
			col=0;
			if(current<condition.length()){
				l_ch=condition.at(current);
				if(l_ch==10){
					current++;
				}
			}
		} else if(l_ch==10){
			current++;
			line++;
			col=0;;
			if(current<condition.length()){
				l_ch=condition.at(current);
				if(l_ch==13){
					current++;
				}
			}
		}  else if(l_ch== 32 || l_ch == 9){
			current++;
			col++;
		} else {
			break;
		}
		
	}
}


