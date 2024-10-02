#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib> 
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
const int window_width = 800;
const int window_height = 600;

void initOpenGL() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Erro ao inicializar o GLEW!" << std::endl;
        exit(EXIT_FAILURE);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

}

GLuint loadTexture(const char* filePath) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
    if (data) {
        std::cout << "Imagem carregada: " << filePath << ", Largura: " << width << ", Altura: " << height << ", Canais: " << nrChannels << std::endl;
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
    }
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

struct Bird {
    float x, y;
    float velocity;
    GLuint texture;
    const float gravity = -0.0001f;
    const float flapStrength = 0.1f;

    Bird() : x(100.0f), y(300.0f), velocity(0.0f) {
        texture = loadTexture("C:/Users/ledua/Source/Repos/FlappyBird/pinto.png");
    }

    void reset() {
        x = 100.0f;
        y = 300.0f;
        velocity = 0.0f;
    }

    void update() {
        velocity += gravity;
        y += velocity;

        if (y < 0.0f) {
            y = 0.0f;
            velocity = 0.0f;
        }
    }

    void flap() {
        velocity = flapStrength;
    }

    void draw() const {
        glBindTexture(GL_TEXTURE_2D, texture); 
        glColor3f(1.0f, 1.0f, 1.0f);  

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(x - 15.0f, y - 15.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(x + 15.0f, y - 15.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(x + 15.0f, y + 15.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(x - 15.0f, y + 15.0f);
        glEnd();
    }
};


struct Pipe {
    float x, height;
    bool passed;

    Pipe(float startX, float h) : x(startX), height(h), passed(false) {}

    void reset(float startX, float h) {
        x = startX;
        height = h;
        passed = false;
    }

    void update(float speed) {
        x -= speed;
        if (x < -50.0f) {
            x = 800.0f;
            height = static_cast<float>(rand() % 300 + 100);
            passed = false;
        }
    }

    void draw() const {
        glColor3f(0.0f, 1.0f, 0.0f); 

        glBegin(GL_QUADS);
        glVertex2f(x - 25.0f, 0.0f);     
        glVertex2f(x + 25.0f, 0.0f);      
        glVertex2f(x + 25.0f, height);     
        glVertex2f(x - 25.0f, height);    
        glEnd();

        glBegin(GL_QUADS);
        glVertex2f(x - 25.0f, 600.0f);        
        glVertex2f(x + 25.0f, 600.0f);          
        glVertex2f(x + 25.0f, height + 200.0f); 
        glVertex2f(x - 25.0f, height + 200.0f); 
        glEnd();
    }


    bool isColliding(const Bird& bird) const {
        return (bird.x + 15.0f > x - 25.0f && bird.x - 15.0f < x + 25.0f &&
            (bird.y - 15.0f < height || bird.y + 15.0f > height + 200.0f));
    }
};

void resetGame(Bird& bird, Pipe pipes[]) {
    bird.reset();
    pipes[0].reset(400.0f, 300.0f);
    pipes[1].reset(650.0f, 250.0f);
    pipes[2].reset(900.0f, 350.0f);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar o GLFW!" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Flappy Bird Luiz Eduardo", nullptr, nullptr);
    if (!window) {
        std::cerr << "Falha ao criar a janela GLFW!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    initOpenGL();
    glEnable(GL_TEXTURE_2D);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, window_width, 0, window_height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Bird bird;
    Pipe pipes[] = { Pipe(400.0f, 300.0f), Pipe(650.0f, 250.0f), Pipe(900.0f, 350.0f) };
    bool gameOver = false;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        if (!gameOver) {
            bird.update();
            for (auto& pipe : pipes) {
                pipe.update(0.05f);
                if (pipe.isColliding(bird)) {
                    gameOver = true;
                }
            }

            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                bird.flap();
            }
        }
        else {
            if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
                resetGame(bird, pipes);
                gameOver = false;
            }
        }

        glDisable(GL_TEXTURE_2D);


        for (const auto& pipe : pipes) {
            pipe.draw();
        }

        glEnable(GL_TEXTURE_2D);
        bird.draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
