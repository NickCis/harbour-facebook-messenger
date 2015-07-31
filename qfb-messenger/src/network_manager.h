#ifndef __NETWORK_MANAGER_H__
#define __NETWORK_MANAGER_H__

#include <QUrl>
#include <QString>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QNetworkAccessManager>

class NetworkManager : public QNetworkAccessManager {
	Q_OBJECT

	public:
		explicit NetworkManager();
		explicit NetworkManager(QObject* parent);

		enum RequestMethod {
			Get = 0,
			Post,
			Delete
		};

		QNetworkReply* createRequest(const QString& url, QUrlQuery* query=NULL, NetworkManager::RequestMethod method=NetworkManager::Get);
		QNetworkReply* createNodeRequest(const QString& node, QUrlQuery* query=NULL, NetworkManager::RequestMethod method=NetworkManager::Get);

		void setHost(const QString&);
		void setProtocol(const QString&);
		void setUserAgent(const QString&);
		const QString& getHost() const;
		const QString& getProtocol() const;
		const QString& getUserAgent() const;

		void setCookie(const QByteArray& name, const QByteArray& value, const QString& path, const QString& domain, bool httpOnly=true, bool secure=true);
		void setCookie(const QString& name, const QString& value, const QString& path, const QString& domain, bool httpOnly=true, bool secure=true);

		QString getCookie(const QString& name, const QString& url=QString());

	protected:
		QString protocol;
		QString host;
		QString userAgent;
};

#endif
