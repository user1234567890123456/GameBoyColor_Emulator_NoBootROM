#include "MyFont.h"

MyFont::MyFont(MyDirectXSystem* myDirectXSystem, const int NUMBER_IMAGE_WIDTH, const int NUMBER_IMAGE_HEIGHT, TCHAR *filename)
	: myDirectXSystem(myDirectXSystem), NUMBER_IMAGE_WIDTH(NUMBER_IMAGE_WIDTH), NUMBER_IMAGE_HEIGHT(NUMBER_IMAGE_HEIGHT)
{
	number_image = new MyDirectXImage(filename, myDirectXSystem->get_pDevice3D());
}

MyFont::~MyFont()
{
	delete number_image;
}
