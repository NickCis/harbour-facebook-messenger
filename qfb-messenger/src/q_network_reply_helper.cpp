#include "q_network_reply_helper.h"

QNetworkReplyHelper::QNetworkReplyHelper(QNetworkReply* r) :
	QObject(NULL),
	reply(r)
{
}

QNetworkReplyHelper::~QNetworkReplyHelper(){
	this->reply->close();
	this->reply->deleteLater();
}

QNetworkReply* QNetworkReplyHelper::operator->(){
	return this->reply;
}

QNetworkReply* QNetworkReplyHelper::operator&(){
	return this->reply;
}
