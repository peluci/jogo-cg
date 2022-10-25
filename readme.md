Como fazer para rodar o jogo Acesso Negado?

Instale os pacotes necessários usando os seguintes comando:
sudo apt update
sudo apt-get install g++
sudo apt-get install libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev

Depois, compile o jogo através deste comando:
g++ -o jogo jogo.cpp -lSDL -lGL -lSDLmain -lSDL_image -lSDL_mixer

Rode o jogo usando
./jogo

Divirta-se!

Vídeo relativo ao jogo: https://youtu.be/E_RJm8jiX_Y

Sobre o Jogo:
O nome dado é um trocadilho com o negacionismo científico, por isso "Acesso Negado". Os protagonistas Atila Iamarino, biologo e professor da UNICAMP, Natália Pasternak - pesquisadora do ICB-USP e PhD, Vitor Mori - membro do grupo Observatório COVID-19 e pesquisador da Universidade de Vermont, e a pesquisadora e divulgadora científica Luiza Caires do Jornal da USP, devem combater algumas figuras imputadas na atualidade, dentre elas, medicamentos dados como tratamentos mesmo sem a comprovação devida, a imagem conspiracionista da Terra Plana e o principal: as Fake News.

Funcionalidades implementadas:
O jogo possui várias telas.
Ao abrir, nos deparamos com a tela de início com instruções, nela, podemos pressionar 'S' para iniciar a jogatina ou 'O' para ir às opções.

Ao entrar nas opções, podemos selecionar qual personagem queremos jogar (usando as teclas numéricas do teclado - 1 a 4). Podemos também apertar a tecla 'L' e ver mais informações a respeito do desenvolvimento do jogo.

Ao apertar S em qualquer momento,  o jogo se inicia na dificuldade 1, na qual os inimigos tentam te atirar pequenos "coronavirus" e o pesquisador deve desviar pelas setas. Ao apertar espaço, o personagem atira vacinas nos oponentes. Ao pressionar qualquer tecla entre 1 e 5, altera-se a dificuldade e os tiros oponentes passam a ser mais frequentes.

Durante a execução do jogo, pode-se apertar 'P' para pausar, nesse momento, os oponentes pausam e a música suspende. Ao pressionar 'S' novamente, o jogo retoma.

Ao longo do jogo, temos 3 vidas. Ao perdê-las, perdemos, também, o jogo. Nesse momento, a tela de "Game Over" é exibida.

Se eliminarmos os 33 oponentes, vencemos o jogo e a tela de "Vitória" é exibida

Portanto, as implementações em acordância com o GitHub são:
1. Jogo base. 
2. Fundo personalizado (mas não animado).
3. Inimigos com texturas diferentes, aleatórios, mas com mesmo comportamento.
4. Vidas
5. Sons (trilha sonora)
6. Telas
7. Seletor de dificuldade
8. Seletor de personagem

