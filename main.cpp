// ImGui - standalone example application for GLFW + OpenGL 3, using
// programmable pipeline If you are new to ImGui, see examples/README.txt and
// documentation at the top of imgui.cpp. (GLFW is a cross-platform general
// purpose library for handling windows, inputs, OpenGL/Vulkan graphics context
// creation, etc.) (GL3W is a helper library to access OpenGL functions since
// there is no standard header to access modern OpenGL functions easily.
// Alternatives are GLEW, Glad, etc.)

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
//#include "imgui_impl_glfw_gl3.h"
#include <GL/gl3w.h>  // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "ImGuiFileDialog.h"

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error %d: %s\n", error, description);
}

int main(int, char**) {
  // Setup window
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) return 1;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  GLFWwindow* window =
      glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);  // Enable vsync
  gl3wInit();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontDefault();

  // Setup ImGui bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 140");

  // Setup style
  // ImGui::StyleColorsDark();

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can
  // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
  // them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
  // need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please
  // handle those errors in your application (e.g. use an assertion, or display
  // an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored
  // into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which
  // ImGui_ImplXXXX_NewFrame below will call.
  // - Read 'extra_fonts/README.txt' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string
  // literal you need to write a double backslash \\ !
  // ImGuiIO& io = ImGui::GetIO();
  // io.Fonts->AddFontDefault();
  // io.Fonts->AddFontFromFileTTF("../../extra_fonts/Roboto-Medium.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
  // io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
  // ImFont* font =
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
  // NULL, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);

  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  int win_x, win_y;
  // Main loop
  while (!glfwWindowShouldClose(window)) {
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
    // tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
    // your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
    // data to your main application. Generally you may always pass all inputs
    // to dear imgui, and hide them from your application based on those two
    // flags.
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. Show a simple window.
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets
    // automatically appears in a window called "Debug".
    {
      static float f = 0.0f;
      ImGui::Text(
          "Hello, world!");  // Some text (you can use a format string too)
      ImGui::SliderFloat("float", &f, 0.0f,
                         1.0f);  // Edit 1 float as a slider from 0.0f to 1.0f
      ImGui::ColorEdit3("clear color",
                        (float*)&clear_color);  // Edit 3 floats as a color
      if (ImGui::Button("Demo Window"))  // Use buttons to toggle our bools. We
                                         // could use Checkbox() as well.
        show_demo_window ^= 1;
      if (ImGui::Button("Another Window")) show_another_window ^= 1;
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      static bool openFileDialog = false;

      if (ImGui::Button("Open File Dialog")) {
        openFileDialog = true;
      }

      static std::string filePathName = "";
      static std::string path = "";
      static std::string fileName = "";
      static std::string filter = "";

      if (openFileDialog) {
        if (ImGuiFileDialog::Instance()->FileDialog(
                "Choose File", ".cpp\0.h\0.hpp\0\0", false, ".", "")) {
          if (ImGuiFileDialog::Instance()->IsOk == true) {
            filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
            path = ImGuiFileDialog::Instance()->GetCurrentPath();
            fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
            filter = ImGuiFileDialog::Instance()->GetCurrentFilter();
          } else {
            filePathName = "";
            path = "";
            fileName = "";
            filter = "";
          }
          openFileDialog = false;
        }
      }

      if (filePathName.size() > 0)
        ImGui::Text("Choosed File Path Name : %s", filePathName.c_str());
      if (path.size() > 0) ImGui::Text("Choosed Path Name : %s", path.c_str());
      if (fileName.size() > 0)
        ImGui::Text("Choosed File Name : %s", fileName.c_str());
      if (filter.size() > 0) ImGui::Text("Choosed Filter : %s", filter.c_str());
    }

    // 2. Show another simple window. In most cases you will use an explicit
    // Begin/End pair to name the window.
    if (show_another_window) {
      ImGui::Begin("Another Window", &show_another_window);
      ImGui::Text("Hello from another window!");
      ImGui::End();
    }

    // 3. Show the ImGui demo window. Most of the sample code is in
    // ImGui::ShowDemoWindow().
    if (show_demo_window) {
      ImGui::SetNextWindowPos(
          ImVec2(650, 20),
          ImGuiCond_FirstUseEver);  // Normally user code doesn't need/want to
                                    // call this because positions are saved in
                                    // .ini file anyway. Here we just want to
                                    // make the demo initial state a bit more
                                    // friendly!
      ImGui::ShowDemoWindow(&show_demo_window);
    }

    // Rendering
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  glfwTerminate();

  return 0;
}
