#pragma once
struct Box
{
	float x1;//��
	float y1;//��
	float x2;//�E
	float y2;//��

	Box()
	{
		x1 = 0.0f;
		y1 = 0.0f;
		x2 = 0.0f;
		y2 = 0.0f;
	}

	Box(float X1, float Y1, float X2, float Y2)
	{
		x1 = X1;
		y1 = Y1;
		x2 = X2;
		y2 = Y2;
	}
};
