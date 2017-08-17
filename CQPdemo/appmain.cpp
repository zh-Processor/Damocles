/*
* CoolQ Demo for VC++ 
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "string"
#include "cqp.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "appmain.h" //Ӧ��AppID����Ϣ������ȷ��д�������Q�����޷�����
#include <time.h>


#include "C:\\Python27\include\Python.h"
#pragma comment(lib,"C:\\Python27\\libs\\python27.lib")
using namespace std;

typedef struct {
	char **word;
	int len;
}keyword;

int flag;

char *welcome[] = { "�����Ǹ���gaylao\n","���ǵ���;���ǳ���󺣣�\n","������������֣�\n","ɶ��Ӧ����զ���°�����զ������������᲻�ᰡ��\n","�����Ǹ������ӣ���������\n","����ٶ������Google�\n","�o���Ҥ�����Ф館!\n","c������ֻ��c primer plus\n","��ӭ�����������\n","Just hack for fun\n","As we do, as you know \n" };
int lenWelcode = sizeof(welcome) / 4;
CRITICAL_SECTION  _critical;

int ac = -1; //AuthCode ���ÿ�Q�ķ���ʱ��Ҫ�õ�
bool enabled = false;
PyObject *pModule;
void checkImage(int64_t fromGroup, int64_t fromQQ, const char *msg);
/* 
* ����Ӧ�õ�ApiVer��Appid������󽫲������
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/* 
* ����Ӧ��AuthCode����Q��ȡӦ����Ϣ��������ܸ�Ӧ�ã���������������������AuthCode��
* ��Ҫ�ڱ��������������κδ��룬���ⷢ���쳣���������ִ�г�ʼ����������Startup�¼���ִ�У�Type=1001����
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 ��Q����
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q������ִ��һ�Σ���������ִ��Ӧ�ó�ʼ�����롣
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}


/*
* Type=1002 ��Q�˳�
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q�˳�ǰִ��һ�Σ���������ִ�в���رմ��롣
* ������������Ϻ󣬿�Q���ܿ�رգ��벻Ҫ��ͨ���̵߳ȷ�ʽִ���������롣
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 Ӧ���ѱ�����
* ��Ӧ�ñ����ú󣬽��յ����¼���
* �����Q����ʱӦ���ѱ����ã�����_eventStartup(Type=1001,��Q����)�����ú󣬱�����Ҳ��������һ�Ρ�
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	Py_SetPythonHome("C:\\python27");
	Py_Initialize();
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('C:\\module')");
	pModule = PyImport_ImportModule("CQTools");
	InitializeCriticalSection(&_critical);
	return 0;
}


/*
* Type=1004 Ӧ�ý���ͣ��
* ��Ӧ�ñ�ͣ��ǰ�����յ����¼���
* �����Q����ʱӦ���ѱ�ͣ�ã��򱾺���*����*�����á�
* ���۱�Ӧ���Ƿ����ã���Q�ر�ǰ��������*����*�����á�
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	Py_Finalize();
	DeleteCriticalSection(&_critical);
	return 0;
}


/*
* Type=21 ˽����Ϣ
* subType �����ͣ�11/���Ժ��� 1/��������״̬ 2/����Ⱥ 3/����������
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, int32_t font) {
	if ( fromQQ == 85645231 || fromQQ == 387210935 ||fromQQ == 269106906 ||fromQQ == 407508177) {
		char *getImage = "[CQ:image,file=";
		char *get = strstr((char *)msg, getImage);
		if (get != NULL) {
			checkImage(0, fromQQ, msg);
		}
	}
	//���Ҫ�ظ���Ϣ������ÿ�Q�������ͣ��������� return EVENT_BLOCK - �ضϱ�����Ϣ�����ټ�������  ע�⣺Ӧ�����ȼ�����Ϊ"���"(10000)ʱ������ʹ�ñ�����ֵ
	//������ظ���Ϣ������֮���Ӧ��/�������������� return EVENT_IGNORE - ���Ա�����Ϣ
	return EVENT_IGNORE;
}

void News(int64_t fromGroup) {
	PyObject *pFunc,*pRet;
	char * bp;
	pFunc = PyObject_GetAttrString(pModule, "getNews");
	pRet = PyObject_CallObject(pFunc, NULL);
	if (pRet == NULL) {
		bp = "[CQ:at,qq=85645231]ÿ�����ͻ�ȡʧ��������ȥ�ޣ���";
	}
	else {
		bp = PyString_AsString(pRet);
		Py_DECREF(pRet);
	}
	CQ_sendGroupMsg(ac, fromGroup, bp);
	Py_DECREF(pFunc);
}

void recentNews(int64_t fromGroup) {
	PyObject *pFunc, *pRet;
	char * bp;
	pFunc = PyObject_GetAttrString(pModule, "getRecentNews");
	pRet = PyObject_CallObject(pFunc, NULL);
	if (pRet == NULL) {
		bp = "[CQ:at,qq=85645231]ÿ�����ͻ�ȡʧ��������ȥ�ޣ���";
	}
	else {
		bp = PyString_AsString(pRet);
		Py_DECREF(pRet);
	}
	CQ_sendGroupMsg(ac, fromGroup, bp);
	Py_DECREF(pFunc);
}
void startGetNews(int64_t fromGroup) {
	PyObject *pFunc, *pRet;
	char * bp;
	pFunc = PyObject_GetAttrString(pModule, "getDailyNews");
	pRet = PyObject_CallObject(pFunc, NULL);
	if (pRet == NULL) {
		bp = "[CQ:at,qq=85645231]ÿ�����ͻ�ȡʧ��������ȥ�ޣ�����";
	}
	else {
		bp = PyString_AsString(pRet);
		Py_DECREF(pRet);
	}
	CQ_addLog(ac, 1, "test", bp);
	if (strcmp(bp,"null")) {
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	Py_DECREF(pFunc);
}

void adminCmd(int64_t fromGroup, const char * msg) {
	int64_t QQId;
	char *t;
	long int ti;
	char *bp = (char *)malloc(0x1000);
	if (strncmp(msg, "ban:", 4) == 0) {
		QQId = strtoll(&msg[4], &t, 10);
		ti = atoi(t + 1);
		CQ_setGroupBan(ac, fromGroup, QQId, 60 * ti);
	}
	if (strncmp(msg, "fban:", 5) == 0) {
		QQId = strtoll(&msg[5],&t,10);
		ti = atoi(t+1);
		flag = 1;
		while (flag) {
			CQ_setGroupBan(ac, fromGroup, QQId, 60 * ti);
			Sleep(1000 * 60 * ti);
		}
	}
	if (strncmp(msg, "unban", 6) == 0) {
		flag = 0;
	}
	if (strncmp(msg, "news", 4) == 0) {
		while (1) {
			startGetNews(fromGroup);
			Sleep(1000 * 60 * 60);
		}
	}
	if (strncmp(msg, "at:", 3) == 0) {
		QQId = atoll(&msg[3]);
		sprintf(bp, "[CQ:at,qq=%lld] hello", QQId);
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	free(bp);
}

int64_t times[200];

void requestAt(int64_t fromGroup, int64_t fromQQ, const char *msg) {
	char *bp = (char *)malloc(0x1000);
	for (int i = 0; i < 200; i++) {
		if (times[i * 2] == fromQQ) {
			times[i * 2 + 1]++;
			if (times[i * 2 + 1] == 4) {
				times[i * 2 + 1] = 0;
				CQ_setGroupBan(ac, fromGroup, fromQQ, 3600);
				sprintf(bp, "[CQ:at,qq=%lld] �������������ɶ��Ƥ��", fromQQ);
				CQ_sendGroupMsg(ac, fromGroup, bp);
				sprintf(bp, "[CQ:at,qq=%lld] ���ϣ�զ��˵˵���ˣ�", fromQQ);
				CQ_sendGroupMsg(ac, fromGroup, bp);
				sprintf(bp, "[CQ:at,qq=%lld] �㵹��˵����!", fromQQ);
				CQ_sendGroupMsg(ac, fromGroup, bp);
				sprintf(bp, "[CQ:at,qq=%lld] ��ɶ����˵�������Ǵ�Ұ�������!", fromQQ);
				CQ_sendGroupMsg(ac, fromGroup, bp);
				return;
			}
			break;
		}
		else if (times[i * 2] == 0) {
			times[i * 2] = fromQQ;
			break;
		}
		free(bp);
	}
	//sprintf(bp, "[CQ:at,qq=%lld] ���Ҹ�ʲô", fromQQ);
	//CQ_sendGroupMsg(ac, fromGroup, bp);
	free(bp);
}

void checkImage(int64_t fromGroup, int64_t fromQQ,const char *msg) {
	PyObject *pFunc,*pArg,*pRet;
	int time;
	char *bp = (char *)malloc(0x1000);
	pFunc = PyObject_GetAttrString(pModule, "aliCheck");
	pArg = Py_BuildValue("(s)", msg);
	pRet = PyObject_CallObject(pFunc, pArg);
	time = PyInt_AsLong(pRet);
	if (time == -1) {
		CQ_addLog(ac, 1, "test", "error");
	}
	else if (time != 0) {
		CQ_setGroupBan(ac, fromGroup, fromQQ, time);
		sprintf(bp, "[CQ:at,qq=%lld] ��˵�˶��ٴ��ˣ�����������Ⱥ��", fromQQ);
		CQ_sendGroupMsg(ac, fromGroup, bp);
		Py_DECREF(pRet);
	}
	free(bp);
	Py_DECREF(pFunc);
	Py_DECREF(pArg);
	
}
char *(keyIsa[]) = {"��Э","Э��","��Ϣ��ȫЭ��","����ռ䰲ȫЭ��","Vidar��Ϣ��ȫʵ����"};
char *(keyWhere[]) = { "��","λ��","��ַ","��ôȥ","��ô��","զȥ"};
int lenIsa = sizeof(keyIsa) / 4;
int lenWhere = sizeof(keyWhere) / 4;
keyword where[2] = { {keyIsa,lenIsa},{keyWhere,lenWhere} };

char *(keyA[]) = { "��","�뿴","��û��" };
char *(keyB[]) = { "nvzhuang","Ůװ" };
char *(keyC[]) = { "Ⱥ��","dalao","����","��","��Ƭ" };
int lenA = sizeof(keyA) / 4;
int lenB = sizeof(keyB) / 4;
int lenC = sizeof(keyC) / 4;
keyword nvzhuang[] = { {keyA,lenA},{keyB,lenB},{keyC,lenC} };

char *(keyWhat[]) = { "c����","C����","���","�ڿ�","�Ű�","��ȫ", "c", "C"};
int lenWhat = sizeof(keyWhat) / 4;
char *(keyHow[]) = { "ʲô","��ô","Ӧ��","���","��Щ","��û��" ,"�й�", "��","��","զ","ô?","ô��","��","��","û��","û?" };
int lenHow = sizeof(keyHow) / 4;
char *(keyLearn[]) = { "ѧ","�˽�","��","д","��","����", "��", "��"};
int lenLearn = sizeof(keyLearn) / 4;
keyword learn[] = { { keyWhat,lenWhat },{ keyHow,lenHow },{ keyLearn,lenLearn } };

char *(keyHack[]) = {"��","��","��","����","����","��","��","ˢ"};
int lenHack = sizeof(keyHack) / 4;
char *(keyWeb[]) = { "����","վ","����","����", "��" ,"QQ","qq","��","��Ա","��","��"};
int lenWeb = sizeof(keyWeb) / 4;
keyword hack[] = { { keyHack ,lenHack },{ keyWeb ,lenWeb }, { keyHow ,lenHow } };

char *(keyReg[]) = {"����","����","����"};
int lenReg = sizeof(keyReg) / 4;
keyword reg[] = { { keyReg ,lenReg }, { keyHow ,lenHow } };

char *(keyPersion[]) = {"��"};
int lenPersion = sizeof(keyPersion) / 4;
keyword persion[] = { { keyHow,lenHow },{ keyPersion,lenPersion },{ keyIsa,lenIsa } };

int checkExist(keyword *key, const char *msg, int len) {
	int i;
	for (i = 0; i < key[len - 1].len; i++) {
		if (strstr(msg, key[len-1].word[i])) {
			if (len != 1) {
				return checkExist(key, msg, len - 1);
			}
			else {
				return 1;
			}
		}
	}
	return 0;
}

void checkWord(int64_t fromGroup, int64_t fromQQ, const char *msg) {
	char *bp = (char *)malloc(0x1000);
	struct tm *local;
	time_t t;
	if (checkExist(nvzhuang, msg, 2)){
		sprintf(bp, "[CQ:at,qq=%lld] �Ҹ���10����ȥ׼�������Ůװ");
		CQ_sendGroupMsg(ac, fromGroup, bp);
		CQ_setGroupBan(ac, fromGroup, fromQQ, 600);
	}
	if (checkExist(where, msg, 2)) {
		sprintf(bp, "[CQ:at,qq=%lld] �������������Ϣ��ȫЭ���ַ�Ļ�������һ�̣�����¥��111������һ����¥��һ�����ȹ��������пƼ���613��\n��ӭ��ʱ����[CQ:face,id=21]", fromQQ);
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	if (checkExist(learn, msg, 3)) {
		sprintf(bp, "[CQ:at,qq=%lld] ��������ŵĻ�������Ҫ��c����Ϊ������\n����ѧϰc��������Ч�Ļ��ǿ�C primer plus��http://t.cn/RCP5AgV \nPS: ��ò�Ҫ��̷��ǿ��XX�쾫ͨ�����Ǵ����ŵ���ͨϵ�� [CQ:face,id=21]", fromQQ);
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	if (checkExist(hack, msg, 3)) {
		sprintf(bp, "[CQ:at,qq=%lld] �����̷��ڶ��ٰ�ʮ�����涨��\n���ڶ������ü��������������Ķ���Υ�����ҹ涨���Լ������Ϣϵͳ���ܽ���ɾ�����޸ġ����ӡ����ţ���ɼ������Ϣϵͳ�����������У�������صģ���������������ͽ�̻��߾��ۣ�����ر����صģ���������������ͽ�̡�\nΥ�����ҹ涨���Լ������Ϣϵͳ�д洢��������ߴ�������ݺ�Ӧ�ó������ɾ�����޸ġ����ӵĲ�����������صģ�����ǰ��Ĺ涨������", fromQQ);
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	if (checkExist(reg, msg, 2)) {
		sprintf(bp, "[CQ:at,qq=%lld] ���ϵı�����ַ��δ���ţ�ֽ�������Ե�ʱ���������\n�Ƽ����ϱ���o(*^��^*)��[CQ:face,id=21]", fromQQ);
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	if (checkExist(persion, msg, 3)) {
		t = time(NULL);
		local = localtime(&t);
		char timeb[200];
		if (local->tm_hour <= 6) {
			sprintf(bp, "[CQ:at,qq=%lld] ��ô�����ûʲô������", fromQQ);
		}
		else if (local->tm_hour >= 7 && local->tm_hour <= 9) {
			sprintf(bp, "[CQ:at,qq=%lld] ��Ŷ���˯���ɣ���֪��������û��������", fromQQ);
		}
		else if (local->tm_hour >= 10 && local->tm_hour <= 11) {
			sprintf(bp, "[CQ:at,qq=%lld] ���Ӧ�����˿�����", fromQQ);
		}
		else if (local->tm_hour == 12) {
			sprintf(bp, "[CQ:at,qq=%lld] �������û�м���ȥ���緹�Ļ�Ӧ������", fromQQ);
		}
		else if (local->tm_hour >= 13 && local->tm_hour <= 16) {
			sprintf(bp, "[CQ:at,qq=%lld] ����һ�㶼�����ڵ�", fromQQ);
		}
		else if (local->tm_hour >= 17 && local->tm_hour <= 18) {
			sprintf(bp, "[CQ:at,qq=%lld] �������û�м���ȥ�����Ļ�Ӧ������", fromQQ);
		}
		else if (local->tm_hour >= 19 && local->tm_hour <= 21) {
			sprintf(bp, "[CQ:at,qq=%lld] ��Ӧ�ö�û��ȥ��", fromQQ);
		}
		else if (local->tm_hour >= 22 && local->tm_hour <= 23) {
			sprintf(bp, "[CQ:at,qq=%lld] ��ô���ˣ���������ͨ������ȻӦ��û����", fromQQ);
		}
		else {
			sprintf(bp, "[CQ:at,qq=85645231] �����bug��", fromQQ);
		}
		sprintf(timeb, " %02d:%02d", local->tm_hour, local->tm_min);
		strcat(bp, timeb);
		CQ_sendGroupMsg(ac, fromGroup, bp);
		free(bp);
	}
	free(bp);
}
void checkWord1(int64_t fromGroup, int64_t fromQQ, const char *msg) {
	int i;
	char *get;
	char *bp = (char *)malloc(0x1000);
	for (i = 0; i < lenIsa; i++) {
		if (strstr((char *)msg, keyIsa[i])) {
			break;
		}
	}
	if (i != lenIsa) {
		for (i = 0; i < lenWhere; i++) {
			if (strstr((char *)msg, keyWhere[i])) {
				break;
			}
		}
		if (i != lenWhere) {
			sprintf(bp, "[CQ:at,qq=%lld] �������������Ϣ��ȫЭ���ַ�Ļ�������һ�̣�����¥��111������һ����¥��һ�����ȹ��������пƼ���613��\n��ӭ��ʱ����[CQ:face,id=21]", fromQQ);
			CQ_sendGroupMsg(ac, fromGroup, bp);
		}
		free(bp);
	}
}
/*
* Type=2 Ⱥ��Ϣ
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	if (fromGroup == 650591057 && (fromQQ == 85645231 || fromQQ == 387210935 ||fromQQ == 269106906 ||fromQQ == 407508177) && *msg == '$') {
		adminCmd(fromGroup,&msg[1]);
	}
	if (fromGroup == 650591057) {
		int64_t QQID;
		int a1;
		int padding;
		char *t;
		long int ti;
		char *bp = (char *)malloc(0x1000);
		srand(time(NULL));
		if (strncmp(msg, "roll:", 5) == 0) {
			QQID = strtoll(&msg[5], &t, 10);
			padding = rand() % 100;
			Sleep(padding);
			a1 = rand() % 100;
			ti = atoi(t + 1);
			ti = ti / 1;
			if (ti >10 ) {
				ti = 10;
			}
			if (fromQQ == QQID || ti < 0) {
				sprintf(bp, "[CQ:at,qq=%lld] Ƥ��", fromQQ);
				CQ_sendGroupMsg(ac, fromGroup, bp);
				CQ_setGroupBan(ac, fromGroup, fromQQ, 600);
				free(bp);
			}
			else switch (ti)
			{
			case 1:
				if (a1 <= 70) {
					sprintf(bp, "[CQ:at,qq=%lld] �ɹ����� [CQ:at,qq=%lld]��ΪRBQ��(�ɹ���70%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, QQID, ti * 60);
					free(bp);
				}
				else {
					sprintf(bp, "[CQ:at,qq=%lld] ��ͼ���� [CQ:at,qq=%lld]��ΪRBQ,�����������(�ɹ���70%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, fromQQ, 60 * ti);
					free(bp);
				}
				break;
			case 2:
				if (a1 <= 65) {
					sprintf(bp, "[CQ:at,qq=%lld] �ɹ����� [CQ:at,qq=%lld]��ΪRBQ��(�ɹ���65%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, QQID, ti * 60);
					free(bp);
				}
				else {
					sprintf(bp, "[CQ:at,qq=%lld] ��ͼ���� [CQ:at,qq=%lld]��ΪRBQ,�����������(�ɹ���65%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, fromQQ, 60 * ti);
					free(bp);
				}
				break;
			case 3:
				if (a1 <= 60) {
					sprintf(bp, "[CQ:at,qq=%lld] �ɹ����� [CQ:at,qq=%lld]��ΪRBQ��(�ɹ���60%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, QQID, ti * 60);
					free(bp);
				}
				else {
					sprintf(bp, "[CQ:at,qq=%lld] ��ͼ���� [CQ:at,qq=%lld]��ΪRBQ,�����������(�ɹ���60%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, fromQQ, 60 * ti);
					free(bp);
				}
				break;
			case 4:
				if (a1 <= 55) {
					sprintf(bp, "[CQ:at,qq=%lld] �ɹ����� [CQ:at,qq=%lld]��ΪRBQ��(�ɹ���55%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, QQID, ti * 60);
					free(bp);
				}
				else {
					sprintf(bp, "[CQ:at,qq=%lld] ��ͼ���� [CQ:at,qq=%lld]��ΪRBQ,�����������(�ɹ���55%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, fromQQ, 60 * ti);
					free(bp);
				}
				break;
			case 5:
				if (a1 <= 50) {
					sprintf(bp, "[CQ:at,qq=%lld] �ɹ����� [CQ:at,qq=%lld]��ΪRBQ��(�ɹ���50%)", fromQQ ,QQID );
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, QQID, ti*60);
					free(bp);
				}
				else {
					sprintf(bp, "[CQ:at,qq=%lld] ��ͼ���� [CQ:at,qq=%lld]��ΪRBQ,�����������(�ɹ���50%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, fromQQ, 60 * ti);
					free(bp);
				}
				break;
			case 6:
				if (a1 <= 40) {
					sprintf(bp, "[CQ:at,qq=%lld] �ɹ����� [CQ:at,qq=%lld]��ΪRBQ��(�ɹ���40%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, QQID, ti * 60);
					free(bp);
				}
				else {
					sprintf(bp, "[CQ:at,qq=%lld] ��ͼ���� [CQ:at,qq=%lld]��ΪRBQ,�����������(�ɹ���40%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, fromQQ, 60 * ti);
					free(bp);
				}
				break;
			case 7:
				if (a1 <= 30) {
					sprintf(bp, "[CQ:at,qq=%lld] �ɹ����� [CQ:at,qq=%lld]��ΪRBQ��(�ɹ���30%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, QQID, ti * 60);
					free(bp);
				}
				else {
					sprintf(bp, "[CQ:at,qq=%lld] ��ͼ���� [CQ:at,qq=%lld]��ΪRBQ,�����������(�ɹ���30%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, fromQQ, 60 * ti);
					free(bp);
				}
				break;
			case 8:
				if (a1 <= 20) {
					sprintf(bp, "[CQ:at,qq=%lld] �ɹ����� [CQ:at,qq=%lld]��ΪRBQ��(�ɹ���20%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, QQID, ti * 60);
					free(bp);
				}
				else {
					sprintf(bp, "[CQ:at,qq=%lld] ��ͼ���� [CQ:at,qq=%lld]��ΪRBQ,�����������(�ɹ���20%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, fromQQ, 60 * ti);
					free(bp);
				}
				break;
			case 9:
				if (a1 <= 10) {
					sprintf(bp, "[CQ:at,qq=%lld] �ɹ����� [CQ:at,qq=%lld]��ΪRBQ��(�ɹ���10%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, QQID, ti * 60);
					free(bp);
				}
				else {
					sprintf(bp, "[CQ:at,qq=%lld] ��ͼ���� [CQ:at,qq=%lld]��ΪRBQ,�����������(�ɹ���10%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, fromQQ, 60 * ti);
					free(bp);
				}
				break;
			case 10:
				if (a1 <= 5) {
					sprintf(bp, "[CQ:at,qq=%lld] �ɹ����� [CQ:at,qq=%lld]��ΪRBQ��(�ɹ���5%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, QQID, ti * 60);
					free(bp);
				}
				else {
					sprintf(bp, "[CQ:at,qq=%lld] ��ͼ���� [CQ:at,qq=%lld]��ΪRBQ,�����������(�ɹ���5%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, fromQQ, 60 * ti);
					free(bp);
				}
				break;
			default:
				if (a1 <= 5) {
					sprintf(bp, "[CQ:at,qq=%lld] �ɹ��� [CQ:at,qq=%lld]��RBQ�н�ȳ�����(�ɹ���5%)", fromQQ, QQID);
					CQ_sendGroupMsg(ac, fromGroup, bp);
					CQ_setGroupBan(ac, fromGroup, QQID, ti * 60);
					free(bp);
				}
				else {
					sprintf(bp, "[CQ:at,qq=%lld] ��ͼ�� [CQ:at,qq=%lld]��RBQ�н�ȳ���,û�뵽 [CQ:at,qq=%lld]�Ѿ��������С�(�ɹ���5%)", fromQQ, QQID ,QQID );
					CQ_sendGroupMsg(ac, fromGroup, bp);
					free(bp);
				}
				break;
			}
		}
	}
	if (fromGroup == 650591057) {
		char *atMe = "[CQ:at,qq=942666657]";
		char *at = strstr((char *)msg, atMe);

		if (strstr(msg, "������") || strstr(msg, "�����") || 
			(strstr(msg, "��") && strstr(msg, "����") && (msg, "��"))) {

			srand((unsigned)time(NULL));
			// talk is cheap, show me your face~ :)
			CQ_setGroupBan(ac, fromGroup, fromQQ, rand() % (6 * 3600));
			CQ_sendGroupMsg(ac, fromGroup, "��ѽ�������˺�����Ƥ��");
			CQ_sendGroupMsg(ac, fromGroup, "[CQ:at,qq=%lld] ���ϣ�զ��˵˵���ˣ�");
			CQ_sendGroupMsg(ac, fromGroup, "[CQ:at,qq=%lld] �㵹��˵����!��");
			CQ_sendGroupMsg(ac, fromGroup, "[CQ:at,qq=%lld] ��ʲôԩ����˵��������");
		}

		if (at != NULL) {
			requestAt(fromGroup, fromQQ, msg);
		}
	}
	if (fromGroup == 650591057) {
		char *getImage = "[CQ:image,file=";
		char *get = strstr((char *)msg, getImage);
		if (get != NULL) {
			EnterCriticalSection(&_critical);
			checkImage(fromGroup, fromQQ, msg);
			LeaveCriticalSection(&_critical);
		}
	}
	if (fromGroup == 650591057 ) {
		checkWord(fromGroup, fromQQ, msg);
	}

	if (fromGroup == 650591057 || fromGroup == 536559442) {
		if (!strcmp(msg, "ÿ����Ϣ����") || !strcmp(msg, "ÿ������") || !strcmp(msg, "��������")) {
			News(fromGroup);
		}
	}

	if (fromGroup == 650591057 || fromGroup == 536559442) {
		if (!strcmp(msg, "������Ϣ����") || !strcmp(msg, "�����Ϣ����") || !strcmp(msg, "�������")) {
			recentNews(fromGroup);
		}
	}

	
	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=4 ��������Ϣ
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t sendTime, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=101 Ⱥ�¼�-����Ա�䶯
* subType �����ͣ�1/��ȡ������Ա 2/�����ù���Ա
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=102 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/ȺԱ�뿪 2/ȺԱ���� 3/�Լ�(����¼��)����
* fromQQ ������QQ(��subTypeΪ2��3ʱ����)
* beingOperateQQ ������QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=103 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/����Ա��ͬ�� 2/����Ա����
* fromQQ ������QQ(������ԱQQ)
* beingOperateQQ ������QQ(����Ⱥ��QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {
	char *bp = (char *)malloc(0x1000);
	if (fromGroup == 650591057) {
		srand(time(NULL));
		int index = rand() % lenWelcode;
		sprintf(bp, "[CQ:at,qq=%lld] ��ӭ����Vidar-Team2017������Ⱥ\n�����Ķ��������\n1��Э�����: https://vidar.club \nwiki��https://wiki.vidar.club/doku.php \ndrops��https://drops.vidar.club/ \n2��Ϊ���ô�Ҹ��õ��໥�˽⣬���ȸ���һ��Ⱥ��Ƭ��\n��ע��ʽΪ17-רҵ-����\n3�������κ����ʣ�����Ⱥ�ﰬ�ع���Ա���� \n PS:%s", beingOperateQQ, welcome[index]);
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	if (fromGroup == 198508284) {
		sprintf(bp, "��ӭ���뺼�ݵ��ӿƼ���ѧ2016������ռ䰲ȫѧԺ����Ⱥ��\nΪ���ô�Ҹ��õ��໥�˽⣬���ȸ���һ��Ⱥ��Ƭ��\n��ע��ʽΪרҵ-ʡ��-����\nPS:�Ծ�����Ŷ[CQ:face,id=21]", beingOperateQQ);
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	free(bp);
	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=201 �����¼�-���������
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=301 ����-�������
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=302 ����-Ⱥ���
* subType �����ͣ�1/����������Ⱥ 2/�Լ�(����¼��)������Ⱥ
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}

/*
* �˵������� .json �ļ������ò˵���Ŀ��������
* �����ʹ�ò˵������� .json ���˴�ɾ�����ò˵�
*/
CQEVENT(int32_t, __menuA, 0)() {
	MessageBoxA(NULL, "����menuA�����������봰�ڣ����߽�������������", "" ,0);
	return 0;
}

CQEVENT(int32_t, __menuB, 0)() {
	MessageBoxA(NULL, "����menuB�����������봰�ڣ����߽�������������", "" ,0);
	return 0;
}
