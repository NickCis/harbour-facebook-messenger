#ifndef __NETWORK_REPLY_HANDLER_H__
#define __NETWORK_REPLY_HANDLER_H__

#include <QObject>
#include <QNetworkReply>

class NetworkReplyHandler : public QObject {
		Q_OBJECT

	public:
		explicit NetworkReplyHandler(QNetworkReply* reply);

	protected:
		QNetworkReply* reply;

	protected slots:
		virtual void replyFinished() = 0;
};

#endif
