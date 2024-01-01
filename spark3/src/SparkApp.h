#include "sparkchain.h"
#include <string>
#include <unistd.h>

#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define RESET "\033[0m"

#define APP_ID "de78a0ea"
#define APP_SECRET "MTY2OGI3YWU2YTYwZmViMGZjMjU4NjQw"
#define APP_KEY "7cf4e869f98ab280e5671feae40810e1"

using namespace SparkChain;
using namespace std;

// class predefinition
class SparkApp;

// class definition
class SparkApp : public LLMCallbacks {
 public:
  SparkApp();
  ~SparkApp();

  static int initSDK() {
    // 全局初始化
    SparkChainConfig *config = SparkChainConfig::builder();
    config
        ->appID(APP_ID)           // 你的appid
        ->apiKey(APP_KEY)         // 你的apikey
        ->apiSecret(APP_SECRET);  // 你的apisecret
    int ret = SparkChain::init(config);
    printf(RED "\ninit SparkChain result:%d" RESET, ret);
    return ret;
  }

  static void uninitSDK() {
    // 全局逆初始化
    SparkChain::unInit();
  }

  static SparkApp *GetApp();

  static string GetFinalResult();

 public:
  void askQuestions();
  LLMConfig *GetLLMConfig();
  LLM *GetLLM();

  void SetQuestion(const string &q) { m_Question = q; };

 private:
  void onLLMResult(LLMResult *result, void *usrContext);
  void onLLMEvent(LLMEvent *event, void *usrContext);
  void onLLMError(LLMError *error, void *usrContext);

 private:
  char *m_UserContext = "myContext";
  string m_Question = "";
  string m_FinalResult = "";
};
