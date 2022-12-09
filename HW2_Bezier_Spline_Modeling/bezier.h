#pragma once
#ifndef BEZIER_H
#define BEZIER_H
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

// �����������α��������ߣ�4�����ƶ��㣩
// ��������������ݣ����ƶ��㡢�����ʡ���ʾ����
// ��Ϊ�������ƶ���Ͳ�����
// ���ø������غ������ɲ��������꣬�������ɫ��
class bezier_line_4_vertex
{
public:
	int sample_rate; // ���ߵĶ���
	float ctrl_verts[16]; // 4�����ƶ��㣬ÿ������4������
	float* disp_verts; // ������ʾ�Ķ��㣬����һ��һβ���������ƶ���һ����sample_rate+1��

	// ��ʼĬ�ϲ�����100��������������Ϊ0.0f
	//bezier_line_4_vertex(int SAMPLE_RATE = 100, float CTRL_VERTS = 0.0f, float DISP_VERTS = 0.0f);
	bezier_line_4_vertex(int SAMPLE_RATE = 100);

	// �������캯��
	bezier_line_4_vertex(const bezier_line_4_vertex&);

	// �ռ��ͷ�
	void clear_and_delete();

	~bezier_line_4_vertex();

	// ������������ʾ�ĵ������
	void update();

	// ���붥�����꣬������16άfloat���飬����ctrl_verts��Ȼ�����disp_verts
	void update_ctrl_verts(float verts[16]);

	// ��������������
	void calculate_curve();

	void updategl(unsigned int cvao, unsigned int cvbo, unsigned int pvao, unsigned int pvbo, unsigned int ivbo);

	// �Ⱥ�����
	bezier_line_4_vertex& operator=(const bezier_line_4_vertex& a);

private:
	// ���ƶ�����С��������ʾ
	const float quad[12] = {-1, -1, 1, -1, -1, 1, 1, 1, -1, 1, 1, -1};

	// ���㰴t�ֱȵ������
	float* get_t_point(float p1[4], float p2[4], float t);
};

#endif
