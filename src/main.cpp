//main.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Maze.h"
#include "Cell.h"
#include "Renderer.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include <iostream>
#include <stdexcept>

// --- 全局变量或应用状态 ---
GLFWwindow* window = nullptr;
ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.15f, 1.00f);
const char *glsl_version = "#version 330 core";

Maze* g_myMaze = nullptr;        // 全局迷宫对象指针
int g_mazeWidth = 20;            // 默认迷宫宽度
int g_mazeHeight = 15;           // 默认迷宫高度
float g_cellSize = 20.0f;        // 每个格子在屏幕上绘制的像素大小

bool show_demo_window = false; // Demo 窗口可以默认不显示，或者通过菜单控制
bool show_maze_controls_window = true;
bool show_maze_view_window = true;

//函数声明
void error_callback(int error, const char *description);
bool initialize_app_environment();
void processInput(GLFWwindow *window);
void cleanup_app_environment();

Renderer g_mazeRenderer;

int main(){
    //初始化
    if(!initialize_app_environment()){
        return -1;
    }

    //RenderLoop
    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();
        processInput(window);

        // Dear ImGUi
        ImGui_ImplOpenGL3_NewFrame(); // 通知 OpenGL3 渲染后端新的一帧
        ImGui_ImplGlfw_NewFrame();    // 通知 GLFW 平台后端新的一帧 (处理输入, 更新DisplaySize等)
        ImGui::NewFrame();            // ImGui 核心库开始新的一帧 (g.WithinFrameScope 在此变为 true)

        if (show_maze_view_window) {
            ImGui::Begin("MazeView", &show_maze_view_window);
            if (g_myMaze != nullptr) { // <--- 添加这个检查
                //std::cout << "DEBUG: g_myMaze is valid. Width: " << g_myMaze->getWidth()
                //        << ", Height: " << g_myMaze->getHeight() << std::endl; // 调试输出
                ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
                g_mazeRenderer.renderMaze(*g_myMaze, canvas_p0, g_cellSize);
            } else {
                //ImGui::Text("错误：迷宫对象 (g_myMaze) 为空指针!");
                //std::cout << "DEBUG: g_myMaze is nullptr!" << std::endl; // 调试输出
            }
            ImGui::End();
        }


        // --- DEMO ---
        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        // --- 迷宫窗口控制 ---
        if (show_maze_controls_window) { // 用一个布尔值控制此窗口是否显示
            ImGui::Begin("Maze Create GUI", &show_maze_controls_window); // 开始定义窗口

            ImGui::Text("Maze parameter settions"); // 静态文本作为标题

            // 为了避免拖动滑块时频繁重新生成迷宫，我们使用临时的 static 变量来接收滑块的输入
            // 只有当点击“生成”按钮时，或者当这些临时值与实际值不同且用户希望应用时，才更新全局变量并重新生成
            static int input_maze_width = g_mazeWidth;
            static int input_maze_height = g_mazeHeight;

            // ImGui::InputInt("宽度", &input_maze_width); // 也可以用 InputInt
            // ImGui::InputInt("高度", &input_maze_height);
            ImGui::SliderInt("Width", &input_maze_width, 5, 100);   // 最小值5，最大值100
            ImGui::SliderInt("Height", &input_maze_height, 5, 100);  // 最小值5，最大值100
            
            // 格子大小的滑块可以直接修改全局变量，因为它只影响显示，不影响迷宫结构
            ImGui::SliderFloat("Cell Size", &g_cellSize, 5.0f, 40.0f, "%.1f pixels");

            ImGui::Separator();

            ImGui::Text("Current Mazesize: %d x %d", g_mazeWidth, g_mazeHeight);
            ImGui::Text("Size to be generated: %d x %d", input_maze_width, input_maze_height);

            bool regenerate_button_pressed = ImGui::Button("create new maze");
            // 如果用户修改了宽度或高度的滑块，并且希望这些改动立即生效（或者配合按钮）
            // 这里我们设计为：按钮按下时，采用滑块的当前值
            if (regenerate_button_pressed) {
                if (input_maze_width > 0 && input_maze_height > 0) {
                    // 更新全局的迷宫尺寸变量
                    g_mazeWidth = input_maze_width;
                    g_mazeHeight = input_maze_height;

                    // 删除旧迷宫（如果存在）
                    if (g_myMaze) {
                        delete g_myMaze;
                        g_myMaze = nullptr;
                    }
                    // 创建并生成新迷宫
                    try {
                        std::cout << "Button pressed: Regenerating maze with size: "
                                << g_mazeWidth << "x" << g_mazeHeight << std::endl;
                        g_myMaze = new Maze(g_mazeWidth, g_mazeHeight);
                        g_myMaze->generate(); // 调用你的迷宫生成函数
                        // 你可能还想清除旧的路径标记（如果你的 Maze 类有 resetPathFlags）
                        // g_myMaze->resetPathFlags(); 
                        std::cout << "Maze regenerated." << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "Error regenerating maze: " << e.what() << std::endl;
                        if (g_myMaze) { delete g_myMaze; g_myMaze = nullptr; }
                    }
                } else {
                    // 如果输入的宽高无效，可以在这里给用户一些提示
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: Width must be positive!");
                }
            }
            
            ImGui::Separator();
            // 控制 Demo 窗口的显示
            ImGui::Checkbox("Show ImGui Demo window", &show_demo_window);
            ImGui::End();
        }//if
        ImGui::Render();

        // 4. OpenGL 绘制准备：获取帧缓冲大小并设置视口，然后清屏
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h); // 获取窗口的实际像素尺寸
        glViewport(0, 0, display_w, display_h);                 // 设置 OpenGL 渲染区域为整个窗口

        // 设置清屏颜色 (clear_color 是你定义的 ImVec4 变量)
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);                           // 清除颜色缓冲区
                                                                // 如果你使用深度测试，还需要 | GL_DEPTH_BUFFER_BIT

        // 5. 绘制 ImGui 的界面数据
        //    这将把 ImGui::Render() 生成的所有绘制命令交给 OpenGL 去执行
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 6. 交换缓冲区，将绘制好的画面显示出来
        glfwSwapBuffers(window);

    }//while

    cleanup_app_environment();
    return 0;
}

// 错误回调
void error_callback(int error, const char* description){
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}
// 处理键盘输入
void processInput(GLFWwindow* window){
    // 按下 ESC 退出
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

//初始化函数
bool initialize_app_environment(){
    // 1. 设置错误回调
    glfwSetErrorCallback(error_callback);

    // 2. 初始化 GLFW
    if(!glfwInit()){
        std::cerr << "Failed to initialize GLFW" << std::endl;
    }

    // 3. 配置 glfwWindowsHint
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 4. 创建窗口
    window = glfwCreateWindow(800, 800, "Maze Creater", NULL, NULL);
    if(window == NULL){
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 你之前移除了，可以根据需要加回来

    // 5. 初始化 GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    // 6. 初始化 Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    //设置平台和渲染器后端
    if(!ImGui_ImplGlfw_InitForOpenGL(window, true)){
        std::cerr << "Failed to initialize ImGui GLFW backend" << std::endl;
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }
    if(!ImGui_ImplOpenGL3_Init(glsl_version)){
        std::cerr << "Failed to initialize ImGui OpenGL3 backend" << std::endl;
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }
    
    std::cout << "Successfully initialized GLFW, GLAD, and Dear ImGui!" << std::endl;
    return true;
}

// 清理函数
void cleanup_app_environment(){
    //imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    std::cout << "clearn uo resource." << std::endl;
}