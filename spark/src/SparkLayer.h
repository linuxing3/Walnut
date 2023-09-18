
#include "Walnut/Application.h"
#include "sparkDemo.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Walnut/UI/UI.h"
#include "imgui.h"
#include "sparkDemo.h"

#include <cstdio>
#include <float.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace AIKIT;
using namespace Walnut;

std::vector<string> g_Responses{};
static char g_ResponseBuf[1024];

struct UsrCtx {
  string chatID;
};

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

void chatOnOutput(AIChat_Handle* handle, const char* role, const char* content,
                  const int& index) {
  if (handle != nullptr) {
    /* cout << "chatID:" << ((UsrCtx*)handle->usrContext)->chatID << ", "; */
  }
  // FIXME:
  string s(content);
  g_Responses.push_back(s);
  /* cout << "role:" << role << ", content: " << content << endl; */
  cout << content << endl;
}

void chatOnError(AIChat_Handle* handle, const int& err, const char* errDesc) {
  if (handle != nullptr) {
    cout << "chatID:" << ((UsrCtx*)handle->usrContext)->chatID << ", ";
  }
  printf("chatOnError: err:%d,desc:%s\n", err, errDesc);
  /* sendRequest(); */
}

void unInit() {
  // 等待异步回调，这里sleep仅是demo最简便运行需要
  // 真实场景，应用侧自行决定合适进行SDK逆初始化
  sleep(500);

  // 逆初始化SDK
  AIKIT_UnInit();
  return;
}

class SparkLayer : public Walnut::Layer {
 public:
  void OnAttach() override {

    m_Questions.push_back(
        "Hi, show me c++ code with imgui to create a window with a label");

    initSDK();
  }
  virtual void OnUpdate(float ts) override {}

  virtual void OnUIRender() override {
    UI_DrawAboutModal();
    UI_DrawSparkChat(&m_SparkChatOpen);
  }

  void Render() {
    Timer timer;

    m_LastRenderTime = timer.ElapsedMillis();
  }

  ~SparkLayer() override { unInit(); };

  // Portable helpers
  static int Stricmp(const char* s1, const char* s2) {
    int d;
    while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
      s1++;
      s2++;
    }
    return d;
  }
  static int Strnicmp(const char* s1, const char* s2, int n) {
    int d = 0;
    while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
      s1++;
      s2++;
      n--;
    }
    return d;
  }
  static char* Strdup(const char* s) {
    IM_ASSERT(s);
    size_t len = strlen(s) + 1;
    void* buf = malloc(len);
    IM_ASSERT(buf);
    return (char*)memcpy(buf, (const void*)s, len);
  }
  static void Strtrim(char* s) {
    char* str_end = s + strlen(s);
    while (str_end > s && str_end[-1] == ' ')
      str_end--;
    *str_end = 0;
  }
  // Demonstrate create a window with multiple child windows.
  void UI_DrawSparkChat(bool* p_open) {
    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Spark Chat", p_open, ImGuiWindowFlags_MenuBar)) {
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
          if (ImGui::MenuItem("Close"))
            *p_open = false;
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }

      // Left
      static int selected = 0;
      {
        ImGui::BeginChild("left pane", ImVec2(150, 0), true);
        for (int i = 0; i < m_Questions.size(); i++) {
          // FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
          char label[128];
          sprintf(label, "%s", m_Questions[i]);
          if (ImGui::Selectable(label, selected == i)) {
            selected = i;
            m_CurrentQuestion = m_Questions[i];
          }
        }
        ImGui::EndChild();
      }
      ImGui::SameLine();

      // Right
      {
        ImGui::BeginGroup();
        ImGui::BeginChild(
            "Chat Window",
            ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));  // Leave room for
                                                              // 1 line below us
        ImGui::Text("Questions %d", selected);
        ImGui::Separator();
        if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
          if (ImGui::BeginTabItem("Description")) {
            ImGui::TextWrapped("%s", m_CurrentQuestion.c_str());
            ImGui::NewLine();

            // FIXME: char to utf-8 code
            for (auto line : g_Responses) {
              ImGui::TextWrapped("%s", line.c_str());
              /* sprintf(label, "%s", g_ResponseBuf[i]); */
            }
            ImGui::EndTabItem();
          }
          if (ImGui::BeginTabItem("Details")) {
            ImGui::Text("%s", m_CurrentQuestion.c_str());
            ImGui::EndTabItem();
          }
          ImGui::EndTabBar();
        }
        ImGui::EndChild();

        // Botton side
        bool reclaim_focus = false;
        ImGuiInputTextFlags input_text_flags =
            ImGuiInputTextFlags_EnterReturnsTrue |
            ImGuiInputTextFlags_CallbackCompletion |
            ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf),
                             input_text_flags,
                             &SparkLayer::TextEditCallbackStub, (void*)this)) {
          char* s = InputBuf;
          Strtrim(s);
          if (s)
            m_CurrentQuestion = s;
          strcpy(s, "");
          reclaim_focus = true;
        }

        if (ImGui::Button("Send")) {
          sendRequest(m_CurrentQuestion);
        }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
          m_Questions.push_back(m_CurrentQuestion.c_str());
        }
        ImGui::EndGroup();
      }
    }
    ImGui::End();
  }

  void UI_DrawAboutModal() {
    if (!m_AboutModalOpen)
      return;

    ImGui::OpenPopup("About");
    m_AboutModalOpen = ImGui::BeginPopupModal(
        "About", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (m_AboutModalOpen) {
      auto image = Walnut::Application::Get().GetApplicationIcon();
      ImGui::Image(image->GetDescriptorSet(), {48, 48});

      ImGui::SameLine();
      Walnut::UI::ShiftCursorX(20.0f);

      ImGui::BeginGroup();
      ImGui::Text("Walnut application framework");
      ImGui::Text("by Studio Cherno.");
      ImGui::EndGroup();

      if (Walnut::UI::ButtonCentered("Close")) {
        m_AboutModalOpen = false;
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }
  }

  // In C++11 you'd be better off using lambdas for this sort of forwarding
  // callbacks
  static int TextEditCallbackStub(ImGuiInputTextCallbackData* data) {
    SparkLayer* spark = (SparkLayer*)data->UserData;
    return spark->TextEditCallback(data);
  }

  int TextEditCallback(ImGuiInputTextCallbackData* data) {
    // AddLog("cursor: %d, selection: %d-%d", data->CursorPos,
    // data->SelectionStart, data->SelectionEnd);
    switch (data->EventFlag) {
      case ImGuiInputTextFlags_CallbackCompletion: {
        // Example of TEXT COMPLETION

        // Locate beginning of current word
        const char* word_end = data->Buf + data->CursorPos;
        const char* word_start = word_end;
        while (word_start > data->Buf) {
          const char c = word_start[-1];
          if (c == ' ' || c == '\t' || c == ',' || c == ';')
            break;
          word_start--;
        }

        // Build a list of candidates
        ImVector<const char*> candidates;
        for (int i = 0; i < Commands.Size; i++)
          if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) ==
              0)
            candidates.push_back(Commands[i]);

        if (candidates.Size == 0) {
          // No match
        } else if (candidates.Size == 1) {
          // Single match. Delete the beginning of the word and replace it
          // entirely so we've got nice casing.
          data->DeleteChars((int)(word_start - data->Buf),
                            (int)(word_end - word_start));
          data->InsertChars(data->CursorPos, candidates[0]);
          data->InsertChars(data->CursorPos, " ");
        } else {
          // Multiple matches. Complete as much as we can..
          // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and
          // "CLASSIFY" as matches.
          int match_len = (int)(word_end - word_start);
          for (;;) {
            int c = 0;
            bool all_candidates_matches = true;
            for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
              if (i == 0)
                c = toupper(candidates[i][match_len]);
              else if (c == 0 || c != toupper(candidates[i][match_len]))
                all_candidates_matches = false;
            if (!all_candidates_matches)
              break;
            match_len++;
          }

          if (match_len > 0) {
            data->DeleteChars((int)(word_start - data->Buf),
                              (int)(word_end - word_start));
            data->InsertChars(data->CursorPos, candidates[0],
                              candidates[0] + match_len);
          }

          // List matches
        }

        break;
      }
      case ImGuiInputTextFlags_CallbackHistory: {
        // Example of HISTORY
        const int prev_history_pos = HistoryPos;
        if (data->EventKey == ImGuiKey_UpArrow) {
          if (HistoryPos == -1)
            HistoryPos = History.Size - 1;
          else if (HistoryPos > 0)
            HistoryPos--;
        } else if (data->EventKey == ImGuiKey_DownArrow) {
          if (HistoryPos != -1)
            if (++HistoryPos >= History.Size)
              HistoryPos = -1;
        }

        // A better implementation would preserve the data on the current input
        // line along with cursor position.
        if (prev_history_pos != HistoryPos) {
          const char* history_str =
              (HistoryPos >= 0) ? History[HistoryPos] : "";
          data->DeleteChars(0, data->BufTextLen);
          data->InsertChars(0, history_str);
        }
      }
    }
    return 0;
  }
  void ShowAboutModal() { m_AboutModalOpen = true; }

 private:
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

 private:
  uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

  float m_LastRenderTime = 0.0F;
  bool m_AboutModalOpen = false;
  bool m_SparkChatOpen = false;

  // for left pane, combo list labels
  std::vector<const char*> m_Questions{};
  std::vector<const char*> m_Responses{};

  // for right pane, questions and contents
  std::string m_CurrentQuestion = "";
  char InputBuf[1024];
  ImVector<char*> Items;
  ImVector<const char*> Commands;
  ImVector<char*> History;
  int HistoryPos;  // -1: new line, 0..History.Size-1 browsing history.
  ImGuiTextFilter Filter;
  bool AutoScroll;
  bool ScrollToBottom;
};
