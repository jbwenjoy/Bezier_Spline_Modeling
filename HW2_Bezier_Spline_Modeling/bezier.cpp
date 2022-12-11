# include "bezier.h"
# include <iostream>
# include <glad/glad.h>

//bezier_line_4_vertex::bezier_line_4_vertex(int SAMPLE_RATE, float CTRL_VERTS, float DISP_VERTS)
//{
//	sample_rate = SAMPLE_RATE;
//	for (int i : ctrl_verts)
//	{
//		ctrl_verts[i] = CTRL_VERTS;
//	}
//	disp_verts = new float[(sample_rate + 1) * 4];
//	for (int i = 0; i < (sample_rate + 1) * 4; ++i)
//	{
//		disp_verts[i] = DISP_VERTS;
//	}
//}

bezier_line_4_vertex::bezier_line_4_vertex(int SAMPLE_RATE)
{
	sample_rate = SAMPLE_RATE;
	disp_verts = new float[(sample_rate + 1) * 4];
}


bezier_line_4_vertex::bezier_line_4_vertex(const bezier_line_4_vertex& a)
{
	sample_rate = a.sample_rate;
	for (int i = 0; i < 16; ++i)
	{
		ctrl_verts[i] = a.ctrl_verts[i];
	}
	disp_verts = new float[(sample_rate + 1) * 4];
	for (int i = 0; i < (sample_rate + 1) * 4; ++i)
	{
		disp_verts[i] = a.disp_verts[i];
	}
}


void bezier_line_4_vertex::clear_and_delete()
{
	delete[] disp_verts;
}

bezier_line_4_vertex::~bezier_line_4_vertex()
{
	delete[] disp_verts;
}


void bezier_line_4_vertex::update()
{
	delete[] disp_verts;
	disp_verts = new float[4 * (sample_rate + 1)];
	calculate_curve();
}


void bezier_line_4_vertex::update_ctrl_verts(float verts[16])
{
	for (int i = 0; i < 16; ++i)
	{
		ctrl_verts[i] = verts[i];
	}
	update();
	std::cout << "\nCtrl Verts Updated\n";
}


float* bezier_line_4_vertex::get_t_point(float p1[4], float p2[4], float t)
{
	if (t < 0) t = 0;
	if (t > 1) t = 1;

	float* result = new float[4];
	result[0] = p1[0] * t + p2[0] * (1 - t);
	result[1] = p1[1] * t + p2[1] * (1 - t);
	result[2] = p1[2] * t + p2[2] * (1 - t);
	result[3] = p1[3] * t + p2[3] * (1 - t);

	return result;
}


void bezier_line_4_vertex::calculate_curve()
{
	// 第0层，控制顶点
	float ctrl_0v0[4];
	float ctrl_0v1[4];
	float ctrl_0v2[4];
	float ctrl_0v3[4];
	for (int i = 0; i < 4; ++i)
	{
		ctrl_0v0[i] = ctrl_verts[i];
		ctrl_0v1[i] = ctrl_verts[i + 4];
		ctrl_0v2[i] = ctrl_verts[i + 8];
		ctrl_0v3[i] = ctrl_verts[i + 12];
	}
	// 第1层
	float ctrl_1v0[4];
	float ctrl_1v1[4];
	float ctrl_1v2[4];
	// 第2层
	float ctrl_2v0[4];
	float ctrl_2v1[4];
	// 第3层
	float ctrl_3v0[4];

	for (int i = 0; i <= sample_rate; ++i)
	{
		float t = 1 - static_cast<float>(i) / sample_rate;

		// 第一层
		auto result = get_t_point(ctrl_0v0, ctrl_0v1, t);
		for (int j = 0; j < 4; ++j)
		{
			ctrl_1v0[j] = result[j];
		}
		result = get_t_point(ctrl_0v1, ctrl_0v2, t);
		for (int j = 0; j < 4; ++j)
		{
			ctrl_1v1[j] = result[j];
		}
		result = get_t_point(ctrl_0v2, ctrl_0v3, t);
		for (int j = 0; j < 4; ++j)
		{
			ctrl_1v2[j] = result[j];
		}

		// 第二层
		result = get_t_point(ctrl_1v0, ctrl_1v1, t);
		for (int j = 0; j < 4; ++j)
		{
			ctrl_2v0[j] = result[j];
		}
		result = get_t_point(ctrl_1v1, ctrl_1v2, t);
		for (int j = 0; j < 4; ++j)
		{
			ctrl_2v1[j] = result[j];
		}

		// 第三层，最终的动点
		result = get_t_point(ctrl_2v0, ctrl_2v1, t);
		for (int j = 0; j < 4; ++j)
		{
			ctrl_3v0[j] = result[j];
		}

		// 写入保存用于显示的顶点的数组
		disp_verts[i * 4] = ctrl_3v0[0];
		disp_verts[i * 4 + 1] = ctrl_3v0[1];
		disp_verts[i * 4 + 2] = ctrl_3v0[2];
		disp_verts[i * 4 + 3] = ctrl_3v0[3];
	}
}


void bezier_line_4_vertex::updategl(unsigned int curveVAO, unsigned int curveVBO, unsigned int pointVAO, unsigned int pointVBO, unsigned int instanceVBO)
{
	//std::cout << "updategl\n";
	// 曲线上的采样点
	glBindVertexArray(curveVAO);
	glBindBuffer(GL_ARRAY_BUFFER, curveVBO);
	glBufferData(GL_ARRAY_BUFFER, (sample_rate + 1) * 4 * sizeof(float), disp_verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// 控制点用小正方形显示
	glBindVertexArray(pointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float) * 2, quad, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// 控制顶点实例化绘制
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), ctrl_verts, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	
	glVertexAttribDivisor(1, 1);
}


bezier_line_4_vertex& bezier_line_4_vertex::operator=(const bezier_line_4_vertex& right)
{
	if (this == &right)
	{
		return *this;
	}

	delete[] disp_verts;

	sample_rate = right.sample_rate;
	for (int i = 0; i < 16; ++i)
	{
		ctrl_verts[i] = right.ctrl_verts[i];
	}
	disp_verts = new float[4 * (sample_rate + 1)];

	//std::cout << "\ncheck if disp_verts successfully assigned\n";
	if (!disp_verts) std::cout << "failed to assign memory for disp_verts\n";

	for (int i = 0; i < 4 * (sample_rate + 1); ++i)
	{
		disp_verts[i] = right.disp_verts[i];
	}

	return *this;
}
