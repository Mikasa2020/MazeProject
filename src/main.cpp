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
bool show_maze_controls_window = false;
bool show_maze_view_window = false;

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

        //--- 创建一个填满整个 GLFW 窗口的“主画布”ImGui 窗口 ---
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(io.DisplaySize);

        // 设置窗口标志，使其没有标题栏、不可移动、不可调整大小
        ImGuiWindowFlags main_canvas_flags = 
            // ImGuiWindowFlags_NoTitleBar |
            // ImGuiWindowFlags_NoResize |
            // ImGuiWindowFlags_NoMove |
            // ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoDecoration | // 无装饰（推荐，替代多个No...标志）
            ImGuiWindowFlags_NoSavedSettings | //不保存窗口位置/大小状态
            ImGuiWindowFlags_NoBringToFrontOnFocus | // 当点击时不自动将此窗口带到最前（因为它是背景)
            ImGuiWindowFlags_MenuBar; //// 在这个主画布顶部添加一个菜单栏

        // 将样式设置包裹 MainApplicationCanvas 的 Begin/End
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); // 让 MainCanvas 内部无边距

        ImGui::Begin("MainApplicationCanvas", nullptr, main_canvas_flags); // nullptr 表示没有关闭按钮
        ImGui::PopStyleVar(3); // 对应上面的三个 PushStyleVar
        
        // --- 主菜单栏 (应该在 MainApplicationCanvas 的 Begin 和 End 之间) ---
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit")) { glfwSetWindowShouldClose(window, true); }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Show Demo Window", nullptr, &show_demo_window);
                ImGui::MenuItem("Show Maze Controls", nullptr, &show_maze_controls_window);
                ImGui::MenuItem("Show Maze View", nullptr, &show_maze_view_window);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // --- 在 MainApplicationCanvas 内部定义固定布局 ---
        float controls_panel_width = 300.0f; // 控制面板宽度
        float status_bar_height = ImGui::GetTextLineHeightWithSpacing() * 1.5f; // 底部状态栏高度

        // --- 左侧控制面板子窗口 ---
        ImGui::BeginChild("Controlsgegion", ImVec2(controls_panel_width, -status_bar_height), true, ImGuiWindowFlags_None);
        // true 表示带边框，ImGuiWindowFlags_None 表示默认子窗口行为
        // ImVec2(controls_panel_width, 0) 中，高度为0表示自动填充父窗口的剩余可用高度 (减去状态栏高度，如果状态栏在它之后)
        // 如果状态栏在底部，这里的0会填充到状态栏上方
        ImGui::Text("Mize parameter siting:");

        // 为了避免拖动滑块时频繁重新生成迷宫，我们使用临时的 static 变量来接收滑块的输入
        // 只有当点击“生成”按钮时，或者当这些临时值与实际值不同且用户希望应用时，才更新全局变量并重新生成
        static int input_maze_width = g_mazeWidth;
        static int input_maze_height = g_mazeHeight;

        // 宽度Sliderint
        ImGui::SliderInt("Width", &input_maze_width, 5, 100);   // 最小值5，最大值100
        // 高度Sliderint
        ImGui::SliderInt("Height", &input_maze_height, 5, 100);  // 最小值5，最大值100
        // 格子大小的滑块可以直接修改全局变量，因为它只影响显示，不影响迷宫结构
        ImGui::SliderFloat("Cell Size", &g_cellSize, 5.0f, 40.0f, "%.1f pixels");
        
        bool regenerate_button_pressed = ImGui::Button("create new maze");
        if(regenerate_button_pressed){
            if(input_maze_width > 0 && input_maze_height > 0){
                //改变生成高度
                g_mazeWidth = input_maze_width;
                g_mazeHeight = input_maze_height;
                //如果存在旧迷宫
                if(g_myMaze){
                    delete g_myMaze;
                    g_myMaze = nullptr;
                }
                //生成新迷宫
                try{
                    std::cout << "Button Pressed: Regeneraing maze and size: "
                                << "Width: " << g_mazeWidth
                                << "Height: " << g_mazeHeight << std::endl;

                    g_myMaze = new Maze(g_mazeWidth, g_mazeHeight);
                    g_myMaze->generate();
                    std::cout << "Maze regenerated." << std::endl;
                    // 显示迷宫窗口
                    show_maze_view_window = true;
                }catch (const std::exception& e){
                    std::cerr << "Error regenerating maze: " << e.what() << std::endl;
                    if (g_myMaze) { delete g_myMaze; g_myMaze = nullptr; }
                }
            }
        }
        ImGui::Separator();
        ImGui::Checkbox("Show ImGui Demo window", &show_demo_window);

        ImGui::EndChild(); // 结束 ControlsRegion 子窗口
        // --- 左窗口结束 ---

        // --- 右侧迷宫视图子窗口 ---
        ImGui::SameLine(); // 让下一个子窗口在控制面板的右边
        ImGui::BeginChild("MazeViewRegion", ImVec2(0, -status_bar_height), true, ImGuiWindowFlags_None);

        if(g_myMaze){
            ImVec2 canvas_p0 = ImGui::GetCursorScreenPos(); // 获取当前子窗口内容区的绘图起点
            g_mazeRenderer.renderMaze(*g_myMaze, canvas_p0, g_cellSize);
            // 告诉 ImGui 迷宫绘制内容有多大
            float rendered_maze_width = static_cast<float>(g_myMaze->getWidth()) * g_cellSize;
            float rendered_maze_height = static_cast<float>(g_myMaze->getHeight()) * g_cellSize;
            ImGui::Dummy(ImVec2(rendered_maze_width, rendered_maze_height));
        }else{
            ImGui::Text("Create New Maze!!!");
            ImGui::Dummy(ImVec2(200, 100)); // 给提示文本一个最小空间
        }
        ImGui::EndChild();
        // --- 右窗口结束 ---
        
        // --- 底部状态栏 ---
        ImGui::BeginChild("StatusBarRegion", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar);
        ImGui::Text("Maze Status Size: %dx%d", g_mazeWidth, g_mazeHeight);
        ImGui::EndChild();

        /*
        if (show_maze_view_window) {
            if (g_myMaze != nullptr) { // <--- 添加这个检查
                float maze_pixel_width = static_cast<float>(g_myMaze->getWidth()) * g_cellSize;
                float maze_pixel_height = static_cast<float>(g_myMaze->getHeight()) * g_cellSize;

                ImGui::SetNextWindowContentSize(ImVec2(maze_pixel_width, maze_pixel_height));
            } else {
                //ImGui::Text("错误：迷宫对象 (g_myMaze) 为空指针!");
                //std::cout << "DEBUG: g_myMaze is nullptr!" << std::endl; // 调试输出
                ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);
            }
            ImGui::Begin("MazeView", &show_maze_view_window);

            if(g_myMaze != nullptr){
                // 获取光标位置，这是在 "MazeView" 窗口内部内容区的当前可用绘图起点
                ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
                // 渲染迷宫到当前窗口
                g_mazeRenderer.renderMaze(*g_myMaze, canvas_p0, g_cellSize);
            }else{
                ImGui::Text("g_myMaze is nullptr! ");
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

            // 用 ImGui::CollapsingHeader("分类标题") 组织窗口
           // ImGui::SetNextItemWidth(width_per_header);
            if(ImGui::CollapsingHeader("MazeSize Siting")){
                // 宽度Sliderint
                ImGui::SliderInt("Width", &input_maze_width, 5, 100);   // 最小值5，最大值100
                // 高度Sliderint
                ImGui::SliderInt("Height", &input_maze_height, 5, 100);  // 最小值5，最大值100
                // 格子大小的滑块可以直接修改全局变量，因为它只影响显示，不影响迷宫结构
                ImGui::SliderFloat("Cell Size", &g_cellSize, 5.0f, 40.0f, "%.1f pixels");
            }

            //ImGui::SetNextItemWidth(width_per_header);
            if(ImGui::CollapsingHeader("operate")){
                bool regenerate_button_pressed = ImGui::Button("create new maze");
                if(regenerate_button_pressed){
                    if(input_maze_width > 0 && input_maze_height > 0){
                        //改变生成高度
                        g_mazeWidth = input_maze_width;
                        g_mazeHeight = input_maze_height;
                        //如果存在旧迷宫
                        if(g_myMaze){
                            delete g_myMaze;
                            g_myMaze = nullptr;
                        }
                        //生成新迷宫
                        try{
                            std::cout << "Button Pressed: Regeneraing maze and size: "
                                      << "Width: " << g_mazeWidth
                                      << "Height: " << g_mazeHeight << std::endl;

                            g_myMaze = new Maze(g_mazeWidth, g_mazeHeight);
                            g_myMaze->generate();
                            std::cout << "Maze regenerated." << std::endl;
                            // 显示迷宫窗口
                            show_maze_view_window = true;
                        }catch (const std::exception& e){
                            std::cerr << "Error regenerating maze: " << e.what() << std::endl;
                            if (g_myMaze) { delete g_myMaze; g_myMaze = nullptr; }
                        }
                    }
                }
            }
            //Demo
            if(ImGui::CollapsingHeader("View options")){
                ImGui::Checkbox("Show ImGui Demo window", &show_demo_window);
            }
            //-----------------组织窗口-----------------

            //----------------
            ImGui::Separator();

            ImGui::Text("Current Mazesize: %d x %d", g_mazeWidth, g_mazeHeight);
            ImGui::Text("Size to be generated: %d x %d", input_maze_width, input_maze_height);
            

            ImGui::End();
        }//if
        */

        if (show_demo_window){
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        ImGui::End();

        // --- 渲染 ---
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

    // --- 字体 ---
    const ImWchar* default_ranges = io.Fonts->GetGlyphRangesDefault();
    //添加字体文件
    float font_size_pixels = 40.0f;
    //字体文件路径
    const char* font_path = "fonts/OpenSans-Light.ttf";
    ImFont* new_default_font = io.Fonts->AddFontFromFileTTF(font_path, font_size_pixels, nullptr, nullptr);


    if (new_default_font != nullptr) {
        io.FontDefault = new_default_font; // 设置为 imgui 的默认字体
        std::cout << "Successfully loaded font: " << font_path << " and set as default." << std::endl;
    }else{
        std::cerr << "Failed to load font: " << font_path << ". Using ImGui's embedded default font." << std::endl;
        io.Fonts->AddFontDefault(); // 加载 ImGui 内嵌的 ProggyClean 字体作为备用
    }

    // --- 字体 UI 缩放 ---
    io.FontGlobalScale = 0.5f;

    //imgui主题
    ImGui::StyleColorsDark();

    //imgui style
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(10.0f, 10.0f); // 窗口内边距
    style.FramePadding = ImVec2(5.0f, 5.0f);    // 控件内边距
    style.ItemSpacing = ImVec2(8.0f, 6.0f);     // 控件之间间距
    style.WindowRounding = 5.0f;                // 窗口圆角
    style.FrameRounding = 4.0f;                 // 控件边框圆角
    style.GrabRounding = 4.0f;                  // 滑块等可拖动部分的圆角

    // 修改特定控件的颜色
    style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f); // 按钮颜色
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f); // CollapsingHeader 颜色
        

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