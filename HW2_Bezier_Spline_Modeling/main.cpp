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


// GLM���0.9.9�汾��Ĭ�ϻὫ�������ͳ�ʼ��Ϊһ�����������Ԫ�ؾ�Ϊ0�������ǵ�λ����
// ʹ��0.9.9.8�����еľ����ʼ�����Ϊ glm::mat4 mat = glm::mat4(1.0f)

#define M_PI 3.14159265358979323846

// ��������
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);
void normalize(float& x, float& y);
void click_verts_combine(float coords[16], float thres = 0.010f);
void save_curves_to_txt();
void read_curves_from_txt();

// ��������
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;

// ���ڼ�¼���״̬��3��ȫ�ֱ���
static bool LMBRelease_for_verts_input;
static float clickPointX = 0;
static float clickPointY = 0;
static double clickPointX_d = 0;
static double clickPointY_d = 0;

// ��������붥���״̬����
static bool isAddingCurve = false; // �Ƿ�����������ߣ������;������������̽���ֵ��Ϊfalse
static bool vertAdded[4] = { false }; // �����������ߣ�����ֵ��Ӧ�Ķ����Ƿ������
static bool cancelAddingCurve = false; // �������������ߣ��Ƿ�ȡ����ӣ���Ϊtrue���˳���ӣ�������Ϊfalse
static bool vertsEnterComplete = false; // ���Ϊtrue��ֹͣ�������룬��ʼ��curve_manager���ݶ�������
static bool sampleRateEnterComplete = false; // ���Ϊtrue��ֹͣ�������룬��ʼ��curve_manager���ݲ���������
static int addingVertIndex = 0; // ���������Ӷ��㣬������ӵڼ������㣬ȡֵΪ0��1��2��3
static bool vertClicked = false; // ���������Ӷ��㣬����Ƿ��£���������ֹͣ��¼���λ��
static bool mouseAlreadyDown = false; // ���ڴ�����갴�����ɿ�ʱ�ı��
static bool goToNextVert[3] = { false };

// ��һ��manager��Ϊ������������������
static bezier_manager curves_manager;

// ������Ⱦ���ƶ��������ε�����С�����ζ���
float quad[] = {
	-1.0f, -1.0f,
	 1.0f, -1.0f,
	-1.0f,  1.0f,
	 1.0f,  1.0f,
	-1.0f,	1.0f,
	 1.0f, -1.0f
};

// ��ת�����˵㣬�൱��y���λ��
float midline_point[] = {
	0.0f,  0.5f,
	0.0f, -0.5f
};


// ���ڲ�������class��main����
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
	/*----------��ʼ��----------*/

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_REFRESH_RATE, 5);

	// glfw ��������
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW2 Bezier Curve Modeling", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;// -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// glad ����OpenGL����ָ��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;// -1;
	}



	/*----------�趨�ص�����----------*/

	// �ı䴰�ڴ�Сʱ�Ļص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// ������ص�����
	glfwSetMouseButtonCallback(window, mouse_button_callback);



	/*----------�趨����----------*/



	/*----------VAOs VBOs shaders----------*/

	// �洢���߲�����
	// ���ֻ�ܴ���10������ <��������֪���ܲ����ö�̬����>
	unsigned int curveVAO[10];
	glGenVertexArrays(10, curveVAO);
	unsigned int curveVBO[10];
	glGenBuffers(10, curveVBO);

	// �洢������ʾ���ƶ���С�ı��εĸ�����
	unsigned int pointVAO[10];
	glGenVertexArrays(10, pointVAO);
	unsigned int pointVBO[10];
	glGenBuffers(10, pointVBO);

	// �洢ʵ����ʵ����
	unsigned int instanceVBO;
	glGenBuffers(1, &instanceVBO);

	// ��ʱ�洢�����������ʱ�Ŀ��ƶ��㣬ͬ������ʵ����
	unsigned int temp_ctrl_point_VAO, temp_ctrl_point_VBO;
	glGenVertexArrays(1, &temp_ctrl_point_VAO);
	glGenBuffers(1, &temp_ctrl_point_VBO);
	unsigned int temp_instanceVBO;
	glGenBuffers(1, &temp_instanceVBO);

	// �洢������Ⱦ�����ߵ����˵�
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
	

	// ����Dear ImGui������
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// ����Dear ImGui���
	ImGui::StyleColorsDark();

	// ����ƽ̨
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");



	/*----------����״̬����----------*/
	// ΪʲôҪ����һ��������Ϊ����Ⱦѭ���в������ٿ�һ���ȴ���������붥�������ѭ��

	bool show_control_panel = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// ��1�׶Σ�ͨ����������붥��
	isAddingCurve = false; // �Ƿ�����������ߣ������;������������̽���ֵ��Ϊfalse
	for (int i = 0; i < 4; ++i) 
		vertAdded[i] = false; // �����������ߣ�����ֵ��Ӧ�Ķ����Ƿ������
	cancelAddingCurve = false; // �������������ߣ��Ƿ�ȡ����ӣ���Ϊtrue���˳���ӣ�������Ϊfalse
	vertsEnterComplete = false; // ���Ϊtrue��ֹͣ�������룬��ʼ��curve_manager���ݶ�������
	sampleRateEnterComplete = false; // ���Ϊtrue��ֹͣ�������룬��ʼ��curve_manager���ݲ���������
	addingVertIndex = 0; // ���������Ӷ��㣬������ӵڼ������㣬ȡֵΪ0��1��2��3
	vertClicked = false; // ���������Ӷ��㣬����Ƿ��£���������ֹͣ��¼���λ��
	mouseAlreadyDown = false; // ���ڴ�����갴�����ɿ�ʱ�ı��
	for (int i = 0; i < 3; ++i) 
		goToNextVert[i] = false;
	bool start_following_last_curve = false; // ѯ���Ƿ���Ҫ���½���������ǰһ�����ߵ����һ�����ƶ�������

	// ��2�׶Σ�����������룬Ҫ������תɨ���ģ��
	bool complete_adding_curve = false; // ѯ���Ƿ����������ӣ���ɺ󽫿�ʼ������תɨ���ģ��
	bool rebuild_model_back_to_adding_curve = false; // ����ģ�ͺ����⣬�ص�������ߵĲ�������

	// ͨ��ImGui�����curve_manager�Ĳ���
	float ctrl_verts[16]; // ������ƶ��������
	int sample_rate = 10; // ��������ʣ�Ĭ��Ϊ10


	/*----------��Ⱦѭ��----------*/

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

		// ImGui ��������
		ImGui::SetWindowPos(ImVec2(2, 2));
		ImGui::SetWindowSize(ImVec2(600, 400));
		ImGui::Begin("Control Panel");
		ImGui::Text("Manage the Curves and Verts Here.");
		ImGui::Text("Curves Added: %d", curves_manager.num_of_curves);
		//ImGui::Checkbox("Demo Window", &show_control_panel);
		//ImGui::ColorEdit3("Set Clear Color", (float*)&clear_color);
		//glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT);


		

		// ImGui �������
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
			// ����ʱ��Ϊtrue���˳��������
			cancelAddingCurve = ImGui::Button("Cancel Adding Curve");

			// �˳��������
			if (cancelAddingCurve)
			{
				// ����״̬��������
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

		// ���û���˳�����ʼ�������
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

				// ������ƶ���
				{
					// �����1������
					if (addingVertIndex == 0 && vertAdded[0] == false/* && LMBRelease_for_verts_input == false*/)
					{
						// ѯ���Ƿ���Ҫ��ǰһ��������β���
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

					// �����2������
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

					// �����3������
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

					// �����4������
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

			// ���ȱ�����������ʣ��������붥�����꣬��ӵڶ�������������ʱ����ָ���Ƿ���ǰһ�����ߵ����һ����Ϊ��ʼ
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
			// д��curve_manager
			if (vertsEnterComplete == true)
			{
				//click_verts_combine(ctrl_verts, 0.01f);
				curves_manager.add_curve(ctrl_verts, sample_rate);
				vertsEnterComplete = false;
			}
		} // ����������

		// ������ʾ����ӵ�����
		curves_manager.updategl(curveVAO, curveVBO, pointVAO, pointVBO, instanceVBO);



		/*----------����������ɣ���ʼ��תɨ�轨ģ----------*/

		//bezier_manager rotate_curves[36];
		//for (int i = 0; i < 36; ++i) // ���10��
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




		/*----------glClear���ڴ�֮�����draw----------*/

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// ������ӽ׶�δ����ʱʼ����Ⱦ������
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
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 4); // ����С�����Σ�6�����㣬����4�����ƶ���
		}

		// ��ʾ������ӵ����ߵĿ��ƶ���λ��
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

		
		// ��ʾ�����Ϣ
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
	} // ��Ⱦѭ������

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
	// ��1�׶Σ�������ߣ�ʵ����������߹��̵Ŀ��ӻ��ͽ��������������ȫ�ֱ���curve_manager��
	modeling_addCurve();


	// ��2�׶Σ�������ת�壬Ԥ��ģ��


	// ��3�׶Σ�����ģ��


	return 0;
}


// ��������
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // ��ESC�˳�
	{
		glfwSetWindowShouldClose(window, true);
	}
}


// ��������¼���λ��
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

			//// ��׼��Ϊ0 - 1
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


// glfw �洰�ڴ�С�ı䣬���޸�SCR_HEIGHT��SCR_WIDTH��ֵ
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCR_HEIGHT = height;
	SCR_WIDTH = width;
}


// �������׼����-1.0��1.0
void normalize(float &x, float &y)
{
	x = (x - SCR_WIDTH / 2) / (SCR_WIDTH / 2);
	y = (-y + SCR_HEIGHT / 2) / (SCR_HEIGHT / 2);
}


float calculate_distance_square(float x1, float y1, float x2, float y2)
{
	return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}


// ʹ������β��ӣ����������ߵĵ�һ�������һ����������������ǰ�ѻ��Ƶ����ߵ���β����Ƚϣ�����Զ�����������Ļ��������ϵΪ׼��
void combine_2_verts(float coord[2], float result[2], float thres = 0.010f) // ������������ʱ��thres��ȡ10�����ñ�׼��������ȡ0.01
{
	float distance_square_1, distance_square_2;
	for (int i = 0; i < curves_manager.num_of_curves; ++i)
	{
		// ͨ�������������һ���������������ж�
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


// �ϲ���Щ����ܽ��Ŀ��ƶ���
void click_verts_combine(float coords[16], float thres) // �����������趨thresĬ��ֵ0.010f
{
	for (int i = 0; i < 4; ++i)
	{
		float vert_coord[2], vert_result[2];
		// ��ȡ��������
		vert_coord[0] = coords[i * 4 + 0];
		vert_coord[1] = coords[i * 4 + 1];
		// �����еĶ����ж��Ƿ�Ҫ�ϲ�
		combine_2_verts(coords, vert_result, thres);
		// �ϲ��Ľ����vert_result��
		coords[i * 4 + 0] = vert_result[0];
		coords[i * 4 + 1] = vert_result[1];
	}
}


// ���������ݱ���Ϊtxt
void save_curves_to_txt()
{
	std::fstream file("CurveDataFile.txt", std::ios::out); // �����
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


// ��txt��ȡ��������
void read_curves_from_txt() // ���������ûд��
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
