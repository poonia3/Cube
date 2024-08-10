#include <bits/stdc++.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

using namespace std;
using matrix = vector<vector<float>>;
using vertex = vector<float>;

const std::string reset = "\033[0m";
const std::string bold = "\033[1m";
const std::string red = "\033[31m";
const std::string blue = "\033[34m";
const std::string yellow = "\033[33m";
const std::string purple = "\033[35m";
const std::string cyan = "\033[36m";
const std::string green = "\033[32m";
const std::string black = "\033[30m";
const std::string white = "\033[37m";
const std::string underline = "\033[4m";

vector<string> rgb{red, blue, green, yellow, purple, cyan, reset};

vector<float> subtract(vector<float> a, vector<float>_b){
    return {a[0]-_b[0], a[1]-_b[1], a[2]-_b[2]};
}

vector<float> sum(vector<float> a, vector<float>b){
    return {a[0]+b[0], a[1]+b[1], a[2]+b[2]};
}

vector<float> multiply(float a, vector<float> v){
    return {a*v[0], a*v[1], a*v[2]};
}

matrix multiply(float a, matrix m){
    matrix copy = m;
    for(int i = 0; i < m.size();i++){
        for(int j = 0; j < m[i].size(); j++){
            copy[i][j] *= a;
        }
    }
    return copy;
}

float length_sq(vector<float> a){
    return a[0]*a[0] + a[1]*a[1] + a[2]*a[2];
}

float dot(vector<float> a, vector<float> b){
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

vector<float> cross(vector<float> a, vector<float> b){
    return {a[1]*b[2] - a[2]*b[1], a[2]*b[0] - a[0]*b[2], a[0]*b[1] - a[1]*b[0]};
}   

vector<float> normalise(vector<float> a){
    float magnitude = 1/sqrtf(length_sq(a));
    return multiply(magnitude, a);
}

class ray{
public:
    vector<float> origin;
    vector<float> direction;
    ray(vector<float> orgn, vector<float> dir):origin(orgn),direction(dir){
        direction = normalise(direction);
    }
    vertex point_at_r(float r){
        //cout << r << "\n";
        //cout << multiply(r, direction)[0] << " " << multiply(r, direction)[1] << " " << multiply(r, direction)[2] <<"\n";
        //cout << sum(multiply(r, direction), origin)[0] << " " << sum(multiply(r, direction), origin)[1] << " " << sum(multiply(r, direction), origin)[2] <<"\n";
        return sum(multiply(r, direction), origin);
    }
};

vector<float> face(vertex a, vertex b, vertex c, vertex d, vertex centroid){
    /*
    cout << "triangle(( " << a[0] << ", " << a[1] << ", " << a[2] << "),";
    cout << "( " << b[0] << ", " << b[1] << ", " << b[2] << "),";
    cout << "( " << c[0] << ", " << c[1] << ", " << c[2] << "))\n";
    cout << "triangle(( " << b[0] << ", " << b[1] << ", " << b[2] << "),";
    cout << "( " << c[0] << ", " << c[1] << ", " << c[2] << "),";
    cout << "( " << d[0] << ", " << d[1] << ", " << d[2] << "))\n";
    */
    vertex abcd_4 = multiply(0.25, sum(sum(a,b),sum(c,d)));
    vertex normal = subtract(abcd_4, centroid);
    normal = normalise(normal);
    return {normal[0], normal[1], normal[2], dot(normal, abcd_4)};
}

vector<float> face(vertex normal, vertex point){
    return {normal[0], normal[1], normal[2], dot(normal, point)};
}

float surface_value(vertex point, vector<float> surface){
    return point[0]*surface[0] + point[1]*surface[1] + point[2]*surface[2] - surface[3];
}

class Cube{
public:
    vector<vector<float>> vertices;
    vector<vector<float>> surfaces;
    vector<vector<float>> face_centres;
    vector<int> color;
    vector<float> operator[](int i){
        return vertices[i];
    }
    Cube(){}
    Cube(vector<vector<float>> points);
    tuple<bool, vector<float>, vector<float>, int> check_collision(ray incident_ray);
};

tuple<bool, vector<float>, vector<float>, int> Cube::check_collision(ray incident_ray){
    float min_distance = INT_MAX;
    vector<float> result{0,0,0};
    vector<float> normal{0,0,1};
    bool check = false;
    int color = 6;
    for(int i = 0; i < surfaces.size(); i++){
        auto &surface = surfaces[i];
        vector<float> surface_normal(surface.begin(), surface.begin()+3);
        float d = surface[3];
        float distance = (d-dot(surface_normal, incident_ray.origin))/(dot(incident_ray.direction, surface_normal));
        vertex point = incident_ray.point_at_r(distance);
        //cout << "( " << point[0] << ", "<< point[1] << ", "<< point[2] << ")\n";
        bool valid = true;
        for(int j = 0; j < surfaces.size(); j++){
            if(i == j) continue;
            auto &s = surfaces[j];
            valid = valid && (surface_value(point, s)*surface_value(face_centres[i], s) > 0);//make collisions work
        }

        if(valid){
            check = true;
            if(min_distance > abs(distance)){
                min_distance = distance;
                result = point;
                normal = vector(surface.begin(), surface.begin()+3);
                color = Cube::color[i];
            }
        }

        //cout << "valid = " << valid << ": face_centre = " << face_centres[i][0] << ", " << face_centres[i][1] << ", " << face_centres[i][2] << "\n"; 
    }
    /*
    for(int i = 0; i < face_centres.size(); i++){
        cout << "( " << face_centres[i][0] << ", " << face_centres[i][1] << ", " << face_centres[i][2] << ")\n";
    }*/
    return {check, result, normal, color};
}

Cube::Cube(vector<vector<float>> points){
    matrix new_points;
    vector<float> distances;
    matrix adjacent;
    vector<int> indices{0,1,2,3,4,5,6,7};
    for(int i = 0; i < 8; i++){
        new_points.push_back(subtract(points[i], points[0]));
        distances.push_back(length_sq(new_points[i]));
    }
    auto compare = [&distances](int a, int b) {
        return distances[a] < distances[b];
    };
    
    sort(indices.begin(), indices.end(), compare);
    vertex a = points[indices[1]], b = points[indices[2]], c = points[indices[3]], o = points[0];
    vertex ab_o = subtract(sum(a, b), o), bc_o = subtract(sum(b, c), o), ca_o = subtract(sum(c, a), o);
    vertex op_o = subtract(sum(ab_o, c), o);
    /*
    cout << "( " << o[0] << ", " << o[1] << ", " << o[2] << ")\n";
    cout << "( " << a[0] << ", " << a[1] << ", " << a[2] << ")\n";
    cout << "( " << b[0] << ", " << b[1] << ", " << b[2] << ")\n";
    cout << "( " << c[0] << ", " << c[1] << ", " << c[2] << ")\n";
    cout << "( " << ab_o[0] << ", " << ab_o[1] << ", " << ab_o[2] << ")\n";
    cout << "( " << bc_o[0] << ", " << bc_o[1] << ", " << bc_o[2] << ")\n";
    cout << "( " << ca_o[0] << ", " << ca_o[1] << ", " << ca_o[2] << ")\n";
    cout << "( " << op_o[0] << ", " << op_o[1] << ", " << op_o[2] << ")\n";
    */
    Cube::vertices = {o,a,b,c,ab_o,bc_o,ca_o,op_o};
    vertex centroid = o;
    for(int i = 1; i < 8; i++){
        centroid = sum(centroid,vertices[i]);
    }

    centroid = multiply(0.125, centroid);

    surfaces.push_back(face(o, a, b, ab_o, centroid));
    color.push_back(0);
    face_centres.push_back(multiply(0.25, sum(sum(o,a),sum(b, ab_o))));
    surfaces.push_back(face(o, b, c, bc_o, centroid));
    color.push_back(1);
    face_centres.push_back(multiply(0.25, sum(sum(o,b),sum(c, bc_o))));
    surfaces.push_back(face(o, c, a, ca_o, centroid));
    color.push_back(2);
    face_centres.push_back(multiply(0.25, sum(sum(o,c),sum(a, ca_o))));
    surfaces.push_back(face(op_o, ab_o, bc_o, b, centroid));
    color.push_back(3);
    face_centres.push_back(multiply(0.25, sum(sum(op_o,ab_o),sum(bc_o, b))));
    surfaces.push_back(face(op_o, bc_o, ca_o, c, centroid));
    color.push_back(4);
    face_centres.push_back(multiply(0.25, sum(sum(op_o, bc_o),sum(ca_o, c))));
    surfaces.push_back(face(op_o, ca_o, ab_o, a, centroid));
    color.push_back(5);
    face_centres.push_back(multiply(0.25, sum(sum(op_o, ca_o),sum(ab_o, a))));
}

matrix multiply(matrix mat1, matrix mat2){
    matrix result = matrix(3,vector<float>(3, 0));
    for(int i = 0; i < 3; i++)for(int j = 0; j < 3; j++)for(int k = 0; k < 3; k++) result[i][j] += mat1[i][k] * mat2[k][j];
    return result;
}

vector<float> tranform(matrix transformation, vector<float> orignal){
    vector<float> result(3,0);
    for(int i = 0; i < 3; i++)for(int j = 0; j < 3; j++) result[i] += transformation[i][j] * orignal[j];
    return result;
}

void printMatrix(matrix input){
    for(auto &x: input){
        for(auto &y: x){
            cout << y << " ";
        }
        cout << "\n";
    }
}

matrix rotate_about_z(float degrees){
    float angle = degrees*M_PI/180.0;
    return{{cosf(angle),-sinf(angle), 0},
           {sinf(angle), cosf(angle), 0},
           {          0,           0, 1}};
}

matrix rotate_about_x(float degrees){
    float angle = degrees*M_PI/180.0;
    return{{ 1,           0,           0},
           { 0, cosf(angle),-sinf(angle)},
           { 0, sinf(angle), cosf(angle)}};
}

matrix rotate_about_y(float degrees){
    float angle = degrees*M_PI/180.0;
    return{{           0, 1,           0},
           {-sinf(angle), 0, cosf(angle)},
           { cosf(angle), 0, sinf(angle)}};
}

void transform_cube(matrix transformation, Cube& cube){
    for(int i = 0; i < cube.vertices.size(); i++){
        cube.vertices[i] = tranform(transformation, cube.vertices[i]);
        if(i >= 6) continue;
        cube.face_centres[i] = tranform(transformation, cube.face_centres[i]);
        vertex surface = tranform(transformation, cube.surfaces[i]);
        cube.surfaces[i][0] = surface[0];cube.surfaces[i][1] = surface[1];cube.surfaces[i][2] = surface[2];
    }
}

int main(){
    string ascii = " ,-:!#$@";
    vector<vector<int>> mat(4, vector<int>(4,0));
    std::ios::sync_with_stdio(false);  // Disable synchronization
    std::cout.tie(nullptr);  // Untie cin and cout
    Cube cube ({{  0.366025,  0.683017,   1.54904},
                { -0.366025,  -1.68302,   0.18301},
                {  0.366025,   1.68302,  -0.18301},
                {   1.36603, -0.816987,  0.683013},
                {  -1.36603,  0.816987, -0.683013},
                {  -1.36603, -0.183013,   1.04904},
                {   1.36603,  0.183013,  -1.04904},
                { -0.366025, -0.683017,  -1.54904}});
    
    /*
    Cube cube ({{  1,  1,   1},
                { -1,  1,   1},
                {  1, -1,   1},
                {  1,  1,  -1},
                {  1, -1,  -1},
                { -1,  1,  -1},
                { -1, -1,   1},
                { -1, -1,  -1},});
    */
    vector<float> current;
    for(int i = 0; i < 8; i++){
        current = cube[i];
        //cout << "( " << current[0] << ", " << current[1] << ", " << current[2] << ")\n";
    }


    ray incident_ray({1.366028,1.683018,-1.4519}, {1,1,-3});
    //auto [a, b, c, d] = cube.check_collision(incident_ray);
    //cout << a << "\n";
    //cout << b[0] << ", " << b[1] << ", " << b[2];

    ray cam({0,0,0}, {0,0,1});
    vertex light = {-10,-10,15};
    cout << "\n";
    std::string out;

    // Initialize the screen with the cursor at the top-left
    std::cout << "\x1b[H";  // Set cursor to home position
    float scale = 0.65;
    for (float k = 0; k < 1000; k += 0.2f) {
        transform_cube(rotate_about_z(1), cube);

        out.clear();  // Clear the output buffer for the new frame
        out+=bold;
        for (float i = -1.5f; i < 1.5f; i += 0.14f*scale) {
            for (float j = -2.0f; j < 2.0f; j += 0.06f*scale) {
                cam = ray({i, j, 3}, {0, 0, 1});
                auto [collision,point,normal,color] = cube.check_collision(cam);
                float intensity = 0;
                if(collision){
                    intensity = dot(normalise(subtract(light, point)), normal);
                    float il = 1.01/ascii.size();
                    intensity = (intensity > il)?intensity:il;
                }
                out += rgb[color] + ascii[int((ascii.size() - 0.01)*intensity)] + reset;
            }
            out += '\n';
        }

        // Move cursor to the top-left corner before printing the next frame
        std::cout << "\x1b[H";  // ANSI escape sequence to move the cursor to home position
        std::cout << out.data(); // Print the current frame

        //usleep(10000);  // Delay for smooth animation
    }
}