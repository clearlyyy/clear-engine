#include "Shader.h"

std::string loadShaderFile(const std::string& filepath);
void checkForShaderErrors(unsigned int shader);

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{

    // Vertex Shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertexSourceStr = loadShaderFile(vertexPath);
    const char* vertexShaderSource = vertexSourceStr.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkForShaderErrors(vertexShader);
    
    // Fragment Shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragmentSourceStr = loadShaderFile(fragmentPath);
    const char* fragmentShaderSource = fragmentSourceStr.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkForShaderErrors(fragmentShader);

    // Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    
    // Check for Shader Program Errors
    int success;
    char infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "[SHADER PROGRAM LINKING FAILED]:\n" << infoLog << std::endl;
    }

    // Delete the shaders.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string& name, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, glm::vec3 value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}




//Helper Function to load shader source code into a cstr
std::string loadShaderFile(const std::string& filepath)
{
    std::string fileContents;

    std::ifstream in(filepath);
    //Allow ifstream objects to throw exceptions
    in.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
    
    if (!in.is_open())
        std::cout << "[FILE LOAD ERROR] Failed to load Shader at: " + filepath << std::endl;
    std::stringstream buffer;
    buffer << in.rdbuf();
    fileContents = buffer.str();

    in.close();

    return fileContents;
}  

// Helper Function to check for shader compile errors.
void checkForShaderErrors(unsigned int shader)
{
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "SHADER COMPILATION ERROR!\n" << infoLog << std::endl;
    }
}