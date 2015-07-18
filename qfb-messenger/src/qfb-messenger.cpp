#include "qfb-messenger.h"

#include <QDebug>
#include <QtGlobal>
#include <QDateTime>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValueRef>
#include <QSignalMapper>
#include <QJsonDocument>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QRegularExpression>

#include "network_reply_reader.h"
#include "network_reply_response.h"

#define USER_AGENT "Mozilla/5.0 (X11; Linux i686) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/43.0.2357.125 Safari/537.36"

#define FB_PROTOCOL "https"
#define FB_HOST "www.messenger.com"
#define FB_INIT_ENDPOINT "login"
#define FB_LOGIN_ENDPOINT "login/password/"
#define FB_CONVERSATIONS_ENDPOINT "/"
#define FB_USERINFO_ENDPOINT "chat/user_info/"
#define FB_THREADINFO_ENDPOINT "ajax/mercury/thread_info.php"
#define FB_SEND_MESSAGES_ENDPOINT "ajax/mercury/send_messages.php"
#define FB_PULL_ENDPOINT "https://4-edge-chat.messenger.com/pull"

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
	lsd(""),
	userId(""),
	pullSeq("0"),
	sessionId(generateSessionId()),
	stickyToken(""),
	stickyPool("")
{
	qDebug() << "Init QFbMessenger: ";
	qsrand(QDateTime::currentDateTime().toTime_t());
	this->man.setHost(FB_HOST);
	this->man.setProtocol(FB_PROTOCOL);
	this->man.setUserAgent(USER_AGENT);
}

void QFbMessenger::init(){
	qDebug() << "Hago request de init!";

	connect(
		new NetworkReplyResponse(this->man.createNodeRequest(FB_INIT_ENDPOINT)),
		SIGNAL(finished(QNetworkReply::NetworkError, const QByteArray&)),
		this,
		SLOT(initFinished(QNetworkReply::NetworkError, const QByteArray&))
	);
}

void QFbMessenger::initFinished(QNetworkReply::NetworkError err, const QByteArray& response){
	if(err != QNetworkReply::NoError){
		qDebug() << "Fallo el request n: " << err;
		emit initResponse(true, QString::number(err));
		return;
	}

	QString responseStr(response);
	this->getRequestId(responseStr);
	this->getIdentifier(responseStr);
	this->getJsDatr(responseStr);
	this->getLsd(responseStr);

	emit initResponse(false, "");
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
	qDebug() << __func__ << "Login: user: " << email << " no password printing :)" /*<< pass*/;
	this->email = email;

	QUrlQuery query;
	query.addQueryItem(QStringLiteral("lsd"), this->lsd);
	query.addQueryItem(QStringLiteral("initial_request_id"), this->requestId);
	query.addQueryItem(QStringLiteral("email"), this->email);
	query.addQueryItem(QStringLiteral("pass"), pass);
	query.addQueryItem(QStringLiteral("default_persistent"), "0");

	connect(
		new NetworkReplyResponse(this->man.createNodeRequest(FB_LOGIN_ENDPOINT, &query, NetworkManager::Post)),
		SIGNAL(finished(QNetworkReply::NetworkError, const QByteArray&)),
		this,
		SLOT(loginFinished(QNetworkReply::NetworkError, const QByteArray&))
	);
}

void QFbMessenger::getUserIdFromCookies(){
	QString urlStr = QString("%1://%2/").arg(FB_PROTOCOL).arg(FB_HOST);
	QList<QNetworkCookie> cookies = this->man.cookieJar()->cookiesForUrl(QUrl(urlStr));
	qDebug() << __func__ << " cookies for " << urlStr << " " << cookies;
	for(QList<QNetworkCookie>::const_iterator it=cookies.begin(); it != cookies.end(); it++){
		if(QString((*it).name()) == "c_user"){
			this->userId = QString((*it).value());
			qDebug() << __func__ << " new userId: " << this->userId;
			break;
		}
	}
}

void QFbMessenger::loginFinished(QNetworkReply::NetworkError err, const QByteArray& response){

	if(err != QNetworkReply::NoError){
		qDebug() << __func__ << " Request Failed! n: " << err;
		emit loginResponse(true, QString::number(err));
		return;
	}

	// TODO: check if correct
	QString responseStr(response);
	this->getUserIdFromCookies();
	//qDebug() << __func__ << " response...\n" << response << '\n';
	emit loginResponse(false, "");
}

void QFbMessenger::getBasicInformation(){
	connect(
		new NetworkReplyResponse(this->man.createNodeRequest(FB_CONVERSATIONS_ENDPOINT)),
		SIGNAL(finished(QNetworkReply::NetworkError, const QByteArray&)),
		this,
		SLOT(getBasicInformationFinished(QNetworkReply::NetworkError, const QByteArray&))
	);
}

void QFbMessenger::getBasicInformationFinished(QNetworkReply::NetworkError err, const QByteArray& response){
	if(err != QNetworkReply::NoError){
		qDebug() << "Request Failed! n: " << err;
		emit getBasicInformationResponse(true, QJsonValue(QString::number(err)));
		return;
	}

	QString responseStr(response);
	//qDebug() << __func__ << " response...\n" << response << '\n';
	this->parseDtsg(responseStr);
	QJsonObject data;
	data.insert("friendsList", QJsonValue(this->parseInitialChatFriendsList(responseStr)));
	//qDebug() << data.value("friendsList");
	data.insert("conversations", QJsonValue(this->parseLastConversations(responseStr)));
	//qDebug() << data.value("conversations");
	emit getBasicInformationResponse(false, QJsonValue(data));
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

QJsonValue QFbMessenger::parseThread(const QString& response){
	QJsonDocument document(QJsonDocument::fromJson(response.toUtf8()));
	return document.object().value("payload");
}

void QFbMessenger::getUserInfo(const QJsonArray& ids){
	QUrlQuery query;
	int i=0;
	for(QJsonArray::const_iterator it=ids.begin(); it != ids.end(); it++, i++)
		query.addQueryItem(QString("ids[%1]").arg(i), (*it).toString().split("-").at(0));

	query.addQueryItem("__a", "1");
	query.addQueryItem("fb_dtsg", this->dtsg);

	connect(
		new NetworkReplyResponse(this->man.createNodeRequest(FB_USERINFO_ENDPOINT, &query, NetworkManager::Post)),
		SIGNAL(finished(QNetworkReply::NetworkError, const QByteArray&)),
		this,
		SLOT(getUserInfoFinished(QNetworkReply::NetworkError, const QByteArray&))
	);
}

void QFbMessenger::getUserInfoFinished(QNetworkReply::NetworkError err, const QByteArray& response){
	if(err != QNetworkReply::NoError){
		qDebug() << "Request Failed! n: " << err;
		emit getUserInfoResponse(true, QJsonValue(QString::number(err)));
		return;
	}

	QString responseStr(response.mid(9));
	//qDebug() << "Response...\n" << responseStr;
	emit getUserInfoResponse(false, this->parseProfiles(responseStr));
}

void QFbMessenger::getThreadInfo(const QString& id, int offset, int limit){
	QUrlQuery query;
	query.addQueryItem("__a", "1");
	query.addQueryItem("fb_dtsg", this->dtsg);
	query.addQueryItem(QString("messages[user_ids][%1][limit]").arg(id), QString::number(limit));
	query.addQueryItem(QString("messages[user_ids][%1][offset]").arg(id), QString::number(offset));
	query.addQueryItem(QString("messages[user_ids][%1][timestamp]").arg(id), "0");

	qDebug() << query.toString();

	connect(
		new NetworkReplyResponse(this->man.createNodeRequest(FB_THREADINFO_ENDPOINT, &query, NetworkManager::Post)),
		SIGNAL(finished(QNetworkReply::NetworkError, const QByteArray&)),
		this, 
		SLOT(getThreadInfoFinished(QNetworkReply::NetworkError, const QByteArray&))
	);
}

void QFbMessenger::getThreadInfoFinished(QNetworkReply::NetworkError err, const QByteArray& response){
	if(err != QNetworkReply::NoError){
		qDebug() << "Request Failed! n: " << err;
		emit getUserInfoResponse(true, QJsonValue(QString::number(err)));
		return;
	}

	QString responseStr(response.mid(9));
	//qDebug() << "Response...\n" << response;
	emit getThreadInfoResponse(false, this->parseThread(responseStr));
}

void QFbMessenger::sendMessages(const QString& id, const QString& msg){
	QUrlQuery query;
	query.addQueryItem("__a", "1");
	query.addQueryItem("fb_dtsg", this->dtsg);
	query.addQueryItem("message_batch[0][action_type]", "ma-type:user-generated-message");
	query.addQueryItem("message_batch[0][body]", msg);
	query.addQueryItem("message_batch[0][source]", "source:messenger:web");
	query.addQueryItem("message_batch[0][specific_to_list][0]", QString("fbid:%1").arg(id));
	query.addQueryItem("message_batch[0][specific_to_list][1]", QString("fbid:%1").arg(this->userId));

	qDebug() << query.toString();

	connect(
		new NetworkReplyResponse(this->man.createNodeRequest(FB_SEND_MESSAGES_ENDPOINT, &query, NetworkManager::Post)), 
		SIGNAL(finished(QNetworkReply::NetworkError, const QByteArray&)),
		this, 
		SLOT(sendMessagesFinished(QNetworkReply::NetworkError, const QByteArray&))
	);
}

void QFbMessenger::sendMessagesFinished(QNetworkReply::NetworkError err, const QByteArray& response){
	if(err != QNetworkReply::NoError){
		qDebug() << __func__ << " Request Failed! n: " << err;
		emit sendMessagesResponse(true, QJsonValue(QString::number(err)));
		return;
	}

	QString responseStr(response.mid(9));
	qDebug() << __func__ << " Response...\n" << responseStr;
	emit sendMessagesResponse(false, QJsonValue());
}

void QFbMessenger::pull(){
	QUrlQuery query;

	query.addQueryItem("channel", QString("p_%1").arg(this->userId));
	query.addQueryItem("cap", "8");
	query.addQueryItem("partition", "-2");
	query.addQueryItem("uid", this->userId);
	query.addQueryItem("viewer_uid", this->userId);
	query.addQueryItem("state", "offline");
	query.addQueryItem("mode", "stream");
	query.addQueryItem("format", "json");
	query.addQueryItem("seq", this->pullSeq);
	query.addQueryItem("msgs_recv", "0");

	// Algo temporal
	query.addQueryItem("idle", "0");
	query.addQueryItem("cb", this->generateCb());
	query.addQueryItem("clientid", this->sessionId);
	// Ni idea que esto de abajo -> algo de un watchdog
	// query.addQueryItem("wtc", "0,0,0.000,0,0");
	query.addQueryItem("wtc", "");

	if(this->stickyToken.size())
		query.addQueryItem("sticky_token", this->stickyToken);
	if(this->stickyPool.size())
		query.addQueryItem("sticky_pool", this->stickyPool);

	qDebug() << query.toString();

	NetworkReplyReader *reader = new NetworkReplyReader(this->man.createRequest(FB_PULL_ENDPOINT, &query));
	connect(reader, SIGNAL(readLine(const QByteArray&)), this, SLOT(pullMessage(const QByteArray&)));
	connect(reader, SIGNAL(finished()), this, SLOT(pullFinished()));
}

void QFbMessenger::pullMessage(const QByteArray& data){
	qDebug() << __func__ << " :: " << QString(data);
	QJsonDocument document(QJsonDocument::fromJson(data[0] == '{' ? data : data.mid(9)));
	QJsonObject obj = document.object();

	QString type = obj.value("t").toString();
	if(type == "lb"){
		this->stickyToken = obj.value("lb_info").toObject().value("sticky").toString();
		this->stickyPool = obj.value("lb_info").toObject().value("pool").toString();
	}else if(type == "fullReload"){
		this->pullSeq = "0";
		this->stickyToken = "";
		this->stickyPool = "";
	}

	QJsonValue seq = obj.value("seq");
	if(!seq.isUndefined() && seq.isDouble()){
		this->pullSeq = QString::number(seq.toInt());
		qDebug() << "new pullSeq: " << this->pullSeq;
	}

	emit pullResponse(false, QJsonValue(obj));
}

void QFbMessenger::pullFinished(){
	qDebug() << __func__ ;
}

QString QFbMessenger::generateCb(){
	// (1048576 * Math.random() | 0).toString(36);
	return QString::number((qrand() % 1048576), 36);
}

QString QFbMessenger::generateSessionId(){
	// (Math.random() * 2147483648 | 0).toString(16)
	ulong r = (qrand() % 2147483648UL);
	return QString::number(r, 16);
}
