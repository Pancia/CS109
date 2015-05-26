#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
    {"define" , &interpreter::do_define },
    {"draw"   , &interpreter::do_draw   },
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
    {"text"     , &interpreter::make_text     },
    {"ellipse"  , &interpreter::make_ellipse  },
    {"circle"   , &interpreter::make_circle   },
    {"polygon"  , &interpreter::make_polygon  },
    {"rectangle", &interpreter::make_rectangle},
    {"square"   , &interpreter::make_square   },
    {"diamond"  , &interpreter::make_diamond  },
    {"triangle" , &interpreter::make_triangle },
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
    for (const auto& itor: objmap) {
        cout << "objmap[" << itor.first << "] = "
            << *itor.second << endl;
    }
}

void interpreter::interpret (const parameters& params) {
    DEBUGF ('i', params);
    param begin = params.cbegin();
    string command = *begin;
    auto itor = interp_map.find (command);
    if (itor == interp_map.end()) throw runtime_error ("syntax error");
    interpreterfn func = itor->second;
    func (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
    DEBUGF ('f', range (begin, end));
    string name = *begin;
    objmap.emplace (name, make_shape (++begin, end));
}

void interpreter::do_draw (param begin, param end) {
    DEBUGF ('f', range (begin, end));
    if (end - begin < 3)
        throw runtime_error ("syntax error");
    string name = begin[0];
    shape_map::const_iterator itor = objmap.find (name);
    if (itor == objmap.end()) {
        throw runtime_error (name + ": no such shape");
    }
    vertex where {from_string<GLfloat> (begin[2]),
        from_string<GLfloat> (begin[3])};
    rgbcolor color {begin[1]};
    window::push_back(object(itor->second, where, color));
}

shape_ptr interpreter::make_shape (param begin, param end) {
    DEBUGF ('f', range (begin, end));
    string type = *begin++;
    auto itor = factory_map.find(type);
    if (itor == factory_map.end()) {
        throw runtime_error (type + ": no such shape");
    }
    factoryfn func = itor->second;
    return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
    DEBUGF ('f', range (begin, end));
    return make_shared<text> (nullptr, string());
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
    DEBUGF ('f', range (begin, end));
    GLfloat width = stof(begin[0]);
    GLfloat height = stof(begin[1]);
    return make_shared<ellipse> (width, height);
}

shape_ptr interpreter::make_circle (param begin, param end) {
    DEBUGF ('f', range (begin, end));
    GLfloat diameter = stof(begin[0]);
    return make_shared<circle> (diameter);
}

shape_ptr interpreter::make_polygon (param begin, param end) {
    DEBUGF ('f', range (begin, end));
    vertex_list vertices;
    for (auto it = begin; it != end;) {
        string x = *it++;
        string y = *it++;
        vertex v {GLfloat(stof(x)), GLfloat(stof(y))}; 
        cout << "vertex.xpos: " << v.xpos << endl;
        cout << "vertex.ypos: " << v.ypos << endl;
        vertices.push_back(v);
    }
    return make_shared<polygon> (vertices);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
    DEBUGF ('f', range (begin, end));
    GLfloat width = stof(begin[0]);
    GLfloat height = stof(begin[1]);
    return make_shared<rectangle> (width, height);
}

shape_ptr interpreter::make_square (param begin, param end) {
    DEBUGF ('f', range (begin, end));
    GLfloat side = stof(begin[0]);
    return make_shared<square> (side);
}

shape_ptr interpreter::make_diamond (param begin, param end) {
    DEBUGF ('f', range (begin, end));
    GLfloat width = stof(begin[0]);
    GLfloat height = stof(begin[1]);
    return make_shared<diamond> (width, height);
}

shape_ptr interpreter::make_triangle (param begin, param end) {
    DEBUGF ('f', range (begin, end));
    vertex_list vertices;
    for (auto it = begin; it != end;) {
        string x = *it++;
        string y = *it++;
        vertex v {GLfloat(stof(x)), GLfloat(stof(y))}; 
        cout << "vertex.xpos: " << v.xpos << endl;
        cout << "vertex.ypos: " << v.ypos << endl;
        vertices.push_back(v);
    }
    return make_shared<triangle> (vertices);
}

