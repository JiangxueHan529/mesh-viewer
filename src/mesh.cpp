
#include "mesh.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <cmath>
#include <iostream>
#include<fstream>
#include <sstream>

using namespace std;
using namespace glm;
using namespace agl;

Mesh::Mesh() 
{
    min_bound = vec3(0);
    max_bound = vec3(0);
    num_vertices = 0;
    num_faces = 0;
    position = new float[0];
    normal = new float[0];
    indice = new unsigned int[0];
}

Mesh::~Mesh()
{
    delete[] position;
    delete[] normal;
    delete[] indice;
}

bool Mesh::loadPLY(const std::string& filename)
{
    ifstream file(filename.c_str());
    if (!file) // true if the file is valid
    {
        cout << "Cannot load file: " << filename << endl;
        return false;
    }
    delete[] position;
    delete[] normal;
    delete[] indice;
    string read;
    file >> read;
    if (read != "ply") {
        cout << "file format error: " << endl;
        return false;
    }

    for (int i = 0; i < 3; ++i) {
        getline(file, read);
    }

    file >> read;
    file >> read;
    file>> num_vertices;

    vector<string> shapes = { "cube", "sphere",  "pyramid" };
    bool more_lines = false;
    for (string& shape : shapes) {
        if (filename.find(shape) != string::npos) {
            more_lines = true;
        }
    }
    int num_line;
    if (more_lines) {
        num_line = 9;
    }
    else {
        num_line = 7;
    }
    for (int i = 0; i < num_line; i++) {
        getline(file, read);
    }
    file >> read;
    file >> read;
    file>> num_faces;

    for (int i = 0; i < 3; i++) {
        getline(file, read);
    }
    float minimum[3];
    float maximum[3];
    position = new float[3 * num_vertices];
    normal = new float[3 * num_vertices];
    indice = new unsigned int[3 * num_faces];
    for (int i = 0; i < num_vertices; i++) {
        file >> position[i * 3];
        file >> position[i * 3 + 1];
        file >> position[i * 3 + 2];
        file >> normal[i * 3];
        file >> normal[i * 3 + 1];
        file >> normal[i * 3 + 2];
        if (more_lines) {
            file >> read;
            file >> read;
        }
        if (i == 0) {
            minimum[0] = position[i * 3];
            minimum[1] = position[3 * i + 1];
            minimum[2] = position[3 * i + 2];
            maximum[0] = position[i * 3];
            maximum[1] = position[3 * i + 1];
            maximum[2] = position[3 * i + 2];

        }
        else {
            if (minimum[0] > position[3 * i]) {
                minimum[0] = position[3 * i];
            }
            if (minimum[1] > position[3 * i + 1]) {
                minimum[1] = position[3 * i + 1];
            }
            if (minimum[2] > position[3 * i + 2]) {
                minimum[2] = position[3 * i + 2];
            }
            if (maximum[0] < position[3 * i]) {
                maximum[0] = position[3 * i];
            }
            if (maximum[1] < position[3 * i + 1]) {
                maximum[1] = position[3 * i + 1];
            }
            if (maximum[2] < position[3 * i + 2]) {
                maximum[2] = position[3 * i + 2];
            }
        }
    }
        min_bound = vec3(minimum[0], minimum[1], minimum[2]);
        max_bound = vec3(maximum[0], maximum[1], maximum[2]);
        
        for (int i = 0; i < num_faces; i++) {
            file >> read;
            file >> indice[i * 3];
            file >> indice[i * 3 + 1];
            file >> indice[i * 3 + 2];
      
        }

        file.close();
        return true;
  
}

glm::vec3 Mesh::getMinBounds() const
{
    return min_bound;
}

glm::vec3 Mesh::getMaxBounds() const
{
  return max_bound;
}

int Mesh::numVertices() const
{
   return num_vertices;
}

int Mesh::numTriangles() const
{
   return num_faces;
}

float* Mesh::positions() const
{
   return position;
}

float* Mesh::normals() const
{
   return normal;
}

unsigned int* Mesh::indices() const
{
   return indice;
}
