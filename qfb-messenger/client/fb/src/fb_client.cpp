#include "fb_client.h"

#include <iostream>

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>

using namespace std;

FbClient::FbClient(QObject* parent) :
	Console(parent),
	qfb(&this->dummyStorage)
{
	this->title = "QFb Messenger Client";
	connect(&qfb, SIGNAL(loginResponse(bool, QString)), this, SLOT(qfbLoginResponse(bool, QString)));
	connect(&qfb, SIGNAL(connectedChanged(bool)), this, SLOT(qfbConnectedChanged(bool)));
}

void FbClient::qfbLoginResponse(bool error, QString desc){
	if(error){
		cout << __func__ << " failed! : " << desc.toStdString() << endl;
	}else{
		cout << __func__ << " :: ok!" << endl;
	}
	this->setStatus(Console::Default);
}

void FbClient::qfbConnectedChanged(bool status){
	cout << __func__ << " connected status changed: " << status << endl;
}

void FbClient::input(const string& line) {
	switch(this->status){
		case FbClient::Login:
			this->showStdinKeyStrokes();
			this->qfb.login(this->email, QString(line.c_str()));
			this->setStatus(Console::Default);
			break;

		default:
			break;
	}
}

void FbClient::executeCommand(const string& line) {
	QString qline(line.c_str());
	if(cin.eof() || line == "quit"){
		cout << "Good bye!" << endl;
		emit quit();
		this->setStatus(Console::Input);
	}else if(qline.startsWith("connect")){
		QStringList list = qline.split(" ");
		if(list.count() < 2)
			cout << "connect <email>" << endl;
		else{
			this->dummyStorage.setAccount(list[1]);
			this->setStatus(Console::Input);
			this->qfb.login();
		}
	}else if(qline.startsWith("login")){
		QStringList list = qline.split(" ");
		if(list.count() < 2)
			cout << "login <email>" << endl;
		else{
			this->email = list[1];
			status = FbClient::Login;
			cout << "password (won't be echoed): ";
			this->hideStdinKeyStrokes();
			this->setStatus(Console::Input);
		}
	}else if(qline.startsWith("pullStart")){
		this->qfb.startPull();
	}else if(qline.startsWith("pullEnd")){
		this->qfb.stopPull();
	}
}
