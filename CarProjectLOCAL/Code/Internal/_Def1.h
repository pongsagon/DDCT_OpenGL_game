#include "_Begin.h"





//
//
//namespace BanKEngine {
//
//
//	namespace GlfwGlad {
//		GLFWwindow* window; 
//		unsigned int SCR_WIDTH = 800;
//		unsigned int SCR_HEIGHT = 400;
//		B_Camera camera(glm::vec3(-0.224556, 10.4038, -18.9259), glm::vec3(0.0f, 1.0f, 0.0f), 89.3999, -34.3001);
//		glm::mat4 projection;
//		float FOV = 45.0f;
//
//
//
//												// glfw: whenever the mouse moves, this callback is called
//												// -------------------------------------------------------
//		//float lastX = (float)BanKEngine::GlfwGlad::SCR_WIDTH / 2.0;
//		//float lastY = (float)BanKEngine::GlfwGlad::SCR_HEIGHT / 2.0;
//		//bool firstMouse = true;
//		//										void B_mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//		//										{
//		//											float xpos = static_cast<float>(xposIn);
//		//											float ypos = static_cast<float>(yposIn);
//		//											if (firstMouse)
//		//											{
//		//												lastX = xpos;
//		//												lastY = ypos;
//		//												firstMouse = false;
//		//											}
//
//		//											float xoffset = xpos - lastX;
//		//											float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//		//											lastX = xpos;
//		//											lastY = ypos;
//		//											
//		//											//////////////////////////////////
//		//											camera.ProcessMouseMovement(xoffset, yoffset);
//		//										}
//
//		//										// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//		//										// ----------------------------------------------------------------------
//		//										void B_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//		//										{
//		//											camera.ProcessMouseScroll(static_cast<float>(yoffset));
//		//										}
//		//bool keys[1024]; // activated keys
//		//bool Wireframe = false;
//		//										void B_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
//		//										{
//		//											static unsigned short wireframe = 0;
//
//		//											if (action == GLFW_PRESS)
//		//											{
//		//												switch (key)
//		//												{
//		//												case GLFW_KEY_ESCAPE:
//		//													glfwSetWindowShouldClose(window, GL_TRUE);
//		//													return;
//		//													// case GLFW_KEY_B:
//		//													//     switchTwoSided(true);
//		//													//     break;
//		//												default:
//		//													keys[key] = true;
//		//													break;
//		//												}
//		//											}
//
//		//											if (action == GLFW_RELEASE)
//		//											{
//		//												if (key == GLFW_KEY_TAB) {
//		//													Wireframe = !Wireframe;
//
//		//														if (Wireframe) {
//		//															glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//		//														}
//		//														else {
//		//															glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//		//														}
//		//												}
//		//													keys[key] = false;
//
//		//											}
//		//										}
//		//										// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//		//										// ---------------------------------------------------------------------------------------------
//		//										void B_framebuffer_size_callback(GLFWwindow* window, int width, int height)
//		//										{
//		//											// make sure the viewport matches the new window dimensions; note that width and
//		//											// height will be significantly larger than specified on retina displays.
//		//											glViewport(0, 0, width, height);
//		//										}
//
//		//												// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//		//												// ---------------------------------------------------------------------------------------------------------
//		//												void processInput()
//		//												{
//		//													float cameraSpeed = Time.Deltatime * 3.0f;
//
//		//													if (BanKEngine::GlfwGlad::keys[GLFW_KEY_W]) {
//		//														BanKEngine::GlfwGlad::camera.ProcessKeyboard(FORWARD, cameraSpeed);
//		//													}
//		//													else if (BanKEngine::GlfwGlad::keys[GLFW_KEY_S]) {
//		//														BanKEngine::GlfwGlad::camera.ProcessKeyboard(BACKWARD, cameraSpeed);
//		//													}
//
//		//													if (BanKEngine::GlfwGlad::keys[GLFW_KEY_A]) {
//		//														BanKEngine::GlfwGlad::camera.ProcessKeyboard(LEFT, cameraSpeed);
//		//													}
//		//													else if (BanKEngine::GlfwGlad::keys[GLFW_KEY_D]) {
//		//														BanKEngine::GlfwGlad::camera.ProcessKeyboard(RIGHT, cameraSpeed);
//		//													}
//
//		//													if (BanKEngine::GlfwGlad::keys[GLFW_KEY_SPACE]) {
//		//														BanKEngine::GlfwGlad::camera.ProcessKeyboard(UP, cameraSpeed);
//		//													}
//		//													else if (BanKEngine::GlfwGlad::keys[GLFW_KEY_Q]) {
//		//														BanKEngine::GlfwGlad::camera.ProcessKeyboard(DOWN, cameraSpeed);
//		//													}
//		//												}
//
//
//
//
//
//		//void Init() {
//		//	// glfw: initialize and configure
//		//	// ------------------------------
//		//	glfwInit();
//		//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//		//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//		//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//		//	#ifdef __APPLE__
//		//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//		//	#endif
//
//		//	// glfw window creation
//		//	// --------------------
//		//	window = glfwCreateWindow(
//		//		SCR_WIDTH, SCR_HEIGHT, "Nigga", NULL, NULL);
//		//	if (window == NULL)
//		//	{
//		//		std::cout << "Failed to create GLFW window" << std::endl;
//		//		glfwTerminate();
//		//		return;
//		//	}
//		//	//Bind Controls
//		//	glfwMakeContextCurrent(window);
//		//	glfwSetFramebufferSizeCallback(window, B_framebuffer_size_callback);
//		//	glfwSetCursorPosCallback(window, B_mouse_callback);
//		//	glfwSetScrollCallback(window, B_scroll_callback);
//		//	glfwSetKeyCallback(window, B_key_callback);
//
//		//	// tell GLFW to capture our mouse
//		//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//
//		//	// GLAD **********************************
//		//	// glad: load all OpenGL function pointers
//		//	// ***************************************
//		//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//		//	{
//		//		std::cout << "Failed to initialize GLAD" << std::endl;
//		//		return;
//		//	}
//
//
//
//
//		//	// configure global opengl state (GLAD)
//		//	// -----------------------------
//		//	glEnable(GL_DEPTH_TEST);
//
//		//	// Enable blending
//		//	glEnable(GL_BLEND);
//
//		//	// Set the blending function
//		//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//		//	stbi_set_flip_vertically_on_load(false);
//
//		//	cout << endl << "BanKEngine | Init GlfwGlad";
//		//}
//		void Update() {
//			projection = glm::perspective(glm::radians(FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 12345.0f);
//
//			//glfwPollEvents(); 
//			//glfwSwapBuffers(window);
//		} 
//
//	}
//}
//

