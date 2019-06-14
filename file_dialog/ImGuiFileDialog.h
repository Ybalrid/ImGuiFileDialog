#ifndef __IMGUI_FILE_DIALOG_H_
#define __IMGUI_FILE_DIALOG_H_
#define NOMINMAX

#include <string>
#include <vector>

#define MAX_FILE_DIALOG_NAME_BUFFER 1024

#ifdef WIN32
#define DIRECTORY_SEPARATOR_CHAR '\\'
#define DIRECTORY_SEPARATOR_STR "\\"
#else
#define DIRECTORY_SEPARATOR_CHAR '/'
#define DIRECTORY_SEPARATOR_STR "/"
#endif

struct FileInfoStruct {
  char type;
  std::string filePath;
  std::string fileName;
  std::string ext;
};

class ImGuiFileDialog {
 private:
  std::vector<FileInfoStruct> m_FileList;
  std::string m_SelectedFileName;
  std::string m_CurrentPath;
  std::vector<std::string> m_CurrentPath_Decomposition;
  std::string m_CurrentFilterExt;

 public:
  static char FileNameBuffer[MAX_FILE_DIALOG_NAME_BUFFER];
  static int FilterIndex;
  bool IsOk;

  static const char* dirLabel;
  static const char* fileLabel;
  static const char* linkLabel;

 public:
  static ImGuiFileDialog* Instance() {
    static ImGuiFileDialog* _instance = new ImGuiFileDialog();
    return _instance;
  }

 public:
  ImGuiFileDialog();
  ~ImGuiFileDialog();

  bool FileDialog(const char* vName, const char* vFilters = 0,
                  bool modal = true, std::string vPath = ".",
                  std::string vDefaultFileName = "");
  std::string GetFilepathName() const;
  std::string GetCurrentPath() const;
  std::string GetCurrentFileName() const;
  std::string GetCurrentFilter() const;
  std::vector<FileInfoStruct> const& GetCurrentFileList() const;

 private:
  void ScanDir(std::string vPath);
  void SetCurrentDir(std::string vPath);
  void ComposeNewPath(std::vector<std::string>::iterator vIter);
};

#endif  // __IMGUI_FILE_DIALOG_H_
