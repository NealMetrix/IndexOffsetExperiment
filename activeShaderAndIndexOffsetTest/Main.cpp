#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <sstream>

//Here we are going to learn about Index Buffers in OpenGL
//Before we get into this let's go over a typical problem that we will develop when trying to render graphics.
//We have just been rendering graphics so far but let's try and draw a square this time. How would we do that? By drawing 2 triangles next to each other.
//After all the triangle is the most primitive 2d shape we can draw. Anything less than 3 vertices would just be a line.

struct shadeComponents
{
    std::string Vertexsource;//Lower in the program we will have a program that returns values in these variables
    std::string Fragmentsource;//This is part of building a parser to seperate a Fragment Shader from the Vertex Shader so we can write the code for both in the same document
};

static shadeComponents parseShader(const std::string& filepath)//This is the function we built to parse through the shader file we made
{
    std::ifstream stream(filepath);//here we are inputting a file to parsing

    enum class shaderType
    { None = -1, Vertex = 0, Fragment = 1 };//Here are the shaderTypes that will be used to specify when we are parsing each shader

    std::cout << filepath << std::endl;

    std::string line;//This will be used to store every line of code while we are parsing
    std::stringstream shaderSequence[2];//We need a stringstream array so we can store each type of shader as a different element. That way there will be a way to differentiate the Vertex and Fragment shader
    shaderType type = shaderType::None;

    //Starts sorting through shader types
    while (getline(stream, line))//while loop doesn't run which means getline(stream, line) must be false from the beginning
    {
        //Switching the value of type if it encounter #shader
        std::cout << line << std::endl;
        if (line.find("#shader") != std::string::npos)//Must be not equal to an invalid string position. I think it is something about this that makes .find work on every white space but that's a theory. .find doesn't find anything by itself
        {
            if (line.find("vertex") != std::string::npos)
                type = shaderType::Vertex;

            else if (line.find("fragment") != std::string::npos)
                type = shaderType::Fragment;
            //Catching if #shader isn't followed by an invalid shader type
            else { std::cout << "Warning: invalid shader specification... shader type remains unchanged"<< std::endl; }
        }
        else
        {
            //Catching the crash if shaderType == None
            if (type == shaderType::None)
                std::cout << "Error: no shaderType" << std::endl;

            shaderSequence[int(type)]<<line<<"\n";//Using type to organize which element we put the line of code into
        }
    }

    return { shaderSequence[0].str(), shaderSequence[1].str() };//returning the seperated shaders
}

static unsigned int CompileShader(unsigned int type, const std::string& source)//Here is a function to automatically compile a shader. We threw in a small bit of error handeling
{
    unsigned int id = glCreateShader(type);
    const char* src = &source[0];

    glShaderSource(id,1,&src,nullptr);
    glCompileShader(id);

    //start of error catch
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);//I am going to need to come back and analyze this error catch because I can't exactly tell what error we are catching
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);//Remember you can find these gl functions in the docs.gl website
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Shader Compile Failure " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;//Here we tell it to output if we had a compile fail on the vertex or fragment shader
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static int CreateShader(const std::string& vertexShade, const std::string& fragmentShade)//Here we write a function that allows us to compile both the vertexShader and fragmentShader simultaneously
{
    unsigned int program = glCreateProgram();//This is what we will be use as the combined result of both of our shader programs
    unsigned int vtx = CompileShader(GL_VERTEX_SHADER,vertexShade);
    unsigned int frag = CompileShader(GL_FRAGMENT_SHADER,fragmentShade);//Here compile via the compileShader we wrote taking each time taking the corresponding shader type as the parameter

    glAttachShader(program, vtx);
    glAttachShader(program, frag);//Here we are adding them to program via the openGL attachShader function
    glLinkProgram(program);//Now we only need to link program to access both vtx and frag shaders
    glValidateProgram(program);/*Validating the program isn't mandatory!!!! But probably should always use it for debugging purposes
    But comment it out to demonstrate that our code still works.*/

    glDeleteShader(vtx);//since they have now been added to program we can delete the individual shaders.
    glDeleteShader(frag);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1000, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glewInit();

    float positions[] = {
        -0.5f, -0.5,//0
         0.5f, -0.5f,//1
         -0.5f, 0.5f,//2
          0.5f, 0.5f//3
    };
    unsigned int indexBuff[] =
    {
        0, 1, 2,
        2, 3, 1
    };

    unsigned int buff;
    glGenBuffers(1, &buff);
    glBindBuffer(GL_ARRAY_BUFFER, buff);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),0);
    glEnableVertexAttribArray(0);

    //Binding the indexBuffer
    unsigned int indBuffOb;
    glGenBuffers(1, &indBuffOb);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBuffOb);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indexBuff, GL_STATIC_DRAW);

    shadeComponents shadeYellow = parseShader("res/otherShaderYellow.shader");
    unsigned int shader2 = CreateShader(shadeYellow.Vertexsource, shadeYellow.Fragmentsource);
    /*For some reason this has a copile Error but it doesn't seem to affect the code*/
    //glUseProgram(shader2);
    std::cout << glGetAttribLocation(shader2, "position") << std::endl;
    //std::cout << glGetAttribLocation(shader2, "color") << std::endl;

    shadeComponents shadeSource = parseShader("res/shaderCollection.shader");
    unsigned int shader = CreateShader(shadeSource.Vertexsource, shadeSource.Fragmentsource);
    glUseProgram(shader);
    std::cout << glGetAttribLocation(shader, "position") << std::endl;
    std::cout << glGetAttribLocation(shader, "color") << std::endl;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);/*Make sure that our blue shader is being used for our 1st 3 indices */
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);/*drawing blue section of square*/
        glUseProgram(shader2);/*Making sure our 2nd shader is used for our final 3 indices.*/
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const void*)(indexBuff[0]+3*sizeof(unsigned int)));/*drawing yellow section of square*/
        /*I have never been so excited to see 2 triangles in my life. I had to work a bit to create an offset for
        the second draw call but it's actually pretty simple. You basically need the starting offset
        (in this case my indexBuffer array) and then you do some vector math that tells the program how much farther
        you would like it to start from.(In this case we want it to step over 3 elements in our array which is 3 unsigned ints)*/

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}