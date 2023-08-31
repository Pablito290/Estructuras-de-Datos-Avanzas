#include <iostream>
#include <vector>
#include <math.h>
using namespace std;

struct punto {
	punto(int _dimensiones) {
		v.resize(_dimensiones);
		dimensiones = _dimensiones;
		rellenar();
	}
	vector<float> v;
	int dimensiones = 0;
	void rellenar() {
		int tam = v.size();
		for (int i = 0; i < tam; i++) {
			v[i] = rand() % 10001;	//Esta parte es para ingresar los valores a los puntos
			v[i] = v[i] / 10000;
		}
	}
};

float distancia(punto* v, punto* v2) {
	int t = v->dimensiones;
	float sum = 0;
	for (int i = 0; i < t; i++) {
		sum = sum + pow(v2->v[i] - v->v[i], 2);
	}
	return sqrt(sum);
}



int main()
{
	vector<punto*> puntos(100);
	for (int i = 0; i < puntos.size(); i++) {
		puntos[i] = new punto(1000);	//en la parte del consturctor se dice de cuantas dimensiones sean los puntos
	}
	cout << "distancias" << endl;
	for (int i = 0; i < puntos.size(); i++) {
		for (int j = i + 1; j < puntos.size(); j++) {
			cout << distancia(puntos[i], puntos[j]) << endl;
		}
	}

}
