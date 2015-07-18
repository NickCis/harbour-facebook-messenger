#include "network_reply_reader.h"
#include "q_network_reply_helper.h"

#include "QDebug"

NetworkReplyReader::NetworkReplyReader(QNetworkReply* r) :
	NetworkReplyHandler(r)
{
	connect(this->reply, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void NetworkReplyReader::readyRead(){
	this->emitAllLines();
}

void NetworkReplyReader::replyFinished(){
	QNetworkReplyHelper reply(this->reply);
	this->emitAllLines();
	emit finished();
}

void NetworkReplyReader::emitAllLines(){
	while(this->reply->canReadLine())
		emit readLine(this->reply->readLine());

	QByteArray data = this->reply->readAll();
	if(data.size())
		emit readLine(data);

}
