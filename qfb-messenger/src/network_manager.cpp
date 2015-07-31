#include "network_manager.h"

#include <QDebug>
#include <QNetworkCookie>
#include <QNetworkCookieJar>

NetworkManager::NetworkManager() {}
NetworkManager::NetworkManager(QObject* p) : QNetworkAccessManager(p) {}

QNetworkReply* NetworkManager::createNodeRequest(const QString& node, QUrlQuery *query, NetworkManager::RequestMethod method){
	return this->createRequest(QString("%1://%2/%3").arg(this->protocol).arg(this->host).arg(node), query, method);
}

QNetworkReply* NetworkManager::createRequest(const QString& urlStr, QUrlQuery *query, NetworkManager::RequestMethod method){
	QNetworkRequest request;
	QNetworkReply* reply;

	qDebug() << "url: " << urlStr;

	QUrl url(urlStr);
	qDebug() << "cookies: " << (this->cookieJar()->cookiesForUrl(url));

	request.setHeader(QNetworkRequest::UserAgentHeader, this->userAgent);

	if(method == NetworkManager::Post){
		request.setUrl(url);
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
		if(query){
			qDebug()<< "Mando post!";
			reply = this->post(request, query->toString(QUrl::FullyEncoded).toUtf8());
		}else{
			QByteArray data;
			reply = this->post(request, data);
		}
	}else{
		if(query)
			url.setQuery(*query);
		request.setUrl(url);
		if(method == NetworkManager::Delete)
			reply = this->deleteResource(request);
		else
			reply = this->get(request);
	}

	/*connect(reply, SIGNAL(finished()), req, SLOT(finished()));
	  connect(reply, SIGNAL(uploadProgress(qint64, qint64)), req, SLOT(upProg(qint64, qint64)));
	  connect(reply, SIGNAL(downloadProgress(qint64,qint64)), req, SLOT(downProg(qint64,qint64)));*/

	return reply;
}

void NetworkManager::setHost(const QString& h){
	this->host = h;
}

void NetworkManager::setProtocol(const QString& p){
	this->protocol = p;
}

void NetworkManager::setUserAgent(const QString& ua){
	this->userAgent = ua;
}

const QString& NetworkManager::getHost() const{
	return this->host;
}

const QString& NetworkManager::getProtocol() const{
	return this->protocol;
}

const QString& NetworkManager::getUserAgent() const{
	return this->userAgent;
}

void NetworkManager::setCookie(const QString& name, const QString& value, const QString& path, const QString& domain, bool httpOnly, bool secure){
	this->setCookie(name.toUtf8(), value.toUtf8(), path, domain, httpOnly, secure);
}
void NetworkManager::setCookie(const QByteArray& name, const QByteArray& value, const QString& path, const QString& domain, bool httpOnly, bool secure){
	QNetworkCookie cookie(name, value);
	cookie.setHttpOnly(httpOnly);
	cookie.setSecure(secure);
	cookie.setPath(path);
	cookie.setDomain(domain);
	this->cookieJar()->insertCookie(cookie);
}

QString NetworkManager::getCookie(const QString& name, const QString& url){
	QList<QNetworkCookie> cookies = this->cookieJar()->cookiesForUrl(QUrl(url.size() ? url : QString("%1://%2/").arg(this->protocol).arg(this->host)));

	for(QList<QNetworkCookie>::const_iterator it=cookies.begin(); it != cookies.end(); it++)
		if(QString((*it).name()) == name)
			return QString((*it).value());

	return QString("");
}
