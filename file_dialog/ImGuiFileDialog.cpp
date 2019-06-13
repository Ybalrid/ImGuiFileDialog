#include "ImGuiFileDialog.h"

#include <iostream>

#ifndef WIN32
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif
#include <dirent.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <algorithm>

#include "imgui.h"
#include "imgui_internal.h"

const char* ImGuiFileDialog::dirLabel = "[Dir]";
const char* ImGuiFileDialog::fileLabel = "[File]";
const char* ImGuiFileDialog::linkLabel = "[Link]";
inline bool ReplaceString(std::string& str, const std::string& oldStr,
                          const std::string& newStr) {
  bool found = false;
  size_t pos = 0;
  while ((pos = str.find(oldStr, pos)) != std::string::npos) {
    found = true;
    str.replace(pos, oldStr.length(), newStr);
    pos += newStr.length();
  }
  return found;
}

inline std::vector<std::string> splitStringVector(const std::string& text,
                                                  char delimiter) {
  std::vector<std::string> arr;
  std::string::size_type start = 0;
  std::string::size_type end = text.find(delimiter, start);
  while (end != std::string::npos) {
    std::string token = text.substr(start, end - start);
    if (token != "") arr.push_back(token);
    start = end + 1;
    end = text.find(delimiter, start);
  }
  arr.push_back(text.substr(start));
  return arr;
}

inline void AppendToBuffer(char* vBuffer, int vBufferLen, std::string vStr) {
  std::string st = vStr;
  if (st != "" && st != "\n") ReplaceString(st, "\n", "");
  int slen = strlen(vBuffer);
  vBuffer[slen] = '\0';
  std::string str = std::string(vBuffer);
  if (str.size() > 0) str += "\n";
  str += vStr;
  int len = vBufferLen - 1;
  if (len > str.size()) len = str.size();
#ifdef MINGW32
  strncpy_s(vBuffer, vBufferLen, str.c_str(), len);
#else
  strncpy(vBuffer, str.c_str(), len);
#endif
  vBuffer[len] = '\0';
}

inline void ResetBuffer(char* vBuffer) { vBuffer[0] = '\0'; }

char ImGuiFileDialog::FileNameBuffer[MAX_FILE_DIALOG_NAME_BUFFER] = "";
int ImGuiFileDialog::FilterIndex = 0;

ImGuiFileDialog::ImGuiFileDialog() {}

ImGuiFileDialog::~ImGuiFileDialog() {}

/* Alphabetical sorting */
static int alphaSort(const dirent** a, const dirent** b) {
  return strcoll((*a)->d_name, (*b)->d_name);
}

static bool stringComparator(FileInfoStruct a, FileInfoStruct b) {
  bool res;
  if (a.type != b.type)
    res = (a.type < b.type);
  else
    res = (a.fileName < b.fileName);
  return res;
}

void ImGuiFileDialog::ScanDir(std::string vPath) {
  struct dirent** files = 0;
  int i = 0;
  int n = 0;

#ifndef WIN32
  if (vPath == ".") {
    char raw_path[PATH_MAX + 1];
    realpath(".", raw_path);
    vPath = std::string(raw_path);
  } else if (vPath.size() > 0 && vPath[0] != DIRECTORY_SEPARATOR_CHAR) {
    vPath = std::string(DIRECTORY_SEPARATOR_STR) + vPath;
  }
#endif

  m_FileList.clear();

  if (m_CurrentPath_Decomposition.size() == 0) {
    // get currentPath
    DIR* currentDir = opendir(vPath.c_str());
    if (currentDir == 0)  // path not existing
    {
      vPath = ".";  // current  app path
      currentDir = opendir(vPath.c_str());
    }
    if (currentDir != 0) {
#ifdef WIN32
      std::wstring ws(currentDir->wdirp->patt);
      m_CurrentPath = std::string(ws.begin(), ws.end());
#else
      char rawPath[PATH_MAX + 1];
      realpath(vPath.c_str(), rawPath);
      m_CurrentPath = std::string(rawPath);
#endif
      ReplaceString(m_CurrentPath, "\\*", "");
      closedir(currentDir);
      m_CurrentPath_Decomposition =
          splitStringVector(m_CurrentPath, DIRECTORY_SEPARATOR_CHAR);
    } else {
      return;
    }
  }
  // std::cerr << "scanning files in " << vPath << "\n";
  /* Scan files in directory */
  n = scandir(vPath.c_str(), &files, NULL, alphaSort);

  // std::cerr << "n is " << n << "\n";

  if (n >= 0) {
    for (i = 0; i < n; i++) {
      struct dirent* ent = files[i];

      FileInfoStruct infos;

      infos.fileName = ent->d_name;
      // std::cout << "scanned filename is " << infos.fileName << "\n";

      if (infos.fileName != ".") {
        switch (ent->d_type) {
          case DT_REG:
            infos.type = 'f';
            break;
          case DT_DIR:
            infos.type = 'd';
            break;
          case DT_LNK:
            infos.type = 'l';
            break;
        }

        if (infos.type == 'f') {
          size_t lpt = infos.fileName.find_last_of(".");
          if (lpt != std::string::npos) infos.ext = infos.fileName.substr(lpt);
        }

        m_FileList.push_back(infos);
      }
    }

    for (i = 0; i < n; i++) {
      free(files[i]);
    }
    free(files);
  }

  std::sort(m_FileList.begin(), m_FileList.end(), stringComparator);

  //  for (auto& file : m_FileList) std::cout << "file : " << file.fileName <<
  //  "\n";
}

void ImGuiFileDialog::SetCurrentDir(std::string vPath) {
  DIR* dir = opendir(vPath.c_str());
  if (dir == 0) {
    vPath = ".";
    dir = opendir(vPath.c_str());
  }
  if (dir != 0) {
#ifdef WIN32
    std::wstring ws(dir->wdirp->patt);
    m_CurrentPath = std::string(ws.begin(), ws.end());
#else
    char rawPath[PATH_MAX + 1];
    realpath(vPath.c_str(), rawPath);
    m_CurrentPath = std::string(rawPath);
#endif
    ReplaceString(m_CurrentPath, "\\*",
                  "");  // TODO understand the pont of this...?
    closedir(dir);
    m_CurrentPath_Decomposition =
        splitStringVector(m_CurrentPath, DIRECTORY_SEPARATOR_CHAR);
  }
}

void ImGuiFileDialog::ComposeNewPath(std::vector<std::string>::iterator vIter) {
  m_CurrentPath = "";
  while (vIter != m_CurrentPath_Decomposition.begin()) {
    if (m_CurrentPath.size() > 0)
      m_CurrentPath = *vIter + DIRECTORY_SEPARATOR_STR + m_CurrentPath;
    else
      m_CurrentPath = *vIter;
    vIter--;
  }

  if (m_CurrentPath.size() > 0)
    m_CurrentPath = *vIter + DIRECTORY_SEPARATOR_STR + m_CurrentPath;
  else
    m_CurrentPath = *vIter + DIRECTORY_SEPARATOR_STR;
}

namespace ImGui {
bool BeginPopupModalSerialized(const char* name, bool* p_open = nullptr,
                               ImGuiWindowFlags flags = 0) {
  ImGuiContext& g = *GImGui;
  ImGuiWindow* window = g.CurrentWindow;
  const ImGuiID id = window->GetID(name);
  if (!ImGui::IsPopupOpen(id)) {
    g.NextWindowData.ClearFlags();  // We behave like Begin() and need to
                                    // consume those values
    return false;
  }

  // Center modal windows by default
  // FIXME: Should test for (PosCond & window->SetWindowPosAllowFlags) with the
  // upcoming window.
  if (g.NextWindowData.PosCond == 0)
    ImGui::SetNextWindowPos(g.IO.DisplaySize * 0.5f, ImGuiCond_Appearing,
                            ImVec2(0.5f, 0.5f));

  flags |= ImGuiWindowFlags_Popup | ImGuiWindowFlags_Modal |
           ImGuiWindowFlags_NoCollapse;
  const bool is_open = ImGui::Begin(name, p_open, flags);
  if (!is_open ||
      (p_open && !*p_open))  // NB: is_open can be 'false' when the popup is
                             // completely clipped (e.g. zero size display)
  {
    ImGui::EndPopup();
    if (is_open) ImGui::ClosePopupToLevel(g.BeginPopupStack.Size, true);
    return false;
  }
  return is_open;
}
}  // namespace ImGui

bool ImGuiFileDialog::FileDialog(const char* vName, const char* vFilters,
                                 bool modal, std::string vPath,
                                 std::string vDefaultFileName) {
  bool res = false;

  IsOk = false;

  modal ? ImGui::OpenPopup(vName),
      ImGui::BeginPopupModalSerialized(vName) : ImGui::Begin(vName);

  if (vPath.size() == 0) vPath = ".";

  if (m_FileList.size() == 0) {
    if (vDefaultFileName.size() > 0) {
      ResetBuffer(FileNameBuffer);
      AppendToBuffer(FileNameBuffer, MAX_FILE_DIALOG_NAME_BUFFER,
                     vDefaultFileName);
    }

    ScanDir(vPath);
  }

  // show current path
  bool pathClick = false;
  for (std::vector<std::string>::iterator itPathDecomp =
           m_CurrentPath_Decomposition.begin();
       itPathDecomp != m_CurrentPath_Decomposition.end(); ++itPathDecomp) {
    if (itPathDecomp != m_CurrentPath_Decomposition.begin()) ImGui::SameLine();
    if (ImGui::Button((*itPathDecomp).c_str())) {
      ComposeNewPath(itPathDecomp);
      pathClick = true;
      break;
    }
  }

  ImVec2 size = ImGui::GetContentRegionMax() - ImVec2(0.0f, 120.0f);

  ImGui::BeginChild("##FileDialog_FileList", size, true,
                    ImGuiWindowFlags_AlwaysVerticalScrollbar);

  for (std::vector<FileInfoStruct>::iterator it = m_FileList.begin();
       it != m_FileList.end(); ++it) {
    FileInfoStruct infos = *it;

    bool show = true;

    std::string str;
    if (infos.type == 'd') str = std::string(dirLabel) + " " + infos.fileName;
    if (infos.type == 'l') str = std::string(linkLabel) + " " + infos.fileName;
    if (infos.type == 'f') str = std::string(fileLabel) + " " + infos.fileName;

    if (infos.type == 'f' && m_CurrentFilterExt.size() > 0 &&
        infos.ext != m_CurrentFilterExt) {
      show = false;
    }
    if (show == true) {
      if (ImGui::Selectable(str.c_str(),
                            (infos.fileName == m_SelectedFileName))) {
        if (infos.type == 'd') {
          if (infos.fileName == "..") {
            if (m_CurrentPath_Decomposition.size() > 1) {
              std::vector<std::string>::iterator itPathDecomp =
                  m_CurrentPath_Decomposition.end() - 2;
              ComposeNewPath(itPathDecomp);
            }
          } else {
            m_CurrentPath += DIRECTORY_SEPARATOR_STR + infos.fileName;
          }
          pathClick = true;
        } else {
          m_SelectedFileName = infos.fileName;
          ResetBuffer(FileNameBuffer);
          AppendToBuffer(FileNameBuffer, MAX_FILE_DIALOG_NAME_BUFFER,
                         m_SelectedFileName);
        }
        break;
      }
    }
  }

  if (pathClick == true) {
    ScanDir(m_CurrentPath);
    m_CurrentPath_Decomposition =
        splitStringVector(m_CurrentPath, DIRECTORY_SEPARATOR_CHAR);
    if (m_CurrentPath_Decomposition.size() == 2)
      if (m_CurrentPath_Decomposition[1] == "")
        m_CurrentPath_Decomposition.erase(m_CurrentPath_Decomposition.end() -
                                          1);
  }

  ImGui::EndChild();

  ImGui::Text("File Name : ");

  ImGui::SameLine();

  float width = ImGui::GetContentRegionAvailWidth();
  if (vFilters != 0) width -= 120.0f;
  ImGui::PushItemWidth(width);
  ImGui::InputText("##FileName", FileNameBuffer, MAX_FILE_DIALOG_NAME_BUFFER);
  ImGui::PopItemWidth();

  if (vFilters != 0) {
    ImGui::SameLine();

    ImGui::PushItemWidth(100.0f);
    bool comboClick = ImGui::Combo("##Filters", &FilterIndex, vFilters) ||
                      m_CurrentFilterExt.size() == 0;
    ImGui::PopItemWidth();
    if (comboClick == true) {
      int itemIdx = 0;
      const char* p = vFilters;
      while (*p) {
        if (FilterIndex == itemIdx) {
          m_CurrentFilterExt = std::string(p);
          break;
        }
        p += strlen(p) + 1;
        itemIdx++;
      }
    }
  }
  ImGui::Separator();
  ImGui::Text(" ");  // Dummy
  ImGui::SameLine(ImGui::GetWindowWidth() - 100);
  if (ImGui::Button("Cancel")) {
    IsOk = false;
    res = true;
  }

  ImGui::SameLine(ImGui::GetWindowWidth() - 40);

  if (ImGui::Button("Ok")) {
    IsOk = true;
    res = true;
  }

  modal ? ImGui::EndPopup() : ImGui::End();

  if (res == true) {
    m_FileList.clear();
  }

  return res;
}

std::string ImGuiFileDialog::GetFilepathName() {
  return GetCurrentPath() + DIRECTORY_SEPARATOR_STR + GetCurrentFileName();
}

std::string ImGuiFileDialog::GetCurrentPath() {
#ifdef WIN32
  return m_CurrentPath;
#else
  return "/" + m_CurrentPath;
#endif
}

std::string ImGuiFileDialog::GetCurrentFileName() {
  return std::string(FileNameBuffer);
}

std::string ImGuiFileDialog::GetCurrentFilter() { return m_CurrentFilterExt; }
