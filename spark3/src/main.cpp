#include "SparkApp.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char const *argv[]) {
  auto *app = new SparkApp();
  // 全局初始化
  int ret = SparkApp::initSDK();
  if (ret != 0) {
    cout << "initSDK failed:" << ret << endl;
    return -1;
  }

  while (true) {
    app->askQuestions();
  }

  // 退出
  SparkApp::uninitSDK();
}
