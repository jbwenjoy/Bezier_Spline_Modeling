#include "bezier.h"
#include "bezier_manager.h"
#include <iostream>
#include <cstddef>
# include <glad/glad.h>

bezier_manager::bezier_manager(int NUM_OF_CURVES)
{
	num_of_curves = NUM_OF_CURVES;
	curves = nullptr;
}

bezier_manager::bezier_manager(const bezier_manager& a)
{
	num_of_curves = a.num_of_curves;
	curves = new bezier_line_4_vertex[num_of_curves];
	for (int i = 0; i < num_of_curves; ++i)
	{
		curves[i] = a.curves[i];
	}
}

bezier_manager::~bezier_manager()
{
	delete[] curves;
}

void bezier_manager::add_curve(float verts[16], int sample_rate)
{
	bezier_line_4_vertex* temp = curves;
	
	num_of_curves += 1;

	//std::cout << "\n\nnum of curves\n\n" << num_of_curves << '\n';

	curves = new bezier_line_4_vertex[num_of_curves];
	//memset(curves, 0, num_of_curves * sizeof(bezier_line_4_vertex));
	
	if (!curves) std::cout << "\n\nfailed\n\n";

	for (int i = 0; i < num_of_curves - 1; ++i)
	{
		curves[i] = temp[i];
		//std::cout << "\n\ncomplete\n\n";
	}
	
	curves[num_of_curves - 1].sample_rate = sample_rate;
	curves[num_of_curves - 1].update_ctrl_verts(verts);

	delete[] temp;
}

void bezier_manager::add_curve(bezier_line_4_vertex curve)
{
	if (!curves)
	{
		curves = new bezier_line_4_vertex[1];
		num_of_curves += 1;
		curves[0] = curve;
		return;
	}

	bezier_line_4_vertex* temp = curves;

	num_of_curves += 1;
	curves = new bezier_line_4_vertex[num_of_curves];
	for (int i = 0; i < num_of_curves - 1; ++i)
	{
		curves[i] = temp[i];
	}
	curves[num_of_curves - 1] = curve;

	delete[] temp;
}

void bezier_manager::delete_all_curves()
{
	if (num_of_curves == 0) return;
	for (int i = 0; i < num_of_curves; ++i)
	{
		delete_curve(i);
	}
	num_of_curves = 0;
}

void bezier_manager::edit_curve(int curve_index, int ctrl_vert_index, float new_x, float new_y, float new_z, float new_w)
{
	// 索引值不存在
	if (curves == nullptr || curve_index >= num_of_curves || curve_index < 0)
	{
		std::cout << "No Curve to Edit!\n";
		return;
	}
	curves[curve_index].ctrl_verts[ctrl_vert_index * 4 + 0] = new_x;
	curves[curve_index].ctrl_verts[ctrl_vert_index * 4 + 1] = new_y;
	curves[curve_index].ctrl_verts[ctrl_vert_index * 4 + 2] = new_z;
	curves[curve_index].ctrl_verts[ctrl_vert_index * 4 + 3] = new_w;
	curves[curve_index].update();
}

void bezier_manager::delete_curve(int curve_index)
{
	// 要删除的索引值不存在
	if (curves == nullptr || curve_index >= num_of_curves || curve_index < 0)
	{
		std::cout << "No Curve to Delete!\n";
		return;
	}

	num_of_curves -= 1;
	bezier_line_4_vertex* temp = curves;
	curves = new bezier_line_4_vertex[num_of_curves];
	for (int i = 0; i < curve_index; ++i)
	{
		curves[i] = temp[i];
	}
	for (int i = curve_index; i < num_of_curves; ++i)
	{
		curves[i] = temp[i + 1];
	}
	delete[] temp;
}


void bezier_manager::updategl(unsigned int curveVAO[10], unsigned int curveVBO[10], unsigned int pointVAO[10], unsigned int pointVBO[10], unsigned int instanceVBO)
{
	int num_of_disp_points = 0;
	for (int i = 0; i < num_of_curves; ++i)
	{
		num_of_disp_points += (curves[i].sample_rate + 1);

		curves[i].updategl(curveVAO[i], curveVBO[i], pointVAO[i], pointVBO[i], instanceVBO);
	}
}