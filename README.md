# Flappy Bird em C++
Este é um jogo simples do estilo Flappy Bird implementado em C++ utilizando OpenGL e GLFW. O objetivo do jogo é controlar um pássaro que deve passar por canos sem colidir com eles.

## Pré-requisitos
#### Antes de executar o jogo, você precisará ter os seguintes pré-requisitos instalados (já estão inclusos no projeto):

• C++ Compiler: GCC ou Visual Studio

• CMake: Para construir o projeto

• OpenGL: Biblioteca gráfica

• GLEW: Biblioteca de extensão OpenGL

• GLFW: Biblioteca para criação de janelas e gerenciamento de entrada

• STB Image: Para carregar imagens de textura

# Instalação

1. Clone o repositório:
``` bash
git clone git clone https://github.com/picoliw/flappy-bird.git
cd flappy-bird
```

2. Crie um diretório de build:
``` bash
mkdir build
cd build
```

3. Compile o projeto
``` bash
cmake ..
make
```

4. Adicione o path completo da imagem do pássaro:
``` bash
Coloque a imagem do pássaro (por exemplo, pinto.png) no caminho especificado no código (atualmente,
C:/Users/ledua/Source/Repos/FlappyBird/pinto.png).
```

5. Execute o projeto
![Logo da Minha Empresa](https://i.imgur.com/b6eRV3Z.png)

# Controles
• Espaço: Faz o pássaro "bater as assas" e subir.

• R: Reinicia o jogo.

# Funcionamento

• Tente passar pelos canos sem colidir com eles.

• O jogo termina se o pássaro colidir com qualquer cano.
