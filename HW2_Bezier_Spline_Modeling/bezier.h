#pragma once
#ifndef BEZIER_H
#define BEZIER_H
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

// 用于生成三次贝塞尔曲线（4个控制顶点）
// 包括三项基本数据：控制顶点、采样率、显示顶点
// 人为给定控制顶点和采样率
// 调用该类的相关函数生成采样点坐标，传输给着色器
class bezier_line_4_vertex
{
public:
	int sample_rate; // 曲线的段数
	float ctrl_verts[16]; // 4个控制顶点，每个顶点4个坐标
	float* disp_verts; // 用于显示的顶点，包括一首一尾的两个控制顶点一共有sample_rate+1个

	// 初始默认采样率100，各顶点各坐标均为0.0f
	//bezier_line_4_vertex(int SAMPLE_RATE = 100, float CTRL_VERTS = 0.0f, float DISP_VERTS = 0.0f);
	bezier_line_4_vertex(int SAMPLE_RATE = 100);

	// 拷贝构造函数
	bezier_line_4_vertex(const bezier_line_4_vertex&);

	// 空间释放
	void clear_and_delete();

	~bezier_line_4_vertex();

	// 仅更新用于显示的点的坐标
	void update();

	// 传入顶点坐标，必须是16维float数组，更新ctrl_verts，然后更新disp_verts
	void update_ctrl_verts(float verts[16]);

	// 计算采样点的坐标
	void calculate_curve();

	void updategl(unsigned int cvao, unsigned int cvbo, unsigned int pvao, unsigned int pvbo, unsigned int ivbo);

	// 等号重载
	bezier_line_4_vertex& operator=(const bezier_line_4_vertex& a);

private:
	// 控制顶点用小正方形显示
	const float quad[12] = {-1, -1, 1, -1, -1, 1, 1, 1, -1, 1, 1, -1};

	// 计算按t分比点的坐标
	float* get_t_point(float p1[4], float p2[4], float t);
};

#endif
