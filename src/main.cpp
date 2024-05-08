#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/libi2d/i2d.h>
#include <dcmtk/dcmdata/libi2d/i2djpgs.h>
#include <dcmtk/dcmdata/libi2d/i2dplsc.h>
#include <dcmtk/dcmdata/dctk.h>
#include <vector>
using namespace std;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"  FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";

vector<Float32> loadDicom(char* file_to_load) {
	DcmFileFormat fileformat;
	OFCondition status = fileformat.loadFile(file_to_load);
	if (status.good())
	{
		OFString patientName;
		if (fileformat.getDataset()->findAndGetOFString(DCM_PatientName, patientName).good())
		{
			cout << "Patient's Name: " << patientName << endl;
		}
		else
			cerr << "Error: cannot access Patient's Name!" << endl;

		//get rows and columns
		Uint16 rows, columns;
		if (fileformat.getDataset()->findAndGetUint16(DCM_Rows, rows).good())
		{
			cout << "Rows: " << rows << endl;
		}
		else
			cerr << "Error: cannot access Rows!" << endl;	
		if (fileformat.getDataset()->findAndGetUint16(DCM_Columns, columns).good())
		{
			cout << "Columns: " << columns << endl;
		}
		else
			cerr << "Error: cannot access Columns!" << endl;

		// read pixel data
		
		vector<Float32> pixel_data(rows * columns, 0);
		const Float32* pixel_data_array = new Float32[rows * columns];
		fileformat.getDataset()->findAndGetFloat32Array(DCM_PixelData, pixel_data_array);
		
		//print all the data in the file
		fileformat.getDataset()->print(cout);

		//print out just the pixel data
		for (int i = 0; i < pixel_data.size(); i++) {
			//cout << pixel_data[i] << endl;
		}
		return pixel_data;
	}
	else
		cerr << "Error: cannot read DICOM file (" << status.text() << ")" << endl;

	return vector<Float32>();
}

int main() {
	//Initializing DCMTK variables
	char uid[100];
	I2DImgSource* inputPlug = new I2DJpegSource();
	I2DOutputPlug* outPlug = new I2DOutputPlugSC();
	Image2Dcm i2d;
	E_TransferSyntax writeXfer;
	DcmDataset* resultDset = NULL;

	vector<Float32> imageData = loadDicom("assets/image-00000.dcm");
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//GLfloat triangleVertices[] =
	//{
	//	//First Triangle
	//	-0.5f, 0.5f, 0.0f, //Top Left
	//	-0.5f, -0.5f, 0.0f, //Bottom Left
	//	0.5f, -0.5f, 0.0f,//Bottom Right
	//	//Second Triangle
	//	-0.5f, 0.5f, 0.0f,//Top Left
	//	0.5f, 0.5f, 0.0f,//Top Right
	//	0.5f, -0.5f, 0.0f,//Bottom Right
	//};

	GLfloat squareVertices[] =
	{
		-0.5f, -0.5f, //Bottom Left
		-0.5f, 0.5f, //Top Left
		0.5f, 0.5f, //Top Right
		0.5f, -0.5f //Bottom Right
	};



	GLFWwindow* window = glfwCreateWindow(800, 800, "YoutubeOpenGL", NULL, NULL);

	//Need to check if the window failed to terminate
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, 800, 800);

	//Prepare my vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//Prepare my fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//To use shaders, we must wrap them up in a shader program
	GLuint shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	//We no longer need the shaders now so we can delete them
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Vertex buffer for our vertex data, and an array to store the buffer (VAO stores VBO's)
	GLuint VAOs[1], VBOs[1];

	//Must be created befpre the VBO
	glGenVertexArrays(1, VAOs);

	glGenBuffers(1, VBOs);

	glBindVertexArray(VAOs[0]);

	//We need to bind our buffer so that functions we call later will act on the
	// binding
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);

	//Now we will store our vertices in the VB0
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);

	//Need to tell gl how to read/use the VBO
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//This part is not necessary but makes sure that we dont change a VAO or VBO accidentally
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//set the color we want to use when we clear the buffer
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	//Clear the buffer, specifying we want fill up the buffer
	//with the color specified earlier after clearing
	glClear(GL_COLOR_BUFFER_BIT);

	//Swap the back buffer with the front buffer
	glfwSwapBuffers(window);


	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(VAOs[0]);
		//Draw the triangles
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		//Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		//Detect GLFW events via polling
		glfwPollEvents();
	}

	//Clean up all the objects we have created in memory
	glDeleteVertexArrays(1, &VAOs[0]);
	glDeleteBuffers(1, &VBOs[0]);
	glDeleteProgram(shaderProgram);


	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}