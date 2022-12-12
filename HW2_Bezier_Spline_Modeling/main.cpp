#include <Windows.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include <GL/glew.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//#include <assimp/config.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "shader.h"
#include "bezier.h"
#include "bezier_manager.h"
#include "camera.h"


// GLM���0.9.9�汾��Ĭ�ϻὫ�������ͳ�ʼ��Ϊһ�����������Ԫ�ؾ�Ϊ0�������ǵ�λ����
// ʹ��0.9.9.8�����еľ����ʼ�����Ϊ glm::mat4 mat = glm::mat4(1.0f)

#define M_PI 3.14159265358979323846


// ��������
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
//void processInput(GLFWwindow* window);
//void normalize(float& x, float& y);
//void click_verts_combine(float coords[16], float thres = 0.010f);
//void save_curves_to_txt();
//void read_curves_from_txt();


// ��������
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;


// �������
Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;


// ���ڼ�¼���״̬��3��ȫ�ֱ���
static bool LMBRelease_for_verts_input;
static float clickPointX = 0;
static float clickPointY = 0;
static double clickPointX_d = 0;
static double clickPointY_d = 0;


// ��������붥���״̬����
static bool isAddingCurve = false; // �Ƿ�����������ߣ������;������������̽���ֵ��Ϊfalse
static bool vertAdded[4] = {false}; // �����������ߣ�����ֵ��Ӧ�Ķ����Ƿ������
static bool cancelAddingCurve = false; // �������������ߣ��Ƿ�ȡ����ӣ���Ϊtrue���˳���ӣ�������Ϊfalse
static bool vertsEnterComplete = false; // ���Ϊtrue��ֹͣ�������룬��ʼ��curve_manager���ݶ�������
static bool sampleRateEnterComplete = false; // ���Ϊtrue��ֹͣ�������룬��ʼ��curve_manager���ݲ���������
static int addingVertIndex = 0; // ���������Ӷ��㣬������ӵڼ������㣬ȡֵΪ0��1��2��3
static bool vertClicked = false; // ���������Ӷ��㣬����Ƿ��£���������ֹͣ��¼���λ��
static bool mouseAlreadyDown = false; // ���ڴ�����갴�����ɿ�ʱ�ı��
static bool goToNextVert[3] = {false};


// ��һ��manager��Ϊ������������������
static bezier_manager curves_manager;


// ������Ⱦ���ƶ��������ε�����С�����ζ���
float quad[] = {
	-1.0f, -1.0f,
	1.0f, -1.0f,
	-1.0f, 1.0f,
	1.0f, 1.0f,
	-1.0f, 1.0f,
	1.0f, -1.0f
};


// ��ת�����˵㣬�൱��y���λ��
float midline_point[] = {
	0.0f, 0.5f,
	0.0f, -0.5f
};


// �׶�1����������
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // ��ESC�˳�
	{
		glfwSetWindowShouldClose(window, true);
	}
}


bool use_mouse_to_click = false;
// �׶�23����������
void processInput2(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);

	// ����LAlt��ʹ�������ImGui����
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
		use_mouse_to_click = true;
	// ����RAlt��ʹ�����������
	if (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
		use_mouse_to_click = false;
}

// �׶�1����������¼���λ��
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


// �׶�23�������������
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (use_mouse_to_click == false)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}


// �׶�2��������
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// glfw �洰�ڴ�С�ı䣬���޸�SCR_HEIGHT��SCR_WIDTH��ֵ
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCR_HEIGHT = height;
	SCR_WIDTH = width;
}


// �������׼����-1.0��1.0
void normalize(float& x, float& y)
{
	x = (x - SCR_WIDTH / 2) / (SCR_WIDTH / 2);
	y = (-y + SCR_HEIGHT / 2) / (SCR_HEIGHT / 2);
}


// �����������
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
		distance_square_2 = calculate_distance_square(coord[0], coord[1], curves_manager.curves->ctrl_verts[12],
		                                              curves_manager.curves->ctrl_verts[13]);
		if (distance_square_2 < thres)
		{
			result[0] = curves_manager.curves->ctrl_verts[12];
			result[1] = curves_manager.curves->ctrl_verts[13];
			std::cout << "Combine Occurred\n";
			return;
		}

		distance_square_1 = calculate_distance_square(coord[0], coord[1], curves_manager.curves->ctrl_verts[0],
		                                              curves_manager.curves->ctrl_verts[1]);
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


// ��txt��ȡ�������ݣ����������ûд��
void read_curves_from_txt()
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


// ��ģ�ͱ���Ϊobj��ʽ��Ĭ��û������
void save_as_obj(int num_of_verts, float* all_verts_3d, int num_of_vns, glm::vec3* all_verts_vn, int num_of_surfaces, unsigned int* indices)
{
	std::fstream file("modelFile.obj", std::ios::out);
	std::ofstream modelFile;
	modelFile.open("modelFile.obj", std::ofstream::app);
	modelFile << "# Generated by HW2_Bezier_Spline_Modeling\n";

	modelFile << "mtllib modelFile.mtl\n";

	// д�붥��
	for (int i = 0; i < num_of_verts; ++i) // Ҫ��all_verts_3dΪ4d
	{
		modelFile << "\nv ";
		modelFile << all_verts_3d[4 * i + 0] << " ";
		modelFile << all_verts_3d[4 * i + 1] << " ";
		modelFile << all_verts_3d[4 * i + 2];
	}

	// д�붥�㷨ʸ
	for (int i = 0; i < num_of_vns; ++i) // Ҫ��all_verts_vnΪ3d
	{
		modelFile << "\nvn ";
		modelFile << all_verts_vn[i].x << " ";
		modelFile << all_verts_vn[i].y << " ";
		modelFile << all_verts_vn[i].z;
	}

	// д���棬������
	for (int i = 0; i < num_of_surfaces; ++i) // ÿ�����indices����3��Ԫ��
	{
		modelFile << "\nf ";
		//modelFile << indices[3 * i + 0] << "//" << indices[3 * i + 0] << " ";
		//modelFile << indices[3 * i + 1] << "//" << indices[3 * i + 1] << " ";
		//modelFile << indices[3 * i + 2] << "//" << indices[3 * i + 2];
		modelFile << indices[3 * i + 0] << "//" << 3 * i + 0 << " ";
		modelFile << indices[3 * i + 1] << "//" << 3 * i + 1 << " ";
		modelFile << indices[3 * i + 2] << "//" << 3 * i + 2;
	}
	modelFile << '\n';

	modelFile << "\ns off";

	file.close();
	modelFile.close();
}


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


// ԭint main()��������������߽׶Σ�������д��ȫ�ֱ���curve_manager
void modeling_addCurve()
{
	/*----------��ʼ��----------*/

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_REFRESH_RATE, 5);
	// glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	// glfw ��������
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW2 Bezier Curve Modeling", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return; // -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// glad ����OpenGL����ָ��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return; // -1;
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

	// ���·��
	Shader shader1("./shaders/test.vert", "./shaders/test.frag");
	Shader shader2("./shaders/point.vert", "./shaders/test.frag");
	Shader temp_shader("./shaders/point.vert", "./shaders/test.frag");
	Shader midline_shader("./shaders/midline.vert", "./shaders/test.frag");


	// ����Dear ImGui������
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	// ����Dear ImGui���
	ImGui::StyleColorsDark();

	// ����ƽ̨
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");


	/*----------����״̬����----------*/
	// ΪʲôҪ����һ��������Ϊ����Ⱦѭ���в������ٿ�һ���ȴ���������붥�������ѭ��

	bool show_control_panel = true;
	auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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
	bool c1_continuity = false;

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
			/*all_verts_3d_length++;
			ImGui::SameLine();
			ImGui::Text("all_verts_3d_length = %d", all_verts_3d_length);*/
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
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
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
								ctrl_verts[i] = curves_manager.curves[curves_manager.num_of_curves - 1].ctrl_verts[12 +
									i];
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
					if (addingVertIndex == 1 && vertAdded[1] == false && goToNextVert[0]
						/* && LMBRelease_for_verts_input == false*/)
					{
						// ѯ���Ƿ���Ҫ��ǰһ������C1����
						if (c1_continuity && curves_manager.num_of_curves > 0)
						{
							for (int i = 4; i < 8; ++i)
							{
								ctrl_verts[i] = 2 * curves_manager.curves[curves_manager.num_of_curves - 1].ctrl_verts[i + 8] - curves_manager.curves[curves_manager.num_of_curves - 1].ctrl_verts[i + 4];
							}

							LMBRelease_for_verts_input = false;
							addingVertIndex = 2;
							vertAdded[1] = true;
						}
						if (vertAdded[1] == false)
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
						
					}
					if (addingVertIndex > 1)
					{
						ImGui::Text("Vert 2 Coords: (%g, %g)", ctrl_verts[4], ctrl_verts[5]);
					}

					// �����3������
					if (addingVertIndex == 2 && vertAdded[2] == false && goToNextVert[1]
						/* && LMBRelease_for_verts_input == false*/)
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
					if (addingVertIndex == 3 && vertAdded[3] == false && goToNextVert[2]
						/* && LMBRelease_for_verts_input == false*/)
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
				{
					ImGui::Checkbox("Follow the Latter Curve?", &start_following_last_curve);
					if (start_following_last_curve)
					{
						ImGui::Checkbox("C1 Continuity?", &c1_continuity);
					}
				}
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
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
			glEnableVertexAttribArray(0);

			glDrawArrays(GL_LINE_STRIP, 0, 2);
		}


		shader1.use();
		shader1.setVec3("color", 1.0f, 1.0f, 1.0f);

		for (int i = 0; i < curves_manager.num_of_curves; ++i)
		{
			glBindVertexArray(curveVAO[i]);
			//glDrawArrays(GL_LINE_STRIP, 0, curves_manager.curves[i].sample_rate + 1);
			glDrawArrays(GL_POINTS, 0, curves_manager.curves[i].sample_rate + 1);
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
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, temp_instanceVBO);
			glBufferData(GL_ARRAY_BUFFER, 4 * addingVertIndex * sizeof(float), ctrl_verts, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));
			glEnableVertexAttribArray(1);
			glVertexAttribDivisor(1, 1);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(temp_ctrl_point_VAO);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, addingVertIndex);
		}


		// ��ʾ�����Ϣ
		//ImGui::Text("Mouse wheel: %.1f", io.MouseWheel);
		ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
		ImGui::Text("Mouse pos normalized: (%g, %g)", (io.MousePos.x - SCR_WIDTH / 2) / (SCR_WIDTH / 2),
		            (-io.MousePos.y + SCR_HEIGHT / 2) / (SCR_HEIGHT / 2));
		//static float f0 = 0.001f;
		//ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
		            ImGui::GetIO().Framerate);
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

	// ImGui����
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwSetWindowShouldClose(window, false); // ���ùرմ��ڵ�״̬
	save_curves_to_txt();
	glfwTerminate();
	//return 0;
}


void modeling_rotate_scan()
{
	//��ʼ��
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_REFRESH_RATE, 5);
	// glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	// glfw ��������
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW2 Bezier Curve Modeling", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return; // -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	// ���ûص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// ��׽���
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad ����OpenGL����ָ��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return; // -1;
	}

	// ����Ȳ���
	glEnable(GL_DEPTH_TEST);

	// ����shader
	Shader rotate_shader("./shaders/rotate_scan.vert", "./shaders/test.frag");


	int amount = 4; // ��ת�������ܹ�Ҫ�ж���������
	float delta_degree = 360 / static_cast<float>(amount); // ���߼���ĽǶ�
	std::cout << "delta_degree = " << delta_degree << '\n';

	auto rotate_curves = new bezier_manager[amount]; // �洢��ת�任�������
	auto trans_mats = new glm::mat4[amount]; // �洢��y����ת��ͬ�Ƕȵľ���
	auto transformLoc = new unsigned int[amount]; // �洢uniform��λ��

	for (int i = 0; i < amount; ++i)
	{
		trans_mats[i] = glm::rotate(trans_mats[i], glm::radians(static_cast<float>(i) * delta_degree),
		                       glm::vec3(0.0, 1.0, 0.0));
		transformLoc[i] = glGetUniformLocation(rotate_shader.ID, "transform");
	}


	// 1��curve_manager�е���disp_vert��
	int total_sample_verts_1_manager = 0;
	for (int i = 0; i < curves_manager.num_of_curves; ++i)
	// ���в������������ע�⼴ʹ���������ʱ��ѡ�ˡ���ǰһ�����ߵ����һ���㿪ʼ�����һ�����ߡ�����Ȼ�Ὣ������õ㱣������
	{
		total_sample_verts_1_manager += (curves_manager.curves[i].sample_rate + 1);
	} // ע��total_sample_verts_1_manager����4

	// ע�����һ�����ߵĵ�Ч��������ʵ��total_sample_verts_1_manager - 1

	// ����1��curve_manager�е�����disp_verts
	auto all_disp_verts_in_1_manager = new float[4 * total_sample_verts_1_manager];
	int arraylength_of_disp_verts_in_1_manager = 0; // �൱��һ����������Ϊѭ�����������ṩ���㣬���յ�ֵΪ����ĳ���
	for (int i = 0; i < curves_manager.num_of_curves; ++i)
	{
		for (int j = 0; j < curves_manager.curves[i].sample_rate + 1; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				all_disp_verts_in_1_manager[arraylength_of_disp_verts_in_1_manager] = curves_manager.curves[i].
					disp_verts[4 * j + k];
				arraylength_of_disp_verts_in_1_manager += 1;
			}
		}
	} // ���ˣ�curve_manager�е�����disp_verts���ѿ���������all_disp_verts_in_1_manager�У������鳤��Ϊarraylength_of_disp_verts_in_1_manager
	//std::cout << arraylength_of_disp_verts_in_1_manager << '\t' << total_sample_verts_1_manager;

	unsigned int one_curve_VAO, one_curve_VBO;
	glGenVertexArrays(1, &one_curve_VAO);
	glGenBuffers(1, &one_curve_VBO);

	unsigned int curve_instance_VBO;
	glGenBuffers(1, &curve_instance_VBO);

	glBindVertexArray(one_curve_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, one_curve_VBO);
	glBufferData(GL_ARRAY_BUFFER, (4 * total_sample_verts_1_manager) * sizeof(float), &all_disp_verts_in_1_manager[0],
	             GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// ��1��curve_manager��Ϊһ��ʵ��
	glBindBuffer(GL_ARRAY_BUFFER, curve_instance_VBO);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &trans_mats[0], GL_STATIC_DRAW); // ����任����

	// ��glm::mat4���4��vec4����
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	std::cout << "amount = " << amount << "\narraylength_of_disp_verts_in_1_manager = " <<
		arraylength_of_disp_verts_in_1_manager << "\ntotal_sample_verts_1_manager = " << total_sample_verts_1_manager <<
		std::endl;

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput2(window);

		// render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// configure transformation matrices
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
		                                        static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f,
		                                        1000.0f);
		glm::mat4 view = camera.GetViewMatrix();

		int projectionLoc = glGetUniformLocation(rotate_shader.ID, "projection");
		int viewLoc = glGetUniformLocation(rotate_shader.ID, "view");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));

		rotate_shader.use();
		rotate_shader.setVec3("color", 1.0f, 1.0f, 1.0f);
		rotate_shader.setFloat("scale", 1.0f);

		glBindVertexArray(one_curve_VAO);
		glDrawArraysInstanced(GL_LINE_STRIP, 0, total_sample_verts_1_manager, amount);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwSetWindowShouldClose(window, false); // ���ùرմ��ڵ�״̬
	delete[] transformLoc;
	delete[] trans_mats;
	delete[] all_disp_verts_in_1_manager;
	delete[] rotate_curves;
	glfwTerminate();
}


void model_convert_to_mesh()
{
	//��ʼ��
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_REFRESH_RATE, 5);
	// glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	// glfw ��������
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW2 Bezier Curve Modeling", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return; // -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	// ���ûص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// ��׽���
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad ����OpenGL����ָ��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return; // -1;
	}

	// ����Ȳ���
	glEnable(GL_DEPTH_TEST);

	// ����shader
	Shader to_mesh_shader("./shaders/to_mesh.vert", "./shaders/to_mesh.frag");


	int amount = 36; // �ܹ�Ҫ�ж��ٸ�curve_manager�������ߣ�����Ӱ��delta_degree
	float delta_degree = 360 / static_cast<float>(amount); // ����ĽǶ�
	std::cout << "delta_degree = " << delta_degree << '\n';
	auto rotate_curves = new bezier_manager[amount];

	auto trans_mats = new glm::mat4[amount];
	//unsigned int* transformLoc = new unsigned int[amount];

	for (int i = 0; i < amount; ++i) // ���10��
	{
		trans_mats[i] = glm::mat4(1.0f);
		trans_mats[i] = rotate(trans_mats[i], glm::radians(static_cast<float>(i) * delta_degree),
		                       glm::vec3(0.0, 1.0, 0.0));
		//trans_mats[i] = glm::rotate(trans_mats[i], 2 * i * M_PI / amount, glm::vec3(0.0, 1.0, 0.0));
		//transformLoc[i] = glGetUniformLocation(to_mesh_shader.ID, "transform");
	}


	// 1��manager�µĲ�������
	int total_sample_verts_1_manager = 0;
	for (int i = 0; i < curves_manager.num_of_curves; ++i) // һ��curve_manager�����в����������
	{
		total_sample_verts_1_manager += (curves_manager.curves[i].sample_rate + 1);
	} // ע��total_sample_verts_1_managerû�г�4

	// ����1��curve_manager�е�����disp_verts
	auto all_disp_verts_in_1_manager = new float[4 * total_sample_verts_1_manager];
	int arraylength_of_disp_verts_in_1_manager = 0; // ��ʾ����һ�λ��������ж����float����ĳ���
	// �����ݿ�����all_disp_verts_in_1_manager��
	for (int i = 0; i < curves_manager.num_of_curves; ++i)
	{
		for (int j = 0; j < curves_manager.curves[i].sample_rate + 1; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				all_disp_verts_in_1_manager[arraylength_of_disp_verts_in_1_manager] = curves_manager.curves[i].
					disp_verts[4 * j + k];
				arraylength_of_disp_verts_in_1_manager += 1;
			}
		}
	} // ���ˣ�curve_manager�е�����disp_verts���ѿ���������all_disp_verts_in_1_manager�У������鳤��Ϊarraylength_of_disp_verts_in_1_manager
	//std::cout << arraylength_of_disp_verts_in_1_manager << '\t' << total_sample_verts_1_manager;


	// 3dģ���ܶ�����
	int total_sample_verts_3d = amount * total_sample_verts_1_manager;

	// ������άģ�����ж���Ĵ�����
	auto all_verts_3d = new float[4 * total_sample_verts_3d];
	int all_verts_3d_length = 0;
	// ��ÿ����ʾ�����任
	for (int j = 0; j < amount; ++j)
	{
		for (int i = 0; i < total_sample_verts_1_manager; ++i)
		{
			float x = all_disp_verts_in_1_manager[4 * i + 0];
			float y = all_disp_verts_in_1_manager[4 * i + 1];
			float z = all_disp_verts_in_1_manager[4 * i + 2];
			float w = all_disp_verts_in_1_manager[4 * i + 3];
			glm::vec4 coord(x, y, z, w);

			glm::vec4 result = trans_mats[j] * coord;
			all_verts_3d[all_verts_3d_length++] = result.x;
			all_verts_3d[all_verts_3d_length++] = result.y;
			all_verts_3d[all_verts_3d_length++] = result.z;
			all_verts_3d[all_verts_3d_length++] = result.w;
		}
	} // ���ˣ�3dģ�����ж�����������ڴ�����all_verts_3d�У�all_verts_3d_lengt��amount * arraylength_of_disp_verts_in_1_manager��Ϊ�䳤��
	std::cout << "\ntotal_sample_verts_3d = " << total_sample_verts_3d;
	std::cout << "\ntotal number of triangles = " << 2 * amount * (total_sample_verts_1_manager - 1);

	// --------������������Ƭ���˹�ϵ����--------
	// ����Ӧ����amount * (total_sample_verts_1_manager - 1)���ı��Σ�ÿ���ı����ֳַ�����С������
	// ��Ӧ��2 * amount * (total_sample_verts_1_manager - 1)��С������
	// (total_sample_verts_1_manager - 1)�൱��һ��curve_manager�Ĳ�����
	// �������Ƶ�������Ҫ����ÿ������������Щ��������
	//
	// ��i����0��ʼ���������Σ�
	//
	// iż��ʱ�Ķ���Ϊ�ڣ�
	// i/(2*(total_sample_verts_1_manager - 1))*total_sample_verts_1_manager + i%(2*(total_sample_verts_1_manager - 1))/2
	// i/(2*(total_sample_verts_1_manager - 1))*total_sample_verts_1_manager + i%(2*(total_sample_verts_1_manager - 1))/2 + 1
	// i/(2*(total_sample_verts_1_manager - 1))*total_sample_verts_1_manager + i%(2*(total_sample_verts_1_manager - 1))/2 + total_sample_verts_1_manager
	// ������
	//
	// i����ʱ�Ķ���Ϊ�ڣ�
	// i/(2*(total_sample_verts_1_manager - 1))*total_sample_verts_1_manager + i%(2*(total_sample_verts_1_manager - 1))/2 + 1
	// i/(2*(total_sample_verts_1_manager - 1))*total_sample_verts_1_manager + i%(2*(total_sample_verts_1_manager - 1))/2 + total_sample_verts_1_manager
	// i/(2*(total_sample_verts_1_manager - 1))*total_sample_verts_1_manager + i%(2*(total_sample_verts_1_manager - 1))/2 + total_sample_verts_1_manager + 1
	// ������
	// **ע���û�г�4���һ���Ҫ���ܶ�����ȡ��**

	// �����������붥�����˹�ϵ����������
	auto indices = new unsigned int[3 * 2 * amount * (total_sample_verts_1_manager - 1)];
	for (int i = 0; i < 2 * amount * (total_sample_verts_1_manager - 1); ++i)
	{
		if (i % 2 == 0) // ż����ע��洢˳��Ҫ��OBJ�ļ�ƥ��
		{
			indices[i * 3 + 0] = (i / (2 * (total_sample_verts_1_manager - 1)) * total_sample_verts_1_manager + i % (2 *
				(total_sample_verts_1_manager - 1)) / 2) % total_sample_verts_3d;
			indices[i * 3 + 2] = (i / (2 * (total_sample_verts_1_manager - 1)) * total_sample_verts_1_manager + i % (2 *
				(total_sample_verts_1_manager - 1)) / 2 + 1) % total_sample_verts_3d;
			indices[i * 3 + 1] = (i / (2 * (total_sample_verts_1_manager - 1)) * total_sample_verts_1_manager + i % (2 *
				(total_sample_verts_1_manager - 1)) / 2 + total_sample_verts_1_manager) % total_sample_verts_3d;
		}
		if (i % 2 == 1) // ����
		{
			indices[i * 3 + 0] = (i / (2 * (total_sample_verts_1_manager - 1)) * total_sample_verts_1_manager + i % (2 *
				(total_sample_verts_1_manager - 1)) / 2 + 1) % total_sample_verts_3d;
			indices[i * 3 + 1] = (i / (2 * (total_sample_verts_1_manager - 1)) * total_sample_verts_1_manager + i % (2 *
				(total_sample_verts_1_manager - 1)) / 2 + total_sample_verts_1_manager) % total_sample_verts_3d;
			indices[i * 3 + 2] = (i / (2 * (total_sample_verts_1_manager - 1)) * total_sample_verts_1_manager + i % (2 *
				(total_sample_verts_1_manager - 1)) / 2 + total_sample_verts_1_manager + 1) % total_sample_verts_3d;
		}
	}


	// ���㶥�㷨ʸ��
	// ���㷨ʸ��������ֵ��Ϊ���㣬ֱ������Ϊ�������ζ�����������һһ��Ӧ
	auto vn_indices = new unsigned int[3 * 2 * amount * (total_sample_verts_1_manager - 1)];

	// ���涥�㷨ʸ�������㷨ʸ�����ܸ���ӦΪ������Ƭ����������ÿ����ʸ������3��float����
	auto vn = new glm::vec3[3 * 2 * amount * (total_sample_verts_1_manager - 1)];

	// ���帨�����飬��glm::vec3��ʽ���涥�����꣬˳�򲻱䣬����w����
	auto all_verts_3d_vec3 = new glm::vec3[total_sample_verts_3d];
	for (int i = 0; i < total_sample_verts_3d; ++i)
	{
		all_verts_3d_vec3[i].x = all_verts_3d[4 * i + 0];
		all_verts_3d_vec3[i].y = all_verts_3d[4 * i + 1];
		all_verts_3d_vec3[i].z = all_verts_3d[4 * i + 2];
	}

	// ������ά��ʸ��
	glm::vec3 zeroVec3 = glm::vec3(0.0f, 0.0f, 0.0f);
	// ���㶥�㷨ʸ��
	for (int i = 0; i < 2 * amount * (total_sample_verts_1_manager - 1); ++i)
	{
		glm::vec3 delta_1 = all_verts_3d_vec3[indices[3 * i + 1]] - all_verts_3d_vec3[indices[3 * i + 0]];
		glm::vec3 delta_2 = all_verts_3d_vec3[indices[3 * i + 2]] - all_verts_3d_vec3[indices[3 * i + 1]];
		glm::vec3 cross = glm::cross(delta_1, delta_2);
		if (cross.x < 1e-6 && cross.y < 1e-6 && cross.z < 1e-6)
		{
			// ���ڵ��غ�
			vn[3 * i + 0] = glm::vec3(1.0f, 0.0f, 0.0f);
			vn[3 * i + 1] = glm::vec3(1.0f, 0.0f, 0.0f);
			vn[3 * i + 2] = glm::vec3(1.0f, 0.0f, 0.0f);
		}
		else
		{
			// ��ˡ���׼�����õ������淨���������ڴ˴�ϣ������ģ�͵���ǣ�ÿ��������ÿ���������ڵķ�������������Ƭ����ͬ
			vn[3 * i + 0] = glm::normalize(cross);
			vn[3 * i + 1] = glm::normalize(cross);
			vn[3 * i + 2] = glm::normalize(cross);
		}
	}

	// ������Ҫ��������
	delete[] all_verts_3d_vec3;

	std::cout << "\ntotal number of vn = " << 3 * 2 * amount * (total_sample_verts_1_manager - 1) << std::endl;

	// ����VAO��VBO��EBO
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * total_sample_verts_3d * sizeof(&all_verts_3d[0]), all_verts_3d, GL_DYNAMIC_DRAW);
	std::cout << "all_verts_3d:\n" << "size = " << sizeof(all_verts_3d) << std::endl;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * 2 * amount * (total_sample_verts_1_manager - 1) * sizeof(&indices[0]),
	             indices, GL_DYNAMIC_DRAW);
	std::cout << "indices:\n" << "size = " << sizeof(indices) << std::endl;
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	std::cout << "\namount = " << amount << "\narraylength_of_disp_verts_in_1_manager = " <<
		arraylength_of_disp_verts_in_1_manager << "\ntotal_sample_verts_1_manager = " << total_sample_verts_1_manager <<
		std::endl;


	// ����Dear ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	// ���÷��
	ImGui::StyleColorsDark();
	// ����ƽ̨
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// �Ƿ���Ҫ����Ⱦѭ�������¼��㶥��λ�ú����˹�ϵ
	bool changed = false;
	// �Ƿ�����߿�ģʽ����
	bool polygon_line = true;
	// �Ƿ��ֶ��ı������
	bool manually_change_sample_rate = false;

	// ��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		if (use_mouse_to_click == true)
		{
			// ȡ�����ص�����
			glfwSetCursorPosCallback(window, nullptr);
			// ȡ����׽���
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			// ���¼������ص�����
			glfwSetCursorPosCallback(window, mouse_callback);
			// ���²�׽���
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput2(window);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// �Ӿ��任����
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
		                                        static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f,
		                                        1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		int projectionLoc = glGetUniformLocation(to_mesh_shader.ID, "projection");
		int viewLoc = glGetUniformLocation(to_mesh_shader.ID, "view");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));

		// �����ģ�͵ľ���
		float dist_to_model = sqrt(
			camera.Position.x * camera.Position.x + camera.Position.y * camera.Position.y + camera.Position.z * camera.
			Position.z);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ImGui ��������
		ImGui::SetWindowPos(ImVec2(2, 2));
		ImGui::SetWindowSize(ImVec2(600, 400));
		ImGui::Begin("Control Panel");
		ImGui::Text("Camera Pos = (%g, %g, %g)", camera.Position.x, camera.Position.y, camera.Position.z);
		ImGui::Text("Dist to Model = %g", dist_to_model);

		ImGui::Text("Manage the Sample Accuracy Here.");

		// �����ֶ��趨�Ƿ�����߿�ģʽ
		ImGui::Checkbox("PolygonMode = LINE?", &polygon_line);
		if (polygon_line)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// �����ֶ��ı������
		ImGui::Checkbox("Manually Change Sample Rate?", &manually_change_sample_rate);

		int sample_rate_for_change = curves_manager.curves[0].sample_rate;
		int old_sample_rate = sample_rate_for_change;
		int old_amount = amount;

		if (manually_change_sample_rate) // �ֶ��޸�ģ�Ͳ�������
		{
			ImGui::InputInt("Sample Rate of Bezier curve", &sample_rate_for_change);
			ImGui::InputInt("Sample Rate of Scanning", &amount);
			// ��֤�����ʺϷ��Ҳ�̫��
			if (sample_rate_for_change < 4)
				sample_rate_for_change = 4;
			if (sample_rate_for_change > 60)
				sample_rate_for_change = 60;
			if (amount < 4)
				amount = 4;
			if (amount > 90)
				amount = 90;
		}
		else // �Զ����ݾ������ģ�;���
		{
			if (dist_to_model < 1.5f) dist_to_model = 1.5f;
			sample_rate_for_change = 100 / static_cast<int>(dist_to_model);
			amount = 180 / static_cast<int>(dist_to_model);
			// ��֤�����ʺϷ��Ҳ�̫��
			if (sample_rate_for_change < 4)
				sample_rate_for_change = 4;
			if (sample_rate_for_change > 60)
				sample_rate_for_change = 60;
			if (amount < 4)
				amount = 4;
			if (amount > 90)
				amount = 90;
			ImGui::Text("Sample Rate of Bezier curve = %d", sample_rate_for_change);
			ImGui::Text("Sample Rate of Scanning = %d", amount);
		}


		// �����ı�
		if (old_sample_rate != sample_rate_for_change || old_amount != amount)
		{
			changed = true;
			if (old_sample_rate != sample_rate_for_change)
			{
				for (int i = 0; i < curves_manager.num_of_curves; ++i)
				{
					curves_manager.curves[i].sample_rate = sample_rate_for_change;
					curves_manager.curves[i].update();
				}
			}
		}


		// �ֶ����²�������
		if (changed == true)
		{
			delta_degree = 360 / static_cast<float>(amount); // ����ĽǶ�
			std::cout << "delta_degree = " << delta_degree << '\n';

			delete[] rotate_curves;
			rotate_curves = new bezier_manager[amount];

			delete[] trans_mats;
			trans_mats = new glm::mat4[amount];

			//delete[] transformLoc;
			//transformLoc = new unsigned int[amount];

			for (int i = 0; i < amount; ++i) // ���10��
			{
				trans_mats[i] = glm::mat4(1.0f);
				trans_mats[i] = rotate(trans_mats[i], glm::radians(static_cast<float>(i) * delta_degree),
				                       glm::vec3(0.0, 1.0, 0.0));
				//trans_mats[i] = glm::rotate(trans_mats[i], 2 * i * M_PI / amount, glm::vec3(0.0, 1.0, 0.0));
				//transformLoc[i] = glGetUniformLocation(to_mesh_shader.ID, "transform");
			}


			// 1��manager�µĲ�������
			total_sample_verts_1_manager = 0;
			for (int i = 0; i < curves_manager.num_of_curves; ++i) // һ��curve_manager�����в����������
			{
				total_sample_verts_1_manager += (curves_manager.curves[i].sample_rate + 1);
			} // ע��total_sample_verts_1_managerû�г�4

			// ����1��curve_manager�е�����disp_verts
			delete[] all_disp_verts_in_1_manager;
			all_disp_verts_in_1_manager = new float[4 * total_sample_verts_1_manager];

			arraylength_of_disp_verts_in_1_manager = 0; // ��ʾ����һ�λ��������ж����float����ĳ���
			// �����ݿ�����all_disp_verts_in_1_manager��
			for (int i = 0; i < curves_manager.num_of_curves; ++i)
			{
				for (int j = 0; j < curves_manager.curves[i].sample_rate + 1; ++j)
				{
					for (int k = 0; k < 4; ++k)
					{
						all_disp_verts_in_1_manager[arraylength_of_disp_verts_in_1_manager] = curves_manager.curves[i].
							disp_verts[4 * j + k];
						arraylength_of_disp_verts_in_1_manager += 1;
					}
				}
			} // ���ˣ�curve_manager�е�����disp_verts���ѿ���������all_disp_verts_in_1_manager�У������鳤��Ϊarraylength_of_disp_verts_in_1_manager
			//std::cout << arraylength_of_disp_verts_in_1_manager << '\t' << total_sample_verts_1_manager;


			// 3dģ���ܶ�����
			total_sample_verts_3d = amount * total_sample_verts_1_manager;

			// ������άģ�����ж���Ĵ�����
			delete[] all_verts_3d;
			all_verts_3d = new float[4 * total_sample_verts_3d];
			all_verts_3d_length = 0;
			// ��ÿ����ʾ�����任
			for (int j = 0; j < amount; ++j)
			{
				for (int i = 0; i < total_sample_verts_1_manager; ++i)
				{
					float x = all_disp_verts_in_1_manager[4 * i + 0];
					float y = all_disp_verts_in_1_manager[4 * i + 1];
					float z = all_disp_verts_in_1_manager[4 * i + 2];
					float w = all_disp_verts_in_1_manager[4 * i + 3];
					glm::vec4 coord(x, y, z, w);

					glm::vec4 result = trans_mats[j] * coord;
					all_verts_3d[all_verts_3d_length++] = result.x;
					all_verts_3d[all_verts_3d_length++] = result.y;
					all_verts_3d[all_verts_3d_length++] = result.z;
					all_verts_3d[all_verts_3d_length++] = result.w;
				}
			} // ���ˣ�3dģ�����ж�����������ڴ�����all_verts_3d�У�all_verts_3d_lengt��amount * arraylength_of_disp_verts_in_1_manager��Ϊ�䳤��
			std::cout << "\ntotal number of verts = " << total_sample_verts_3d;
			std::cout << "\ntotal number of triangles = " << 2 * amount * (total_sample_verts_1_manager - 1);

			// �����������붥�����˹�ϵ����������
			delete[] indices;
			indices = new unsigned int[3 * 2 * amount * (total_sample_verts_1_manager - 1)];
			for (int i = 0; i < 2 * amount * (total_sample_verts_1_manager - 1); ++i)
			{
				if (i % 2 == 0) // ͬ����Ҫע��˳��Ҫ����OBJ�ļ���Ҫ��
				{
					indices[i * 3 + 0] = (i / (2 * (total_sample_verts_1_manager - 1)) * total_sample_verts_1_manager +
						i % (2 * (total_sample_verts_1_manager - 1)) / 2) % total_sample_verts_3d;
					indices[i * 3 + 2] = (i / (2 * (total_sample_verts_1_manager - 1)) * total_sample_verts_1_manager +
						i % (2 * (total_sample_verts_1_manager - 1)) / 2 + 1) % total_sample_verts_3d;
					indices[i * 3 + 1] = (i / (2 * (total_sample_verts_1_manager - 1)) * total_sample_verts_1_manager +
							i % (2 * (total_sample_verts_1_manager - 1)) / 2 + total_sample_verts_1_manager) %
						total_sample_verts_3d;
				}
				if (i % 2 == 1)
				{
					indices[i * 3 + 0] = (i / (2 * (total_sample_verts_1_manager - 1)) * total_sample_verts_1_manager +
						i % (2 * (total_sample_verts_1_manager - 1)) / 2 + 1) % total_sample_verts_3d;
					indices[i * 3 + 1] = (i / (2 * (total_sample_verts_1_manager - 1)) * total_sample_verts_1_manager +
							i % (2 * (total_sample_verts_1_manager - 1)) / 2 + total_sample_verts_1_manager) %
						total_sample_verts_3d;
					indices[i * 3 + 2] = (i / (2 * (total_sample_verts_1_manager - 1)) * total_sample_verts_1_manager +
							i % (2 * (total_sample_verts_1_manager - 1)) / 2 + total_sample_verts_1_manager + 1) %
						total_sample_verts_3d;
				}
			}


			// ���¶��㷨ʸ��
			delete[] vn_indices;
			vn_indices = new unsigned int[3 * 2 * amount * (total_sample_verts_1_manager - 1)];
			delete[] vn;
			vn = new glm::vec3[3 * 2 * amount * (total_sample_verts_1_manager - 1)];
			//delete[] all_verts_3d_vec3; // ��Ϊ֮ǰ�����ֱ��delete�ˣ���Ӧ�ظ�delete
			all_verts_3d_vec3 = new glm::vec3[total_sample_verts_3d];
			for (int i = 0; i < total_sample_verts_3d; ++i)
			{
				all_verts_3d_vec3[i].x = all_verts_3d[4 * i + 0];
				all_verts_3d_vec3[i].y = all_verts_3d[4 * i + 1];
				all_verts_3d_vec3[i].z = all_verts_3d[4 * i + 2];
			}
			for (int i = 0; i < 2 * amount * (total_sample_verts_1_manager - 1); ++i)
			{
				glm::vec3 delta_1 = all_verts_3d_vec3[indices[3 * i + 1]] - all_verts_3d_vec3[indices[3 * i + 0]];
				glm::vec3 delta_2 = all_verts_3d_vec3[indices[3 * i + 2]] - all_verts_3d_vec3[indices[3 * i + 1]];

				glm::vec3 cross = glm::cross(delta_1, delta_2);

				if (cross.x < 1e-6 && cross.y < 1e-6 && cross.z < 1e-6)
				{
					// ���ڵ��غ�
					vn[3 * i + 0] = glm::vec3(1.0f, 0.0f, 0.0f);
					vn[3 * i + 1] = glm::vec3(1.0f, 0.0f, 0.0f);
					vn[3 * i + 2] = glm::vec3(1.0f, 0.0f, 0.0f);
				}
				else
				{
					// ��ˡ���׼�����õ������淨���������ڴ˴�ϣ������ģ�͵���ǣ�ÿ��������ÿ���������ڵķ�������������Ƭ����ͬ
					vn[3 * i + 0] = glm::normalize(cross);
					vn[3 * i + 1] = glm::normalize(cross);
					vn[3 * i + 2] = glm::normalize(cross);
				}
			}
			delete[] all_verts_3d_vec3;
			std::cout << "\ntotal number of vn = " << 3 * 2 * amount * (total_sample_verts_1_manager - 1) << std::endl;


			// ����״̬
			changed = false;
		}

		// ����VAO��VBO��EBO
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 4 * total_sample_verts_3d * sizeof(float), all_verts_3d, GL_DYNAMIC_DRAW);
		//std::cout << "all_verts_3d:\n" << "size = " << sizeof(all_verts_3d) << std::endl;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER, 3 * 2 * amount * (total_sample_verts_1_manager - 1) * sizeof(unsigned int),
			indices, GL_DYNAMIC_DRAW);
		//std::cout << "indices:\n" << "size = " << sizeof(indices) << std::endl;
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		to_mesh_shader.use();
		to_mesh_shader.setVec3("color", 1.0f, 1.0f, 1.0f);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 3 * 2 * amount * (total_sample_verts_1_manager - 1), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// ��indices������д�������
	std::fstream IndicesDataFile("IndicesData.txt", std::ios::out); // �����
	std::ofstream IndicesDataOutput;
	IndicesDataOutput.open("IndicesData.txt", std::ofstream::app);
	IndicesDataOutput << "*Indices\n";
	for (int i = 0; i < 2 * amount * (total_sample_verts_1_manager - 1); ++i)
	{
		IndicesDataOutput << indices[3 * i + 0] << '\t';
		IndicesDataOutput << indices[3 * i + 1] << '\t';
		IndicesDataOutput << indices[3 * i + 2] << '\n';
	}
	//IndicesDataFile.close();

	// ������д�������
	std::fstream VertsData3dFile("VertsData3d.txt", std::ios::out); // �����
	std::ofstream VertsData3dOutput;
	VertsData3dOutput.open("VertsData3d.txt", std::ofstream::app);
	VertsData3dOutput << "*Verts\n";
	for (int i = 0; i < total_sample_verts_3d; ++i)
	{
		VertsData3dOutput << all_verts_3d[4 * i + 0] << '\t';
		VertsData3dOutput << all_verts_3d[4 * i + 1] << '\t';
		VertsData3dOutput << all_verts_3d[4 * i + 2] << '\t';
		VertsData3dOutput << all_verts_3d[4 * i + 3] << '\n';
	}

	// д��obj�ļ�
	save_as_obj(total_sample_verts_3d, all_verts_3d, 3 * 2 * amount * (total_sample_verts_1_manager - 1), vn, 2 * amount * (total_sample_verts_1_manager - 1), indices);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwSetWindowShouldClose(window, false); // ���ùرմ��ڵ�״̬
	//delete[] transformLoc;
	delete[] trans_mats;
	delete[] all_disp_verts_in_1_manager;
	delete[] rotate_curves;
	delete[] all_verts_3d;
	delete[] indices;
	delete[] vn_indices;
	delete[] vn;
	glfwTerminate();
}


//bool readOBJ()
//{
//	std::vector<int> vertexIndices, uvIndices, normalIndices;
//	std::vector<float> temp_vertices;
//	std::vector<float> temp_uvs;
//	std::vector<float> temp_normals;
//
//	FILE* file = fopen("modelFile.obj", "r");
//	if (file == NULL) {
//		printf("Impossible to open the file !n");
//		return false;
//	}
//
//	while (true)
//	{
//		char lineHeader[128];
//		// read the first word of the line
//		int res = fscanf(file, "%s", lineHeader);
//		if (res == EOF)
//			break; // EOF = End Of File. Quit the loop.
//	
//
//	if (strcmp(lineHeader, "v") == 0)
//	{
//		glm::vec3 vertex;
//		fscanf(file, "%f %f %fn", &vertex.x, &vertex.y, &vertex.z);
//		temp_vertices.push_back(vertex);
//	}
//	else if (strcmp(lineHeader, "vt") == 0)
//	{
//		glm::vec2 uv;
//		fscanf(file, "%f %fn", &uv.x, &uv.y);
//		temp_uvs.push_back(uv);
//	}
//	else if (strcmp(lineHeader, "vn") == 0)
//	{
//		glm::vec3 normal;
//		fscanf(file, "%f %f %fn", &normal.x, &normal.y, &normal.z);
//		temp_normals.push_back(normal);
//	}
//	else if (strcmp(lineHeader, "f") == 0)
//	{
//		std::string vertex1, vertex2, vertex3;
//		unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
//		int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%dn", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
//		if (matches != 9)
//		{
//			printf("File can't be read by our simple parser : ( Try exporting with other optionsn");
//			return false;
//		}
//		vertexIndices.push_back(vertexIndex[0]);
//		vertexIndices.push_back(vertexIndex[1]);
//		vertexIndices.push_back(vertexIndex[2]);
//		uvIndices.push_back(uvIndex[0]);
//		uvIndices.push_back(uvIndex[1]);
//		uvIndices.push_back(uvIndex[2]);
//		normalIndices.push_back(normalIndex[0]);
//		normalIndices.push_back(normalIndex[1]);
//		normalIndices.push_back(normalIndex[2]);
//
//		// For each vertex of each triangle
//		for (unsigned int i = 0; iunsigned int vertexIndex = vertexIndices[i];
//}
//}


int main()
{
	// ��1�׶Σ�������ߣ�ʵ����������߹��̵Ŀ��ӻ��ͽ��������������ȫ�ֱ���curve_manager��
	std::cout << "\n1. Enter SampleRate and Control Vertexes\n";
	modeling_addCurve();
	//Sleep(500);

	// ��2�׶Σ�������ת�壬Ԥ��ģ��
	std::cout << "\n2. Preview Rotate Scan\n";
	modeling_rotate_scan();
	//Sleep(500);

	// ��3�׶Σ���������
	std::cout << "\n3. Preview Mesh Model\n";
	model_convert_to_mesh();

	return 0;
}
