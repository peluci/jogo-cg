/*g++ -o jogo jogo.cpp -lSDLmain -lSDL -lGL -lSDL_image -lSDL_mixer*/


#include <SDL/SDL.h>
#include "SDL/SDL_opengl.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_audio.h"
#include "SDL/SDL_mixer.h"
#include <stdio.h>
#include "string"

//Definindo alguns valores constantes
#define INIQTD 33 //Quantidade de inimigos
#define VELOCIDADETIRO 6
#define VELOCIDADEATACANTES 0.01
#define VELOCIDADEATIRADOR 1
#define RESOLUCAOX 800
#define RESOLUCAOY 1200
#define ATIRADORDIMENSOES 60

//Incluir função para exibir nossas texturas
GLuint loadTexture(const std::string &fileName)
{
    SDL_Surface *imagem = IMG_Load(fileName.c_str()); //Para carregar a imagem
    SDL_DisplayFormatAlpha(imagem);                   // Formato alpha
    unsigned objeto(0);                               //Criando a textura em "objeto"
    glGenTextures(1, &objeto);                        //Gerando a textura em "objeto"
    glBindTexture(GL_TEXTURE_2D, objeto);             //Configurando como 2d

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //Parametros
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imagem->w, imagem->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imagem->pixels);

    SDL_FreeSurface(imagem);

    return objeto;
};

//Criando função para incluir trilha sonora, com ajuda do fórum de CG!
void iniciar_musica(const std::string &fileName,int volume)
{

    Mix_Music *musicMenu=NULL;

    if (!Mix_PlayingMusic())
    {
        Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);
        musicMenu = Mix_LoadMUS(fileName.c_str());
        Mix_VolumeMusic(volume);
        Mix_PlayMusic(musicMenu, -1);
    }
};
void parar_musica()
{   
    int fade_out = 100;
    if (Mix_PlayingMusic())
    {
        Mix_FadeOutMusic(fade_out);
    }
};

//Criando uma função para facilitar imprimir as texturas
void imprimeImagem(float Xinicial, float Yinicial, float Xfinal, float Yfinal, unsigned int texturausada)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturausada);

    glBegin(GL_QUADS);
    glTexCoord2d(0, 0);
    glVertex2f(Xinicial, Yinicial);
    glTexCoord2d(1, 0);
    glVertex2f(Xfinal, Yinicial);
    glTexCoord2d(1, 1);
    glVertex2f(Xfinal, Yfinal);
    glTexCoord2d(0, 1);
    glVertex2f(Xinicial, Yfinal);
    glEnd();
    glDisable(GL_TEXTURE_2D);
};
//Criando uma struct para o nosso inimigo
struct Inimigo
{
    float x;
    float y;
    float dim;
    bool tomoutiro;
    int valorrandomico;
};

struct Vidas
{
    float x;
    float y;
    float dim;
    bool aparecendo;
};

//Criando uma struct para nossos tiros, pois são mais de um
struct Tiro
{

    float x;
    float y;
    float dim;
    bool tiropegou;
};

//Implementando uma função de colisões, onde:
//A eh o objeto que colide
//B eh o objeto que busca colidir
bool colisao(float Ax, float Ay, float Acomp, float Aalt, float Bx, float By, float Bcomp, float Balt)
{
    if (Ay + Aalt < By)
    {
        return false;
    }
    else if (Ay > By + Balt)
    {
        return false;
    }
    else if (Ax + Acomp < Bx)
    {
        return false;
    }
    else if (Ax > Bx + Bcomp)
    {
        return false;
    }

    return true;
};

int main(int argc, char *args[])
{
    //Iniciando o SDL
    SDL_Init(SDL_INIT_EVERYTHING);

    //Definindo atributos padrões do SDL
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    //Nome do jogo
    SDL_WM_SetCaption("Acesso Negado", NULL);
    //Criando a janela
    SDL_SetVideoMode(RESOLUCAOY, RESOLUCAOX, 32, SDL_OPENGL);

    //Define a cor da janela, que será branca
    glClearColor(255, 255, 255, 255);

    //Exibindo a janela
    glViewport(0, 0, RESOLUCAOY, RESOLUCAOX);

    //Sombras
    glShadeModel(GL_SMOOTH);

    //Desenha a matriz 2d
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //3d
    glDisable(GL_DEPTH_TEST);

    //Um booleano para iniciar e encerrar o jogo
    bool running = true;
    bool pausou = false;
    bool mostrarinstrucoes = false;
    bool reiniciar = true;
    bool sobre = false;
    bool vitoria = false;
    bool derrota = false;
    //Iniciando a quantidade de tiros dados como zero
    int qtdtirosdados = 0;
    int qtdtirosinimigos = 0;
    int volumemusica = 100;
    //Criando um objeto de eventos a partir de SDL_Event
    SDL_Event eventos;

    //Importante para usar as texturas
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //Criando variaveis para receber as texturas
    unsigned int atirador_textura[4] = {0, 0, 0, 0};
    unsigned int dificuldade_textura[5] = {0, 0, 0, 0, 0};
    unsigned int inicio_textura = 0;
    unsigned int pausa_textura = 0;
    unsigned int vitoria_textura = 0;
    unsigned int derrota_textura = 0;
    unsigned int inimigo_textura[4] = {0, 0, 0, 0};
    unsigned int tiro_textura = 0;
    unsigned int tiro_inimigo_textura = 0;
    unsigned int wallpaper_textura = 0;
    unsigned int instrucoes_textura = 0;
    unsigned int sobre_textura = 0;
    unsigned int vidas_textura = 0;
    //Recebendo as texturas
    //Texturas dos atiradores
    atirador_textura[0] = loadTexture("texturas/protagonistas/atila.png");
    atirador_textura[1] = loadTexture("texturas/protagonistas/pasternak.png");
    atirador_textura[2] = loadTexture("texturas/protagonistas/vitor.png");
    atirador_textura[3] = loadTexture("texturas/protagonistas/luiza.png");

    //Texturas dos inimigos
    inimigo_textura[0] = loadTexture("texturas/inimigos/inimigo1.png");
    inimigo_textura[1] = loadTexture("texturas/inimigos/inimigo2.png");
    inimigo_textura[2] = loadTexture("texturas/inimigos/inimigo3.png");
    inimigo_textura[3] = loadTexture("texturas/inimigos/inimigo4.png");

    //Texturas das dificuldades
    dificuldade_textura[0] = loadTexture("texturas/dificuldades/dificuldade1.png");
    dificuldade_textura[1] = loadTexture("texturas/dificuldades/dificuldade2.png");
    dificuldade_textura[2] = loadTexture("texturas/dificuldades/dificuldade3.png");
    dificuldade_textura[3] = loadTexture("texturas/dificuldades/dificuldade4.png");
    dificuldade_textura[4] = loadTexture("texturas/dificuldades/dificuldade5.png");

    //Texturas das telas
    inicio_textura = loadTexture("texturas/telas/home.png");
    pausa_textura = loadTexture("texturas/telas/pausa.png");
    vitoria_textura = loadTexture("texturas/telas/vitoria.png");
    derrota_textura = loadTexture("texturas/telas/derrota.png");
    instrucoes_textura = loadTexture("texturas/telas/opcoes.png");
    sobre_textura = loadTexture("texturas/telas/sobre.png");

    //Outras texturas
    tiro_textura = loadTexture("texturas/outros/tiro.png");
    wallpaper_textura = loadTexture("texturas/outros/wallpaper.png");
    tiro_inimigo_textura = loadTexture("texturas/outros/tiroinimigo.png");
    vidas_textura = loadTexture("texturas/outros/vidas.png");

    //Declarando variaveis para a posição do personagem atirador

    float atiradordim = ATIRADORDIMENSOES;
    float atiradorY = 325;
    float atiradorX = 275; //Esses valores foram testados até encontrar
    int atiradortomoutiro = 0;

    //Serão usados pelo teclado
    bool esq = false, dir = false;
    bool atirando = false;
    bool jogoiniciou = false;

    float velocidadeatacante = 0;
    float velocidadetiro = VELOCIDADETIRO;

    int contador = 0;
    int qualpersonagem = 0;
    int dificuldade = 0;
    //Instanciando os inimigos, chamando eles de atacantes pois estão atacando

    Inimigo atacante[INIQTD];
    Vidas vidas[3];
    Tiro municao[100000]; //Você tem no máximo 1000 munições, a princípio
    int tiroscertos = 0;  //Vai encerrar o jogo com mensagem de parabenização ao atingir INIQTD

    Tiro fogoinimigo[100000];

    //preenchendo as caracteristicas dos inimigos

    for (int i = 0, x = 6, y = 10; i < INIQTD; i++, x += 55)
    {
        if (x > 566)
        {
            x = 6;
            y += 55;
        }
        atacante[i].x = x;
        atacante[i].y = y;
        atacante[i].dim = 36;
        atacante[i].valorrandomico = rand() % 4;
        atacante[i].tomoutiro = false;
    }

    //Preenchendo o local de exibição das vidas

    for (int i = 0, x = 10, y = 350; i < 3; i++, x += 40)
    {
        if (x > 566)
        {
            x = 6;
            y += 55;
        }
        vidas[i].x = x;
        vidas[i].y = y;
        vidas[i].dim = 20;
        vidas[i].aparecendo = true;
    }

    //Criando um loop  do jogo

    while (running)
    {
        while (SDL_PollEvent(&eventos))
        {
            if (eventos.type == SDL_QUIT)
                running = false;

            if (eventos.type == SDL_KEYUP && eventos.key.keysym.sym == SDLK_ESCAPE)
                running = false;

            //Movimento do atirador, setas

            if (eventos.type == SDL_KEYDOWN)
            {
                if (eventos.key.keysym.sym == SDLK_LEFT)
                {
                    esq = true;
                }
                if (eventos.key.keysym.sym == SDLK_RIGHT)
                {
                    dir = true;
                }
            }
            else if (eventos.type == SDL_KEYUP)
            {
                if (eventos.key.keysym.sym == SDLK_LEFT)
                {
                    esq = false;
                }
                if (eventos.key.keysym.sym == SDLK_RIGHT)
                {
                    dir = false;
                }
            }
            //Tiros disparados, spacebar
            if (eventos.type == SDL_KEYDOWN)
            {
                if (eventos.key.keysym.sym == SDLK_SPACE)
                {
                    qtdtirosdados++;
                    atirando = true;
                }
            }
            else if (eventos.type == SDL_KEYUP)
            {
                if (eventos.key.keysym.sym == SDLK_SPACE)
                    atirando = false;
            }
        }

        //Implementando a lógica do jogo
        //Movimentação
        //Personagem atirador
        if (esq == true)
        {
            if (atiradorX != 0)
            {
                atiradorX -= VELOCIDADEATIRADOR;
            }
        }
        if (dir == true)
        {
            if (atiradorX != 550)
            {
                atiradorX += VELOCIDADEATIRADOR;
            }
        }
        //2 personagem inimigo

        //Velocidade vertical
        for (int i = 0; i < INIQTD; i++)
        {
            atacante[i].y += velocidadeatacante;
        }
        contador++;
        //Samba no pé (velocidade horizontal)
        for (int at = 0; at < INIQTD; at++)
        {
            if (contador > 33)
            {
                atacante[at].x -= velocidadeatacante * 15;
                if (contador == 66)
                    contador = 0;
            }
            else
                atacante[at].x += velocidadeatacante * 15;
        }

        //Renderizando
        glClear(GL_COLOR_BUFFER_BIT); //limpa o buffer

        //Inicia matriz do jogo
        glPushMatrix();
        //Dimensoes matriz
        glOrtho(0, 600, 400, 0, -1, 1);
        glColor4ub(255, 255, 255, 255); //Branco

        //Textura inicial do jogo

        imprimeImagem(0, 0, 600, 400, inicio_textura);

        //Pedindo ao usuário para apertar alguma tecla para começar

        if (eventos.type == SDL_KEYDOWN)
        {
            if (eventos.key.keysym.sym == SDLK_s)
            {
                jogoiniciou = true;
                tiroscertos = 0;
                atiradortomoutiro = 0;
                pausou = false;
                velocidadeatacante = VELOCIDADEATACANTES;
                vitoria = false;
                derrota = false;
                atiradortomoutiro = 0;
                for (int i = 0; i < 3; i++)
                    vidas[i].aparecendo = true;
            }
        }
        if (eventos.type == SDL_KEYDOWN)
        {
            if (eventos.key.keysym.sym == SDLK_p)
            {
                jogoiniciou = false;
                velocidadeatacante = 0;
                pausou = true;
                parar_musica();
            }
        }
        if (eventos.type == SDL_KEYDOWN)
        {
            if (eventos.key.keysym.sym == SDLK_o)
            {
                mostrarinstrucoes = true;
            }
        }

        if (eventos.type == SDL_KEYDOWN)
        {
            if (eventos.key.keysym.sym == SDLK_r)
            {
                atiradortomoutiro = 0;
                jogoiniciou = false;
                sobre = false;
                mostrarinstrucoes = false;
                vitoria = false;
                derrota = false;
                tiroscertos = 0;
                atiradortomoutiro = 0;
                volumemusica=100;
                for (int i = 0; i < 3; i++)
                    vidas[i].aparecendo = true;
                for (int i = 0, x = 6, y = 10; i < INIQTD; i++, x += 55)
                {
                    if (x > 566)
                    {
                        x = 6;
                        y += 55;
                    }
                    atacante[i].x = x;
                    atacante[i].y = y;
                    atacante[i].dim = 36;
                    atacante[i].valorrandomico = rand() % 4;
                    atacante[i].tomoutiro = false;
                }
                imprimeImagem(0, 0, 600, 400, inicio_textura);
            }
        }

        if (mostrarinstrucoes == true)
        {
            imprimeImagem(0, 0, 600, 400, instrucoes_textura);
            if (eventos.type == SDL_KEYDOWN)
            {
                if (eventos.key.keysym.sym == SDLK_1)
                {

                    qualpersonagem = 0;
                }
                if (eventos.key.keysym.sym == SDLK_2)
                {

                    qualpersonagem = 1;
                }
                if (eventos.key.keysym.sym == SDLK_3)
                {

                    qualpersonagem = 2;
                }
                if (eventos.key.keysym.sym == SDLK_4)
                {

                    qualpersonagem = 3;
                }
                if (eventos.key.keysym.sym == SDLK_l)
                {
                    sobre = true;
                }
            }
        }

        if (sobre == true)
            imprimeImagem(0, 0, 600, 400, sobre_textura);
        if (pausou == true)
            imprimeImagem(0, 0, 600, 400, pausa_textura);

        if (jogoiniciou == true)
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clean the screen and the depth buffer
            // Reset The Projection Matrix
            iniciar_musica("musica/musica1.mp3",volumemusica);
            imprimeImagem(0, 0, 600, 400, wallpaper_textura); //Wallpaper que fica por baixo
            imprimeImagem(500, 300, 585, 385, dificuldade_textura[dificuldade]);

            if (eventos.type == SDL_KEYDOWN)
            {
                if (eventos.key.keysym.sym == SDLK_1)
                    dificuldade = 0;
                if (eventos.key.keysym.sym == SDLK_2)
                    dificuldade = 1;
                if (eventos.key.keysym.sym == SDLK_3)
                    dificuldade = 2;
                if (eventos.key.keysym.sym == SDLK_4)
                    dificuldade = 3;
                if (eventos.key.keysym.sym == SDLK_5)
                    dificuldade = 4;
            }

            //Essas sao as vidas do jogo
            for (int i = 0; i < 3; i++)
            {
                if (vidas[i].aparecendo == true)
                {
                    int randomica = rand() % 4;
                    imprimeImagem(vidas[i].x, vidas[i].y, vidas[i].x + vidas[i].dim, vidas[i].y + vidas[i].dim, vidas_textura);
                }
            }

            //esse eh o nosso personagem
            imprimeImagem(atiradorX, atiradorY, atiradorX + atiradordim, atiradorY + atiradordim, atirador_textura[qualpersonagem]);

            //Esses sao os quadrados atacantes inimigos

            for (int i = 0; i < INIQTD; i++)
            {
                if (atacante[i].tomoutiro == false)
                {
                    int randomica = rand() % 4;
                    imprimeImagem(atacante[i].x, atacante[i].y, atacante[i].x + atacante[i].dim, atacante[i].y + atacante[i].dim, inimigo_textura[atacante[i].valorrandomico]);
                }
            }

            //Esse eh o tiro disparado por nós
            //Posicionando o tiro
            municao[qtdtirosdados].x = atiradorX + atiradordim / 3; //Mesma posição que o atirador em X
            municao[qtdtirosdados].y = atiradorY;                   //E em Y
            municao[qtdtirosdados].dim = 25;                        //Dimensões quaisquer
            municao[qtdtirosdados].tiropegou = false;               //Tiro ainda não atingiu ninguém

            //Exibindo o tiro até ele acertar algum lugar
            if (municao[qtdtirosdados].tiropegou == false) //Se o tiro nao tiver pegado em lugar nenhum
            {
                for (int multiplos = 0; multiplos < qtdtirosdados; multiplos++) //Esse for serve para exibir multiplos tiros de uma vez
                {
                    imprimeImagem(municao[multiplos].x, municao[multiplos].y, municao[multiplos].x + municao[multiplos].dim, municao[multiplos].y + municao[multiplos].dim, tiro_textura);
                    municao[multiplos].y -= velocidadetiro; //Atualiza a posicao do tiro
                }
            }

            for (int f = 0; f < INIQTD; f++)
            {
                for (int multiplos = 0; multiplos < qtdtirosdados; multiplos++) //Esse for serve para trabalhar com multiplos tiros por vez
                {
                    if (((colisao(atacante[f].x, atacante[f].y, atacante[f].dim, atacante[f].dim, municao[multiplos].x, municao[multiplos].y, municao[multiplos].dim, municao[multiplos].dim)) == true) && (atacante[f].tomoutiro == false) && (municao[multiplos].tiropegou == false))
                    {
                        atacante[f].tomoutiro = true;
                        municao[multiplos].tiropegou = true;
                        municao[multiplos].y += velocidadetiro;
                        tiroscertos++;
                        qtdtirosdados--;
                    }
                }
            }

            //INICIOINIMIGOTIRO
            //Esse eh o tiro disparado

            if (rand() % 245 / (dificuldade + 1) == 1) //Quando aleatorio for verdadeiro
            {

                int v = rand() % 33; //Escolhe um dos 33 atacantes de forma aleatória
                if (atacante[v].tomoutiro == false)
                {
                    qtdtirosinimigos++;
                    fogoinimigo[qtdtirosinimigos].x = atiradorX + atiradordim / 3;
                    fogoinimigo[qtdtirosinimigos].y = atacante[v].y;
                    fogoinimigo[qtdtirosinimigos].dim = 25;
                    fogoinimigo[qtdtirosinimigos].tiropegou = false;
                }
            }
            //Exibindo o tiro até ele acertar algum lugar
            if (fogoinimigo[qtdtirosinimigos].tiropegou == false)
            {
                for (int multiplos = 0; multiplos < qtdtirosinimigos; multiplos++)
                {
                    imprimeImagem(fogoinimigo[multiplos].x, fogoinimigo[multiplos].y, fogoinimigo[multiplos].x + fogoinimigo[multiplos].dim, fogoinimigo[multiplos].y + fogoinimigo[multiplos].dim, tiro_inimigo_textura);
                    fogoinimigo[multiplos].y += velocidadetiro / 4;
                }
            }

            //Calculando a colisão do tiro inimigo e do personagem (100% funcionando)
            for (int f = 0; f < INIQTD; f++)
            {
                //if (atacante[f].tomoutiro==false)
                for (int multiplos = 0; multiplos < qtdtirosinimigos; multiplos++)
                {
                    if (((colisao(atiradorX, atiradorY, atiradordim, atiradordim, fogoinimigo[multiplos].x, fogoinimigo[multiplos].y, fogoinimigo[multiplos].dim, fogoinimigo[multiplos].dim)) == true) && (atacante[f].tomoutiro == false) && (fogoinimigo[multiplos].tiropegou == false))
                    {
                        vidas[atiradortomoutiro].aparecendo = false;
                        atiradortomoutiro++;
                        fogoinimigo[multiplos].tiropegou = true;
                        fogoinimigo[multiplos].y += velocidadetiro;
                        qtdtirosinimigos--;
                    }
                }

            } //FIMINIMIGOTIRO
        }

        if (atiradortomoutiro == 3)
            derrota = true;
        if (derrota){
            imprimeImagem(0, 0, 600, 400, derrota_textura);
            parar_musica();
        }
            

        if (tiroscertos == 33)
            vitoria = true;
        if (vitoria){
imprimeImagem(0, 0, 600, 400, vitoria_textura);
parar_musica();
        }
            

        //fecha matriz
        glPopMatrix();

        //animacao
        SDL_GL_SwapBuffers();
    }

    glDisable(GL_BLEND);
    SDL_Quit();

    return 0;
}
