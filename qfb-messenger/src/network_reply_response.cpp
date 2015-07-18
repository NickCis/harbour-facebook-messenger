#include "network_reply_response.h"

#include "q_network_reply_helper.h"
#include <QDebug>

NetworkReplyResponse::NetworkReplyResponse(QNetworkReply* r) : NetworkReplyHandler(r){
}

void NetworkReplyResponse::replyFinished(){
	QNetworkReplyHelper reply(this->reply);
	emit finished(reply->error(), reply->readAll());
}
