## Integrantes

109910: Carlos Francisco Pinheiro
228844: Larissa Rozales Gonçalves
221055: Matheus Cezimbra Barbieri

## Instruções de jogo
* Use a seta para frente (↑) para se mover para frente

* Use as setas para os lados (← e →) para girar

* As teclas "[" e "]" são usadas para diminuir ou aumentar a velocidade do personagem, respeitando um limite mínimo e máximo.  O personagem começa em velocidade 0.5 e pode aumentar até 5, em incrementos de 0.5.

* Use a tecla "Q" para encerrar o jogo no meio.  Ela também pode ser usada para sair do jogo após completado, na tela de informações.

* Use a tecla "R", na tela de informações após um jogo acabar, para reinciá-lo.

## Instruções de compilação

Bibliotecas utilizadas: OpenCV para leitura de imagens, e OpenGL para implementação.

Comando usado para compilar:
```
clang++ --std=c++14 -ggdb `pkg-config --cflags --libs opencv` -framework GLUT -framework OpenGL -framework  Cocoa -Wno-deprecated-declarations
```
