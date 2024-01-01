#include "SparkApp.h"
#include <atomic>
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>
using namespace SparkChain;
using namespace std;

// singleton
static SparkApp *s_SparkApp;

static LLMConfig *s_LLMConfig;
static Memory *s_WindowMemory;
static LLM *s_Asyncllm;

// async status tag
static atomic_bool finish(false);
// result cache
static string s_FinalResult = "";

void SparkApp::askQuestions() {
  cout << "\n######### 异步调用 #########" << endl;
  // 配置大模型参数
  s_LLMConfig = LLMConfig::builder();
  s_LLMConfig->domain("generalv3");

  s_WindowMemory = Memory::WindowMemory(5);
  s_Asyncllm = LLM::create(s_LLMConfig, s_WindowMemory);

  // Memory* token_memory = Memory::TokenMemory(500);
  // LLM *s_Asyncllm = LLM::create(llmConfig,token_memory);

  if (s_Asyncllm == nullptr) {
    printf(RED "\nLLMTest fail, please setLLMConfig before" RESET);
    return;
  }
  // 注册监听回调
  s_Asyncllm->registerLLMCallbacks(this);

  // 异步请求
  int i = 0;
  while (i++ < 2) {
    finish = false;

    /* char question_buf[1024]; */
    /* strcpy(question_buf, m_Question.c_str()); */
    const char *question_buf = m_Question.c_str();
    /* question_buf = m_Question.c_str(); */
    int ret = s_Asyncllm->arun(question_buf, m_UserContext);
    if (ret != 0) {
      printf(RED "\narun failed: %d\n\n" RESET, ret);
      finish = true;
      continue;
    }

    int times = 0;
    while (!finish) {  // 等待结果返回退出
      sleep(1);
      if (times++ > 10)  // 等待十秒如果没有最终结果返回退出
        break;
    }
  }
}

SparkApp::~SparkApp() {
  // 垃圾回收
  if (s_Asyncllm != nullptr) {
    LLM::destroy(s_Asyncllm);
  }
}

void SparkApp::onLLMResult(LLMResult *result, void *usrContext) {
  int status = result->getStatus();
  /* printf(RED "%d:%s:%s \n" RESET, status, result->getRole(), usrContext);
   */
  s_FinalResult += string(result->getContent());
  // TODO: need to return
  /* std::cout << s_FinalResult << std::endl; */
  if (status == 2) {
    printf(GREEN "tokens:%d + %d = %d\n" RESET, result->getCompletionTokens(),
           result->getPromptTokens(), result->getTotalTokens());
    finish = true;
  }
}

void SparkApp::onLLMEvent(LLMEvent *event, void *usrContext) {
  /* printf(YELLOW "onLLMEventCB\n  eventID:%d eventMsg:%s\n" RESET, */
  /*        event->getEventID(), event->getEventMsg()); */
}

void SparkApp::onLLMError(LLMError *error, void *usrContext) {
  printf(RED "onLLMErrorCB\n errCode:%d errMsg:%s \n" RESET,
         error->getErrCode(), error->getErrMsg());
  finish = true;
}

LLMConfig *SparkApp::GetLLMConfig() { return s_LLMConfig; }

LLM *SparkApp::GetLLM() { return s_Asyncllm; }

string SparkApp::GetFinalResult() { return s_FinalResult; }

SparkApp *SparkApp::GetApp() { return s_SparkApp; };

SparkApp::SparkApp() { s_SparkApp = this; };
