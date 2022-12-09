#include <Windows.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//#include <assimp/config.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "bezier.h"
#include "bezier_manager.h"


// GLM库从0.9.9版本起，默认会将矩阵类型初始化为一个零矩阵（所有元素均为0），而非单位矩阵
// 使用0.9.9.8，所有的矩阵初始化需改为 glm::mat4 mat = glm::mat4(1.0f)

#define M_PI 3.14159265358979323846

// 函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);
void normalize(float& x, float& y);
void click_verts_combine(float coords[16], float thres = 0.010f);
void save_curves_to_txt();
void read_curves_from_txt();

// 窗口设置
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;

// 用于记录鼠标状态的3个全局变量
static bool LMBRelease_for_verts_input;
static float clickPointX = 0;
static float clickPointY = 0;
static double clickPointX_d = 0;
static double clickPointY_d = 0;

// 鼠标点击输入顶点的状态变量
static bool isAddingCurve = false; // 是否正在添加曲线，如果中途放弃添加则立刻将该值置为false
static bool vertAdded[4] = { false }; // 如果在添加曲线，索引值对应的顶点是否已添加
static bool cancelAddingCurve = false; // 如果正在添加曲线，是否取消添加，置为true后退出添加，需重置为false
static bool vertsEnterComplete = false; // 如果为true则停止交互输入，开始向curve_manager传递顶点数据
static bool sampleRateEnterComplete = false; // 如果为true则停止交互输入，开始向curve_manager传递采样率数据
static int addingVertIndex = 0; // 如果正在添加顶点，正在添加第几个顶点，取值为0，1，2，3
static bool vertClicked = false; // 如果正在添加顶点，鼠标是否按下，若按下则停止记录鼠标位置
static bool mouseAlreadyDown = false; // 用于处理鼠标按下再松开时的标记
static bool goToNextVert[3] = { false };

// 用一个manager作为集合来管理所有曲线
static bezier_manager curves_manager;

// 用于渲染控制顶点正方形的两个小三角形顶点
float quad[] = {
	-1.0f, -1.0f,
	 1.0f, -1.0f,
	-1.0f,  1.0f,
	 1.0f,  1.0f,
	-1.0f,	1.0f,
	 1.0f, -1.0f
};

// 旋转轴两端点，相当于y轴的位置
float midline_point[] = {
	0.0f,  0.5f,
	0.0f, -0.5f
};


// 用于测试两个class的main函数
/*int main()
{
	bezier_manager curve_manager;

	bezier_line_4_vertex a_bezier_line_4_vertex;

	float the_verts[16] = {
		1.0f, 1.9f, 0.0f, 1.0f,
		1.3f, 1.1f, 0.0f, 1.0f,
		1.5f, 1.5f, 0.0f, 1.0f,
		1.7f, 0.8f, 0.0f, 1.0f };

	float the_verts_2[16] = {
		2.0f, 1.9f, 0.0f, 1.0f,
		2.3f, 1.1f, 0.0f, 1.0f,
		2.5f, 1.5f, 0.0f, 1.0f,
		2.7f, 2.8f, 0.0f, 1.0f };

	a_bezier_line_4_vertex.update_ctrl_verts(the_verts);

	std::cout << "\nsample rate\n\n";
	std::cout << a_bezier_line_4_vertex.sample_rate;
	std::cout << "\n\nctrl\n\n";
	for (int i = 0; i < 16; ++i)
	{
		if (i % 4 == 0) std::cout << i / 4 << '\t';
		std::cout << a_bezier_line_4_vertex.ctrl_verts[i] << '\t';
		if (i % 4 == 3) std::cout << '\n';
	}
	std::cout << "\n\ndisp\n\n";
	for (int i = 0; i < (a_bezier_line_4_vertex.sample_rate + 1) * 4; ++i)
	{
		if (i % 4 == 0) std::cout << i / 4 << '\t';
		std::cout << a_bezier_line_4_vertex.disp_verts[i] << '\t';
		if (i % 4 == 3) std::cout << '\n';
	}

	std::cout << "\n\nnum of curves\n\n";
	std::cout << curve_manager.num_of_curves;
	//std::cout << "\n\ncurve ptr\n\n";
	//std::cout << curve_manager.curves;

	curve_manager.add_curve(the_verts);

	std::cout << "\n\nnum of curves\n\n";
	std::cout << curve_manager.num_of_curves;
	std::cout << "\n\ncurve 0 sample rate\n\n";
	std::cout << curve_manager.curves[0].sample_rate;
	std::cout << "\n\ncurve 0 ctrl verts\n\n";
	for (int i = 0; i < 16; ++i)
	{
		if (i % 4 == 0) std::cout << i / 4 << '\t';
		std::cout << curve_manager.curves[0].ctrl_verts[i] << '\t';
		if (i % 4 == 3) std::cout << '\n';
	}
	std::cout << "\n\ncurve 0 disp\n\n";
	for (int i = 0; i < (curve_manager.curves[0].sample_rate + 1) * 4; ++i)
	{
		if (i % 4 == 0) std::cout << i/4 << '\t';
		std::cout << curve_manager.curves[0].disp_verts[i] << '\t';
		if (i % 4 == 3) std::cout << '\n';
	}

	curve_manager.add_curve(the_verts_2, 100);

	std::cout << "\n\nnum of curves\n\n";
	std::cout << curve_manager.num_of_curves;

	std::cout << "\n\ncurve 1 sample rate\n\n";
	std::cout << curve_manager.curves[1].sample_rate;

	std::cout << "\n\ncurve 1 ctrl verts\n\n";
	for (int i = 0; i < 16; ++i)
	{
		if (i % 4 == 0) std::cout << i / 4 << '\t';
		std::cout << curve_manager.curves[1].ctrl_verts[i] << '\t';
		if (i % 4 == 3) std::cout << '\n';
	}
	std::cout << "\n\ncurve 1 disp\n\n";
	for (int i = 0; i < (curve_manager.curves[1].sample_rate + 1) * 4; ++i)
	{
		if (i % 4 == 0) std::cout << i / 4 << '\t';
		std::cout << curve_manager.curves[1].disp_verts[i] << '\t';
		if (i % 4 == 3) std::cout << '\n';
	}

	curve_manager.add_curve(the_verts_2, 100);
	curve_manager.add_curve(the_verts, 100);

	std::cout << "\n\nnum of curves\n\n";
	std::cout << curve_manager.num_of_curves;

	curve_manager.edit_curve(1, 3, 0, 0, 0);
	std::cout << "\n\ncurve 1 ctrl verts\n\n";
	for (int i = 0; i < 16; ++i)
	{
		if (i % 4 == 0) std::cout << i / 4 << '\t';
		std::cout << curve_manager.curves[1].ctrl_verts[i] << '\t';
		if (i % 4 == 3) std::cout << '\n';
	}
	std::cout << "\n\ncurve 1 disp\n\n";
	for (int i = 0; i < (curve_manager.curves[1].sample_rate + 1) * 4; ++i)
	{
		if (i % 4 == 0) std::cout << i / 4 << '\t';
		std::cout << curve_manager.curves[1].disp_verts[i] << '\t';
		if (i % 4 == 3) std::cout << '\n';
	}

	curve_manager.delete_curve(2);

	std::cout << "\n\nnum of curves\n\n";
	std::cout << curve_manager.num_of_curves;

	curve_manager.delete_all_curves();

	std::cout << "\n\nnum of curves\n\n";
	std::cout << curve_manager.num_of_curves;

	curve_manager.add_curve(the_verts_2, 20);
	curve_manager.add_curve(the_verts_2, 15);

	std::cout << "\n\nnum of curves\n\n";
	std::cout << curve_manager.num_of_curves;
	std::cout << "\n\ncurve 1 ctrl verts\n\n";
	for (int i = 0; i < 16; ++i)
	{
		if (i % 4 == 0) std::cout << i / 4 << '\t';
		std::cout << curve_manager.curves[1].ctrl_verts[i] << '\t';
		if (i % 4 == 3) std::cout << '\n';
	}
	std::cout << "\n\ncurve 1 disp\n\n";
	for (int i = 0; i < (curve_manager.curves[1].sample_rate + 1) * 4; ++i)
	{
		if (i % 4 == 0) std::cout << i / 4 << '\t';
		std::cout << curve_manager.curves[1].disp_verts[i] << '\t';
		if (i % 4 == 3) std::cout << '\n';
	}

	a_bezier_line_4_vertex.update_ctrl_verts(the_verts_2);

	std::cout << "\nsample rate\n\n";
	std::cout << a_bezier_line_4_vertex.sample_rate;
	std::cout << "\n\nctrl\n\n";
	for (int i = 0; i < 16; ++i)
	{
		if (i % 4 == 0) std::cout << i / 4 << '\t';
		std::cout << a_bezier_line_4_vertex.ctrl_verts[i] << '\t';
		if (i % 4 == 3) std::cout << '\n';
	}
	std::cout << "\n\ndisp\n\n";
	for (int i = 0; i < (a_bezier_line_4_vertex.sample_rate + 1) * 4; ++i)
	{
		if (i % 4 == 0) std::cout << i / 4 << '\t';
		std::cout << a_bezier_line_4_vertex.disp_verts[i] << '\t';
		if (i % 4 == 3) std::cout << '\n';
	}

	return 0;
}*/

// int main()
void modeling_addCurve()
{
	/*----------初始化----------*/

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_REFRESH_RATE, 5);

	// glfw 创建窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW2 Bezier Curve Modeling", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;// -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// glad 加载OpenGL函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;// -1;
	}



	/*----------设定回调函数----------*/

	// 改变窗口大小时的回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 鼠标点击回调函数
	glfwSetMouseButtonCallback(window, mouse_button_callback);



	/*----------设定变量----------*/



	/*----------VAOs VBOs shaders----------*/

	// 存储曲线采样点
	// 最多只能创建10条曲线 <哈哈哈不知道能不能用动态数组>
	unsigned int curveVAO[10];
	glGenVertexArrays(10, curveVAO);
	unsigned int curveVBO[10];
	glGenBuffers(10, curveVBO);

	// 存储用于显示控制顶点小四边形的各顶点
	unsigned int pointVAO[10];
	glGenVertexArrays(10, pointVAO);
	unsigned int pointVBO[10];
	glGenBuffers(10, pointVBO);

	// 存储实例，实例化
	unsigned int instanceVBO;
	glGenBuffers(1, &instanceVBO);

	// 临时存储正在添加曲线时的控制顶点，同样采用实例化
	unsigned int temp_ctrl_point_VAO, temp_ctrl_point_VBO;
	glGenVertexArrays(1, &temp_ctrl_point_VAO);
	glGenBuffers(1, &temp_ctrl_point_VBO);
	unsigned int temp_instanceVBO;
	glGenBuffers(1, &temp_instanceVBO);

	// 存储用于渲染中轴线的两端点
	unsigned int midlineVAO, midlineVBO;
	glGenVertexArrays(1, &midlineVAO);
	glGenBuffers(1, &midlineVBO);

	
	//Shader shader1("E:/OpenGL_projects/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/shaders/test.vert", "E:/OpenGL_projects/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/shaders/test.frag");
	//Shader shader2("E:/OpenGL_projects/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/shaders/point.vert", "E:/OpenGL_projects/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/shaders/test.frag");
	//Shader temp_shader("E:/OpenGL_projects/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/shaders/point.vert", "E:/OpenGL_projects/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/shaders/test.frag");
	//Shader midline_shader("E:/OpenGL_projects/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/shaders/midline.vert", "E:/OpenGL_projects/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/HW2_Bezier_Spline_Modeling/shaders/test.frag");
	Shader shader1("./shaders/test.vert", "./shaders/test.frag");
	Shader shader2("./shaders/point.vert", "./shaders/test.frag");
	Shader temp_shader("./shaders/point.vert", "./shaders/test.frag");
	Shader midline_shader("./shaders/midline.vert", "./shaders/test.frag");
	

	// 设置Dear ImGui上下文
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// 设置Dear ImGui风格
	ImGui::StyleColorsDark();

	// 设置平台
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");



	/*----------设置状态变量----------*/
	// 为什么要有这一部分是因为在渲染循环中不可能再开一个等待鼠标点击输入顶点坐标的循环

	bool show_control_panel = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// 第1阶段：通过鼠标点击输入顶点
	isAddingCurve = false; // 是否正在添加曲线，如果中途放弃添加则立刻将该值置为false
	for (int i = 0; i < 4; ++i) 
		vertAdded[i] = false; // 如果在添加曲线，索引值对应的顶点是否已添加
	cancelAddingCurve = false; // 如果正在添加曲线，是否取消添加，置为true后退出添加，需重置为false
	vertsEnterComplete = false; // 如果为true则停止交互输入，开始向curve_manager传递顶点数据
	sampleRateEnterComplete = false; // 如果为true则停止交互输入，开始向curve_manager传递采样率数据
	addingVertIndex = 0; // 如果正在添加顶点，正在添加第几个顶点，取值为0，1，2，3
	vertClicked = false; // 如果正在添加顶点，鼠标是否按下，若按下则停止记录鼠标位置
	mouseAlreadyDown = false; // 用于处理鼠标按下再松开时的标记
	for (int i = 0; i < 3; ++i) 
		goToNextVert[i] = false;
	bool start_following_last_curve = false; // 询问是否需要让新建的曲线与前一条曲线的最后一个控制顶点相连

	// 第2阶段：完成曲线输入，要生成旋转扫描的模型
	bool complete_adding_curve = false; // 询问是否完成曲线添加，完成后将开始生成旋转扫描的模型
	bool rebuild_model_back_to_adding_curve = false; // 生成模型后不满意，回到添加曲线的步骤重做

	// 通过ImGui输入给curve_manager的参数
	float ctrl_verts[16]; // 保存控制顶点的数组
	int sample_rate = 10; // 保存采样率，默认为10


	/*----------渲染循环----------*/

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		// glfwPollEvents();
		// Sleep(100);

		
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//std::cout << clickPointX << '\t' << clickPointY << std::endl;
		//for (int i = 0; i < 3; ++i) std::cout << goToNextVert[i] << '\t'; std::cout << '\n';

		// ImGui 窗口设置
		ImGui::SetWindowPos(ImVec2(2, 2));
		ImGui::SetWindowSize(ImVec2(600, 400));
		ImGui::Begin("Control Panel");
		ImGui::Text("Manage the Curves and Verts Here.");
		ImGui::Text("Curves Added: %d", curves_manager.num_of_curves);
		//ImGui::Checkbox("Demo Window", &show_control_panel);
		//ImGui::ColorEdit3("Set Clear Color", (float*)&clear_color);
		//glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT);


		

		// ImGui 添加曲线
		if (isAddingCurve == false)
		{
			isAddingCurve = ImGui::Button("Add Bezier Curve");
			if (isAddingCurve) addingVertIndex = 0;
		}

		if (isAddingCurve)
		{
			isAddingCurve = true;
			/*counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);*/
			//if (ImGui::IsMouseDown(0) == true) LMBRelease_for_verts_input = false;
		}

		if (isAddingCurve)
		{
			// 按下时变为true，退出添加曲线
			cancelAddingCurve = ImGui::Button("Cancel Adding Curve");

			// 退出添加曲线
			if (cancelAddingCurve)
			{
				// 所有状态变量重置
				cancelAddingCurve = false;
				isAddingCurve = false;
				for (int i = 0; i < 4; ++i)
					vertAdded[i] = false;
				vertsEnterComplete = false;
				sampleRateEnterComplete = false;
				addingVertIndex = 0;
				vertClicked = false;
				std::cout << "Cancel Adding Curve\n";
			}
		}

		// 如果没有退出，则开始添加曲线
		if (isAddingCurve)
		{
			
			if (sampleRateEnterComplete)
			{
					
				ImGui::Text("Sample Rate = %d", sample_rate);

				glBindVertexArray(temp_ctrl_point_VAO);
				glBindBuffer(GL_ARRAY_BUFFER, temp_ctrl_point_VBO);
				glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float) * 2, quad, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				// 输入控制顶点
				{
					// 输入第1个顶点
					if (addingVertIndex == 0 && vertAdded[0] == false/* && LMBRelease_for_verts_input == false*/)
					{
						// 询问是否需要与前一条曲线首尾相接
						if (start_following_last_curve && curves_manager.num_of_curves > 0)
						{
							for (int i = 0; i < 4; ++i)
							{
								ctrl_verts[i] = curves_manager.curves[curves_manager.num_of_curves - 1].ctrl_verts[12 + i];
							}

							LMBRelease_for_verts_input = false;
							addingVertIndex = 1;
							vertAdded[0] = true;
						}
						if (vertAdded[0] == false)
						{
							//std::cout << LMBRelease_for_verts_input << std::endl;
							if (LMBRelease_for_verts_input)
							{
								//std::cout << "000000000000\n";
								ctrl_verts[0] = clickPointX;
								ctrl_verts[1] = clickPointY;
								normalize(ctrl_verts[0], ctrl_verts[1]);
								ctrl_verts[2] = 0.0f;
								ctrl_verts[3] = 1.0f;

								LMBRelease_for_verts_input = false;
								addingVertIndex = 1;
								vertAdded[0] = true;
							}
							//std::cout << "aaaaa\n";
						}

					}
					if (addingVertIndex > 0)
					{
						ImGui::Text("Vert 1 Coords: (%g, %g)", ctrl_verts[0], ctrl_verts[1]);
					}

					// 输入第2个顶点
					if (addingVertIndex == 1 && vertAdded[1] == false && goToNextVert[0] /* && LMBRelease_for_verts_input == false*/)
					{
						if (LMBRelease_for_verts_input)
						{
							ctrl_verts[4] = clickPointX;
							ctrl_verts[5] = clickPointY;
							normalize(ctrl_verts[4], ctrl_verts[5]);
							ctrl_verts[6] = 0.0f;
							ctrl_verts[7] = 1.0f;

							LMBRelease_for_verts_input = false;
							addingVertIndex = 2;
							vertAdded[1] = true;
						}
					}
					if (addingVertIndex > 1)
					{
						ImGui::Text("Vert 2 Coords: (%g, %g)", ctrl_verts[4], ctrl_verts[5]);
					}

					// 输入第3个顶点
					if (addingVertIndex == 2 && vertAdded[2] == false && goToNextVert[1] /* && LMBRelease_for_verts_input == false*/)
					{
						if (LMBRelease_for_verts_input)
						{
							ctrl_verts[8] = clickPointX;
							ctrl_verts[9] = clickPointY;
							normalize(ctrl_verts[8], ctrl_verts[9]);
							ctrl_verts[10] = 0.0f;
							ctrl_verts[11] = 1.0f;

							LMBRelease_for_verts_input = false;
							addingVertIndex = 3;
							vertAdded[2] = true;
						}
					}
					if (addingVertIndex > 2)
					{
						ImGui::Text("Vert 3 Coords: (%g, %g)", ctrl_verts[8], ctrl_verts[9]);
					}

					// 输入第4个顶点
					if (addingVertIndex == 3 && vertAdded[3] == false && goToNextVert[2] /* && LMBRelease_for_verts_input == false*/)
					{
						if (LMBRelease_for_verts_input)
						{
							ctrl_verts[12] = clickPointX;
							ctrl_verts[13] = clickPointY;
							normalize(ctrl_verts[12], ctrl_verts[13]);
							ctrl_verts[14] = 0.0f;
							ctrl_verts[15] = 1.0f;

							LMBRelease_for_verts_input = false;
							addingVertIndex = 4;
							vertAdded[3] = true;
						}
					}
					if (addingVertIndex > 3)
					{
						ImGui::Text("Vert 4 Coords: (%g, %g)", ctrl_verts[12], ctrl_verts[13]);
					}
				}
				

				if (vertAdded[0] == true && vertAdded[1] == false) goToNextVert[0] = true;
				if (vertAdded[1] == true && vertAdded[2] == false) goToNextVert[1] = true;
				if (vertAdded[2] == true && vertAdded[3] == false) goToNextVert[2] = true;
			}

			// 首先必须输入采样率，才能输入顶点坐标，添加第二条及后续曲线时还需指定是否以前一条曲线的最后一个点为起始
			if (sampleRateEnterComplete == false)
			{
				ImGui::InputInt("Sample Rate", &sample_rate);
				if (curves_manager.num_of_curves > 0)
					ImGui::Checkbox("Follow the Latter Curve?", &start_following_last_curve);

				sampleRateEnterComplete = ImGui::Button("Comfirm");
			}

			if (addingVertIndex == 4)
			{
				vertsEnterComplete = true;
				isAddingCurve = false;
				cancelAddingCurve = false;
				isAddingCurve = false;
				for (int i = 0; i < 4; ++i)
					vertAdded[i] = false;
				sampleRateEnterComplete = false;
				addingVertIndex = 0;
				vertClicked = false;
				std::cout << "Adding Complete!" << std::endl;
			}
			// 写入curve_manager
			if (vertsEnterComplete == true)
			{
				//click_verts_combine(ctrl_verts, 0.01f);
				curves_manager.add_curve(ctrl_verts, sample_rate);
				vertsEnterComplete = false;
			}
		} // 曲线添加完成

		// 更新显示已添加的曲线
		curves_manager.updategl(curveVAO, curveVBO, pointVAO, pointVBO, instanceVBO);



		/*----------曲线输入完成，开始旋转扫描建模----------*/

		//bezier_manager rotate_curves[36];
		//for (int i = 0; i < 36; ++i) // 间隔10°
		//{
		//	glm::mat4 trans;
		//	trans = glm::rotate(trans, glm::radians(10.0f), glm::vec3(0.0, 1.0, 0.0));
		//	unsigned int transformLoc = glGetUniformLocation(rotate_shader.ID, "transform");
		//	for (int j = 0; j < curves_manager.num_of_curves; ++j)
		//	{
		//		rotate_curves[i].curves[j].sample_rate = curves_manager.curves[j].sample_rate;
		//		for (int k = 0; k < 16; ++k)
		//		{
		//			rotate_curves[i].curves[j].ctrl_verts[k] = curves_manager.curves[j].ctrl_verts[k];
		//		}
		//	}
		//}




		/*----------glClear，在此之后才能draw----------*/

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// 曲线添加阶段未结束时始终渲染中轴线
		if (complete_adding_curve == false)
		{
			midline_shader.use();
			shader1.setVec3("color", 0.7f, 0.7f, 0.7f);
			glBindVertexArray(midlineVAO);
			glBindBuffer(GL_ARRAY_BUFFER, midlineVBO);
			glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), midline_point, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glDrawArrays(GL_LINE_STRIP, 0, 2);
		}


		shader1.use();
		shader1.setVec3("color", 1.0f, 1.0f, 1.0f);

		for (int i = 0; i < curves_manager.num_of_curves; ++i)
		{
			glBindVertexArray(curveVAO[i]);
			glDrawArrays(GL_LINE_STRIP, 0, curves_manager.curves[i].sample_rate + 1);
		}

		shader2.use();
		shader2.setVec3("color", 1.0f, 0.0f, 0.0f);
		shader2.setFloat("scale", 0.01f);
		for (int i = 0; i < curves_manager.num_of_curves; ++i)
		{
			glBindVertexArray(pointVAO[i]);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 4); // 两个小三角形，6个顶点，共有4个控制顶点
		}

		// 显示正在添加的曲线的控制顶点位置
		if (addingVertIndex > 0) 
		{
			temp_shader.use();
			temp_shader.setVec3("color", 1.0f, 0.0f, 0.0f);
			temp_shader.setFloat("scale", 0.01f);

			glBindVertexArray(temp_ctrl_point_VAO);
			glBindBuffer(GL_ARRAY_BUFFER, temp_ctrl_point_VBO);
			glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), quad, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, temp_instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, 4 * addingVertIndex * sizeof(float), ctrl_verts, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribDivisor(1, 1);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(temp_ctrl_point_VAO);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, addingVertIndex);
		}

		
		// 显示鼠标信息
		//ImGui::Text("Mouse wheel: %.1f", io.MouseWheel);
		ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
		ImGui::Text("Mouse pos normalized: (%g, %g)", (io.MousePos.x - SCR_WIDTH / 2) / (SCR_WIDTH / 2), (-io.MousePos.y + SCR_HEIGHT / 2) / (SCR_HEIGHT / 2));
		//static float f0 = 0.001f;
		//ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Mouse down:");
		for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
		{
			if (ImGui::IsMouseDown(i))
			{
				ImGui::SameLine();
				ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]);
			}
		}

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	} // 渲染循环结束

	save_curves_to_txt();
	glfwTerminate();
	//return 0;
}


void modeling_rotate_scan()
{
	Shader rotate_shader("./shaders/rotate_scan.vert", "./shaders/test.frag");
}

int main()
{
	// 第1阶段：添加曲线，实现了添加曲线过程的可视化和交互，结果保存在全局变量curve_manager中
	modeling_addCurve();


	// 第2阶段：生成旋转体，预览模型


	// 第3阶段：导出模型


	return 0;
}


// 处理输入
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // 按ESC退出
	{
		glfwSetWindowShouldClose(window, true);
	}
}


// 处理鼠标事件和位置
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	/*if (action == GLFW_PRESS)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			LMBRelease_for_verts_input = false;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			break;
		default:
			return;
		}
	}*/
	if (action == GLFW_RELEASE)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			glfwGetCursorPos(window, &clickPointX_d, &clickPointY_d);

			//// 标准化为0 - 1
			//clickPointX = static_cast<float>(- 1 + (clickPointX_d / SCR_WIDTH) * 2);
			//clickPointY = static_cast<float>(- 1 + (clickPointY_d / SCR_HEIGHT) * 2);
			if (isAddingCurve && sampleRateEnterComplete)
			{
				clickPointX = static_cast<float>(clickPointX_d);
				clickPointY = static_cast<float>(clickPointY_d);
				LMBRelease_for_verts_input = true;
			}
			//std::cout << LMBRelease_for_verts_input << std::endl;

			//std::cout << clickPointX_d << '\t' << clickPointY_d << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			break;
		default:
			return;
		}
	}
	
}


// glfw 随窗口大小改变，会修改SCR_HEIGHT和SCR_WIDTH的值
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCR_HEIGHT = height;
	SCR_WIDTH = width;
}


// 将坐标标准化到-1.0至1.0
void normalize(float &x, float &y)
{
	x = (x - SCR_WIDTH / 2) / (SCR_WIDTH / 2);
	y = (-y + SCR_HEIGHT / 2) / (SCR_HEIGHT / 2);
}


float calculate_distance_square(float x1, float y1, float x2, float y2)
{
	return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}


// 使曲线首尾相接，仅将新曲线的第一个、最后一个点与其他所有先前已绘制的曲线的首尾点相比较（鼠标自动吸附，以屏幕像素坐标系为准）
void combine_2_verts(float coord[2], float result[2], float thres = 0.010f) // 采用像素坐标时，thres可取10，采用标准化坐标后可取0.01
{
	float distance_square_1, distance_square_2;
	for (int i = 0; i < curves_manager.num_of_curves; ++i)
	{
		// 通常更可能与最后一个点相连，故先判断
		distance_square_2 = calculate_distance_square(coord[0], coord[1], curves_manager.curves->ctrl_verts[12], curves_manager.curves->ctrl_verts[13]);
		if (distance_square_2 < thres)
		{
			result[0] = curves_manager.curves->ctrl_verts[12];
			result[1] = curves_manager.curves->ctrl_verts[13];
			std::cout << "Combine Occurred\n";
			return;
		}

		distance_square_1 = calculate_distance_square(coord[0], coord[1], curves_manager.curves->ctrl_verts[0], curves_manager.curves->ctrl_verts[1]);
		if (distance_square_1 < thres)
		{
			result[0] = curves_manager.curves->ctrl_verts[0];
			result[1] = curves_manager.curves->ctrl_verts[1];
			std::cout << "Combine Occurred\n";
		}
	}
}


// 合并那些距离很近的控制顶点
void click_verts_combine(float coords[16], float thres) // 函数声明中设定thres默认值0.010f
{
	for (int i = 0; i < 4; ++i)
	{
		float vert_coord[2], vert_result[2];
		// 提取顶点坐标
		vert_coord[0] = coords[i * 4 + 0];
		vert_coord[1] = coords[i * 4 + 1];
		// 与已有的顶点判断是否要合并
		combine_2_verts(coords, vert_result, thres);
		// 合并的结果在vert_result中
		coords[i * 4 + 0] = vert_result[0];
		coords[i * 4 + 1] = vert_result[1];
	}
}


// 将曲线数据保存为txt
void save_curves_to_txt()
{
	std::fstream file("CurveDataFile.txt", std::ios::out); // 先清空
	std::ofstream curveDataFile;
	curveDataFile.open("CurveDataFile.txt", std::ofstream::app);
	curveDataFile << "*CURVE_DATA_FILE\n";

	for (int i = 0; i < curves_manager.num_of_curves; ++i)
	{
		curveDataFile << "\n*CURVE_INDEX = \n" << i;
		curveDataFile << "\n*SAMPLE_RATE = \n" << curves_manager.curves[i].sample_rate;
		curveDataFile << "\n*CURVE_CTRL_VERTS = \n";
		for (int j = 0; j < 16; ++j)
		{
			curveDataFile << curves_manager.curves[i].ctrl_verts[j] << '\t';
		}
		curveDataFile << "\n";
	}
}


// 从txt读取曲线数据
void read_curves_from_txt() // 这个函数还没写完
{
	std::ofstream curveDataFile;
	curveDataFile.open("CurveDataFile.txt", std::ofstream::app);
	curveDataFile << "CURVE DATA FILE\n";

	for (int i = 0; i < curves_manager.num_of_curves; ++i)
	{
		curveDataFile << "\nCURVE_INDEX = " << i << "\n";
		curveDataFile << "SAMPLE_RATE = " << curves_manager.curves[i].sample_rate << "\n";
		curveDataFile << "CURVE_CTRL_VERTS: ";
		for (int j = 0; j < 16; ++j)
		{
			curveDataFile << curves_manager.curves[i].ctrl_verts[j] << '\t';
		}
		curveDataFile << "\n";
	}
}
