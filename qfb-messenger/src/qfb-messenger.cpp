#include "qfb-messenger.h"

#include <QDebug>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValueRef>
#include <QSignalMapper>
#include <QJsonDocument>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QRegularExpression>

#define USER_AGENT "Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/43.0.2357.125 Safari/537.36"

#define FB_PROTOCOL "https"
#define FB_HOST "www.messenger.com"
#define FB_INIT_ENDPOINT "login"
#define FB_LOGIN_ENDPOINT "login/password/"
#define FB_CONVERSATIONS_ENDPOINT "/"
#define FB_USERINFO_ENDPOINT "chat/user_info/"

#define FB_COOKIE_DOMAIN ".messenger.com"
#define FB_COOKIE_PATH "/"

#define REQUEST_ID_REGEXP ".*\"initialRequestID\":\"([^\"]*)\".*"
#define IDENTIFIER_REGEXP ".*\"identifier\":\"([^\"]*)\".*"
#define JS_DATR_REGEXP ".*\"_js_datr\",\"([^\"]*)\".*"
#define LSD_REGEXP ".*name=\"lsd\" *value=\"([^\"]*)\".*"

QFbMessenger::QFbMessenger(QObject *parent) :
	QObject(parent),
	requestId(""),
	identifier(""),
	jsDatr(""),
	lsd("")
{
	qDebug() << "Init QFbMessenger: ";
}

QNetworkReply* QFbMessenger::createRequest(const QString& node){
	QUrlQuery query;
	return this->createRequest(node, &query, QFbMessenger::Get);
}

QNetworkReply* QFbMessenger::createRequest(const QString& node, QUrlQuery *query, QFbMessenger::RequestMethod method){
	QNetworkRequest request;
	QNetworkReply* reply;

	QString urlStr("%1://%2/%3");
	urlStr = urlStr.arg(FB_PROTOCOL).arg(FB_HOST).arg(node);
	qDebug() << "url: " << urlStr;

	QUrl url(urlStr);
	qDebug() << "cookies: " << (this->man.cookieJar()->cookiesForUrl(url));

	request.setRawHeader("User-Agent", USER_AGENT);

	if(method == QFbMessenger::Post){
		request.setUrl(url);
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
		if(query){
			qDebug()<< "Mando post!";
			reply = this->man.post(request, query->toString(QUrl::FullyEncoded).toUtf8());
		}else{
			QByteArray data;
			reply = this->man.post(request, data);
		}
	}else{
		if(query)
			url.setQuery(*query);
		request.setUrl(url);
		if(method == QFbMessenger::Delete)
			reply = this->man.deleteResource(request);
		else
			reply = this->man.get(request);
	}

	/*connect(reply, SIGNAL(finished()), req, SLOT(finished()));
	  connect(reply, SIGNAL(uploadProgress(qint64, qint64)), req, SLOT(upProg(qint64, qint64)));
	  connect(reply, SIGNAL(downloadProgress(qint64,qint64)), req, SLOT(downProg(qint64,qint64)));*/

	return reply;
}

void QFbMessenger::init(){
	qDebug() << "Hago request de init!";

	QNetworkReply* reply = this->createRequest(FB_INIT_ENDPOINT);
	QSignalMapper *mapper = new QSignalMapper(reply);
	connect(reply, SIGNAL(finished()), mapper, SLOT(map()));
	mapper->setMapping(reply, reply);
	connect(mapper, SIGNAL(mapped(QObject*)), this, SLOT(initFinished(QObject*)));
}

void QFbMessenger::initFinished(QObject* obj){
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(obj);
	QNetworkReply::NetworkError err = reply->error();

	if(err != QNetworkReply::NoError){
		qDebug() << "Fallo el request n: " << err << " str: '" << reply->errorString() << "'";
		emit initResponse(true, reply->errorString());
	}else{
		QString response = QString(reply->readAll());
		this->getRequestId(response);
		this->getIdentifier(response);
		this->getJsDatr(response);
		this->getLsd(response);
	}

	emit initResponse(false, "");
	reply->close();
	reply->deleteLater();
}


QString QFbMessenger::getMatch(const QString& regexp, const QString& text){
	QRegularExpression re(regexp);
	QRegularExpressionMatch match = re.match(text);
	if(match.hasMatch())
		return match.captured(1);
	return "";
}

void QFbMessenger::getRequestId(const QString& response){
	this->requestId = this->getMatch(REQUEST_ID_REGEXP, response);
	qDebug() << "new requestId val: " << this->requestId;
}

void QFbMessenger::getIdentifier(const QString& response){
	this->identifier = this->getMatch(IDENTIFIER_REGEXP, response);
	qDebug() << "new identifier val: " << this->identifier;
}

void QFbMessenger::getJsDatr(const QString& response){
	this->jsDatr = this->getMatch(JS_DATR_REGEXP, response);
	QNetworkCookie cookie(QString("datr").toUtf8(), this->jsDatr.toUtf8());
	cookie.setHttpOnly(true);
	cookie.setSecure(true);
	cookie.setPath(FB_COOKIE_PATH);
	cookie.setDomain(FB_COOKIE_DOMAIN);
	this->man.cookieJar()->insertCookie(cookie);

	qDebug() << "new cookie " << cookie;
	qDebug() << "new jsDatr val: " << this->jsDatr;
}

void QFbMessenger::getLsd(const QString& response){
	this->lsd = this->getMatch(LSD_REGEXP, response);
	qDebug() << "new lsd val: " << this->lsd;
}

void QFbMessenger::login(const QString &email, const QString &pass){
	qDebug() << "Login: user: " << email << " no password printing :)" /*<< pass*/;
	this->email = email;

	QUrlQuery query;
	query.addQueryItem(QStringLiteral("lsd"), this->lsd);
	query.addQueryItem(QStringLiteral("initial_request_id"), this->requestId);
	query.addQueryItem(QStringLiteral("email"), this->email);
	query.addQueryItem(QStringLiteral("pass"), pass);
	query.addQueryItem(QStringLiteral("default_persistent"), "0");

	QNetworkReply* reply = this->createRequest(FB_LOGIN_ENDPOINT, &query, QFbMessenger::Post);
	QSignalMapper *mapper = new QSignalMapper(reply);
	connect(reply, SIGNAL(finished()), mapper, SLOT(map()));
	mapper->setMapping(reply, reply);
	connect(mapper, SIGNAL(mapped(QObject*)), this, SLOT(loginFinished(QObject*)));
}

void QFbMessenger::loginFinished(QObject* obj){
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(obj);
	QNetworkReply::NetworkError err = reply->error();

	if(err != QNetworkReply::NoError){
		qDebug() << "Request Failed! n: " << err << " str: '" << reply->errorString() << "'";
		emit loginResponse(true, reply->errorString());
	}else{
		QString response = QString(reply->readAll());
		//qDebug() << "Login response...\n" << response << '\n';
		emit loginResponse(false, "");
	}

	reply->close();
	reply->deleteLater();
}

void QFbMessenger::getBasicInformation(){
	QNetworkReply* reply = this->createRequest(FB_CONVERSATIONS_ENDPOINT);
	QSignalMapper *mapper = new QSignalMapper(reply);
	connect(reply, SIGNAL(finished()), mapper, SLOT(map()));
	mapper->setMapping(reply, reply);
	connect(mapper, SIGNAL(mapped(QObject*)), this, SLOT(getBasicInformationFinished(QObject*)));
}

void QFbMessenger::getBasicInformationFinished(QObject* obj){
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(obj);
	QNetworkReply::NetworkError err = reply->error();

	if(err != QNetworkReply::NoError){
		qDebug() << "Request Failed! n: " << err << " str: '" << reply->errorString() << "'";
		emit getBasicInformationResponse(true, QJsonValue(reply->errorString()));
	}else{
		QString response = QString(reply->readAll());
		//qDebug() << "GetConversations response...\n" << response << '\n';
		this->parseDtsg(response);
		QJsonObject data;
		data.insert("friendsList", QJsonValue(this->parseInitialChatFriendsList(response)));
		//qDebug() << data.value("friendsList");
		data.insert("conversations", QJsonValue(this->parseLastConversations(response)));
		//qDebug() << data.value("conversations");
		emit getBasicInformationResponse(false, QJsonValue(data));
	}

	reply->close();
	reply->deleteLater();
}

QJsonArray QFbMessenger::parseInitialChatFriendsList(const QString& response){
	QString json = this->getMatch("\\(require\\(\"ServerJSDefine\"\\)\\).handleDefines\\((.*)\\);require\\(\"InitialJSLoader\"\\)", response);
	QJsonDocument document(QJsonDocument::fromJson(json.toUtf8()));
	QJsonArray array = document.array();

	for(QJsonArray::iterator it= array.begin(); it != array.end(); it++){
		if(!(*it).isArray())
			continue;

		QJsonArray option = (*it).toArray();
		if(option.first().toString() == "InitialChatFriendsList")
			return option[2].toObject().value("list").toArray();
	}

	return QJsonArray();
}

QJsonArray QFbMessenger::parseLastConversations(const QString& response){
	QString json = this->getMatch("require\\(\"InitialJSLoader\"\\).handleServerJS\\((.*)\\);</script>", response);
	QJsonDocument document(QJsonDocument::fromJson(json.toUtf8()));
	QJsonArray requireArray = document.object().value("require").toArray();

	for(QJsonArray::iterator it= requireArray.begin(); it != requireArray.end(); it++){
		if(!(*it).isArray())
			continue;

		QJsonArray option = (*it).toArray();
		if(option.first().toString() == "MessengerMount")
			return option[3].toArray()[1].toObject().value("mercuryPayload").toObject().value("threads").toArray();
	}

	return QJsonArray();
}

void QFbMessenger::parseDtsg(const QString& response){
	QString json = this->getMatch("\\(require\\(\"ServerJSDefine\"\\)\\).handleDefines\\((.*)\\);new \\(require\\(\"ServerJS\"", response);
	//qDebug() << "json text...\n" << json;
	QJsonDocument document(QJsonDocument::fromJson(json.toUtf8()));
	QJsonArray array = document.array();

	for(QJsonArray::iterator it= array.begin(); it != array.end(); it++){
		if(!(*it).isArray())
			continue;

		QJsonArray option = (*it).toArray();
		if(option.first().toString() == "DTSGInitialData"){
			this->dtsg = option[2].toObject().value("token").toString();
			break;
		}
	}

	qDebug() << "new dtsg val: " << this->dtsg;
}

QJsonValue QFbMessenger::parseProfiles(const QString& response){
	QJsonDocument document(QJsonDocument::fromJson(response.toUtf8()));
	return document.object().value("payload").toObject().value("profiles");
}

void QFbMessenger::getUserInfo(const QJsonArray& ids){
	QUrlQuery query;
	int i=0;
	for(QJsonArray::const_iterator it=ids.begin(); it != ids.end(); it++, i++)
		query.addQueryItem(QString("ids[%1]").arg(i), (*it).toString().split("-").at(0));

	query.addQueryItem("__a", "1");
	query.addQueryItem("fb_dtsg", this->dtsg);

	QNetworkReply* reply = this->createRequest(FB_USERINFO_ENDPOINT, &query, QFbMessenger::Post);
	QSignalMapper *mapper = new QSignalMapper(reply);
	connect(reply, SIGNAL(finished()), mapper, SLOT(map()));
	mapper->setMapping(reply, reply);
	connect(mapper, SIGNAL(mapped(QObject*)), this, SLOT(getUserInfoFinished(QObject*)));
}

void QFbMessenger::getUserInfoFinished(QObject* obj){
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(obj);
	QNetworkReply::NetworkError err = reply->error();

	if(err != QNetworkReply::NoError){
		qDebug() << "Request Failed! n: " << err << " str: '" << reply->errorString() << "'";
		emit getUserInfoResponse(true, QJsonValue(reply->errorString()));
	}else{
		QString response = QString(reply->readAll()).remove(0, 9);
		//qDebug() << "Response...\n" << response;
		emit getUserInfoResponse(false, this->parseProfiles(response));
	}

	reply->close();
	reply->deleteLater();
}
