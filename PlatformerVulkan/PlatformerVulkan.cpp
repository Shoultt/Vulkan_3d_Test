#include "DrawVulkan.h"
#include "destroy.h"

initVK init;

int main()
{
	createWindow();
	init.InitVulkan();
	loop();
	destroy();
}