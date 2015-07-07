#ifndef __QFB_MESSENGER_H__
#define __QFB_MESSENGER_H__

#include <QObject>
#include <QUrl>
#include <QUrlQuery>
#include <QVariant>
#include <QSettings>
#include <QNetworkReply>
#include <QNetworkAccessManager>


class QFbMessenger : public QObject {
		Q_OBJECT

	public:
		explicit QFbMessenger(QObject *parent = 0);
		Q_INVOKABLE void init();
		Q_INVOKABLE void login(const QString &user, const QString &pass);
		Q_INVOKABLE void getConversations();

	signals:
		void initResponse(bool error, QString desc);
		void loginResponse(bool error, QString desc);
		void getConversationsResponse(bool error, QString desc);

	public slots:
		void initFinished(QObject* o);
		void loginFinished(QObject* o);
		void getConversationsFinished(QObject* o);


	protected:
		enum RequestMethod {
			Get = 0,
			Post,
			Delete
		};

		QNetworkAccessManager man;

		QNetworkReply* createRequest(const QString& node);
		QNetworkReply* createRequest(const QString& node, QUrlQuery* query, QFbMessenger::RequestMethod method);

		QString getMatch(const QString& regexp, const QString& text);
		void getRequestId(const QString&);
		void getIdentifier(const QString&);
		void getJsDatr(const QString&);
		void getLsd(const QString&);

		QString requestId;
		QString identifier;
		QString jsDatr;
		QString lsd;

		QString email;
};

#endif
