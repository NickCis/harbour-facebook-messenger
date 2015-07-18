#ifndef __NETWORK_REPLY_RESPONSE_H__
#define __NETWORK_REPLY_RESPONSE_H__

#include <QObject>
#include "network_reply_handler.h"

class NetworkReplyResponse : public NetworkReplyHandler {
		Q_OBJECT

	public:
		explicit NetworkReplyResponse(QNetworkReply* reply);

	signals:
		void finished(QNetworkReply::NetworkError err, const QByteArray& response);

	protected slots:
		virtual void replyFinished();
};

#endif
