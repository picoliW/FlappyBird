#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib> 
const int window_width = 800;
const int window_height = 600;

void initOpenGL() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Erro ao inicializar o GLEW!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

struct Bird {
    float x, y;
    float velocity;
    const float gravity = -0.001f;
    const float flapStrength = 0.2f;

    Bird() : x(100.0f), y(300.0f), velocity(0.0f) {}

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
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(x - 15.0f, y - 15.0f);
        glVertex2f(x + 15.0f, y - 15.0f);
        glVertex2f(x + 15.0f, y + 15.0f);
        glVertex2f(x - 15.0f, y + 15.0f);
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
                pipe.update(0.2f);
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

        bird.draw();
        for (const auto& pipe : pipes) {
            pipe.draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
