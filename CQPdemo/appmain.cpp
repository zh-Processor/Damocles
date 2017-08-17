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
#include "appmain.h" //应用AppID等信息，请正确填写，否则酷Q可能无法加载
#include <time.h>


#include "C:\\Python27\include\Python.h"

#pragma comment(lib, "C:\\Python27\\libs\\python27.lib")
using namespace std;

typedef struct {
    char **word;
    int len;
} keyword;

int flag;

char *welcome[] = {"三金是个大gaylao\n", "我们的征途是星辰与大海！\n", "溜金哇啦啊酷咧！\n", "啥玩应啊？咋回事啊？这咋整啊？大佬你会不会啊？\n", "土土是个萌妹子，人美声甜\n", "不会百度嘛？不会Google嘛？\n", "竜が我が敵を喰らえ!\n", "c语言我只服c primer plus\n",
                   "欢迎来到翻车大道\n", "Just hack for fun\n", "As we do, as you know \n"};
int lenWelcode = sizeof(welcome) / 4;
CRITICAL_SECTION _critical;

int ac = -1; //AuthCode 调用酷Q的方法时需要用到
bool enabled = false;
PyObject *pModule;
void checkImage(int64_t fromGroup, int64_t fromQQ, const char *msg);
/* 
* 返回应用的ApiVer、Appid，打包后将不会调用
*/
CQEVENT(const char*, AppInfo, 0)()
{
    return CQAPPINFO;
}


/* 
* 接收应用AuthCode，酷Q读取应用信息后，如果接受该应用，将会调用这个函数并传递AuthCode。
* 不要在本函数处理其他任何代码，以免发生异常情况。如需执行初始化代码请在Startup事件中执行（Type=1001）。
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode)
{
    ac = AuthCode;
    return 0;
}


/*
* Type=1001 酷Q启动
* 无论本应用是否被启用，本函数都会在酷Q启动后执行一次，请在这里执行应用初始化代码。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventStartup, 0)()
{

    return 0;
}


/*
* Type=1002 酷Q退出
* 无论本应用是否被启用，本函数都会在酷Q退出前执行一次，请在这里执行插件关闭代码。
* 本函数调用完毕后，酷Q将很快关闭，请不要再通过线程等方式执行其他代码。
*/
CQEVENT(int32_t, __eventExit, 0)()
{

    return 0;
}

/*
* Type=1003 应用已被启用
* 当应用被启用后，将收到此事件。
* 如果酷Q载入时应用已被启用，则在_eventStartup(Type=1001,酷Q启动)被调用后，本函数也将被调用一次。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventEnable, 0)()
{
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
* Type=1004 应用将被停用
* 当应用被停用前，将收到此事件。
* 如果酷Q载入时应用已被停用，则本函数*不会*被调用。
* 无论本应用是否被启用，酷Q关闭前本函数都*不会*被调用。
*/
CQEVENT(int32_t, __eventDisable, 0)()
{
    enabled = false;
    Py_Finalize();
    DeleteCriticalSection(&_critical);
    return 0;
}


/*
* Type=21 私聊消息
* subType 子类型，11/来自好友 1/来自在线状态 2/来自群 3/来自讨论组
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, int32_t font)
{
    if (fromQQ == 85645231 || fromQQ == 387210935 || fromQQ == 269106906 || fromQQ == 407508177) {
        char *getImage = "[CQ:image,file=";
        char *get = strstr((char *) msg, getImage);
        if (get != NULL) {
            checkImage(0, fromQQ, msg);
        }
    }
    //如果要回复消息，请调用酷Q方法发送，并且这里 return EVENT_BLOCK - 截断本条消息，不再继续处理  注意：应用优先级设置为"最高"(10000)时，不得使用本返回值
    //如果不回复消息，交由之后的应用/过滤器处理，这里 return EVENT_IGNORE - 忽略本条消息
    return EVENT_IGNORE;
}

void News(int64_t fromGroup)
{
    PyObject *pFunc, *pRet;
    char *bp;
    pFunc = PyObject_GetAttrString(pModule, "getNews");
    pRet = PyObject_CallObject(pFunc, NULL);
    if (pRet == NULL) {
        bp = "[CQ:at,qq=85645231]每日推送获取失败啦，快去修！！";
    } else {
        bp = PyString_AsString(pRet);
        Py_DECREF(pRet);
    }
    CQ_sendGroupMsg(ac, fromGroup, bp);
    Py_DECREF(pFunc);
}

void recentNews(int64_t fromGroup)
{
    PyObject *pFunc, *pRet;
    char *bp;
    pFunc = PyObject_GetAttrString(pModule, "getRecentNews");
    pRet = PyObject_CallObject(pFunc, NULL);
    if (pRet == NULL) {
        bp = "[CQ:at,qq=85645231]每日推送获取失败啦，快去修！！";
    } else {
        bp = PyString_AsString(pRet);
        Py_DECREF(pRet);
    }
    CQ_sendGroupMsg(ac, fromGroup, bp);
    Py_DECREF(pFunc);
}

void startGetNews(int64_t fromGroup)
{
    PyObject *pFunc, *pRet;
    char *bp;
    pFunc = PyObject_GetAttrString(pModule, "getDailyNews");
    pRet = PyObject_CallObject(pFunc, NULL);
    if (pRet == NULL) {
        bp = "[CQ:at,qq=85645231]每日推送获取失败啦，快去修！！！";
    } else {
        bp = PyString_AsString(pRet);
        Py_DECREF(pRet);
    }
    CQ_addLog(ac, 1, "test", bp);
    if (strcmp(bp, "null")) {
        CQ_sendGroupMsg(ac, fromGroup, bp);
    }
    Py_DECREF(pFunc);
}

void adminCmd(int64_t fromGroup, const char *msg)
{
    int64_t QQId;
    char *t;
    long int ti;
    char *bp = (char *) malloc(0x1000);
    if (strncmp(msg, "ban:", 4) == 0) {
        QQId = strtoll(&msg[4], &t, 10);
        ti = atoi(t + 1);
        CQ_setGroupBan(ac, fromGroup, QQId, 60 * ti);
    }
    if (strncmp(msg, "fban:", 5) == 0) {
        QQId = strtoll(&msg[5], &t, 10);
        ti = atoi(t + 1);
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

void requestAt(int64_t fromGroup, int64_t fromQQ, const char *msg)
{
    cachedMembers[fromQQ].qq = fromQQ;
    cachedMembers[fromQQ].atMasterCount ++;
    char *bp = (char *) malloc(0x1000);
    if(cachedMembers[fromQQ].atMasterCount > AVAILABLE_DAILY_AT)
    {
        CQ_setGroupBan(ac, fromGroup, fromQQ, 60);
        sprintf(bp, "[CQ:at,qq=%lld] 好烦啊你，今天你都@老子%d次了", cachedMembers[fromQQ].atMasterCount);
        CQ_sendGroupMsg(ac, fromGroup, bp);
    }
    for (int i = 0; i < 200; i++) {
        if (times[i * 2] == fromQQ) {
            times[i * 2 + 1]++;
            if (times[i * 2 + 1] == 4) {
                times[i * 2 + 1] = 0;
                CQ_setGroupBan(ac, fromGroup, fromQQ, 3600);
                sprintf(bp, "[CQ:at,qq=%lld] 老是找我是想干啥？皮？", fromQQ);
                CQ_sendGroupMsg(ac, fromGroup, bp);
                sprintf(bp, "[CQ:at,qq=%lld] 哎呦，咋不说说话了？", fromQQ);
                CQ_sendGroupMsg(ac, fromGroup, bp);
                sprintf(bp, "[CQ:at,qq=%lld] 你倒是说话啊!", fromQQ);
                CQ_sendGroupMsg(ac, fromGroup, bp);
                sprintf(bp, "[CQ:at,qq=%lld] 有啥问题说出来我们大家帮你解决啊!", fromQQ);
                CQ_sendGroupMsg(ac, fromGroup, bp);
                return;
            }
            break;
        } else if (times[i * 2] == 0) {
            times[i * 2] = fromQQ;
            break;
        }
        free(bp);
    }
    //sprintf(bp, "[CQ:at,qq=%lld] 找我干什么", fromQQ);
    //CQ_sendGroupMsg(ac, fromGroup, bp);
    free(bp);
}

void checkImage(int64_t fromGroup, int64_t fromQQ, const char *msg)
{
    PyObject *pFunc, *pArg, *pRet;
    int time;
    char *bp = (char *) malloc(0x1000);
    pFunc = PyObject_GetAttrString(pModule, "aliCheck");
    pArg = Py_BuildValue("(s)", msg);
    pRet = PyObject_CallObject(pFunc, pArg);
    time = PyInt_AsLong(pRet);
    if (time == -1) {
        CQ_addLog(ac, 1, "test", "error");
    } else if (time != 0) {
        CQ_setGroupBan(ac, fromGroup, fromQQ, time);
        sprintf(bp, "[CQ:at,qq=%lld] 都说了多少次了，我们是正紧群！", fromQQ);
        CQ_sendGroupMsg(ac, fromGroup, bp);
        Py_DECREF(pRet);
    }
    free(bp);
    Py_DECREF(pFunc);
    Py_DECREF(pArg);

}

char *(keyIsa[]) = {"安协", "协会", "信息安全协会", "网络空间安全协会", "Vidar信息安全实验室"};
char *(keyWhere[]) = {"哪", "位置", "地址", "怎么去", "怎么走", "咋去"};
int lenIsa = sizeof(keyIsa) / 4;
int lenWhere = sizeof(keyWhere) / 4;
keyword where[2] = {{keyIsa,   lenIsa},
                    {keyWhere, lenWhere}};

char *(keyA[]) = {"求", "想看", "有没有"};
char *(keyB[]) = {"nvzhuang", "女装"};
char *(keyC[]) = {"群主", "dalao", "大佬", "照", "照片"};
int lenA = sizeof(keyA) / 4;
int lenB = sizeof(keyB) / 4;
int lenC = sizeof(keyC) / 4;
keyword nvzhuang[] = {{keyA, lenA},
                      {keyB, lenB},
                      {keyC, lenC}};

char *(keyWhat[]) = {"c语言", "C语言", "编程", "黑客", "信安", "安全", "c", "C"};
int lenWhat = sizeof(keyWhat) / 4;
char *(keyHow[]) = {"什么", "怎么", "应该", "如何", "哪些", "有没有", "有关", "想", "教", "咋", "么?", "么？", "呢", "吗", "没？", "没?"};
int lenHow = sizeof(keyHow) / 4;
char *(keyLearn[]) = {"学", "了解", "书", "写", "看", "入门", "当", "做"};
int lenLearn = sizeof(keyLearn) / 4;
keyword learn[] = {{keyWhat,  lenWhat},
                   {keyHow,   lenHow},
                   {keyLearn, lenLearn}};

char *(keyHack[]) = {"日", "拿", "黑", "入侵", "攻击", "拖", "盗", "刷"};
int lenHack = sizeof(keyHack) / 4;
char *(keyWeb[]) = {"数据", "站", "杭电", "官网", "库", "QQ", "qq", "钻", "会员", "号", "挂"};
int lenWeb = sizeof(keyWeb) / 4;
keyword hack[] = {{keyHack, lenHack},
                  {keyWeb,  lenWeb},
                  {keyHow,  lenHow}};

char *(keyReg[]) = {"招新", "报名", "加入"};
int lenReg = sizeof(keyReg) / 4;
keyword reg[] = {{keyReg, lenReg},
                 {keyHow, lenHow}};

char *(keyPersion[]) = {"人"};
int lenPersion = sizeof(keyPersion) / 4;
keyword persion[] = {{keyHow,     lenHow},
                     {keyPersion, lenPersion},
                     {keyIsa,     lenIsa}};

int checkExist(keyword *key, const char *msg, int len)
{
    int i;
    for (i = 0; i < key[len - 1].len; i++) {
        if (strstr(msg, key[len - 1].word[i])) {
            if (len != 1) {
                return checkExist(key, msg, len - 1);
            } else {
                return 1;
            }
        }
    }
    return 0;
}

void checkWord(int64_t fromGroup, int64_t fromQQ, const char *msg)
{
    char *bp = (char *) malloc(0x1000);
    struct tm *local;
    time_t t;
    if (checkExist(nvzhuang, msg, 2)) {
        sprintf(bp, "[CQ:at,qq=%lld] 我给你10分钟去准备好你的女装");
        CQ_sendGroupMsg(ac, fromGroup, bp);
        CQ_setGroupBan(ac, fromGroup, fromQQ, 600);
    }
    if (checkExist(where, msg, 2)) {
        sprintf(bp, "[CQ:at,qq=%lld] 如果你是想问信息安全协会地址的话。是在一教（信仁楼）111，或者一教三楼“一教卖热狗”，还有科技馆613。\n欢迎随时过来[CQ:face,id=21]", fromQQ);
        CQ_sendGroupMsg(ac, fromGroup, bp);
    }
    if (checkExist(learn, msg, 3)) {
        sprintf(bp, "[CQ:at,qq=%lld] 如果想入门的话，还是要以c语言为基础。\n至于学习c语言最有效的还是看C primer plus。http://t.cn/RCP5AgV \nPS: 最好不要看谭浩强，XX天精通或者是从入门到精通系列 [CQ:face,id=21]", fromQQ);
        CQ_sendGroupMsg(ac, fromGroup, bp);
    }
    if (checkExist(hack, msg, 3)) {
        sprintf(bp,
                "[CQ:at,qq=%lld] 国家刑法第二百八十六条规定，\n关于恶意利用计算机犯罪相关条文对于违反国家规定，对计算机信息系统功能进行删除、修改、增加、干扰，造成计算机信息系统不能正常运行，后果严重的，处五年以下有期徒刑或者拘役；后果特别严重的，处五年以上有期徒刑。\n违反国家规定，对计算机信息系统中存储、处理或者传输的数据和应用程序进行删除、修改、增加的操作，后果严重的，依照前款的规定处罚。",
                fromQQ);
        CQ_sendGroupMsg(ac, fromGroup, bp);
    }
    if (checkExist(reg, msg, 2)) {
        sprintf(bp, "[CQ:at,qq=%lld] 线上的报名地址暂未开放，纸质在面试的时候带过来。\n推荐线上报名o(*^▽^*)┛[CQ:face,id=21]", fromQQ);
        CQ_sendGroupMsg(ac, fromGroup, bp);
    }
    if (checkExist(persion, msg, 3)) {
        t = time(NULL);
        local = localtime(&t);
        char timeb[200];
        if (local->tm_hour <= 6) {
            sprintf(bp, "[CQ:at,qq=%lld] 这么早估计没什么人起来", fromQQ);
        } else if (local->tm_hour >= 7 && local->tm_hour <= 9) {
            sprintf(bp, "[CQ:at,qq=%lld] 大概都在睡觉吧，不知道今天有没有人早起", fromQQ);
        } else if (local->tm_hour >= 10 && local->tm_hour <= 11) {
            sprintf(bp, "[CQ:at,qq=%lld] 这点应该有人开门了", fromQQ);
        } else if (local->tm_hour == 12) {
            sprintf(bp, "[CQ:at,qq=%lld] 不清楚，没有集体去吃午饭的话应该有人", fromQQ);
        } else if (local->tm_hour >= 13 && local->tm_hour <= 16) {
            sprintf(bp, "[CQ:at,qq=%lld] 下午一般都有人在的", fromQQ);
        } else if (local->tm_hour >= 17 && local->tm_hour <= 18) {
            sprintf(bp, "[CQ:at,qq=%lld] 不清楚，没有集体去吃晚饭的话应该有人", fromQQ);
        } else if (local->tm_hour >= 19 && local->tm_hour <= 21) {
            sprintf(bp, "[CQ:at,qq=%lld] 人应该都没回去呢", fromQQ);
        } else if (local->tm_hour >= 22 && local->tm_hour <= 23) {
            sprintf(bp, "[CQ:at,qq=%lld] 这么晚了，除非有人通宵，不然应该没人了", fromQQ);
        } else {
            sprintf(bp, "[CQ:at,qq=85645231] 你代码bug了", fromQQ);
        }
        sprintf(timeb, " %02d:%02d", local->tm_hour, local->tm_min);
        strcat(bp, timeb);
        CQ_sendGroupMsg(ac, fromGroup, bp);
        free(bp);
    }
    free(bp);
}

void checkWord1(int64_t fromGroup, int64_t fromQQ, const char *msg)
{
    int i;
    char *get;
    char *bp = (char *) malloc(0x1000);
    for (i = 0; i < lenIsa; i++) {
        if (strstr((char *) msg, keyIsa[i])) {
            break;
        }
    }
    if (i != lenIsa) {
        for (i = 0; i < lenWhere; i++) {
            if (strstr((char *) msg, keyWhere[i])) {
                break;
            }
        }
        if (i != lenWhere) {
            sprintf(bp, "[CQ:at,qq=%lld] 如果你是想问信息安全协会地址的话。是在一教（信仁楼）111，或者一教三楼“一教卖热狗”，还有科技馆613。\n欢迎随时过来[CQ:face,id=21]", fromQQ);
            CQ_sendGroupMsg(ac, fromGroup, bp);
        }
        free(bp);
    }
}
/*
* Type=2 群消息
*/
static time_t lastCheckTime;
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font)
{
    time_t timeCheck = time(NULL);
    struct tm *local = localtime(&timeCheck);
    struct tm *last  = localtime(&lastCheckTime);
    if(local->tm_yday != last->tm_yday)
    {
        lastCheckTime = timeCheck;
        cachedMembers.clear();//刷新所有人状态（清空cache）
    }
    if (fromGroup == ACTIVATED_QQGROUP && inList(fromQQ) && *msg == '$') {
        adminCmd(fromGroup, &msg[1]);
    }
    if (fromGroup == ACTIVATED_QQGROUP) {
        char *t;
        int64_t QQID = strtoll(&msg[5], &t, 10);
        int32_t reqTime = atoi(t + 1);
        srand(time(NULL));
        if (strncmp(msg, "roll:", 5) == 0) {
            cachedMembers[QQID].qq = QQID;
            cachedMembers[fromQQ].qq = fromQQ;
            char *bp = (char *) malloc(0x1000);
            if (fromQQ == QQID) {
                sprintf(bp, "[CQ:at,qq=%lld] 皮？", fromQQ);
                CQ_sendGroupMsg(ac, fromGroup, bp);
                CQ_setGroupBan(ac, fromGroup, fromQQ, 1 * 60);
                free(bp);
            } else {
                cachedMembers[fromQQ].cmdCount++;
                if (cachedMembers[fromQQ].cmdCount < AVAILABLE_DAILY_CMD)
                    rollFight(reqTime, cachedMembers[fromQQ], cachedMembers[QQID], fromGroup);
            }
        }
        if (strncmp(msg, "$revenge", 5) == 0) {//复仇
            cachedMembers[fromQQ].qq = fromQQ;
            //cachedMembers[fromQQ].cmdCount++;//算了，复仇不限次数
            //if (cachedMembers[fromQQ].cmdCount < AVAILABLE_DAILY_CMD)
            revenge(cachedMembers[fromQQ], fromGroup);
        }
    }
    if (fromGroup == ACTIVATED_QQGROUP) {
        char *atMe = "[CQ:at,qq=942666657]";
        char *at = strstr((char *) msg, atMe);

        if (strstr(msg, "禁言我") || strstr(msg, "求禁言") ||
            (strstr(msg, "求") && strstr(msg, "禁言") && (msg, "我"))) {

            srand((unsigned) time(NULL));
            // talk is cheap, show me your face~ :)
            CQ_setGroupBan(ac, fromGroup, fromQQ, rand() % (6 * 3600));
            CQ_sendGroupMsg(ac, fromGroup, "来呀！互相伤害啊！皮？");
            CQ_sendGroupMsg(ac, fromGroup, "[CQ:at,qq=%lld] 哎呦，咋不说说话了？");
            CQ_sendGroupMsg(ac, fromGroup, "[CQ:at,qq=%lld] 你倒是说话啊!？");
            CQ_sendGroupMsg(ac, fromGroup, "[CQ:at,qq=%lld] 有什么冤屈你说出来啊！");
        }

        if (at != NULL) {
            requestAt(fromGroup, fromQQ, msg);
        }
    }
    if (fromGroup == ACTIVATED_QQGROUP) {
        char *getImage = "[CQ:image,file=";
        char *get = strstr((char *) msg, getImage);
        if (get != NULL) {
            EnterCriticalSection(&_critical);
            checkImage(fromGroup, fromQQ, msg);
            LeaveCriticalSection(&_critical);
        }
    }
    if (fromGroup == ACTIVATED_QQGROUP) {
        checkWord(fromGroup, fromQQ, msg);
    }

    if (fromGroup == ACTIVATED_QQGROUP || fromGroup == 536559442) {
        if (!strcmp(msg, "每日消息推送") || !strcmp(msg, "每日新闻") || !strcmp(msg, "今日新闻")) {
            News(fromGroup);
        }
    }

    if (fromGroup == ACTIVATED_QQGROUP || fromGroup == 536559442) {
        if (!strcmp(msg, "近期消息推送") || !strcmp(msg, "最近消息推送") || !strcmp(msg, "最近新闻")) {
            recentNews(fromGroup);
        }
    }


    return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=4 讨论组消息
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t sendTime, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font)
{

    return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=101 群事件-管理员变动
* subType 子类型，1/被取消管理员 2/被设置管理员
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ)
{

    return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=102 群事件-群成员减少
* subType 子类型，1/群员离开 2/群员被踢 3/自己(即登录号)被踢
* fromQQ 操作者QQ(仅subType为2、3时存在)
* beingOperateQQ 被操作QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ)
{

    return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=103 群事件-群成员增加
* subType 子类型，1/管理员已同意 2/管理员邀请
* fromQQ 操作者QQ(即管理员QQ)
* beingOperateQQ 被操作QQ(即加群的QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ)
{
    char *bp = (char *) malloc(0x1000);
    if (fromGroup == ACTIVATED_QQGROUP) {
        srand(time(NULL));
        int index = rand() % lenWelcode;
        sprintf(bp,
                "[CQ:at,qq=%lld] 欢迎加入Vidar-Team2017届新生群\n请先阅读以下事项：\n1、协会官网: https://vidar.club \nwiki：https://wiki.vidar.club/doku.php \ndrops：https://drops.vidar.club/ \n2、为了让大家更好的相互了解，请先更改一下群名片。\n备注格式为17-专业-姓名\n3、如有任何疑问，请在群里艾特管理员提问 \n PS:%s",
                beingOperateQQ, welcome[index]);
        CQ_sendGroupMsg(ac, fromGroup, bp);
    }
    if (fromGroup == 198508284) {
        sprintf(bp, "欢迎加入杭州电子科技大学2016级网络空间安全学院新生群。\n为了让大家更好的相互了解，请先更改一下群名片。\n备注格式为专业-省份-姓名\nPS:自觉爆照哦[CQ:face,id=21]", beingOperateQQ);
        CQ_sendGroupMsg(ac, fromGroup, bp);
    }
    free(bp);
    return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=201 好友事件-好友已添加
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ)
{

    return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=301 请求-好友添加
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag)
{

    //CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

    return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=302 请求-群添加
* subType 子类型，1/他人申请入群 2/自己(即登录号)受邀入群
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag)
{

    //if (subType == 1) {
    //	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
    //} else if (subType == 2) {
    //	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
    //}

    return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}

/*
* 菜单，可在 .json 文件中设置菜单数目、函数名
* 如果不使用菜单，请在 .json 及此处删除无用菜单
*/
CQEVENT(int32_t, __menuA, 0)()
{
    MessageBoxA(NULL, "这是menuA，在这里载入窗口，或者进行其他工作。", "", 0);
    return 0;
}

CQEVENT(int32_t, __menuB, 0)()
{
    MessageBoxA(NULL, "这是menuB，在这里载入窗口，或者进行其他工作。", "", 0);
    return 0;
}

int64_t rollFight(uint32_t requestTime, MemberState &qq1, MemberState &qq2, int64_t fromGroup)
{
    int player1Roll = rand() % 100;
    Sleep(player1Roll);
    int player2Roll = rand() % 100;

    int64_t winner = -1;
    //根据rp修正roll点结果
    qq1.rpValue = qq1.rpValue > 1000 ? 1 : qq1.rpValue;//这叫 物极必反
    qq1.rpValue = qq1.rpValue < -500 ? -1 : qq1.rpValue;
    qq2.rpValue = qq2.rpValue > 1000 ? 1 : qq2.rpValue;
    qq2.rpValue = qq2.rpValue < -500 ? -1 : qq2.rpValue;

    int32_t rp1 = qq1.rpValue;
    int32_t rp2 = qq2.rpValue;

    if (inList(qq1.qq))
        rp1 += 200;//管理员buff
    if (inList(qq2.qq))
        rp2 += 200;

    //rp值最高可以提供3倍roll点加成，30roll成90
    //但，也可能roll成负的，如果成功太多次rp值负得很多，那。。。
    double correctionRatio = rp1 * 2.0 / 1000.0;
    player1Roll *= 1 + correctionRatio;
    correctionRatio = rp2 * 2.0 / 1000.0;
    player2Roll *= 1 + correctionRatio;

    //根据请求时间修正阈值
    // 1min  -> 60%
    // 5min  -> 40%
    // y=-0.05x+0.65
    // 10min -> 15%
    // 10min > ? < 30min:
    // y=1/x
    // >30min=>impossible
    int threshold = 0;
    if (requestTime == 0)
        threshold = 20;
    if (requestTime <= 10)
        threshold = 65 - 5 * requestTime;
    else if (requestTime > 10 && requestTime < 30)
        threshold = 100 / requestTime;

    //判断输赢了
    if (requestTime != 0) {//杀
        bool player1Pass = player1Roll >= (100 - threshold);
        bool player2Pass = player2Roll >= threshold;
        char *bp = (char *) malloc(0x1000);
        if (player1Pass && !player2Pass)//p1 win
        {
            qq2.linkedQQ = qq1.qq;
            qq2.lastBanMinutes = qq2.banMinutes;
            qq2.banMinutes = requestTime;
            qq1.linkedQQ = 0;//杀赢了，就不再有复仇心了
            qq1.rpValue -= rand() % 100;//杀得越多，rp越低，越难赢
            qq2.rpValue += rand() % 50;
            sprintf(bp, "[CQ:at,qq=%lld] 成功捕获 [CQ:at,qq=%lld]作为RBQ。", qq1.qq, qq2.qq);
            CQ_sendGroupMsg(ac, fromGroup, bp);
            CQ_setGroupBan(ac, fromGroup, qq2.qq, requestTime * 60);
            sprintf(bp, "（本轮[CQ:at,qq=%lld] Roll出%d点，[CQ:at,qq=%lld] Roll出%d点，点数大于%d判定成功）", qq1.qq, player1Roll, qq2.qq, player2Roll, threshold);
            CQ_sendGroupMsg(ac, fromGroup, bp);
            winner = qq1.qq;
        }
        if (player1Pass && player2Pass)//成功但是被格挡，自己也要吃伤害
        {
            qq2.linkedQQ = qq1.qq;
            qq2.lastBanMinutes = qq2.banMinutes;
            qq2.banMinutes = requestTime;
            qq1.lastBanMinutes = qq1.banMinutes;
            qq1.banMinutes = 1;
            qq1.linkedQQ = qq2.qq;
            qq1.rpValue += rand() % 20;//安慰性鼓励
            sprintf(bp, "[CQ:at,qq=%lld] 试图捕获 [CQ:at,qq=%lld]作为RBQ，但对方战力高强，竟被格挡而遭反噬震伤", qq1.qq, qq2.qq);
            CQ_sendGroupMsg(ac, fromGroup, bp);
            CQ_setGroupBan(ac, fromGroup, qq2.qq, requestTime * 60);
            CQ_setGroupBan(ac, fromGroup, qq1.qq, 1 * 60);
            sprintf(bp, "（本轮[CQ:at,qq=%lld] Roll出%d点，[CQ:at,qq=%lld] Roll出%d点，点数大于%d判定成功）", qq1.qq, player1Roll, qq2.qq, player2Roll, threshold);
            CQ_sendGroupMsg(ac, fromGroup, bp);
            winner = 0;
        }
        if (!player1Pass && player2Pass)//哇被反杀，会不会玩，怼自己的吧
        {
            qq1.linkedQQ = qq1.qq;//哈哈哈，下次触发复仇复仇的是自己，这里挖个坑
            qq1.lastBanMinutes = qq1.banMinutes;
            qq1.banMinutes = requestTime;
            qq2.lastBanMinutes = qq2.banMinutes;
            qq2.banMinutes = 0;
            qq1.rpValue += rand() % 100;
            qq2.rpValue -= rand() % 50;
            sprintf(bp, "[CQ:at,qq=%lld] 试图捕获 [CQ:at,qq=%lld]作为RBQ，但", qq1.qq, qq2.qq);
            CQ_sendGroupMsg(ac, fromGroup, bp);
            sprintf(bp, "。。真鸡儿弱， [CQ:at,qq=%lld] 色厉内荏，竟然被反杀了，会不会玩？氪金py删号重练，选一个吧", qq1.qq);
            CQ_sendGroupMsg(ac, fromGroup, bp);
            CQ_setGroupBan(ac, fromGroup, qq1.qq, requestTime * 60);
            sprintf(bp, "（本轮[CQ:at,qq=%lld] Roll出%d点，[CQ:at,qq=%lld] Roll出%d点，点数大于%d判定成功）", qq1.qq, player1Roll, qq2.qq, player2Roll, threshold);
            CQ_sendGroupMsg(ac, fromGroup, bp);
            winner = qq2.qq;
        }
        if (!player1Pass && !player2Pass)//非洲难兄难弟，相安无事
        {
            qq1.linkedQQ = qq2.qq;
            qq2.linkedQQ = qq1.qq;//虽然是菜鸡互啄，但仇恨必须建立
            qq1.lastBanMinutes = qq1.banMinutes;
            qq1.banMinutes = 0;
            qq2.lastBanMinutes = qq2.banMinutes;
            qq2.banMinutes = 0;
            qq1.rpValue += rand() % 100;//你俩真惨，鼓励一下
            qq2.rpValue += rand() % 100;
            sprintf(bp, "[CQ:at,qq=%lld] 试图捕获 [CQ:at,qq=%lld]作为RBQ。", qq1.qq, qq2.qq);
            CQ_sendGroupMsg(ac, fromGroup, bp);
            sprintf(bp, "。。菜鸡儿打架，不看也罢", qq1.qq);
            CQ_sendGroupMsg(ac, fromGroup, bp);
            sprintf(bp, "（本轮[CQ:at,qq=%lld] Roll出%d点，[CQ:at,qq=%lld] Roll出%d点，点数大于%d判定成功）", qq1.qq, player1Roll, qq2.qq, player2Roll, threshold);
            CQ_sendGroupMsg(ac, fromGroup, bp);
            winner = 0;
        }
        free(bp);
    } else //救人，只对发起者做判定
    {
        qq2.linkedQQ = 0;//被救的人不能复仇
        char *bp = (char *) malloc(0x1000);
        if (player1Roll >= threshold)//成功
        {
            qq1.rpValue -= 20;//救人成功 rp率不要降那么快
            sprintf(bp, "[CQ:at,qq=%lld] 试图拯救 [CQ:at,qq=%lld] ，他的宅心仁厚感动了操纵概率波函数的虚无之神，[CQ:at,qq=%lld] 得到了解脱。", qq1.qq, qq2.qq, qq2.qq);
            CQ_sendGroupMsg(ac, fromGroup, bp);
            CQ_setGroupBan(ac, fromGroup, qq2.qq, 0);
            winner = qq1.qq;
        } else//失败
        {
            //失败也不加rp值
            sprintf(bp, "[CQ:at,qq=%lld] 试图拯救 [CQ:at,qq=%lld] ，然而功力不足，反被连坐，你陪 [CQ:at,qq=%lld] 一起修炼修炼吧", qq1.qq, qq2.qq, qq2.qq);
            CQ_sendGroupMsg(ac, fromGroup, bp);
            int ban = qq2.banMinutes / 3;
            if (ban < 1)
                ban = 1;
            qq1.linkedQQ = qq2.qq;//人心险恶，我救你，你害我，我恨你。
            qq1.lastBanMinutes = qq1.banMinutes;
            qq1.banMinutes = (uint32_t) ban;
            CQ_setGroupBan(ac, fromGroup, qq1.qq, ban * 60);
            winner = 0;
        }
        free(bp);
    }
    return winner;
}

int64_t revenge(MemberState &qq1, int64_t fromGroup)//复仇
{
    char *bp = (char *) malloc(0x1000);
    sprintf(bp, "丧心病狂的 [CQ:at,qq=%lld] 想要向 [CQ:at,qq=%lld]发起复仇", qq1.qq, qq1.linkedQQ);
    CQ_sendGroupMsg(ac, fromGroup, bp);
    int64_t winner = -1;
    if (qq1.qq == qq1.linkedQQ) {
        qq1.linkedQQ = 0;
        sprintf(bp, "疯起来连自己都砍，他顺利把自己口上了10分钟");
        CQ_sendGroupMsg(ac, fromGroup, bp);
        CQ_setGroupBan(ac, fromGroup, qq1.qq, 10 * 60);
        winner = 0;
    } else if (qq1.linkedQQ == 0) {
        sprintf(bp, "……砍空气吗？只见它朝空中胡乱挥舞了几下，便随着一阵颤抖疲软下来。他累了，需要休息。");
        CQ_sendGroupMsg(ac, fromGroup, bp);
        CQ_setGroupBan(ac, fromGroup, qq1.qq, 2 * 60);
        winner = 0;
    } else {
        int32_t rp = qq1.rpValue;
        qq1.rpValue = 500;//复仇buff
        winner = rollFight(qq1.banMinutes, qq1, cachedMembers[qq1.linkedQQ], fromGroup);
        qq1.rpValue = rp + (qq1.rpValue - 500);//恢复buff
    }
    free(bp);

    return winner;
}
