
//3D Point Cloud Visualization
//Filename: main.cpp
//Release: 1.0 (version ECM)
//*********************************************************
//Librairie STL
#include <iostream>
#include <vector>
#include <utility>
#include <limits>
#include <math.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <string.h>
#include <stdio.h>
#include <iomanip>
#include <time.h>
#include <stdlib.h>
//********************************************************
#include "GL/glut.h"
#include <GL/glu.h>
#include <GL/gl.h>

//********************************************************


using namespace std;
int nombreDeDivisions = 30;
struct Point3DRgb {
    double x, y, z;
    unsigned char r, g, b;
};



template<class T>
class point3D
{

public:
    T x;
    T y;
    T z;
    T r;
    T g;
    T b;
    point3D();
    point3D(T& a,T& b,T& c,T& d,T& e,T& f);
};


template<class T>
point3D<T>::point3D():
        x(0),
        y(0),
        z(0)
{
}

template<class T>
point3D<T>::point3D(T& a,T& b,T& c,T& d,T& e,T& f):
        x(a),
        y(b),
        z(c),
        r(d),
        g(e),
        b(f)
{
}
//********************************************************
template<class T>
class point2D
{

public:
    point2D(const double d, const double d1) {

    }

    T x;
    T y;
    point2D();
    point2D(T& a,T& b);
};


template<class T>
point2D<T>::point2D():
        x(0),
        y(0)
{
}

template<class T>
point2D<T>::point2D(T& a,T& b):
        x(a),
        y(b)
{
}




point3D<double> pointTemp,barycentre;
vector< point3D<double> > nuage;
double Xmin,Ymin,Zmin,Xmax,Ymax,Zmax;
// Nouvelle méthode à ajouter dans votre main.cpp
vector<point3D<double>> calculerElevationParQuadrilateres(const vector<point3D<double>>& nuageInitial) {
    vector<point3D<double>> nuageTraite = nuageInitial;

    double longueurX = Xmax - Xmin;
    double longueurY = Ymax - Ymin;
    double longueurZ = Zmax - Zmin;

    double plusPetiteLongueur = std::min(longueurX, longueurY);
    double pas = plusPetiteLongueur / nombreDeDivisions;
    // Parcourir chaque quadrilatère
    for (double x = Xmin; x < Xmax; x += pas) {
        for (double z = Ymin; z < Ymax; z += pas) {
            // Définir les limites du quadrilatère

            double quadX_min = x;
            double quadX_max = x + pas;
            double quadZ_min = z;
            double quadZ_max = z + pas;

            // Collecter les points dans ce quadrilatère
            vector<point3D<double>> pointsDuQuadrilatere;
            for (auto& point : nuageTraite) {
                // Vérifier si le point est dans le quadrilatère
                if (point.x >= quadX_min && point.x < quadX_max &&
                    point.y >= quadZ_min && point.y < quadZ_max) {
                    pointsDuQuadrilatere.push_back(point);
                }
            }

            // Calculer l'élévation moyenne si des points sont présents
            if (!pointsDuQuadrilatere.empty()) {
                double elevationMoyenne = 0.0;
                for (const auto& point : pointsDuQuadrilatere) {
                    elevationMoyenne += point.z;
                }
                elevationMoyenne /= pointsDuQuadrilatere.size();

                // Modifier l'élévation des points dans ce quadrilatère
                for (auto& point : pointsDuQuadrilatere) {
                    point.z = elevationMoyenne;
                }

                // Mettre à jour les points dans le nuage original
                for (auto& point : nuageTraite) {
                    if (point.x >= quadX_min && point.x < quadX_max &&
                        point.y >= quadZ_min && point.y < quadZ_max) {
                        point.z = elevationMoyenne;
                    }
                }
            }
        }
    }

    return nuageTraite;
}

// Calcul des min, max et barycentre
void calculExtremumsEtBarycentre() {
    // Initialisation des valeurs min/max
    Xmin = Ymin = Zmin = numeric_limits<double>::max();       // Le plus grand nombre possible
    Xmax = Ymax = Zmax = numeric_limits<double>::lowest();    // Le plus petit nombre possible

    // Variables de sommation pour le barycentre
    double sommeX = 0, sommeY = 0, sommeZ = 0;
    size_t nombrePoints = nuage.size();

    // Parcourir tous les points du nuage
    for (const auto& point : nuage) {
        // Met à jour les extrêmes
        if (point.x < Xmin) Xmin = point.x;
        if (point.y < Ymin) Ymin = point.y;
        if (point.z < Zmin) Zmin = point.z;

        if (point.x > Xmax) Xmax = point.x;
        if (point.y > Ymax) Ymax = point.y;
        if (point.z > Zmax) Zmax = point.z;

        // Sommes pour le calcul du barycentre
        sommeX += point.x;
        sommeY += point.y;
        sommeZ += point.z;
    }

    // Calcul du barycentre
    if (nombrePoints > 0) {
        barycentre.x = sommeX / nombrePoints;
        barycentre.y = sommeY / nombrePoints;
        barycentre.z = sommeZ / nombrePoints;
    } else {
        // Si le nuage est vide
        barycentre.x = barycentre.y = barycentre.z = 0;
    }
}


//*********************************************
//fonctions d'affichage des donn es avec GLUT
//*********************************************
void Display();
void Init();
void Reshape (int width, int height);
void Idle();
void Keyboard (unsigned char, int, int);
void ShowInfos();

//vue de face
float angle = 92;
float vitesse = 0;
char axe = 'y';
int perspective=20;

void Display()
{


    glClearColor(0, 0, 0, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(4,3,3,0,0,0,0,1,0);

    switch (axe)
    {
        case 'x': glRotated(angle,1,0,0); break;
        case 'y': glRotated(angle,0,1,0); break;
        case 'z': glRotated(angle,0,0,1); break;
    }
    double rangeX = Xmax - Xmin;
    double rangeY = Ymax - Ymin;
    double rangeZ = Zmax - Zmin;
//affichage du nuage de points
    double scale = 50;

    scale = std::max({rangeX, rangeY, rangeZ}) / 2.0;    glEnable(GL_COLOR_MATERIAL);




//3D point cloud
    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3d(0,0,1);
    for(vector<point3D<double> >::iterator j=nuage.begin();j!=nuage.end();++j)
    {
        pointTemp=*j;
        glVertex3f((pointTemp.x-barycentre.x)/scale, (pointTemp.z-barycentre.z)/scale, (pointTemp.y-barycentre.y)/scale);
    }
    glEnd();

//centre
    glPointSize(5);
    glBegin(GL_POINTS);
    glColor3d(1,0,0);
    glVertex3f(0, 0, 0);
    glEnd();

//bounding box 2D
//    glPointSize(2);
//    glBegin(GL_LINE_STRIP);
//    glColor3d(1,1,1);
//    glVertex3f((Xmin-barycentre.x)/scale,(Zmin-barycentre.z)/scale,(Ymin-barycentre.y)/scale);;
//    glVertex3f((Xmax-barycentre.x)/scale,(Zmin-barycentre.z)/scale,(Ymin-barycentre.y)/scale);;
//    glVertex3f((Xmax-barycentre.x)/scale,(Zmin-barycentre.z)/scale,(Ymax-barycentre.y)/scale);;
//    glVertex3f((Xmin-barycentre.x)/scale,(Zmin-barycentre.z)/scale,(Ymax-barycentre.y)/scale);;
//    glVertex3f((Xmin-barycentre.x)/scale,(Zmin-barycentre.z)/scale,(Ymin-barycentre.y)/scale);;
//    glEnd();
    // Affichage du barycentre (point rouge)
    glPointSize(5);
    glBegin(GL_POINTS);
    glColor3d(1, 0, 0); // Rouge pour le barycentre
    glVertex3f(0, 0, 0);
    glEnd();
    //initialisation du générateur de nombres aléatoires
    srand(42);

    glBegin(GL_TRIANGLES);

    double longueurX = Xmax - Xmin;
    double longueurY = Ymax - Ymin;
    double longueurZ = Zmax - Zmin;
    double pasX = longueurX / nombreDeDivisions;
    double pasY = longueurY / nombreDeDivisions;
    double plusPetiteLongueur = std::min(longueurX, longueurY);

    double pas = plusPetiteLongueur / nombreDeDivisions;

// Calculer les élévations des quadrilatères
    vector<vector<double>> elevationsQuadrilateres(nombreDeDivisions,
                                                   vector<double>(nombreDeDivisions, Zmin));
    vector<vector<float>> couleursMoyennesR(nombreDeDivisions,
                                            vector<float>(nombreDeDivisions, 0.0f));
    vector<vector<float>> couleursMoyennesG(nombreDeDivisions,
                                            vector<float>(nombreDeDivisions, 0.0f));
    vector<vector<float>> couleursMoyennesB(nombreDeDivisions,
                                            vector<float>(nombreDeDivisions, 0.0f));
    vector<vector<int>> compteurPoints(nombreDeDivisions,
                                       vector<int>(nombreDeDivisions, 0));


    for (const auto& point : nuage) {
        int indexX = static_cast<int>((point.x - Xmin) / pasX);
        int indexY = static_cast<int>((point.y - Ymin) / pasY);

        // Vérifier les bornes
        indexX = std::max(0, std::min(indexX, nombreDeDivisions - 1));
        indexY = std::max(0, std::min(indexY, nombreDeDivisions - 1));

        if (indexX >= 0 && indexX < nombreDeDivisions &&
            indexY >= 0 && indexY < nombreDeDivisions) {
            elevationsQuadrilateres[indexX][indexY] = std::max(
                    elevationsQuadrilateres[indexX][indexY],
                    point.z
            );
            // Accumulation des couleurs
            couleursMoyennesR[indexX][indexY] += point.r;
            couleursMoyennesG[indexX][indexY] += point.g;
            couleursMoyennesB[indexX][indexY] += point.b;
            compteurPoints[indexX][indexY]++;

        }
    }

// Remplissage des cases avec des couleurs aléatoires
    for (int i = 0; i < nombreDeDivisions; i++) {
        for (int j = 0; j < nombreDeDivisions; j++) {
            double x = Xmin + i * pasX;
            double y = Ymin + j * pasY;
            double zQuadrilatere = elevationsQuadrilateres[i][j];

            // Calcul de la couleur moyenne
            float r = 0.0f, g = 0.0f, b = 0.0f;

            if (compteurPoints[i][j] > 0) {
                // Conversion et normalisation des couleurs
                r = static_cast<float>(couleursMoyennesR[i][j] / compteurPoints[i][j]) / 255.0f;
                g = static_cast<float>(couleursMoyennesG[i][j] / compteurPoints[i][j]) / 255.0f;
                b = static_cast<float>(couleursMoyennesB[i][j] / compteurPoints[i][j]) / 255.0f;
            }

            // Forcer les valeurs de couleur dans la plage [0, 1]
            r = std::max(0.0f, std::min(r, 1.0f));
            g = std::max(0.0f, std::min(g, 1.0f));
            b = std::max(0.0f, std::min(b, 1.0f));



// Premier triangle (P1, P2, P3)
            glColor3f(r,g, b);
            glVertex3f((x - barycentre.x) / scale, (zQuadrilatere - barycentre.z) / scale, (y - barycentre.y) / scale);
            glVertex3f((x + pas - barycentre.x) / scale, (zQuadrilatere - barycentre.z) / scale, (y - barycentre.y) / scale);
            glVertex3f((x + pas - barycentre.x) / scale, (zQuadrilatere - barycentre.z) / scale, (y + pas - barycentre.y) / scale);

// Deuxième triangle (P1, P3, P4)
            glColor3f(r, g, b);
            glVertex3f((x - barycentre.x) / scale, (zQuadrilatere - barycentre.z) / scale, (y - barycentre.y) / scale);
            glVertex3f((x + pas - barycentre.x) / scale, (zQuadrilatere - barycentre.z) / scale, (y + pas - barycentre.y) / scale);
            glVertex3f((x - barycentre.x) / scale, (zQuadrilatere - barycentre.z) / scale, (y + pas - barycentre.y) / scale);

        }
    }

    glEnd();




    // Tracé de la boîte englobante
    glColor3d(1, 1, 1); // Couleur blanche pour la boîte
    glBegin(GL_LINES);

// Bas de la boîte (Zmin)
    glVertex3f((Xmin - barycentre.x) / scale, (Zmin - barycentre.z) / scale, (Ymin - barycentre.y) / scale);
    glVertex3f((Xmax - barycentre.x) / scale, (Zmin - barycentre.z) / scale, (Ymin - barycentre.y) / scale);

    glVertex3f((Xmax - barycentre.x) / scale, (Zmin - barycentre.z) / scale, (Ymin - barycentre.y) / scale);
    glVertex3f((Xmax - barycentre.x) / scale, (Zmin - barycentre.z) / scale, (Ymax - barycentre.y) / scale);

    glVertex3f((Xmax - barycentre.x) / scale, (Zmin - barycentre.z) / scale, (Ymax - barycentre.y) / scale);
    glVertex3f((Xmin - barycentre.x) / scale, (Zmin - barycentre.z) / scale, (Ymax - barycentre.y) / scale);

    glVertex3f((Xmin - barycentre.x) / scale, (Zmin - barycentre.z) / scale, (Ymax - barycentre.y) / scale);
    glVertex3f((Xmin - barycentre.x) / scale, (Zmin - barycentre.z) / scale, (Ymin - barycentre.y) / scale);

    // Haut de la boîte (Zmax)
    glVertex3f((Xmin - barycentre.x) / scale, (Zmax - barycentre.z) / scale, (Ymin - barycentre.y) / scale);
    glVertex3f((Xmax - barycentre.x) / scale, (Zmax - barycentre.z) / scale, (Ymin - barycentre.y) / scale);

    glVertex3f((Xmax - barycentre.x) / scale, (Zmax - barycentre.z) / scale, (Ymin - barycentre.y) / scale);
    glVertex3f((Xmax - barycentre.x) / scale, (Zmax - barycentre.z) / scale, (Ymax - barycentre.y) / scale);

    glVertex3f((Xmax - barycentre.x) / scale, (Zmax - barycentre.z) / scale, (Ymax - barycentre.y) / scale);
    glVertex3f((Xmin - barycentre.x) / scale, (Zmax - barycentre.z) / scale, (Ymax - barycentre.y) / scale);

    glVertex3f((Xmin - barycentre.x) / scale, (Zmax - barycentre.z) / scale, (Ymax - barycentre.y) / scale);
    glVertex3f((Xmin - barycentre.x) / scale, (Zmax - barycentre.z) / scale, (Ymin - barycentre.y) / scale);

    // Lignes verticales (sides)
    glVertex3f((Xmin - barycentre.x) / scale, (Zmin - barycentre.z) / scale, (Ymin - barycentre.y) / scale);
    glVertex3f((Xmin - barycentre.x) / scale, (Zmax - barycentre.z) / scale, (Ymin - barycentre.y) / scale);

    glVertex3f((Xmax - barycentre.x) / scale, (Zmin - barycentre.z) / scale, (Ymin - barycentre.y) / scale);
    glVertex3f((Xmax - barycentre.x) / scale, (Zmax - barycentre.z) / scale, (Ymin - barycentre.y) / scale);

    glVertex3f((Xmax - barycentre.x) / scale, (Zmin - barycentre.z) / scale, (Ymax - barycentre.y) / scale);
    glVertex3f((Xmax - barycentre.x) / scale, (Zmax - barycentre.z) / scale, (Ymax - barycentre.y) / scale);

    glVertex3f((Xmin - barycentre.x) / scale, (Zmin - barycentre.z) / scale, (Ymax - barycentre.y) / scale);
    glVertex3f((Xmin - barycentre.x) / scale, (Zmax - barycentre.z) / scale, (Ymax - barycentre.y) / scale);

    glEnd();


    glutSwapBuffers();
//glFlush();// ATTENTION : si on flush au lieu de swapper tout l'ordi plante !
}

void Init()
{
    glEnable(GL_DEPTH_TEST); // activation du test de Z-Buffering
    glutSetCursor(GLUT_CURSOR_NONE); // curseur invisible
}

void Reshape (int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(perspective, (float) width/height, 1.0 /*0.001*/, 10 /*100000*/);
}

void Idle()
{
    angle = angle + vitesse;
    if (angle > 360) angle=0;
    glutPostRedisplay();
}

void Keyboard (unsigned char key, int x, int y)
{
    //printf("%d\n", key);
    switch (key)
    {

        case 27: // 'ESC'
            printf("\nFermeture en cours...\n");
            exit(0);
            break;

        case 120: // 'x'
            axe = 'x';
            ShowInfos();
            break;

        case 121: // 'y'
            axe = 'y';
            ShowInfos();
            break;

        case 122: // 'z'
            axe = 'z';
            ShowInfos();
            break;

        case 42: // '*'
            vitesse = -vitesse;
            ShowInfos();
            break;

        case 43: // '+'
            if (vitesse < 359.9) vitesse += 0.1;
            ShowInfos();
            break;

        case 45: // '-'
            if (vitesse > 0.1) vitesse -= 0.1;
            ShowInfos();
            break;

        case 48: // '0'
            perspective--;
            Reshape(640,480);
            ShowInfos();
            break;

        case 49: // '1'
            perspective++;
            Reshape(640,480);
            ShowInfos();
            break;

        case 127: // 'DEL'
            axe = 'y';
            vitesse = 0.1 ;
            perspective = 40;/*45*/
            Reshape(640,480);
            ShowInfos();
            break;
    }
}

void ShowInfos()
{
    system("cls");
    printf("+ augmente la vitesse\n");
    printf("- diminue la vitesse\n");
    printf("1 augmente la perspective\n");
    printf("0 diminue la perspective\n");
    printf("* inverse le sens de rotation\n");
    printf("x, y et z pour changer l'axe de rotation\n");
    printf("DEL pour restaurer les parametres par defaut\n");
    printf("ESC pour quitter\n\n");
    printf("Vitesse de rotation : %.1f\n", vitesse);
    printf("Axe de rotation : %c\n", axe);
    printf("Perspective : %d\n", perspective);
}
//********************************************************

int main(int argc, char **argv)
//int main(int argc, char argv[])
{
//declare variables 
int nombreDimension = 30;
/* Fen tre GLUT*/
    glutInit(&argc, argv);

    point3D<double> pointTemp;

    int compteur=0;
    string ligne;
    double a,b,c,d,e,f,g,h;
    nuage.reserve(400000);

//*********************************

    ifstream fichier("C:\\Users\\awilhelm\\CLionProjects\\GenET_enrchssement\\st-helens.txt");
    if ( !fichier )
    {
        cout << "fichier inexistant" << endl;
    }
    else
    {
        while( !fichier.eof() )
        {
            getline(fichier, ligne);
            sscanf(ligne.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf", &a, &b, &c, &d, &e, &f,&g, &h);
            //sscanf_s(ligne.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf", &a, &b, &c, &d, &e, &f,&g, &h);
            //cout << "a=" << a << ", b=" << b << ", c=" << c << ", d=" << d << ", e=" << e << ", f=" << f << ", g=" << g << ", h=" << h << endl;
            point3D<double> pointTemp;
            //b coordonnees en x
            //c coordonnees en y
            //d coordonnees en z
            pointTemp.x=a;
            pointTemp.y=b;
            pointTemp.z=c;
            pointTemp.r=d;
            pointTemp.g=e;
            pointTemp.b=f;
            nuage.push_back(pointTemp);


            ++compteur;
        }
    }
    fichier.close();
    cout  << "fichier xyz charge" << endl;
    calculExtremumsEtBarycentre();
    nuage = calculerElevationParQuadrilateres(nuage);
    calculExtremumsEtBarycentre();

    cout << "nb de points dans le nuage : " << compteur << endl;


    cout << "coordonnees du barycentre : (" << barycentre.x << "," << barycentre.y <<  "," << barycentre.z << ")" << endl;
    cout << "Xmin : " << Xmin << endl;
    cout << "Ymin : " << Ymin << endl;
    cout << "Zmin : " << Zmin << endl;
    cout << "Xmax : " << Xmax << endl;
    cout << "Ymax : " << Ymax << endl;
    cout << "Zmax : " << Zmax << endl;


    ::nuage;

//-------------------------------------------------------------------------
//Affichage SGL
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(40,40);

    glutCreateWindow("3D Point Cloud Visualization (ECM)");
//glutFullScreen();

    Init();
    glutDisplayFunc(Display);

    glutReshapeFunc(Reshape);
    glutIdleFunc(Idle);
    glutKeyboardFunc(Keyboard);

//ShowInfos();
    glutMainLoop();
//------------------------------------------------------------------------

    return 0;
}