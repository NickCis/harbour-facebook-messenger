#include "qfb-messenger.h"
#include <QTimer>
#include <QDebug>

QFbMessenger::QFbMessenger(QFbStorage* s, QObject *parent) : 
	QObject(parent),
	storage(s),
	email(""),
	pass(""),
	connected(false),
	pulling(false)
{
	connect(&this->network, SIGNAL(initResponse(bool, QString)), this, SLOT(networkInitAnswer(bool, QString)));
	connect(&this->network, SIGNAL(loginResponse(bool, QString)), this, SLOT(networkLoginAnswer(bool, QString)));
	connect(&this->network, SIGNAL(getBasicInformationResponse(bool, QJsonValue)), this, SLOT(networkGetBasicInformationAnswer(bool, QJsonValue)));
	//connect(&this->network, SIGNAL(getUserInfoResponse(bool, QJsonValue)), this, SLOT(networkInitAnswer(bool, QJsonValue)));
	//connect(&this->network, SIGNAL(getThreadInfoResponse(bool, QJsonValue)), this, SLOT(networkInitAnswer(bool, QJsonValue)));
	//connect(&this->network, SIGNAL(sendMessagesResponse(bool, QJsonValue)), this, SLOT(networkInitAnswer(bool, QJsonValue)));
	//connect(&this->network, SIGNAL(pullResponse(bool, QJsonValue)), this, SLOT(networkInitAnswer(bool, QJsonValue)));
	connect(&this->network, SIGNAL(pullEnd()), this, SLOT(networkPullEnd()));
	connect(&this->network, SIGNAL(newConfigurationValue(const QString&, const QString&)), this, SLOT(networkNewConfigurationValue(const QString&, const QString&)));
}

void QFbMessenger::login(){
	QHash<QString, QString> configurationValues = this->storage->getConfigurationValues();
	QList<QString> keys = configurationValues.keys();
	for(QList<QString>::const_iterator it=keys.begin(); it!=keys.end(); it++)
		this->network.setConfigurationValue(*it, configurationValues[*it]);

	// TODO: test connection?
	this->setConnected(true);
	emit loginResponse(false, __func__);
}

void QFbMessenger::login(const QString &email, const QString &pass){
	this->email = email;
	this->pass = pass;
	this->storage->setAccount(email);
	this->network.init();
}

void QFbMessenger::networkInitAnswer(bool error, QString desc){
	if(error){
		emit loginResponse(true, __func__);
		this->setConnected(false);
		return;
	}

	this->network.login(this->email, this->pass);
}

void QFbMessenger::networkLoginAnswer(bool error, QString desc){
	if(error){
		emit loginResponse(true, __func__);
		this->setConnected(false);
		return;
	}

	this->network.getBasicInformation();
}

void QFbMessenger::networkGetBasicInformationAnswer(bool error, QJsonValue data){
	if(error){
		emit loginResponse(true, __func__);
		this->setConnected(false);
		return;
	}

	emit loginResponse(false, __func__);
	this->setConnected(true);
}

void QFbMessenger::setConnected(bool c){
	bool old = this->connected;
	this->connected = c;

	if(old != c)
		emit connectedChanged(c);
}

void QFbMessenger::networkNewConfigurationValue(const QString& name, const QString& value){
	this->storage->setConfigurationValue(name, value);
}

bool QFbMessenger::isConnected() const{
	return this->connected;
}

void QFbMessenger::startPull(){
	this->pulling = true;
	this->network.pull();
}

void QFbMessenger::stopPull(){
	this->pulling = false;
}

void QFbMessenger::networkPullEnd(){
	qDebug() << "Pull ended";
	QTimer::singleShot(5000, this, SLOT(sendPullRequest()));
}

void QFbMessenger::sendPullRequest(){
	qDebug() << "need to pull";
	if(!this->pulling)
		return;

	qDebug() << "send pull";
	this->network.pull();
}
