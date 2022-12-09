#pragma once
#ifndef BEZIER_MANAGER_H
#define BEZIER_MANAGER_H

#include "bezier.h"

class bezier_manager
{
public:
	int num_of_curves;
	bezier_line_4_vertex* curves;

	// 构造函数，初始为0
	bezier_manager(int NUM_OF_CURVES = 0);

	// 拷贝构造函数
	bezier_manager(const bezier_manager& a);

	// 析构
	~bezier_manager();

	// 添加一条曲线，参数为控制顶点和采样率（默认100）
	void add_curve(float verts[16], int sample_rate = 100);

	// 添加一条曲线，参数为曲线类
	void add_curve(bezier_line_4_vertex curve);

	// 删除所有曲线，恢复到刚构造好
	void delete_all_curves();

	// 编辑已有曲线
	void edit_curve(int curve_index, int ctrl_vert_index, float new_x, float new_y, float new_z, float new_w = 1);

	// 删除特定曲线
	void delete_curve(int curve_index);

	// 更新OpenGL
	void updategl(unsigned int cvao[10], unsigned int cvbo[10], unsigned int pvao[10], unsigned int pvbo[10], unsigned int ivbo);

};

#endif
