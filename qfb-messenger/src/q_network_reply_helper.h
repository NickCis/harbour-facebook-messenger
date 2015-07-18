#ifndef __Q_NETWORK_REPLY_HELPER_H__
#define __Q_NETWORK_REPLY_HELPER_H__

#include <QObject>
#include <QNetworkReply>

class QNetworkReplyHelper : public QObject {
		Q_OBJECT

	public:
		explicit QNetworkReplyHelper(QNetworkReply* reply);
		~QNetworkReplyHelper();

		QNetworkReply* operator->();
		QNetworkReply* operator&();

	private:
		QNetworkReply* reply;
};

#endif
