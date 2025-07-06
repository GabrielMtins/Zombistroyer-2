# Zombistroyer-2

![Imagem do Jogo](https://img.itch.zone/aW1nLzE1NTU4MzkzLnBuZw==/original/UUhh7J.png)

Zombistroyer 2 é um topdown shooter gratuito. O código do jogo está licenciado
sob a licença GNU GPLv3+. O jogo está disponível para download no site
[itch.io](https://gabrielmtins.itch.io/zombistroyer-2)

## Compilação

Para compilar o jogo, é necessário ter instalado como dependências:
- libsdl2 
- libsdl2\_image
- libsdl2\_ttf
- libsdl2\_mixer
- gcc
- cmake

As dependências devem ser configuradas no ambiente de desenvolvimento, podendo
ser instaladas através de um instalador de pacotes.

Para compilar o jogo, siga os seguintes passos:
```
git clone https://github.com/GabrielMtins/Zombistroyer-2.git
cd Zombistroyer-2
cmake -B build
cd build
make
```

Depois de compilado, volte para a raiz do diretório e execute o jogo:

```
cd ..
./build/Zombistroyer2
```

O jogo necessita da pasta de *materials*, que está na raiz desse repositório.
Garanta que essa pasta esteja lá, caso contrário o jogo não consegue iniciar.
