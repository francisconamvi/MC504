Para a execução do código é necessario a biblioteca OpenCV instalada. Para compilar basta executar a seguinte linha:
g++ main.cpp img.cpp -pthread -lX11 -o output1 `pkg-config --cflags --libs opencv`


