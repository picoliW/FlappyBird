#include <GL/glew.h>  // Biblioteca para facilitar o uso de extensões do OpenGL.
#include <GLFW/glfw3.h>  // Biblioteca para criar janelas, contextos OpenGL e capturar entradas.
#include <iostream>  // Biblioteca padrão C++ para entrada e saída.
#include <cstdlib>  // Biblioteca para funções utilitárias, como rand().
#define STB_IMAGE_IMPLEMENTATION  // Define a implementação de stb_image.
#include "stb_image.h"  // Biblioteca para carregar imagens.

const int window_width = 800;  // Largura da janela.
const int window_height = 600;  // Altura da janela.

// Função para inicializar o OpenGL e verificar se GLEW foi carregado corretamente.
void initOpenGL() {
    glewExperimental = GL_TRUE;  // Configuração experimental do GLEW.
    if (glewInit() != GLEW_OK) {
        std::cerr << "Erro ao inicializar o GLEW!" << std::endl;
        exit(EXIT_FAILURE);
    }

    glEnable(GL_BLEND);  // Habilita a mistura de cores.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Define a função de mistura para suporte a transparência.
}

// Função de callback para ajustar o tamanho da janela de exibição.
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);  // Ajusta o OpenGL para o novo tamanho da janela.
}

// Função para carregar uma textura de uma imagem no formato .png, .jpg, etc.
GLuint loadTexture(const char* filePath) {
    GLuint textureID;
    glGenTextures(1, &textureID);  // Gera um ID de textura.
    glBindTexture(GL_TEXTURE_2D, textureID);  // Vincula a textura ao OpenGL.

    int width, height, nrChannels;
    // Carrega a imagem usando stb_image.
    unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
    if (data) {
        std::cout << "Imagem carregada: " << filePath << ", Largura: " << width << ", Altura: " << height << ", Canais: " << nrChannels << std::endl;
        // Define o formato da imagem com base no número de canais (1 = preto e branco, 3 = RGB, 4 = RGBA).
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        // Envia a imagem para o OpenGL como textura 2D.
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
    }
    stbi_image_free(data);  // Libera a memória da imagem carregada.

    // Configura parâmetros de filtragem e repetição da textura.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;  // Retorna o ID da textura carregada.
}

// Estrutura que define o pássaro do jogo.
struct Bird {
    float x, y;  // Posição do pássaro.
    float velocity;  // Velocidade vertical do pássaro.
    GLuint texture;  // Textura associada ao pássaro.
    const float gravity = -0.0001f;  // Constante para gravidade.
    const float flapStrength = 0.1f;  // Força do "batimento de asas".

    // Construtor que inicializa o pássaro na posição inicial e carrega sua textura.
    Bird() : x(100.0f), y(300.0f), velocity(0.0f) {
        texture = loadTexture("C:/Users/Aluno/Source/Repos/FlappyBird/pinto.png");  // Carrega a textura.
    }

    // Reinicia o pássaro para a posição inicial.
    void reset() {
        x = 100.0f;
        y = 300.0f;
        velocity = 0.0f;
    }

    // Atualiza a posição do pássaro de acordo com a gravidade e a velocidade.
    void update() {
        velocity += gravity;  // Acelera pela gravidade.
        y += velocity;  // Atualiza a posição vertical.

        // Evita que o pássaro caia abaixo do chão.
        if (y < 0.0f) {
            y = 0.0f;
            velocity = 0.0f;
        }
    }

    // Aplica a força de "batimento de asas" ao pássaro.
    void flap() {
        velocity = flapStrength;
    }

    // Desenha o pássaro usando OpenGL.
    void draw() const {
        glBindTexture(GL_TEXTURE_2D, texture);  // Vincula a textura do pássaro.
        glColor3f(1.0f, 1.0f, 1.0f);  // Define a cor como branca.

        // Desenha um quadrado na posição do pássaro.
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(x - 15.0f, y - 15.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(x + 15.0f, y - 15.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(x + 15.0f, y + 15.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(x - 15.0f, y + 15.0f);
        glEnd();
    }
};

// Estrutura que define os tubos (obstáculos) do jogo.
struct Pipe {
    float x, height;  // Posição e altura do tubo.
    bool passed;  // Indica se o pássaro já passou pelo tubo.

    // Construtor que inicializa o tubo na posição e altura fornecidas.
    Pipe(float startX, float h) : x(startX), height(h), passed(false) {}

    // Reinicia o tubo na posição e altura fornecidas.
    void reset(float startX, float h) {
        x = startX;
        height = h;
        passed = false;
    }

    // Atualiza a posição do tubo (movendo-se da direita para a esquerda).
    void update(float speed) {
        x -= speed;  // Move o tubo para a esquerda.

        // Se o tubo sair da tela, reinicializa-o em uma nova posição com nova altura.
        if (x < -50.0f) {
            x = 800.0f;
            height = static_cast<float>(rand() % 300 + 100);  // Altura aleatória.
            passed = false;
        }
    }

    // Desenha o tubo usando OpenGL.
    void draw() const {
        glColor3f(0.0f, 1.0f, 0.0f);  // Define a cor do tubo como verde.

        // Desenha a parte inferior do tubo.
        glBegin(GL_QUADS);
        glVertex2f(x - 25.0f, 0.0f);
        glVertex2f(x + 25.0f, 0.0f);
        glVertex2f(x + 25.0f, height);
        glVertex2f(x - 25.0f, height);
        glEnd();

        // Desenha a parte superior do tubo.
        glBegin(GL_QUADS);
        glVertex2f(x - 25.0f, 600.0f);
        glVertex2f(x + 25.0f, 600.0f);
        glVertex2f(x + 25.0f, height + 200.0f);
        glVertex2f(x - 25.0f, height + 200.0f);
        glEnd();
    }

    // Verifica se o pássaro colidiu com o tubo.
    bool isColliding(const Bird& bird) const {
        return (bird.x + 15.0f > x - 25.0f && bird.x - 15.0f < x + 25.0f &&
            (bird.y - 15.0f < height || bird.y + 15.0f > height + 200.0f));
    }
};

// Função que reinicia o jogo, resetando o pássaro e os tubos.
void resetGame(Bird& bird, Pipe pipes[]) {
    bird.reset();
    pipes[0].reset(400.0f, 300.0f);
    pipes[1].reset(650.0f, 250.0f);
    pipes[2].reset(900.0f, 350.0f);
}

// Função principal do programa.
int main() {
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar o GLFW!" << std::endl;
        return -1;
    }

    // Cria uma janela e contexto OpenGL.
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Flappy Bird Luiz Eduardo", nullptr, nullptr);
    if (!window) {
        std::cerr << "Falha ao criar a janela GLFW!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);  // Define o contexto OpenGL da janela.
    initOpenGL();  // Inicializa o OpenGL e configurações de renderização.
    glEnable(GL_TEXTURE_2D);  // Habilita o uso de texturas.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  // Configura callback para redimensionamento.

    // Define a cor de fundo e o sistema de coordenadas.
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, window_width, 0, window_height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Inicializa o pássaro e os tubos.
    Bird bird;
    Pipe pipes[] = { Pipe(400.0f, 300.0f), Pipe(650.0f, 250.0f), Pipe(900.0f, 350.0f) };
    bool gameOver = false;

    // Loop principal do jogo.
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);  

        // Se o jogo não terminou, atualiza o estado do pássaro e tubos.
        if (!gameOver) {
            bird.update();  // Atualiza o pássaro.
            for (auto& pipe : pipes) {
                pipe.update(0.05f);  // Move os tubos.
                if (pipe.isColliding(bird)) {
                    gameOver = true;  // Verifica colisão com os tubos.
                }
            }

            // Se a tecla de espaço for pressionada, o pássaro "bate as asas".
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                bird.flap();
            }
        }
        else {
            // Se o jogo terminou, permite reiniciar pressionando a tecla "R".
            if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
                resetGame(bird, pipes);
                gameOver = false;
            }
        }

        glDisable(GL_TEXTURE_2D);  // Desabilita texturas para desenhar os tubos.
        for (const auto& pipe : pipes) {
            pipe.draw();  // Desenha os tubos.
        }

        glEnable(GL_TEXTURE_2D);  // Habilita texturas novamente para desenhar o pássaro, depois de desenhar os tubos, se não eles ficam invisíveis por algum motivo (kkkkkkk).
        bird.draw();  // Desenha o pássaro.

        glfwSwapBuffers(window);  // Troca os buffers de exibição.
        glfwPollEvents();  // Verifica eventos como teclas pressionadas.
    }

    glfwTerminate();  // Finaliza o GLFW.
    return 0;
}
