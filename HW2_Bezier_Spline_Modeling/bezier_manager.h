#pragma once
#ifndef BEZIER_MANAGER_H
#define BEZIER_MANAGER_H

#include "bezier.h"

class bezier_manager
{
public:
	int num_of_curves;
	bezier_line_4_vertex* curves;

	// ���캯������ʼΪ0
	bezier_manager(int NUM_OF_CURVES = 0);

	// �������캯��
	bezier_manager(const bezier_manager& a);

	// ����
	~bezier_manager();

	// ���һ�����ߣ�����Ϊ���ƶ���Ͳ����ʣ�Ĭ��100��
	void add_curve(float verts[16], int sample_rate = 100);

	// ���һ�����ߣ�����Ϊ������
	void add_curve(bezier_line_4_vertex curve);

	// ɾ���������ߣ��ָ����չ����
	void delete_all_curves();

	// �༭��������
	void edit_curve(int curve_index, int ctrl_vert_index, float new_x, float new_y, float new_z, float new_w = 1);

	// ɾ���ض�����
	void delete_curve(int curve_index);

	// ����OpenGL
	void updategl(unsigned int cvao[10], unsigned int cvbo[10], unsigned int pvao[10], unsigned int pvbo[10], unsigned int ivbo);

};

#endif
