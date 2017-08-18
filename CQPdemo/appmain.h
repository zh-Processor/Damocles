#define CQAPPID "pw.libc.Damocles" //请修改AppID
#define CQAPPINFO CQAPIVERTEXT "," CQAPPID

#include <stdint.h>
#include <map>

#define ACTIVATED_QQGROUP 650591057
#define AVAILABLE_DAILY_CMD 20
#define AVAILABLE_DAILY_AT 20

typedef struct _tagMemberState {
    int64_t qq;//能不能改uint啊???
    int64_t linkedQQ;//被连坐惩罚自
    uint32_t cmdCount;//调用命令计数
    uint32_t atCount;//@人计数
    uint32_t atMasterCount;//@执剑人计数
    uint32_t banMinutes;
    uint32_t lastBanMinutes;
    int32_t rpValue;//用于修正roll点结果（黑幕注意）
    _tagMemberState() : qq(0), linkedQQ(0), cmdCount(0), atCount(0), atMasterCount(0), banMinutes(0), lastBanMinutes(0), rpValue(0) {}
} MemberState;

inline bool qqInList(int64_t qq)
{
    return (qq == 85645231
            || qq == 387210935
            || qq == 269106906
            || qq == 407508177);
}
inline bool groupInList(int64_t group)
{
    return (group == ACTIVATED_QQGROUP
           ||group == 558908229);
}

int64_t rollFight(uint32_t requestTime, MemberState &qq1, MemberState &qq2, int64_t fromGroup);// return winner
int64_t revenge(MemberState &qq1, int64_t fromGroup);
static std::map<int64_t, MemberState> cachedMembers;
static int g_rollPlayEnabled = 0;
