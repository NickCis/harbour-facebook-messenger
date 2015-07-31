#include "qfb-network.h"

#include <QDebug>
#include <QtGlobal>
#include <QDateTime>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValueRef>
#include <QSignalMapper>
#include <QJsonDocument>
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

QFbNetwork::QFbNetwork(QObject *parent) :
	QObject(parent),
	requestId(""),
	identifier(""),
	lsd(""),
	userId(""),
	pullSeq("0"),
	sessionId(generateSessionId()),
	stickyToken(""),
	stickyPool("")
{
	qDebug() << "New QFbNetwork :: protocol: '" << FB_PROTOCOL << "' host: '" << FB_HOST << "' user agent: '" << USER_AGENT << "'";
	qsrand(QDateTime::currentDateTime().toTime_t());
	this->man.setProtocol(FB_PROTOCOL);
	this->man.setHost(FB_HOST);
	this->man.setUserAgent(USER_AGENT);
}

void QFbNetwork::init(){
	qDebug() << "init request. endpoint: " << FB_INIT_ENDPOINT;

	connect(
		new NetworkReplyResponse(this->man.createNodeRequest(FB_INIT_ENDPOINT)),
		SIGNAL(finished(QNetworkReply::NetworkError, const QByteArray&)),
		this,
		SLOT(initFinished(QNetworkReply::NetworkError, const QByteArray&))
	);
}

void QFbNetwork::initFinished(QNetworkReply::NetworkError err, const QByteArray& response){
	if(err != QNetworkReply::NoError){
		qDebug() << "init failed n: " << err;
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


QString QFbNetwork::getMatch(const QString& regexp, const QString& text){
	QRegularExpression re(regexp);
	QRegularExpressionMatch match = re.match(text);
	if(match.hasMatch())
		return match.captured(1);
	return "";
}

void QFbNetwork::getRequestId(const QString& response){
	this->requestId = this->getMatch(REQUEST_ID_REGEXP, response);
	qDebug() << "new requestId val: " << this->requestId;
	emit newConfigurationValue("requestId", this->requestId);
}

void QFbNetwork::getIdentifier(const QString& response){
	this->identifier = this->getMatch(IDENTIFIER_REGEXP, response);
	qDebug() << "new identifier val: " << this->identifier;
	emit newConfigurationValue("identifier", this->identifier);
}

void QFbNetwork::getJsDatr(const QString& response){
	QString jsDatr = this->getMatch(JS_DATR_REGEXP, response);
	this->man.setCookie("datr", jsDatr, FB_COOKIE_PATH, FB_COOKIE_DOMAIN);
	qDebug() << "new jsDatr val: " << jsDatr;
	emit newConfigurationValue("datr", jsDatr);
}

void QFbNetwork::getLsd(const QString& response){
	this->lsd = this->getMatch(LSD_REGEXP, response);
	qDebug() << "new lsd val: " << this->lsd;
	emit newConfigurationValue("lsd", this->lsd);
}

void QFbNetwork::login(const QString &email, const QString &pass){
	qDebug() << "Login: user: " << email << " no password printing :)" /*<< pass*/;

	QUrlQuery query;
	query.addQueryItem(QStringLiteral("lsd"), this->lsd);
	query.addQueryItem(QStringLiteral("initial_request_id"), this->requestId);
	query.addQueryItem(QStringLiteral("email"), email);
	query.addQueryItem(QStringLiteral("pass"), pass);
	query.addQueryItem(QStringLiteral("default_persistent"), "0");

	connect(
		new NetworkReplyResponse(this->man.createNodeRequest(FB_LOGIN_ENDPOINT, &query, NetworkManager::Post)),
		SIGNAL(finished(QNetworkReply::NetworkError, const QByteArray&)),
		this,
		SLOT(loginFinished(QNetworkReply::NetworkError, const QByteArray&))
	);
}

void QFbNetwork::loginFinished(QNetworkReply::NetworkError err, const QByteArray& response){

	if(err != QNetworkReply::NoError){
		qDebug() << " Request Failed! n: " << err;
		emit loginResponse(true, QString::number(err));
		return;
	}

	// TODO: check if correct
	QString responseStr(response);
	this->userId = this->man.getCookie("c_user");
	emit newConfigurationValue("c_user", this->userId);
	emit newConfigurationValue("xs", this->man.getCookie("xs"));
	emit newConfigurationValue("csm", this->man.getCookie("csm"));
	emit newConfigurationValue("s", this->man.getCookie("s"));
	emit newConfigurationValue("lu", this->man.getCookie("lu"));

	//qDebug() << " response...\n" << response << '\n';
	emit loginResponse(false, "");
}

void QFbNetwork::getBasicInformation(){
	connect(
		new NetworkReplyResponse(this->man.createNodeRequest(FB_CONVERSATIONS_ENDPOINT)),
		SIGNAL(finished(QNetworkReply::NetworkError, const QByteArray&)),
		this,
		SLOT(getBasicInformationFinished(QNetworkReply::NetworkError, const QByteArray&))
	);
}

void QFbNetwork::getBasicInformationFinished(QNetworkReply::NetworkError err, const QByteArray& response){
	if(err != QNetworkReply::NoError){
		qDebug() << "Request Failed! n: " << err;
		emit getBasicInformationResponse(true, QJsonValue(QString::number(err)));
		return;
	}

	QString responseStr(response);
	//qDebug() << " response...\n" << response << '\n';
	this->parseDtsg(responseStr);
	QJsonObject data;
	data.insert("friendsList", QJsonValue(this->parseInitialChatFriendsList(responseStr)));
	//qDebug() << data.value("friendsList");
	data.insert("conversations", QJsonValue(this->parseLastConversations(responseStr)));
	//qDebug() << data.value("conversations");
	emit getBasicInformationResponse(false, QJsonValue(data));
}

QJsonArray QFbNetwork::parseInitialChatFriendsList(const QString& response){
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

QJsonArray QFbNetwork::parseLastConversations(const QString& response){
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

void QFbNetwork::parseDtsg(const QString& response){
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
			emit newConfigurationValue("dtsg", this->dtsg);
			break;
		}
	}

	qDebug() << "new dtsg val: " << this->dtsg;
}

QJsonValue QFbNetwork::parseProfiles(const QString& response){
	QJsonDocument document(QJsonDocument::fromJson(response.toUtf8()));
	return document.object().value("payload").toObject().value("profiles");
}

QJsonValue QFbNetwork::parseThread(const QString& response){
	QJsonDocument document(QJsonDocument::fromJson(response.toUtf8()));
	return document.object().value("payload");
}

void QFbNetwork::getUserInfo(const QJsonArray& ids){
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

void QFbNetwork::getUserInfoFinished(QNetworkReply::NetworkError err, const QByteArray& response){
	if(err != QNetworkReply::NoError){
		qDebug() << "Request Failed! n: " << err;
		emit getUserInfoResponse(true, QJsonValue(QString::number(err)));
		return;
	}

	QString responseStr(response.mid(9));
	//qDebug() << "Response...\n" << responseStr;
	emit getUserInfoResponse(false, this->parseProfiles(responseStr));
}

void QFbNetwork::getThreadInfo(const QString& id, int offset, int limit){
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

void QFbNetwork::getThreadInfoFinished(QNetworkReply::NetworkError err, const QByteArray& response){
	if(err != QNetworkReply::NoError){
		qDebug() << "Request Failed! n: " << err;
		emit getUserInfoResponse(true, QJsonValue(QString::number(err)));
		return;
	}

	QString responseStr(response.mid(9));
	//qDebug() << "Response...\n" << response;
	emit getThreadInfoResponse(false, this->parseThread(responseStr));
}

void QFbNetwork::sendMessages(const QString& id, const QString& msg){
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

void QFbNetwork::sendMessagesFinished(QNetworkReply::NetworkError err, const QByteArray& response){
	if(err != QNetworkReply::NoError){
		qDebug() << " Request Failed! n: " << err;
		emit sendMessagesResponse(true, QJsonValue(QString::number(err)));
		return;
	}

	QString responseStr(response.mid(9));
	qDebug() <<  " Response...\n" << responseStr;
	emit sendMessagesResponse(false, QJsonValue());
}

void QFbNetwork::pull(){
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

void QFbNetwork::pullMessage(const QByteArray& data){
	qDebug() << " :: " << QString(data);
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

void QFbNetwork::pullFinished(){
	qDebug() << "pull finished" ;
}

QString QFbNetwork::generateCb(){
	// (1048576 * Math.random() | 0).toString(36);
	return QString::number((qrand() % 1048576), 36);
}

QString QFbNetwork::generateSessionId(){
	// (Math.random() * 2147483648 | 0).toString(16)
	ulong r = (qrand() % 2147483648UL);
	return QString::number(r, 16);
}

bool QFbNetwork::setConfigurationValue(const QString& name, const QString& value){
	if(name == "requestId")
		this->requestId = value;
	else if(name == "identifier")
		this->identifier = value;
	else if(name == "datr")
		this->man.setCookie("datr", value, FB_COOKIE_PATH, FB_COOKIE_DOMAIN);
	else if(name == "lst")
		this->lsd = value;
	else if(name == "c_user")
		this->man.setCookie("c_user", value, FB_COOKIE_PATH, FB_COOKIE_DOMAIN);
	else if(name == "xs")
		this->man.setCookie("xs", value, FB_COOKIE_PATH, FB_COOKIE_DOMAIN);
	else if(name == "csm")
		this->man.setCookie("csm", value, FB_COOKIE_PATH, FB_COOKIE_DOMAIN);
	else if(name == "lu")
		this->man.setCookie("lu", value, FB_COOKIE_PATH, FB_COOKIE_DOMAIN);
	else if(name == "dtsg")
		this->dtsg = value;
	else
		return false;

	return true;
}
