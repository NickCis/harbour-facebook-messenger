#include "network_reply_handler.h"

NetworkReplyHandler::NetworkReplyHandler(QNetworkReply* r) :
	QObject(r),
	reply(r)
{
	connect(this->reply, SIGNAL(finished()), this, SLOT(replyFinished()));
}


