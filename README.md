Projeto 3 - Computação Gráfica - Simulação do Sistema Solar V2
======
## Autores
### Fellip da Silva Ribeiro - RA: 11201921507
### Verônica Agatha Gonçalves Isobe - RA: 11201920292
------
### Descrição
A aplicação se trata de um observatório 3D do sistema solar com animação. O observador pode mover a câmera entre os planetas para observar melhor o sistema.
O projeto foi desenvolvido utilizando a biblioteca abcg e modelos 3D e texturas gratuitas do site [Free3D](https://free3d.com/). Os modelos .obj utlizados representam os seguintes corpos celestes (hiperlinks para a fonte):

- [Sol](https://free3d.com/3d-model/sun-43982.html)
- [Mercúrio](https://free3d.com/3d-model/mercury-23007.html)
- [Vênus](https://free3d.com/3d-model/venus-98714.html)
- [Terra](https://free3d.com/3d-model/photorealistic-earth-98256.html)
- [Lua](https://free3d.com/3d-model/moon-photorealistic-2k-853071.html)
- [Marte](https://free3d.com/3d-model/mars-photorealistic-2k-671043.html)
- [Júpiter](https://free3d.com/3d-model/jupiter-v1--853820.html)
- [Saturno](https://free3d.com/3d-model/saturn-v1--741827.html)
- [Urano](https://free3d.com/3d-model/uranus-v2--767518.html)
- [Netuno](https://free3d.com/3d-model/neptune-82847.html)
- [Plutão](https://free3d.com/3d-model/pluto-v1--424613.html)

Além desse modelos 3D, também foi utizado uma textura cubemap para o Skybox implementado. Essa textura foi obtida de forma gratuita no site [MapCore](https://www.mapcore.org) e modificada para melhor atender a estética que buscavamos. A seguir o link para a textura:

- [High Resolution 2048 Space Skybox (Source/VTF)](https://www.mapcore.org/topic/19867-high-res-2048-space-skybox-sourcevtf/)

Vale ressaltar que os tamanhos e orbitas dos planetas não estão em escala, sendo o único dado fudamentado na realidade a velocidade das orbitas.

### Movimento da câmera e controles
Os movimentos da camera seguem como padrão:

![Imgur](https://i.imgur.com/OA3We8k.png)

E seus controles são:

Tecla | Movimento
:------: | :------:
8 / 2  | Dolly	        
4 / 6	 | Truck		
A / D	 | Pan	        
w / S	 | Tilt		
↑ / ↓	 | Pedestal	
R		    | Reset		
P	     | Pause		

Além desses controles, há também um slider para controlar a velocidade da simulação, variando de 0.1x a 10x a velocidade original e um menu de coordenadas para melhor orientação do usuário. Todas essas informações e controles podem ser acessados pelos widgets minimizaveis no topo da tela.

### Código Fonte
O código fonte está dentro da pasta Examples, dentro do diretório ABCg, como "SolarSystemModelV2".

### Link do Github Pages
https://fellip13.github.io/SolarSystemModelV2/

### Observação
Caso ache necessário, é possivel achar o SolarSystemModel.exe nesse repositório pelo seguinte caminho: abcg → build4 → bin → SolarSystemModelV2 → SolarSystemModelV2.exe

------
Projeto 2 - Computação Gráfica - Sistema Solar
======
## Autores
### Fellip da Silva Ribeiro - RA: 11201921507
### Verônica Agatha Gonçalves Isobe - RA: 11201920292
------
### Descrição
A aplicação se trata de um observatório 3D do sistema solar com animação. O observador pode mover a câmera entre os planetas para observar melhor o sistema.
O projeto foi desenvolvido utilizando a biblioteca abcg e modelos gratuitos 3D do site [Free3D](https://free3d.com/). Os modelos .obj utlizados representam os seguintes corpos celestes (hiperlinks para a fonte):

- [Sol](https://free3d.com/3d-model/sun-43982.html)
- [Mercúrio](https://free3d.com/3d-model/mercury-23007.html)
- [Vênus](https://free3d.com/3d-model/venus-98714.html)
- [Terra](https://free3d.com/3d-model/photorealistic-earth-98256.html)
- [Lua](https://free3d.com/3d-model/moon-photorealistic-2k-853071.html)
- [Marte](https://free3d.com/3d-model/mars-photorealistic-2k-671043.html)
- [Júpiter](https://free3d.com/3d-model/jupiter-v1--853820.html)
- [Saturno](https://free3d.com/3d-model/saturn-v1--741827.html)
- [Urano](https://free3d.com/3d-model/uranus-v2--767518.html)
- [Netuno](https://free3d.com/3d-model/neptune-82847.html)
- [Plutão](https://free3d.com/3d-model/pluto-v1--424613.html)

### Movimento da câmera e controles
Os movimentos da camera seguem como padrão:

![Imgur](https://i.imgur.com/OA3We8k.png)

E seus controles são:

Tecla | Movimento
:------: | :------:
8 / 2  | Dolly	        
4 / 6	 | Truck		
A / D	 | Pan	        
w / S	 | Tilt		
↑ / ↓	 | Pedestal	
R		    | Reset		
P	     | Pause		


### Código Fonte
O código fonte está dentro da pasta Examples, dentro do diretório ABCg, como "SolarSystemModel".

### Link do Github Pages
https://fellip13.github.io/SolarSystemModel/

### Observação
Caso ache necessário, é possivel achar o SolarSystemModel.exe nesse repositório pelo seguinte caminho: abcg → build3 → bin → SolarSystemModel → SolarSystemModel.exe

------
# Projeto 1 - Computação Gráfica - PONG
## Autores
### Fellip da Silva Ribeiro - RA: 11201921507
### Verônica Agatha Gonçalves Isobe - RA: 11201920292
------
### Descrição
A aplicação se trata de uma cópia do clássico jogo PONG. Duas barras se movimentam pelo eixo Y (uma de cada lado da tela) rebatendo uma bola de uma lado para o outro. Quando uma barrinha não conseguir rebater a bolinha, o outro jogador ganhara um ponto e aquele que fizer 5 pontos primeiro vence.

### Modos de jogo
O jogo possui 3 modos, apresentados no menu inicial:
- O modo "Player 1 - Right", onde a barrinha da esquerda se torna uma AI. 
- O modo "Player 2 - Left", onde a barrinha da direita se torna uma AI. 
- O modo "Two Playes", onde ambas as barrinhas são controladas pelos jogadores.

### Controles
Os controles são validos para todos os modos, com a AI assumindo o controle de uma das barrinhas no caso dos modos para um jogador. <br />
As teclas para controle dos players estarão sempre visiveis no topo da janela durante o jogo.

Tecla | Ação
:------: | :------:
ESC    | Encerra o programa
↑      | Movimenta a barrinha direita para cima
↓      | Movimenta a barrinha direita para baixo.
W      | Movimenta a barrinha esquerda para cima.
S      | Movimenta a barrinha esquerda para baixo.

### Código Fonte
O código fonte está dentro da pasta Examples, dentro do diretório ABCg, como "PONG".

### Link do Github Pages
https://fellip13.github.io/PONG/

### Observação
Caso ache necessário, é possivel achar o PONG.exe nesse repositório pelo seguinte caminho: abcg → build2 → bin → PONG → PONG.exe
