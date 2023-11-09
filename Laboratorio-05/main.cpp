#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <sstream>
#include <cmath>
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;


double generateRandomNumber(double min = 0, double max = 10) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dist(min, max);

    return dist(gen);
}

struct Point {
    vector<double> Dimensions;
    int cluster;
    double ws = INT_MAX;
};

double Euclidean(Point a, Point b) {
    double total = 0;
    for (int i = 0; i < a.Dimensions.size(); i++) {
        total = total + pow((a.Dimensions[i] - b.Dimensions[i]), 2);
    }
    total = sqrt(total);
    return total;
}

Point* Nearest(Point p, vector<Point*> v) {
    Point* paux = nullptr;
    double nearest = INT_MAX;
    for (int i = 0; i < v.size(); i++) {
        double aux = Euclidean(p, *v[i]);
        if (aux < nearest && aux != 0) {
            nearest = aux;
            paux = v[i];
        }
    }
    return paux;
}

void maxmin(vector<Point*>& points, Point& bottom, Point& top) { //vector donde se buscará
    for (int i = 0; i < points.size(); i++) {
        for (int j = 0; j < points[i]->Dimensions.size(); j++) {
            if (points[i]->Dimensions[j] > top.Dimensions[j]) {
                top.Dimensions[j] = points[i]->Dimensions[j];
            }
            if (points[i]->Dimensions[j] < bottom.Dimensions[j]) {
                bottom.Dimensions[j] = points[i]->Dimensions[j];
            }
        }
    }
}

struct Kmeans {
    vector<Point*> points;
    int k_clusters;
    vector<Point*> centroids;
    vector<vector<Point*>> Kgroup;
    Point bottom;
    Point top;

    Kmeans(int K) {
        k_clusters = K;
        Kgroup.resize(K);
    }

    void setPoints(vector<Point*> v) {
        points = v;
        bottom.Dimensions.resize(v[0]->Dimensions.size(), INT_MAX);
        top.Dimensions.resize(v[0]->Dimensions.size(), INT_MIN);
    }


    void assignClusters() {
        for (int i = 0; i < points.size(); i++) {
            for (int j = 0; j < centroids.size(); j++) {
                double aux = Euclidean(*points[i], *centroids[j]);
                if (aux < points[i]->ws) {
                    points[i]->ws = aux;
                    points[i]->cluster = j;
                }
            }
            Kgroup[points[i]->cluster].push_back(points[i]);
        }
    }

    void Mv_Clusters() {
        for (int i = 0; i < centroids.size(); i++) {
            Point b;
            b.Dimensions.resize(centroids[i]->Dimensions.size(), INT_MAX);
            Point t;
            t.Dimensions.resize(centroids[i]->Dimensions.size(), INT_MIN);
            maxmin(Kgroup[i], b, t);

            for (int j = 0; j < centroids[i]->Dimensions.size(); j++) {
                centroids[i]->Dimensions[j] = (b.Dimensions[j] + t.Dimensions[j]) / 2.0;
            }
        }
    }

    //crea los clusters originales
    void createClusters() {
        maxmin(points, bottom, top);
        for (int i = 0; i < k_clusters; i++) {
            centroids.push_back(new Point);
            for (int j = 0; j < points[0]->Dimensions.size(); j++) {
                centroids[i]->Dimensions.push_back(generateRandomNumber(bottom.Dimensions[j], top.Dimensions[j]));
            }
        }
    }


    void algorithm() {
        createClusters();
        int iteraciones = 0;
        vector<Point*> oldcentroid(k_clusters);
        while (true) {
            assignClusters();
            Mv_Clusters();
            if (centroidUnchanged(oldcentroid, iteraciones)) {
                break; 
            }
            iteraciones++;
            oldcentroid.clear();
            for (int i = 0; i < k_clusters; i++) {
                Point* newCentroid = new Point;
                newCentroid->Dimensions = centroids[i]->Dimensions;
                oldcentroid.push_back(newCentroid);
            }
            Kgroup.clear();
            Kgroup.resize(k_clusters);
        }
        cout << "Total iteraciones " << iteraciones;
    }

    bool centroidUnchanged(vector<Point*>& oldcluster, int isFirstIteration) {
        if (isFirstIteration == 0) {
            return false; 
        }
        for (int i = 0; i < k_clusters; i++) {
            double distance = Euclidean(*centroids[i], *oldcluster[i]);
            if (distance != 0) {
                return false; 
            }
        }
        return true; 
    }

    void printWss() {
        vector<double> Wss(k_clusters);
        for (int i = 0; i < centroids.size(); i++) {
            for (int j = 0; j < centroids[i]->Dimensions.size(); j++) {
                Wss[i] += Kgroup[i][j]->ws;
            }
        }
        double aux=0;
        cout << endl;
        for (int i = 0; i < centroids.size(); i++) {
            aux += Wss[i];
            cout <<"El cluster " << i << " tiene un wss de " << Wss[i]<<endl;
        }
        cout << "El wss total es: " << aux;
    }
};
CircleShape createCircle(Point* p, Color c, const Vector2u& windowSize, double minX, double maxX, double minY, double maxY, bool cen, int tam = 10) {
    CircleShape cir(tam);
    double scaledX = ((p->Dimensions[0] - minX) / (maxX - minX));
    double scaledY = 1.0 - ((p->Dimensions[1] - minY) / (maxY - minY));  
    float x = 50+(scaledX * (windowSize.x - 100));
    float y = 50+(scaledY * (windowSize.y-100));
    cir.setPosition(x, y);
    cir.setOrigin(tam / 2, tam / 2);  
    cir.setFillColor(c);
    if (cen == false) {
        cir.setOutlineColor(c);
    }
    else {
        cir.setOutlineColor(Color::Black);
    }
    cir.setOutlineThickness(1);  
    return cir;
}


vector<Point> readCSVFile(const string& filename) {
    vector<Point> points;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error al abrir el archivo: " << filename << endl;
        return points;
    }

    string line;
    while (getline(file, line)) {
        istringstream ss(line);
        Point p;
        string value;
        for (int i = 0; i < 4; i++) {
            if (getline(ss, value, ',')) {
                p.Dimensions.push_back(stod(value));
            }
            else {
                cerr << "Error al leer el archivo CSV: formato incorrecto" << endl;
                return points;
            }
        }
        points.push_back(p);
    }

    file.close();
    return points;
}



int main() {
    RenderWindow window(VideoMode(900, 600), "EDA");
    window.setFramerateLimit(30);
    Event event;

    //dataset de prueba
    //vector<Point*> v;


    //for (int i = 0; i < 5000; i++) {
    //    Point* p = new Point;
    //    p->Dimensions = { generateRandomNumber(0, 500), generateRandomNumber(0, 500) };
    //    v.push_back(p);
    //}

    //Kmeans kmeans(7);
    //kmeans.setPoints(v);
    //kmeans.algorithm();




    vector<Point> data = readCSVFile("dataset.csv");
    Kmeans kmeans(3); 
    vector<Point*> dataPtrs;
    for (Point& p : data) {
        dataPtrs.push_back(&p);
    }
    kmeans.setPoints(dataPtrs);
    kmeans.algorithm();


    double minX = kmeans.bottom.Dimensions[0];
    double maxX = kmeans.top.Dimensions[0];
    double minY = kmeans.bottom.Dimensions[1];
    double maxY = kmeans.top.Dimensions[1];

    kmeans.printWss();
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }

        window.clear(Color::White);
        Color c;
        for (int i = 0; i < kmeans.Kgroup.size(); i++) {
            if (i == 0) {
                c = Color::Red;
            }
            else if (i == 1) {
                c = Color::Blue;
            }
            else if (i == 2) {
                c = Color::Green;
            }
            else if (i == 3) {
                c = Color::Yellow;
            }
            else if (i == 4) {
                c = Color::Magenta;
            }
            else if (i == 5) {
                c = Color::Cyan;
            }
            else if (i == 6) {
                c = Color::Black;
            }
            for (int j = 0; j < kmeans.Kgroup[i].size(); j++) {
                window.draw(createCircle(kmeans.Kgroup[i][j], c, window.getSize(), minX, maxX, minY, maxY, false,7));
            }
            window.draw(createCircle(kmeans.centroids[i], c, window.getSize(), minX, maxX, minY, maxY, true,15));
        }

        window.display();
    }

    return 0;
}
