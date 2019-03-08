// Std. Includes
#include <string>

// GLEW

#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<cstdlib>
// Other Libs
#include "SOIL2/SOIL2.h"
#include"Texture.h"
#include"drawskybox.h"
// Properties
#include <irrKlang.h>
using namespace irrklang;



const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
glm::vec3 lightPos = glm::vec3(50.0f, 210.0f, 30.0f);
GLint const no_of_enemy = 5;
int main()
{
	ISoundEngine *SoundEngine = createIrrKlangDevice();
	ISoundEngine *beepsound = createIrrKlangDevice();
	SoundEngine->play2D("audio/breakout.mp3", GL_TRUE);
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Setup and compile our shaders
	Shader landshader("modelLoading.vs", "modelLoading.frag");
	Shader gunshader("gunLoading.vs", "gunloading.frag");
	Shader targetshader("target.vs", "target.frag");
	Shader lightingShader("lighting.vs", "lighting.frag");

	// Load models
	Model landModel("res/model/island.obj");
	Model gunmodel("res/art/AK-103.fbx");
	Model targetmodel("res/model/Cube.obj");
	Shader skyboxShader("skybox.vs", "skybox.frag");
	skybox sky;

	GLuint landtexture = TextureLoading::LoadTexture("res/model/green.jpg");
	GLuint guntexture = TextureLoading::LoadTexture("res/art/AK_103_Base_Color.png");

	// Draw in wireframe
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glm::mat4 projection(1);
	projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
	glm::vec3 ambient[5]=
	{
		glm::vec3(0.0f,0.0f,0.0f),
		glm::vec3(1.0f,0.0f,0.0f),
		glm::vec3(0.0f,1.0f,0.0f),
		glm::vec3(0.0f,0.0f,1.0f),
		glm::vec3(1.0f,1.0f,0.0f),

	};
	float xcube[no_of_enemy];
	float ycube[no_of_enemy];
	float zcube[no_of_enemy];
	float normal[no_of_enemy];
	glm::vec3 position[no_of_enemy];
	glm::vec3 targetvec[no_of_enemy];
	glm::vec3 targetpos[no_of_enemy];
	float theta = 360.0f / no_of_enemy;
	float targetrad = 3.0f;
	for (int i = 0;i < no_of_enemy;i++)
	{
		xcube[i] = rand() % 200 - 100;
		ycube[i] = rand() % 100;
		zcube[i] = rand() % 200 - 100;
		normal[i] = 1 / sqrt(xcube[i] * xcube[i] + ycube[i] * ycube[i] + zcube[i] * zcube[i]);
		xcube[i] *= normal[i] * 100;
		ycube[i] *= normal[i] * 100;
		zcube[i] *= normal[i] * 100;
		position[i] = glm::vec3(xcube[i], ycube[i], zcube[i]);
		targetpos[i] = glm::vec3(targetrad*cos(i*glm::radians(theta)), 0, targetrad*sin(i*glm::radians(theta)));
		targetvec[i] = glm::normalize(glm::vec3(xcube[i], ycube[i], zcube[i])-targetpos[i]);
	}


	GLfloat lastshot = 0.0f;
	GLfloat currentshot = 0.0f;
	GLfloat deltashot = 0.0f;
	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glm::vec3 front = camera.GetFront();

		// Check and call events
		glfwPollEvents();
		DoMovement();

		// Clear the colorbuffer
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 view;
		view = camera.GetViewMatrix();
		landshader.Use();

		glUniform3f(glGetUniformLocation(landshader.Program, "material.ambient"), 0.0f, 0.5f, 0.31f);
		glUniform3f(glGetUniformLocation(landshader.Program, "material.diffuse"), 1.0f, 0.5f, 0.31f);
		glUniform3f(glGetUniformLocation(landshader.Program, "material.specular"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(landshader.Program, "material.shininess"), 32.0f);
		glUniform3f(glGetUniformLocation(landshader.Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(landshader.Program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(landshader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(landshader.Program, "objectColor"), 1.0f, 0.5f, 0.31f);
		glUniform3f(glGetUniformLocation(landshader.Program, "lightColor"), 1.0f, 1.0f, 1.0f);
		glUniform3fv(glGetUniformLocation(landshader.Program, "lightPos"), 1, glm::value_ptr(lightPos));
		glUniform3fv(glGetUniformLocation(landshader.Program, "viewPos"), 1, glm::value_ptr(camera.GetPosition()));
		
		glUniformMatrix4fv(glGetUniformLocation(landshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(landshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, landtexture);
		glUniform1i(glGetUniformLocation(landshader.Program, "material.diffuse"), 1);


		// Draw the loaded cube
		glm::mat4 land;
		land = glm::translate(land, glm::vec3(0.0f, -6.0f, 1.0f)); // Translate it down a bit so it's at the center of the scene
		//cube = glm::rotate(cube, (GLfloat)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(landshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(land));
		landModel.Draw(landshader);
		



		//draw loaded gun
		gunshader.Use();
		glUniform3f(glGetUniformLocation(gunshader.Program, "material.ambient"), 0.0f, 0.5f, 0.31f);
		glUniform3f(glGetUniformLocation(gunshader.Program, "material.diffuse"), 1.0f, 0.5f, 0.31f);
		glUniform3f(glGetUniformLocation(gunshader.Program, "material.specular"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(gunshader.Program, "material.shininess"), 32.0f);
		glUniform3f(glGetUniformLocation(gunshader.Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(gunshader.Program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(gunshader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(gunshader.Program, "objectColor"), 1.0f, 0.5f, 0.31f);
		glUniform3f(glGetUniformLocation(gunshader.Program, "lightColor"), 1.0f, 1.0f, 1.0f);
		glUniform3fv(glGetUniformLocation(gunshader.Program, "lightPos"), 1, glm::value_ptr(lightPos));
		glUniform3fv(glGetUniformLocation(gunshader.Program, "viewPos"), 1, glm::value_ptr(camera.GetPosition()));

		glUniformMatrix4fv(glGetUniformLocation(gunshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(gunshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, guntexture);
		glUniform1i(glGetUniformLocation(gunshader.Program, "material.diffuse"), 0);

		glm::mat4 gun;
		glm::vec3 gunposition = glm::vec3(camera.GetPosition().x + 0.5f*front.x, camera.GetPosition().y + 0.5f*front.y, camera.GetPosition().z + 0.5f*front.z) + 0.2f*camera.GetRight() - 0.2f*camera.GetUp();
		gun = glm::translate(gun, gunposition);

		gun = glm::rotate(gun, glm::radians(camera.GetPitch()), camera.GetRight());
		gun = glm::rotate(gun, glm::radians(camera.GetYaw()), glm::vec3(0.0f, -1.0f, 0.0f));

		glUniformMatrix4fv(glGetUniformLocation(gunshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(gun));
		gunmodel.Draw(gunshader);


		//draw loaded target
		targetshader.Use();
		glUniform3f(glGetUniformLocation(targetshader.Program, "material.ambient"), 0.0f, 0.5f, 0.31f);
		glUniform3f(glGetUniformLocation(targetshader.Program, "material.diffuse"), 1.0f, 0.5f, 0.31f);
		glUniform3f(glGetUniformLocation(targetshader.Program, "material.specular"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(targetshader.Program, "material.shininess"), 32.0f);
		glUniform3f(glGetUniformLocation(targetshader.Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(targetshader.Program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(targetshader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);

		glUniform3fv(glGetUniformLocation(targetshader.Program, "lightPos"), 1, glm::value_ptr(lightPos));
		glUniform3fv(glGetUniformLocation(targetshader.Program, "viewPos"), 1, glm::value_ptr(camera.GetPosition()));

		glUniformMatrix4fv(glGetUniformLocation(targetshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(targetshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		//check

		glm::vec3 model(1);



		//std::cout << position.x << " " << position.y << " " << position.z << endl;
		glm::mat4 target[no_of_enemy];
		//target= glm::translate(target, glm::vec3(camera.GetPosition().x + 0.5f*front.x, camera.GetPosition().y + 0.5f*front.y, camera.GetPosition().z + 0.5f*front.z) );
		glm::vec3 aimvec[no_of_enemy];
		GLfloat angle[no_of_enemy];


		for (int i = 0;i < no_of_enemy;i++)
		{
			glUniform3f(glGetUniformLocation(targetshader.Program, "material.ambient"), ambient[i].x,ambient[i].y,ambient[i].z);
			aimvec[i] = (position[i] - gunposition);
			if (abs(sqrt((position[i].x-targetpos[i].x)*(position[i].x - targetpos[i].x) + (position[i].y - targetpos[i].y)*(position[i].y - targetpos[i].y) + (position[i].z - targetpos[i].z)*(position[i].z - targetpos[i].z))) > 0.1)
			{
				position[i] -= targetvec[i] * 4.0f*deltaTime;
			}
			angle[i] = glm::acos(glm::dot(glm::normalize(aimvec[i]), camera.GetFront()));
			angle[i] = 180 / 3.14*angle[i];


			//target[i] = glm::scale(target[i],glm::vec3(0.5));
			target[i] = glm::translate(target[i], position[i]);
			//target = glm::rotate(target,GLfloat(glfwGetTime()),glm::vec3(0.0f,1.0f,0.0f));
			float length = sqrt(aimvec[i].x*aimvec[i].x + aimvec[i].y*aimvec[i].y + aimvec[i].z*aimvec[i].z);
			float ango = atan(1.71f / length) * 180 / 3.14;
			if (angle[i] <= ango)
			{
				cout << "matched";
			}
			if (keys[GLFW_KEY_F])
			{
				
				currentshot = glfwGetTime();
				deltashot = currentshot - lastshot;
				lastshot = currentshot;
				if (deltashot > 0.01)
					beepsound->play2D("audio/gunshot.mp3", GL_FALSE);
				if (angle[i] <= ango)
				{
					cout << "yes matched";
					xcube[i] = rand() % 200 - 100;
					ycube[i] = rand() % 100;
					zcube[i] = rand() % 200 - 100;
					normal[i] = 1 / sqrt(xcube[i] * xcube[i] + ycube[i] * ycube[i] + zcube[i] * zcube[i]);
					xcube[i] *= normal[i] * 100;
					ycube[i] *= normal[i] * 100;
					zcube[i] *= normal[i] * 100;
					position[i] = glm::vec3(xcube[i], ycube[i], zcube[i]);
					targetvec[i] = glm::normalize(glm::vec3(xcube[i], ycube[i], zcube[i]));
				}
			}




			glUniformMatrix4fv(glGetUniformLocation(gunshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(target[i]));


			targetmodel.Draw(targetshader);

		}
			/*glUniformMatrix4fv(glGetUniformLocation(gunshader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

			targetmodel.Draw(targetshader);
			*/

			glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
			skyboxShader.Use();
			view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix

			glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			sky.drawskybox();

			// Swap the buffers
			glfwSwapBuffers(window);
		}

		SoundEngine->drop();
		glfwTerminate();

		return 0;
	}

// Moves/alters the camera positions based on user input
void DoMovement()
{
	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}



}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}