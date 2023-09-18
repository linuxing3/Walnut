#include "sparkDemo.h"
using namespace std;
using namespace AIKIT;

struct UsrCtx {
  string chatID;
};

string getUsrInput() {
  cout << "请输入用户问题：" << endl;
  string text;
  cin >> text;
  return text;
}

int sendRequest(const string& question) {
  // 请求参数配置
  ChatParam* config = ChatParam::builder();
  config->uid("xxxid")
      ->domain("generalv2")
      ->auditing("default")
      ->url("ws://spark-api.xf-yun.com/v2.1/chat");

  // 设置chatID,使用static变量,防止回调时被销毁。
  // 用于用户动态控制会话轮次
  static UsrCtx usr = {"FistRound"};

  // int ret = AIKIT_AsyncChat(config, getUsrInput().c_str(), &usr);
  int ret = AIKIT_AsyncChat(config, question.c_str(), &usr);
  if (ret != 0) {
    printf("AIKIT_AsyncChat failed:%d\n", ret);
    return ret;
  }
  return ret;
}

void chatOnToken(AIChat_Handle* handle, const int& completionTokens,
                 const int& promptTokens, const int& totalTokens) {
  if (handle != nullptr) {
    cout << "chatID:" << ((UsrCtx*)handle->usrContext)->chatID << ", ";
  }
  cout << "completionTokens:" << completionTokens
       << " promptTokens:" << promptTokens << " totalTokens:" << totalTokens
       << endl;
  /* sendRequest(); */
}

static void chatOnOutput(AIChat_Handle* handle, const char* role,
                         const char* content, const int& index) {
  if (handle != nullptr) {
    cout << "chatID:" << ((UsrCtx*)handle->usrContext)->chatID << ", ";
  }
  cout << "role:" << role << ", content: " << content << endl;
}

static void chatOnError(AIChat_Handle* handle, const int& err,
                        const char* errDesc) {
  if (handle != nullptr) {
    cout << "chatID:" << ((UsrCtx*)handle->usrContext)->chatID << ", ";
  }
  printf("chatOnError: err:%d,desc:%s\n", err, errDesc);
  /* sendRequest(); */
}

void initSDK() {
  AIKIT_InitParam initParam{};
  AIKIT_SetLogInfo(100, 0, nullptr);
  initParam.appID = "de78a0ea";
  initParam.apiKey = "7cf4e869f98ab280e5671feae40810e1";
  initParam.apiSecret = "MTY2OGI3YWU2YTYwZmViMGZjMjU4NjQw";
  int ret = AIKIT_Init(&initParam);
  if (ret != 0) {
    printf("AIKIT_Init failed:%d\n", ret);
    return;
  }

  // 异步回调注册
  AIKIT_ChatCallback({chatOnOutput, chatOnToken, chatOnError});
  return;
}

void unInit() {
  // 等待异步回调，这里sleep仅是demo最简便运行需要
  // 真实场景，应用侧自行决定合适进行SDK逆初始化
  sleep(500);

  // 逆初始化SDK
  AIKIT_UnInit();
  return;
}
