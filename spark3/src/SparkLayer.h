#include "SparkApp.h"
#include "Walnut/Application.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Walnut/UI/UI.h"
#include "imgui.h"

#include <cstdio>
#include <cstring>
#include <float.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace Walnut;

std::vector<string> g_Responses{};

class SparkLayer : public Walnut::Layer {
 public:
  SparkLayer() { initSDK(); }
  void OnAttach() override {

    const char* defaultQuestion =
        "Show me c++ code to create a window with a label, with imgui and "
        "glfw.";

    m_Questions.push_back(defaultQuestion);

    m_ChatList.push_back(defaultQuestion);

    m_CurrentQuestion =
        "Hi, I'm your AI Assistant Spark, type your question to ask me ...";
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

  ~SparkLayer() override { SparkApp::uninitSDK(); };

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

  // NOTE:
  // Demonstrate create a window with multiple child windows.
  void UI_DrawSparkChat(bool* p_open) {
    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    static int selected = 0;
    ImGui::Begin("Spark Chat");

    // Left Pane
    {
      ImGui::BeginChild("left pane", ImVec2(150, 0), true);
      for (int i = 0; i < m_Questions.size(); i++) {
        // FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
        char label[128];
        sprintf(label, "%s", m_Questions[i]);
        if (ImGui::Selectable(label, selected == i)) {
          selected = i;
          char* s = InputBuf;
          strcpy(s, m_Questions[i]);
        }
      }
      ImGui::EndChild();
    }
    ImGui::SameLine();

    // Right Pane
    {
      ImGui::BeginGroup();
      ImGui::BeginChild(
          "Chat Window",
          ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));  // Leave room for
                                                            // 1 line below us
      ImGui::Text("Questions %d", selected + 1);
      ImGui::Separator();
      if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {

        for (uint32_t chatTabId = 0; chatTabId < m_ChatList.size();
             chatTabId++) {
          char label[128];
          sprintf(label, "Chat %u", chatTabId + 1);
          if (ImGui::BeginTabItem(label)) {
            ImGui::TextColored({0.0, 0.0, 1.0, 1.0}, "Question:\n%s",
                               m_CurrentQuestion.c_str());
            ImGui::NewLine();

            ImGui::Separator();

            ImGui::TextColored({1.0, 1.0, 0.0, 1.0}, "Answer:\n");

            // FIXME: test pulling data from ai
            if (getResponses()) {
              uint32_t lineNumber = 1;
              for (auto line : g_Responses) {
                /* ImGui::TextColored({1.0, 1.0, 0.0, 1.0}, "%u   %s",
                 * lineNumber,
                 */
                /*                    line.c_str()); */
                ImGui::TextColored({1.0, 1.0, 0.0, 1.0}, "%s", line.c_str());
                lineNumber++;
              }
            }

            ImGui::Separator();
            ImGui::NewLine();

            ImGui::EndTabItem();
          }
        }
        ImGui::EndTabBar();
      }
      ImGui::EndChild();

      // Text Input
      {

        bool reclaim_focus = false;
        ImGuiInputTextFlags input_text_flags =
            ImGuiInputTextFlags_EnterReturnsTrue |
            ImGuiInputTextFlags_CallbackCompletion |
            ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText("Enter to chat", InputBuf, IM_ARRAYSIZE(InputBuf),
                             input_text_flags,
                             &SparkLayer::TextEditCallbackStub, (void*)this)) {
          char* s = InputBuf;
          Strtrim(s);
          if (s) {
            m_CurrentQuestion = s;
            sendRequest(m_CurrentQuestion);
          }
          strcpy(s, "");
          reclaim_focus = true;
        }
      }

      // Button Group
      {

        if (ImGui::Button("Save History")) {
          m_Questions.push_back(m_CurrentQuestion.c_str());
        }
        ImGui::SameLine();
        if (ImGui::Button("New Chat")) {
          m_ChatList.push_back(m_CurrentQuestion.c_str());
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear History")) {
          char* s = InputBuf;
          strcpy(s, "");
          g_Responses.clear();
        }
      }
      ImGui::EndGroup();
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

  void sendRequest(const string& question) {
    m_SparkApp->SetQuestion(m_CurrentQuestion);
    m_SparkApp->askQuestions();
  }

  bool getResponses() {
    /* string result = SparkApp::GetApp()->GetFinalResult(); */
    string result = SparkApp::GetFinalResult();
    if (result.length() > 0) {
      g_Responses.clear();
      g_Responses.push_back(result);
      return true;
    }
    return false;
  }

  void initSDK() {
    // 全局初始化
    int ret = SparkApp::initSDK();
    if (ret != 0) {
      cout << "initSDK failed:" << ret << endl;
    }

    m_SparkApp = new SparkApp();
  }

 private:
  SparkApp* m_SparkApp;
  uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

  float m_LastRenderTime = 0.0F;
  bool m_AboutModalOpen = false;
  bool m_SparkChatOpen = false;

  // for left pane, combo list labels
  std::vector<const char*> m_Questions{};
  std::vector<const char*> m_Responses{};

  std::vector<const char*> m_ChatList{};

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
