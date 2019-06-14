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

/// Represent a file
struct FileInfo {
  char type;
  std::string filePath;
  std::string fileName;
  std::string ext;
};

/// Main class of the file dialog. This is a singleton. Use
/// ImGuiFileDialog::Instance() to create one
class ImGuiFileDialog {
 private:
  /// List of files in current directory
  std::vector<FileInfo> m_FileList;
  /// File curently selected
  std::string m_SelectedFileName;
  /// Path to the current directory
  std::string m_CurrentPath;
  /// List of folders, in order, to get from system root to current path
  std::vector<std::string> m_CurrentPath_Decomposition;
  /// Currently active filter
  std::string m_CurrentFilterExt;

 public:
  /// Buffer to serve as text input
  static char FileNameBuffer[MAX_FILE_DIALOG_NAME_BUFFER];
  /// Index of currently selected filter on filter list
  static int FilterIndex;
  /// Value that is set to true at the moment the user finished it's intended
  /// input
  bool IsOk;

  /// String to be displayed next to directory names. You can change it.
  static const char* dirLabel;
  /// String to be displayed next to file names. You can change it.
  static const char* fileLabel;
  /// String to be displayed next to symlink names. You can change it.
  static const char* linkLabel;

 public:
  /// Create (if called first) and return an instance to the file dialog class.
  static ImGuiFileDialog* Instance() {
    static ImGuiFileDialog* _instance = new ImGuiFileDialog();
    return _instance;
  }

 public:
  /// Construct the file dialog
  ImGuiFileDialog();

  /// Destruct the file dialog
  ~ImGuiFileDialog();

  ///\brief Main function to invoke the file dialog "immediate mode" style
  ///\param vName Name of the window to open
  ///\param vFilters list of null terminated strings, terminated by null.
  /// Example : ".txt\0.ini\0.toml\0\0"
  ///\param modal If set to true, the window will be modal and block other UI
  /// actions until it finished
  ///\param vPath Path from where to start
  ///\param vDefaultFileName file name that will be populated by default in the
  /// input filed
  bool FileDialog(const char* vName, const char* vFilters = 0,
                  bool modal = true, std::string vPath = ".",
                  std::string vDefaultFileName = "");

  /// Return the current file path + file name
  std::string GetFilepathName() const;
  /// Return the path of the current folder
  std::string GetCurrentPath() const;
  /// Return the name of the current folder
  std::string GetCurrentFileName() const;
  /// Return the currently active filter
  std::string GetCurrentFilter() const;
  /// Return the list of FileInfo currently displayed
  std::vector<FileInfo> const& GetCurrentFileList() const;

 private:
  /// Scan a directory and populate filenaem
  void ScanDir(std::string vPath);
  /// Set the current work dir
  void SetCurrentDir(std::string vPath);
  /// Compose the current path string from a list of strings
  void ComposeNewPath(std::vector<std::string>::iterator vIter);
};

#endif  // __IMGUI_FILE_DIALOG_H_
