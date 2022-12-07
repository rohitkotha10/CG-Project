#include "program.h"

using namespace std;

namespace CG {
    string parse(string shaderPath) {
        ifstream ifs(shaderPath);

        string final;

        int cnt = 0;
        while (!ifs.eof()) {
            string temp;
            getline(ifs, temp);
            final += temp + '\n';
        }
        return final;
    };

    void Program::create(string vsString, string fsString) {
        // vertex shader
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        string trial = parse(vsString);
        const GLchar* vsSource = trial.c_str();
        glShaderSource(vs, 1, &vsSource, NULL);
        glCompileShader(vs);

        int success;
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(vs, 512, NULL, infoLog);
            cout << "Vertex Shader Error: " << vsString << endl << infoLog << endl;
        }

        // frag shader
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        trial = parse(fsString);
        const GLchar* fsSource = trial.c_str();
        glShaderSource(fs, 1, &fsSource, NULL);
        glCompileShader(fs);

        success = 0;
        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(fs, 512, NULL, infoLog);
            cout << "Frag Shader Error: " << fsString << endl << infoLog << endl;
        }

        this->id = glCreateProgram();
        glAttachShader(this->id, vs);
        glAttachShader(this->id, fs);
        glLinkProgram(this->id);

        success = 0;
        glGetProgramiv(this->id, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(this->id, 512, NULL, infoLog);
            cout << "Shader Linking Error\n" << infoLog << endl;
        }

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    void Program::use() {
        glUseProgram(this->id);
    }

    void Program::shutdown() {
        glDeleteProgram(this->id);
    }

    void Program::setInt(string name, int value) {
        glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
    }

    void Program::setFloat(string name, float value) {
        glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
    }

    void Program::setMat4(string name, const glm::mat4& data) {
        glUniformMatrix4fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, &data[0][0]);
    }

    void Program::setVec3(string name, float x, float y, float z) {
        glUniform3f(glGetUniformLocation(this->id, name.c_str()), x, y, z);
    }

    void Program::setVec3(string name, const glm::vec3& data) {
        glUniform3fv(glGetUniformLocation(this->id, name.c_str()), 1, &data[0]);
    }

    void Program::setVec4(string name, float x, float y, float z, float a) {
        glUniform4f(glGetUniformLocation(this->id, name.c_str()), x, y, z, a);
    }

    void Program::setVec4(string name, const glm::vec4& data) {
        glUniform4fv(glGetUniformLocation(this->id, name.c_str()), 1, &data[0]);
    }
}  // namespace CG
