# Projeto 1 - Mapa de Propagação de Calor em uma Chapa
### MC504: Sistemas Operacionais
Grupo: 
- Francisco Namias Vicente - 216028
- Gustavo Praciano Barros - 217555
- Hitalo Cesar Alves - 217878
- Luiz Felipe Eiske Kina - 183020

![Exemplo 1](./movie/movie1.gif)

## Visão Geral
O projeto visa simular a propagação de calor em uma barra que tem temperatura constante em suas extremidades. Temperaturas em cada extremidade da barra e a quantidade de ciclos são aceitas como *input*. O programa utiliza a média das temperaturas dos *pixels* vizinhos para calcular uma nova temperatura em cada *pixel* a cada ciclo.
O diretório foi dividido em 3 partes, uma pasta de input que possui arquivos com os parâmetros iniciais, sendo eles, o tamanho da barra, a temperatura em cada uma das 4 extremidades da barra e a quantidade de ciclos que serão realizadas; além disso, há uma pasta denominada *movie* que possui um *gif* com um exemplo; e, por fim, há uma pasta *src* que possui o código do programa.

Este último diretório possui um *main* em *python*, um *main* em *c* e um último programa para produzir o gif. O código em *python* é utilizado para criação de uma placa com uma interface mais agradável, utilizando a biblioteca matplotlib, enquanto o código em *c* é a aplicação do programa, a utilização e o gerenciamento de múltiplas threads, o qual é perfeitamente funcional sozinho.

## Instruções de uso
Para rodar o programa completo basta rodar o comando:

 ``bash run``
 
Porém se não desejar ver a interface gráfica e sim apenas o programa C rodando em seu terminal, basta rodar o comando:

``gcc src/main.c -pthread -w -o src/main``

``./src/main < input/input.txt``

## Estratégia Multithread adotada
As threads são utilizadas para calcular paralelamente o valor em cada linha da barra de acordo com a temperatura de sua vizinhança no momento anterior. Na função *main* do código em *c*, a função *next_step* é chamada a cada passo que se altera a temperatura, ela altera a matriz que é chamada de *h_* que posteriormente é copiada na matriz h. Essa distinção é feita pois as operações em cada thread dependem de um mapa original que é armazenado em h, caso as alterações fossem feitas ao mesmo tempo em que uma thread em paralelo realiza uma ação os resultados entrariam em conflito. Por isso, é mapeada uma matriz h em que é mantido o estado da barra antes de começar a execução e o resultado é mantido em uma matriz h_ distinta.

Dentro da função *next_step* é chamada a função *next_step_thr* que é passada para cada thread realizar a operação para calcular a temperatura em determinado *pixel* em cada linha, portanto, cada thread contém as informações de uma linha da matriz que foi alocada. A operação consiste em calcular a média de todas as temperaturas ao redor do *pixel*.

Essa divisão em *next_step* e *next_step_thr* não é necessária a princípio, entretanto optamos por isso por conta da integração com o código em python utilizando os ctypes.

Ao fim da função é realizado um *pthread_join* para sincronizar as threads que foram alocadas para calcular a nova matriz que foi criada a partir da matriz anterior.
