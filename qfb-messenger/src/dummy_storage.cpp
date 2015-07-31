#include "dummy_storage.h"

#include <QFile>
#include <QDataStream>

DummyStorage::DummyStorage(QObject* parent) :
	QFbStorage(parent),
	account("")
{
}

DummyStorage::~DummyStorage(){
	QFile config(QString("%1.config").arg(this->account));
	if(config.open(QIODevice::WriteOnly)){
		QDataStream stream(&config);
		stream << this->configValues;
		config.close();
	}
}

QHash<QString, QString> DummyStorage::getConfigurationValues(){
	return this->configValues;
}

void DummyStorage::setConfigurationValue(const QString& name, const QString& value){
	this->configValues[name] = value;
}

void DummyStorage::setAccount(const QString& email){
	this->account = email;
	this->readAccount();
}

void DummyStorage::readAccount(){
	QFile config(QString("%1.config").arg(this->account));
	if(config.open(QIODevice::ReadOnly)){
		QDataStream stream(&config);
		stream >> this->configValues;
		config.close();
	}
}
