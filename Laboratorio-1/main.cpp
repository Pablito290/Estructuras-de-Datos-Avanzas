#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <conio.h>
#include <stack>
#include <cstdlib> // Para funciones de generación aleatoria
#include <ctime> 
using namespace sf;
using namespace std;

RenderWindow window(VideoMode(800, 600), "SFML Circle Example");
Event event;
template <class T>
struct node;

struct Circulo {
    CircleShape cir;
    pair<int, int> cord;
    Circulo(int x=0, int y=0) {
        cir.setRadius(25);
        cir.setFillColor(Color::Red);
        cord = { x,y };
    }
    void set(int x, int y) {
        cord = { x,y };
        cir.setPosition(x, y);
    }
    void draw() {
        window.draw(cir);
    }
    bool clicked(int x, int y) {
        FloatRect bounds = cir.getGlobalBounds();
        return bounds.contains(x, y);
    }
};


template <class T>
struct edge {
    int v;
    vector<node<T>*> nodes;
    edge(int _v, node<T>* n1, node<T>* n2) {
        nodes.push_back(n1);
        nodes.push_back(n2);
        v = _v;
    }
    int get_v() {
        return v;
    }
    void set_v(int _v) {
        v = _v;
    }
};

template <class T>
struct node {
    T v;    
    vector<edge<T>*> edges;
    Circulo cir;
    pair<int, int> cord;
    node(T _v,int x=0,int y=0) {
        v = _v;
    }
    void set(int x, int y) {
        cord = { x,y };
        cir.set(cord.first, cord.second);
    }
    void set_v(T _v) {
        v = _v;
    }
    T get_v() {
        return v;
    }
};

template <class T>
struct graph {
    vector<edge<T>*> edges;
    vector<node<T>*> nodes;

    void ins_node(T v) {
        node<T>* nod = new node<T>(v);
        nodes.push_back(nod);
    }

    void ins_edge(T a, T b, int v=1) {
        edge<T>* tmp = new edge<T>(v, find(a), find(b));
        edges.push_back(tmp);
        find(a)->edges.push_back(tmp);
        find(b)->edges.push_back(tmp);
    }
    void del_edge(T a,T b) {
        node<T>* n1 = find(a);
        node<T>* n2 = find(b);
        for (auto it = edges.begin(); it != edges.end(); ++it) {
            if (((*it)->nodes[0] == n1 && (*it)->nodes[1] == n2) ||
                ((*it)->nodes[0] == n2 && (*it)->nodes[1] == n1)) {
                edges.erase(it);
                break;
            }
        }
    }
    void del_node(T a) {
        // Find the node to be deleted
        node<T>* n = find(a);
        if (n == nullptr) {
            return;
        }
        // Remove all edges that connect to the node
        for (auto it = edges.begin(); it != edges.end(); ) {
            if ((*it)->nodes[0] == n || (*it)->nodes[1] == n) {
                delete* it;
                it = edges.erase(it);
            }
            else {
                ++it;
            }
        }
        // Remove the node from the nodes vector
        for (auto it = nodes.begin(); it != nodes.end(); ) {
            if (*it == n) {
                delete* it;
                it = nodes.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void print() {
        cout << "Nodos" << endl;
        for (int i = 0; i < nodes.size(); i++) {
            cout << nodes[i]->get_v() << " ";
        }
        cout << endl << "Vertices" << endl;
        for (int i = 0; i < edges.size(); i++) {
            cout << edges[i]->nodes[0]->v << " " << edges[i]->get_v() << " " << edges[i]->nodes[1]->v << endl;
        }
    }
    node<T>* find(T v) {
        for (int i = 0; i < nodes.size(); i++) {
            if (nodes[i]->v == v) {
                return nodes[i];
            }
        }
    }


    bool ciclo(vector<node<T>*> &v, node<T> *n) {
        for (int i = 0; i < v.size(); i++) {
            if (v[i] == n) {
                return true;
            }
        }
        return false;
    }

    void prints(stack<vector<node<T>*>> s) {
        while (!s.empty()) {
            std::vector<node<T>*> vec = s.top();
            s.pop();
            cout << "{";
            for (const node<T>* n : vec) {
                std::cout << n->v << " ";
            }
            cout << "} ";
        }
        cout << endl << endl << endl;
    }

    vector<node<T>*> BP(T a, T b) {
        stack<vector<node<T>*>> s;
        node<T>* ini = find(a);
        node<T>* fin = find(b);
        s.push(vector<node<T>*>{ini});

        while (s.top()[0] != fin) {
            //prints(s);
            vector<node<T>*> aux = s.top();
            s.pop();
            if (aux[0]->edges.size() != 1)
            {
                for (auto& it : aux[0]->edges) {
                    vector<node<T>*> tmp = aux;
                    if (it->nodes[0] == aux[0] and !ciclo(tmp, it->nodes[1])) {
                        tmp.insert(tmp.begin(), it->nodes[1]);
                        s.push(tmp);
                    }
                    else if (it->nodes[1] == aux[1] and !ciclo(tmp, it->nodes[0])) {
                        tmp.insert(tmp.begin(), it->nodes[0]);
                        s.push(tmp);
                    }
            }
        }
        }
        return s.top();
    }
    void setpos(T a, int x, int y) {
        node<T>* tmp = find(a);
        tmp->set(x, y);
    }
    void drawe() {

        for (int i = 0; i < edges.size(); i++) {
            VertexArray lines(LineStrip); // Crea un conjunto de líneas
            lines.append(Vertex(Vector2f(edges[i]->nodes[0]->cord.first + 25, edges[i]->nodes[0]->cord.second + 25), Color::Red));   // Punto de inicio de la línea
            lines.append(Vertex(Vector2f(edges[i]->nodes[1]->cord.first + 25, edges[i]->nodes[1]->cord.second + 25), Color::Blue));  // Punto de fin de la línea
            window.draw(lines);
        }
    }
    void drawn() {
        for (int i = 0; i < nodes.size();i++) {
            nodes[i]->cir.draw();
        }
    }
    void clicked(node<T> *&n) {
        n = nullptr;
        static bool circleClicked = false; // Keep track of whether a circle has been clicked
        if (event.type == Event::MouseButtonPressed) {
            if (event.mouseButton.button == Mouse::Left) {
                int mouseX = event.mouseButton.x;
                int mouseY = event.mouseButton.y;
                for (int i = 0; i < nodes.size(); i++) {
                    if (nodes[i]->cir.clicked(mouseX, mouseY)) {
                        if (!circleClicked) { // Only print "circle" once
                            cout << "circle: "<< nodes[i]->v<< endl;
                            n = nodes[i];
                            circleClicked = true;
                            nodes[i]->cir.cir.setFillColor(Color::Green); // Change the color of the clicked circle
                            break; // Exit the loop since we only want to change the color of one circle
                        }
                    }
                        else {
                            nodes[i]->cir.cir.setFillColor(Color::Red); // Reset the color of all other circles
                        }
                }
            }
        }
        else if (event.type == Event::MouseButtonReleased) {
            if (event.mouseButton.button == Mouse::Left) {
                circleClicked = false; // Reset the circleClicked flag when the left mouse button is released
            }
        }
    }

    void color_path(vector<node<T>*> v) {
        for (int i = 0; i < v.size() - 1; i++) {
            VertexArray lines(LineStrip); // Crea un conjunto de líneas
            lines.append(Vertex(Vector2f(v[i]->cord.first + 25, v[i]->cord.second + 25), Color::Green));   // Punto de inicio de la línea
            lines.append(Vertex(Vector2f(v[i+1]->cord.first + 25, v[i+1]->cord.second + 25), Color::Green));  // Punto de fin de la línea
            window.draw(lines);
        }
    }
};






int main() {
    graph<char> G;
    char ewe[8] = { 'a','b','c','d','e','f','s','g' };
    for (int i = 0; i < 8; i++) {
        G.ins_node(ewe[i]);
    }

    G.ins_edge('s', 'a', 3);
    G.ins_edge('s', 'd', 4);
    G.ins_edge('d', 'e', 2);
    G.ins_edge('a', 'd', 5);
    G.ins_edge('a', 'b', 4);
    G.ins_edge('e', 'b', 5);
    G.ins_edge('b', 'c', 4);
    G.ins_edge('e', 'f', 4);
    G.ins_edge('f', 'g', 3);

    G.setpos('s', 50, 200);

    G.setpos('a', 200, 100);
    G.setpos('d', 200, 300);
    G.setpos('b', 350, 100);
    G.setpos('e', 350, 300);

    G.setpos('c', 500, 100);
    G.setpos('f', 500, 300);

    G.setpos('g', 650, 200);
    cout << "-----------------------" << endl;
    G.print();

    auto x = G.BP('s', 'g');
    for (int i = x.size() - 1; i >= 0; --i) {
        cout << x[i]->v << "->";
    }

    /*
    graph<int> a;
    for (int i = 0; i < 23; i++) {
        a.ins_node(i);
    }
    a.ins_edge(1, 4);   //
    a.ins_edge(1, 3);
    a.ins_edge(1, 2);
    a.ins_edge(2, 5);
    a.ins_edge(2, 6);
    a.ins_edge(5, 11);
    a.ins_edge(11, 18);
    a.ins_edge(11, 19);
    a.ins_edge(11, 20);
    a.ins_edge(3, 7);
    a.ins_edge(3, 8);
    a.ins_edge(7, 12);
    a.ins_edge(7, 13);
    a.ins_edge(8, 14);
    a.ins_edge(4, 9);
    a.ins_edge(4, 10);
    a.ins_edge(10, 15);
    a.ins_edge(10, 16);
    a.ins_edge(10, 17);
    a.ins_edge(17, 21);
    a.ins_edge(17, 22);

    auto x = a.BP(1, 15);
    for (int i = x.size() - 1; i >= 0; --i) {
        cout << x[i]->v << "->";
    }*/

    bool waitForClick = true;
    node<char>* aux = nullptr;
    while (window.isOpen())
    {

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            if (waitForClick && event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                // Detecta el clic y cambia el estado
                waitForClick = false;
            }
        }


        window.clear();
        G.drawn();
        G.drawe();
        G.clicked(aux); // Pasa aux como parámetro
        G.color_path(x); // Dibuja el camino cuando aux no es nulo
        if (!waitForClick) {
            cout << "AUX==" << aux->v << endl;
            waitForClick = true;
        }

        window.display();
    }

    return 0;
}
