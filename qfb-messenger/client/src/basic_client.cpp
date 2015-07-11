#include "basic_client.h"

#include <iostream>

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>

using namespace std;

BasicClient::BasicClient() {
	this->title = "QFb Messenger Client";
	connect(&qfb, SIGNAL(initResponse(bool, QString)), this, SLOT(initResponse(bool, QString)));
	connect(&qfb, SIGNAL(loginResponse(bool, QString)), this, SLOT(loginResponse(bool, QString)));
	connect(&qfb, SIGNAL(getBasicInformationResponse(bool, QJsonValue)), this, SLOT(getBasicInformationResponse(bool, QJsonValue)));
	connect(&qfb, SIGNAL(getUserInfoResponse(bool, QJsonValue)), this, SLOT(getUserInfoResponse(bool, QJsonValue)));
	connect(&qfb, SIGNAL(getThreadInfoResponse(bool, QJsonValue)), this, SLOT(getThreadInfoResponse(bool, QJsonValue)));
	connect(&qfb, SIGNAL(sendMessagesResponse(bool, QJsonValue)), this, SLOT(sendMessagesResponse(bool, QJsonValue)));
}

void BasicClient::initResponse(bool error, QString desc){
	if(error){
		cout << __func__ << " failed! : " << desc.toStdString() << endl;
	}else{
		cout << __func__ << " :: ok!" << endl;
	}
	this->setStatus(Console::Default);
}

void BasicClient::loginResponse(bool error, QString desc){
	if(error){
		cout << __func__ << " failed! : " << desc.toStdString() << endl;
	}else{
		cout << __func__ << " :: ok!" << endl;
	}
	this->setStatus(Console::Default);
}

void BasicClient::getBasicInformationResponse(bool error, QJsonValue data){
	if(error){
		cout << __func__ << " failed!" << endl;
	}else{
		cout << __func__ << " :: ok!" << endl;
		this->friendsList = data.toObject().value("friendsList").toArray();
		this->conversations = data.toObject().value("conversations").toArray();

		cout << QString(QJsonDocument(this->conversations).toJson()).toStdString();
	}
	this->setStatus(Console::Default);
}

void BasicClient::getUserInfoResponse(bool error, QJsonValue data){
	if(error){
		cout << __func__ << " failed!" << endl;
	}else{
		cout << __func__ << " :: ok!" << endl;
		this->profiles = data.toObject();
	}
	this->setStatus(Console::Default);
}

void BasicClient::getThreadInfoResponse(bool error, QJsonValue data){
	if(error){
		cout << __func__ << " failed!" << endl;
	}else{
		cout << __func__ << " :: ok!" << endl;
		//cout << QString(QJsonDocument(data.toObject()).toJson()).toStdString();
		QJsonArray msgs = data.toObject().value("actions").toArray();
		for(QJsonArray::const_iterator it=msgs.begin(); it!=msgs.end(); it++){
			const QJsonObject obj = (*it).toObject();
			cout << "\t " << this->getUsername(obj.value("author").toString()) << ": ";
			cout << obj.value("body").toString().toStdString() << endl;
		}
	}
	this->setStatus(Console::Default);
}

void BasicClient::sendMessagesResponse(bool error, QJsonValue data){
	if(error){
		cout << __func__ << " failed!" << endl;
	}else{
		cout << __func__ << " :: ok!" << endl;
	}
	this->setStatus(Console::Default);
}

void BasicClient::input(const string& line) {
	switch(this->status){
		case BasicClient::Login:
			this->showStdinKeyStrokes();
			this->qfb.login(this->email, QString(line.c_str()));
			this->setStatus(Console::Default);
			break;

		default:
			break;
	}
}

void BasicClient::executeCommand(const string& line) {
	QString qline(line.c_str());
	if(cin.eof() || line == "quit"){
		cout << "Good bye!" << endl;
		emit quit();
		this->setStatus(Console::Input);
	}else if(line == "init"){
		qfb.init();
		this->setStatus(Console::Input);
	}else if(qline.startsWith("login")){
		QStringList list = qline.split(" ");
		if(list.count() < 2)
			cout << "login <email>" << endl;
		else{
			this->email = list[1];
			status = BasicClient::Login;
			cout << "password (won't be echoed): ";
			this->hideStdinKeyStrokes();
			this->setStatus(Console::Input);
		}
	}else if(qline.startsWith("getBasicInformation")){
		qfb.getBasicInformation();
		this->setStatus(Console::Input);
	}else if(qline.startsWith("getUserInfo")){
		qfb.getUserInfo(this->friendsList);
		this->setStatus(Console::Input);
	}else if(qline.startsWith("profile")){
		QStringList list = qline.split(" ");
		if(list.count() < 2){
			cout << "profile <userid / list>" << endl;
		}else{
			if(list[1] == "list"){
				QStringList keys = this->profiles.keys();
				for(QStringList::const_iterator it= keys.begin(); it!=keys.end(); it++)
					cout << "\t* " << (*it).toStdString() << endl;
			}else{
				QJsonDocument document;
				cout << QString(QJsonDocument(this->profiles.value(list[1]).toObject()).toJson()).toStdString();
			}
		}
	}else if(qline.startsWith("conversations")){
		for(QJsonArray::const_iterator it=this->conversations.begin(); it != this->conversations.end(); it++){
			const QJsonObject obj = (*it).toObject();
			cout << getUsername(obj.value("thread_fbid").toString()) << " :: [ " << obj.value("thread_fbid").toString().toStdString() << " ] | " << obj.value("snippet").toString().toStdString() << endl;

		}
	}else if(qline.startsWith("thread")){
		QStringList list = qline.split(" ");
		if(list.count() < 2)
			cout << "thread <id>" << endl;
		else{
			qfb.getThreadInfo(list[1]);
			this->setStatus(Console::Input);
		}
	}else if(qline.startsWith("send")){
		QStringList list = qline.split(" ");
		if(list.count() < 3){
			cout << "send <conversation id> <message>" << endl;
		}else{
			qfb.sendMessages(list[1], list[2]);
			this->setStatus(Console::Input);
		}
	}
}

string BasicClient::getUsername(const QString& fbid){
	QJsonValue val = this->profiles.value(fbid.split(":").last());

	if(val.isUndefined())
		return fbid.toStdString();

	return val.toObject().value("name").toString().toStdString();
}
