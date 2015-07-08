#include "console.h"
#include <iostream>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>

#include <termios.h>

using namespace std;

Console::Console() : status(Console::DEFAULT){
	this->notifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
}

void Console::run() {
	cout << "QFb Messenger Client" << endl;
	cout << "> " << flush;
	connect(notifier, SIGNAL(activated(int)), this, SLOT(readCommand()));
	connect(&qfb, SIGNAL(getBasicInformationResponse(bool, QJsonValue)), this, SLOT(getBasicInformationResponse(bool, QJsonValue)));
	connect(&qfb, SIGNAL(getUserInfoResponse(bool, QJsonValue)), this, SLOT(getUserInfoResponse(bool, QJsonValue)));
	connect(&qfb, SIGNAL(loginResponse(bool, QString)), this, SLOT(loginResponse(bool, QString)));
}

void Console::readCommand() {
	string line;
	getline(cin, line);
	switch(this->status){
		case Console::DEFAULT:
			this->executeCommand(line);
			break;

		default:
			this->statusCommand(line);
			break;
	}
}

void Console::statusCommand(const string& line) {
	switch(this->status){
		case Console::LOGIN:
			this->status = Console::DEFAULT;
			this->showStdinKeyStrokes();
			this->qfb.login(this->email, QString(line.c_str()));
			break;

		case Console::DEFAULT:
		default:
			break;
	}

	cout << endl << "> " << flush;
}

void Console::executeCommand(const string& line) {
	QString qline(line.c_str());
	if(cin.eof() || line == "quit"){
		cout << "Good bye!" << endl;
		emit quit();
		return;
	}else if(line == "init"){
		qfb.init();
	}else if(qline.startsWith("login")){
		QStringList list = qline.split(" ");
		if(list.count() < 2)
			cout << "login <email>" << endl;
		else{
			this->email = list[1];
			status = Console::LOGIN;
			cout << "password (won't be echoed): ";
			this->hideStdinKeyStrokes();
			cout << flush;
			return;
		}
	}else if(qline.startsWith("getBasicInformation")){
		qfb.getBasicInformation();
	}else if(qline.startsWith("getUserInfo")){
		qfb.getUserInfo(this->friendsList);
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
	}

	cout << "> " << flush;
}

void Console::hideStdinKeyStrokes(){
	termios tty;
	tcgetattr(fileno(stdin), &tty);
	tty.c_lflag &= ~ECHO;
	tcsetattr(fileno(stdin), TCSANOW, &tty);
}

void Console::showStdinKeyStrokes(){
	termios tty;
	tcgetattr(fileno(stdin), &tty);
	tty.c_lflag |= ECHO;
	tcsetattr(fileno(stdin), TCSANOW, &tty);
}

void Console::getBasicInformationResponse(bool error, QJsonValue data){
	if(error){
		cout << __func__ << " failed!" << endl;
		return;
	}

	cout << __func__ << " :: ok!" << endl;
	this->friendsList = data.toObject().value("friendsList").toArray();
	this->conversations = data.toObject().value("conversations").toArray();
}

void Console::getUserInfoResponse(bool error, QJsonValue data){
	if(error){
		cout << __func__ << " failed!" << endl;
		return;
	}

	cout << __func__ << " :: ok!" << endl;
	this->profiles = data.toObject();
}

void Console::loginResponse(bool error, QString desc){
	if(error){
		cout << __func__ << " failed! : " << desc.toStdString() << endl;
		return;
	}

	cout << __func__ << " :: ok!" << endl;
}
