#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVertexGlyphFilter.h>

#include <vtkNamedColors.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <stack>
#include <sstream>
#include <vtkCubeSource.h>

using namespace std;

int PO = 1;

struct Point {
    double x;
    double y;
    double z;
    Point(double _x = 0, double _y = 0, double _z = 0) {
        x = _x;
        y = _y;
        z = _z;
    }
    void print() {
        cout << "x=" << x << " y=" << y << " z=" << z;
    }

};
// Función para leer un archivo CSV y almacenar los puntos en un vector
vector<Point> ReadCSV(const string& filename) {
    vector<Point> points;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << filename << endl;
        return points;
    }

    string line;
    while (getline(file, line)) {
        // Dividir la línea en tokens usando ';' como delimitador
        stringstream ss(line);
        string token;
        vector<int> tokens;
        while (getline(ss, token, ';')) {
            int value = stoi(token); // Convertir el token a entero
            tokens.push_back(value);
        }

        // Verificar si se obtuvieron suficientes tokens (x, y, z)
        if (tokens.size() == 3) {
            Point p(tokens[0], tokens[1], tokens[2]);
            points.push_back(p);
            cout<<p.x<<" "<<p.y<<" "<<p.z<<endl;
        }
        else {
            cerr << "Advertencia: Se omitió una línea con un formato incorrecto en el archivo." << endl;
        }
    }

    file.close();
    return points;
}

struct Octal {
    Point bottomLeft;
    double h = 0;
    vector<Octal*> Childrens;
    vector<Point> hojas;
    Octal(Point bl, double _h) {
        Childrens.resize(8, nullptr);
        bottomLeft = bl;
        h = _h;
    }

    void add_point(Point p) {
        hojas.push_back(p);
    }

    void newOctans() {
        Childrens[0] = new Octal(bottomLeft, h / 2); // 0 0
        Childrens[1] = new Octal(Point(bottomLeft.x + (h / 2), bottomLeft.y, bottomLeft.z), h / 2); //x
        Childrens[2] = new Octal(Point(bottomLeft.x, bottomLeft.y, bottomLeft.z + (h / 2)), h / 2); //z
        Childrens[3] = new Octal(Point(bottomLeft.x + (h / 2), bottomLeft.y, bottomLeft.z + (h / 2)), h / 2); //x z
        Childrens[4] = new Octal(Point(bottomLeft.x, bottomLeft.y + (h / 2), bottomLeft.z), h / 2); //y
        Childrens[5] = new Octal(Point(bottomLeft.x + (h / 2), bottomLeft.y + (h / 2), bottomLeft.z), h / 2); //x y
        Childrens[6] = new Octal(Point(bottomLeft.x, bottomLeft.y + (h / 2), bottomLeft.z + (h / 2)), h / 2); // y z
        Childrens[7] = new Octal(Point(bottomLeft.x + (h / 2), bottomLeft.y + (h / 2), bottomLeft.z + (h / 2)), h / 2); //x y z
    }

    void asignOctan() {
        if (hojas.size() > PO or Childrens[0]) {
            if (!Childrens[0]) {
                newOctans();
            }
            double midx = bottomLeft.x + (h / 2);
            double midy = bottomLeft.y + (h / 2);
            double midz = bottomLeft.z + (h / 2);
            for (auto& it : hojas) {
                if (it.x <= midx and it.y <= midy and it.z <= midz) { Childrens[0]->hojas.push_back(it); Childrens[0]->asignOctan(); }
                else if (it.x > midx and it.y <= midy and it.z <= midz) { Childrens[1]->hojas.push_back(it); Childrens[1]->asignOctan(); }
                else if (it.x <= midx and it.y <= midy and it.z > midz) { Childrens[2]->hojas.push_back(it); Childrens[2]->asignOctan(); }
                else if (it.x > midx and it.y <= midy and it.z > midz) { Childrens[3]->hojas.push_back(it); Childrens[3]->asignOctan(); }
                else if (it.x <= midx and it.y > midy and it.z <= midz) { Childrens[4]->hojas.push_back(it); Childrens[4]->asignOctan(); }
                else if (it.x > midx and it.y > midy and it.z <= midz) { Childrens[5]->hojas.push_back(it); Childrens[5]->asignOctan(); }
                else if (it.x <= midx and it.y > midy and it.z > midz) { Childrens[6]->hojas.push_back(it); Childrens[6]->asignOctan(); }
                else if (it.x > midx and it.y > midy and it.z > midz) { Childrens[7]->hojas.push_back(it); Childrens[7]->asignOctan(); }
            }
            hojas.clear();
        }
        else {
            return;
        }
    }

};

struct OcTree {
    Octal* root = nullptr;
    vector<Point> puntos;
    double dif = 0;

    void setpuntos(vector<Point> p) {
        puntos = p;
    }

    Point maxmin() {
        int maxx = INT_MIN;
        int maxy = INT_MIN;
        int maxz = INT_MIN;
        int minx = INT_MAX;
        int miny = INT_MAX;
        int minz = INT_MAX;
        for (auto& it : puntos) {
            if (it.x < minx) minx = it.x;
            if (it.y < miny) miny = it.y;
            if (it.z < minz) minz = it.z;

            if (it.x > maxx) maxx = it.x;
            else if (it.y > maxy) maxy = it.y;
            else if (it.z > maxz) maxz = it.z;

        }
        if (abs(minx - maxx) > dif) dif = abs(minx - maxx);
        if (abs(miny - maxy) > dif) dif = abs(miny - maxy);
        if (abs(minz - maxz) > dif) dif = abs(minz - maxz);
        Point min(minx, miny, minz);

        return min;

    }

    Octal* OctalContainer(Point p, double radius) {
        Octal* tmp = root;
        while (tmp and tmp->h > radius) {

            double midx = tmp->bottomLeft.x + (tmp->h / 2);
            double midy = tmp->bottomLeft.y + (tmp->h / 2);
            double midz = tmp->bottomLeft.z + (tmp->h / 2);
            if (tmp->Childrens[0] and p.x <= midx and p.y <= midy and p.z <= midz) { tmp = tmp->Childrens[0]; }
            else if (tmp->Childrens[1] and p.x > midx and p.y <= midy and p.z <= midz) { tmp = tmp->Childrens[1]; }
            else if (tmp->Childrens[2] and p.x <= midx and p.y <= midy and p.z > midz) { tmp = tmp->Childrens[2]; }
            else if (tmp->Childrens[3] and p.x > midx and p.y <= midy and p.z > midz) { tmp = tmp->Childrens[3]; }
            else if (tmp->Childrens[4] and p.x <= midx and p.y > midy and p.z <= midz) { tmp = tmp->Childrens[4]; }
            else if (tmp->Childrens[5] and p.x > midx and p.y > midy and p.z <= midz) { tmp = tmp->Childrens[5]; }
            else if (tmp->Childrens[6] and p.x <= midx and p.y > midy and p.z > midz) { tmp = tmp->Childrens[6]; }
            else if (tmp->Childrens[7] and p.x > midx and p.y > midy and p.z > midz) { tmp = tmp->Childrens[7]; }
            else {
                return nullptr;
            }
        }
        return tmp;
    }

    Octal* Find(Point p) {
        Octal* tmp = root;
        while (tmp) {
            for (int i = 0; i < tmp->hojas.size(); i++) {
                if (tmp->hojas[i].x == p.x and tmp->hojas[i].y == p.y and tmp->hojas[i].z == p.z) {
                    return tmp;
                }
            }
            double midx = tmp->bottomLeft.x + (tmp->h / 2);
            double midy = tmp->bottomLeft.y + (tmp->h / 2);
            double midz = tmp->bottomLeft.z + (tmp->h / 2);
            if (tmp->Childrens[0] and p.x <= midx and p.y <= midy and p.z <= midz) { tmp = tmp->Childrens[0]; }
            else if (tmp->Childrens[1] and p.x > midx and p.y <= midy and p.z <= midz) { tmp = tmp->Childrens[1]; }
            else if (tmp->Childrens[2] and p.x <= midx and p.y <= midy and p.z > midz) { tmp = tmp->Childrens[2]; }
            else if (tmp->Childrens[3] and p.x > midx and p.y <= midy and p.z > midz) { tmp = tmp->Childrens[3]; }
            else if (tmp->Childrens[4] and p.x <= midx and p.y > midy and p.z <= midz) { tmp = tmp->Childrens[4]; }
            else if (tmp->Childrens[5] and p.x > midx and p.y > midy and p.z <= midz) { tmp = tmp->Childrens[5]; }
            else if (tmp->Childrens[6] and p.x <= midx and p.y > midy and p.z > midz) { tmp = tmp->Childrens[6]; }
            else if (tmp->Childrens[7] and p.x > midx and p.y > midy and p.z > midz) { tmp = tmp->Childrens[7]; }
            else {
                return NULL;
            }
        }
        return NULL;
    }

    void Leafs(Octal* p, vector<Point>& hojas) {
        if (!p) {
            return;
        }
        if (!p->hojas.empty()) {
            for (const Point& hoja : p->hojas) {
                hojas.push_back(hoja);
            }
        }
        for (Octal* suboctal : p->Childrens) {
            Leafs(suboctal, hojas);
        }
    }
    void OctalLeafs_R(Octal* p, vector<Octal*>& v) {
        if (!p) { return; }
        if (!p->hojas.empty()) {
            v.push_back(p);
        }
        for (Octal* suboctal : p->Childrens) {
            OctalLeafs_R(suboctal, v);
        }
    }
    void OctalLeafs(vector<Octal*>& v) {
        Octal* tmp = root;
        OctalLeafs_R(tmp, v);
    }
    Point find_closest(Point p, double radius) {
        Octal* tmp = OctalContainer(p, radius);
        vector<Point> Candidatos;
        Leafs(tmp, Candidatos);
        double min = INT_MAX;
        Point Closest;
        for (auto& it : Candidatos) {
            double calc = calc = distance(p, it);
            if (calc < min and calc != 0 and calc < radius) {
                Closest = it;
                min = calc;
            }
        }
        return Closest;
    }

    void insert(Point p) {
        if (!root) {
            root = new Octal(maxmin(), dif);
            root->add_point(p);
        }
        else if (!Find(p)) {
            root->hojas.push_back(p);
            root->asignOctan();
        }
    }

    double distance(const Point& p1, const Point& p2) {
        int dx = p1.x - p2.x;
        int dy = p1.y - p2.y;
        int dz = p1.z - p2.z;
        return sqrt(dx * dx + dy * dy + dz * dz);
    }

    void make() {
        int iteracion = 0;
        for (int i = 0; i < puntos.size(); i++, iteracion++) {
            insert(puntos[i]);
        }
    }
};
vtkNew<vtkActor> drawCube(double x, double y, double z, double h) {
    std::array<std::array<double, 3>, 8> pts = {
            {{x, y, z},
             {x + h, y, z},
             {x + h, y + h, z},
             {x, y + h, z},
             {x, y, z + h},
             {x + h, y, z + h},
             {x + h, y + h, z + h},
             {x, y + h, z + h}}};

    std::array<std::array<vtkIdType, 4>, 6> ordering = {
            {{0, 3, 2, 1},
             {4, 5, 6, 7},
             {0, 1, 5, 4},
             {1, 2, 6, 5},
             {2, 3, 7, 6},
             {3, 0, 4, 7}}};

    vtkNew<vtkPolyData> cube;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> polys;
    vtkNew<vtkFloatArray> scalars;

    for (auto i = 0ul; i < pts.size(); ++i) {
        points->InsertPoint(i, pts[i].data());
        scalars->InsertTuple1(i, i);
    }

    for (auto&& i : ordering) {
        polys->InsertNextCell(vtkIdType(i.size()), i.data());
    }

    cube->SetPoints(points);
    cube->SetPolys(polys);
    cube->GetPointData()->SetScalars(scalars);

    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputData(cube);
    cubeMapper->SetScalarRange(cube->GetScalarRange());

    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);

    return cubeActor;
}

void drawCubesFromPoints(vector<Octal*> octal) {
    // Crear un objeto vtkRenderer.
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    // Iterar sobre el vector de pares de puntos.
    for (auto pointPair : octal) {
        // Crear un objeto vtkCubeSource.
        vtkSmartPointer<vtkCubeSource> cubeSource = vtkSmartPointer<vtkCubeSource>::New();

        // Establecer los puntos opuestos del cubo.
        double center[3] = {(pointPair->bottomLeft.x+pointPair->bottomLeft.x+pointPair->h) / 2.0, (pointPair->bottomLeft.y+pointPair->bottomLeft.y+pointPair->h) / 2.0, (pointPair->bottomLeft.z+pointPair->bottomLeft.z+pointPair->h) / 2.0};
        double bounds[6] = {pointPair->bottomLeft.x  , pointPair->bottomLeft.x+pointPair->h, pointPair->bottomLeft.y, pointPair->bottomLeft.y+pointPair->h, pointPair->bottomLeft.z, pointPair->bottomLeft.z+pointPair->h};
        cubeSource->SetCenter(center);
        cubeSource->SetBounds(bounds);

        // Crear un objeto vtkPolyDataMapper y establecer el objeto vtkCubeSource como entrada.
        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(cubeSource->GetOutputPort());

        // Crear un objeto vtkActor y establecer el objeto vtkPolyDataMapper como entrada.
        vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);

        // Agregar el objeto vtkActor al objeto vtkRenderer.
        renderer->AddActor(actor);
    }

    // Crear un objeto vtkRenderWindow y agregar el objeto vtkRenderer.
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    // Crear un objeto vtkRenderWindowInteractor y establecer el objeto vtkRenderWindow como entrada.
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);

    // Visualizar el objeto vtkRenderWindow.
    renderWindow->Render();
    interactor->Start();
}
int main() {
    OcTree octree;
    string filename = "../Puntos1.csv"; // Reemplaza con el nombre de tu archivo CSV
    vector<Point> points = ReadCSV(filename);

    octree.setpuntos(points);
    octree.make();
    vector<Octal*> octales;
    octree.OctalLeafs(octales);
    drawCubesFromPoints(octales);
    cout << "yei";
}
