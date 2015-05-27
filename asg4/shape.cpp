#include <math.h>
#include <typeinfo>
#include <unordered_map>
using namespace std;

#include "shape.h"
#include "util.h"

static unordered_map<void*,string> fontname {
    {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
    {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
    {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
    {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
    {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
    {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
    {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
    {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
    {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
    {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
    {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
    {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
    {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
    {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

ostream& operator<<(ostream& out, const vertex& where) {
    out << "(" << where.xpos << "," << where.ypos << ")";
    return out;
}

shape::shape() {
    DEBUGF('c', this);
}

text::text(const string& font, const string& textdata):
    glut_bitmap_font(fontcode[font]), textdata(textdata) {
        DEBUGF('c', this);
    }

ellipse::ellipse(GLfloat width, GLfloat height):
    dimension({width, height}) {
        DEBUGF('c', this << "(" << width << "," << height << ")");
    }

circle::circle(GLfloat diameter): ellipse(diameter, diameter) {
    DEBUGF('c', this << "(" << diameter << ")");
}

polygon::polygon(const vertex_list& vertices): vertices(vertices) {
    DEBUGF('c', this << "[" << vertices << "]");
}

rectangle::rectangle(GLfloat width, GLfloat height):
    polygon(([] (GLfloat w, GLfloat h) -> vertex_list {
        vertex_list vs;

        //top-left
        GLfloat x0 = - (w / 2);
        GLfloat y0 = (h / 2);
        DEBUGF('d', "x0:" << x0 << ", y0:" <<  y0);
        vs.push_back(vertex{x0, y0});

        //top-right
        GLfloat x1 = (w / 2);
        GLfloat y1 = (h / 2);
        DEBUGF('d', "x1:" << x1 << ", y1:" <<  y1);
        vs.push_back(vertex{x1, y1});

        //btm-right
        GLfloat x2 = (w / 2);
        GLfloat y2 = - (h / 2);
        DEBUGF('d', "x2:" << x2 << ", y2:" <<  y2);
        vs.push_back(vertex{x2, y2});

        //btm-let
        GLfloat x3 = - (w / 2);
        GLfloat y3 = - (h / 2);
        DEBUGF('d', "x3:" << x3 << ", y3:" <<  y3);
        vs.push_back(vertex{x3, y3});

        return vs;
    })(width, height)) {
    }

square::square(GLfloat width):
    rectangle(width, width) {
    }

diamond::diamond(const GLfloat width, const GLfloat height):
    polygon(([] (GLfloat w, GLfloat h) -> vertex_list {
        vertex_list vs;

        //top
        GLfloat x0 = 0;
        GLfloat y0 = (h / 2);
        DEBUGF('d', "x0:" << x0 << ", y0:" <<  y0);
        vs.push_back(vertex{x0, y0});

        //left
        GLfloat x2 = - (w / 2);
        GLfloat y2 = 0;
        DEBUGF('d', "x2:" << x2 << ", y2:" <<  y2);
        vs.push_back(vertex{x2, y2});

        //btm
        GLfloat x1 = 0;
        GLfloat y1 = - (h / 2);
        DEBUGF('d', "x1:" << x1 << ", y1:" <<  y1);
        vs.push_back(vertex{x1, y1});

        //right
        GLfloat x3 = (w / 2);
        GLfloat y3 = 0;
        DEBUGF('d', "x3:" << x3 << ", y3:" <<  y3);
        vs.push_back(vertex{x3, y3});

        return vs;
    })(width, height)) {
    }

triangle::triangle(const vertex_list& vertices):
    polygon(vertices) {
    }

right_triangle::right_triangle(const GLfloat width,
                               const GLfloat height):
    triangle(([] (const GLfloat w, const GLfloat h) -> vertex_list {
        vertex_list vs;

        //top
        DEBUGF('d', "RIGHT_TRIANGLE");
        GLfloat x0 = - ((2.0/3.0) * w);
        GLfloat y0 = - ((1.0/3.0) * h);
        DEBUGF('d', "x0:" << x0 << ", y0:" <<  y0);
        vs.push_back(vertex{x0, y0});

        //left
        GLfloat x2 = ((1.0/3.0) * w);
        GLfloat y2 = ((2.0/3.0) * h);
        DEBUGF('d', "x2:" << x2 << ", y2:" <<  y2);
        vs.push_back(vertex{x2, y2});

        //btm
        GLfloat x1 = ((1.0/3.0) * w);
        GLfloat y1 = - ((1.0/3.0) * h);
        DEBUGF('d', "x1:" << x1 << ", y1:" <<  y1);
        vs.push_back(vertex{x1, y1});

        return vs;
    })(width, height)) {
    }

isosceles::isosceles(const GLfloat width,
                     const GLfloat height):
    triangle(([] (const GLfloat w, const GLfloat h) -> vertex_list {
        vertex_list vs;

        //top
        DEBUGF('d', "ISOSCELES");
        GLfloat x0 = - (w / 2);
        GLfloat y0 = - (h / 2);
        DEBUGF('d', "x0:" << x0 << ", y0:" <<  y0);
        vs.push_back(vertex{x0, y0});

        //left
        GLfloat x2 = 0.0;
        GLfloat y2 = (h / 2);
        DEBUGF('d', "x2:" << x2 << ", y2:" <<  y2);
        vs.push_back(vertex{x2, y2});

        //btm
        GLfloat x1 = (w / 2);
        GLfloat y1 = - (h / 2);
        DEBUGF('d', "x1:" << x1 << ", y1:" <<  y1);
        vs.push_back(vertex{x1, y1});

        return vs;
    })(width, height)) {
    }

equilateral::equilateral(const GLfloat width):
    triangle(([] (const GLfloat w) -> vertex_list {
        vertex_list vs;

        //top
        DEBUGF('d', "RIGHT_TRIANGLE");
        GLfloat x0 = - (w / 2);
        GLfloat y0 = - (w / 2);
        DEBUGF('d', "x0:" << x0 << ", y0:" <<  y0);
        vs.push_back(vertex{x0, y0});

        //left
        GLfloat x2 = 0.0;
        GLfloat y2 = (w / 2);
        DEBUGF('d', "x2:" << x2 << ", y2:" <<  y2);
        vs.push_back(vertex{x2, y2});

        //btm
        GLfloat x1 = (w / 2);
        GLfloat y1 = - (w / 2);
        DEBUGF('d', "x1:" << x1 << ", y1:" <<  y1);
        vs.push_back(vertex{x1, y1});

        return vs;
    })(width)) {
    }

//===================== DRAWING =====================
//===================== DRAWING =====================
//===================== DRAWING =====================
//===================== DRAWING =====================
//===================== DRAWING =====================

void text::draw(const vertex& center, const rgbcolor& color) const {
    DEBUGF('d', this << "(" << center << "," << color << ")");
    DEBUGF('d', "TEXT: " << textdata);
    auto font = glut_bitmap_font;
    glColor3ubv(color.ubvec);

    GLfloat w = glutBitmapLength(font, (GLubyte*)textdata.c_str());
    GLfloat h = glutBitmapHeight(font);
    GLfloat xpos = center.xpos;// - (w / 2.0);
    GLfloat ypos = center.ypos;// - (h / 4.0);

    DEBUGF('d', this << "(" << xpos << "," << ypos << ")");
    glRasterPos2f(xpos,ypos);

    for (auto ch : textdata)
        glutBitmapCharacter(font, ch);
}

void ellipse::draw(const vertex& center, const rgbcolor& color) const {
    DEBUGF('d', this << "(" << center << "," << color << ")");
    glBegin(GL_LINE_LOOP);
    glColor3ubv(color.ubvec);
    int sides = 36;
    for(int i = 0; i < sides; i++) {
        float theta = 2.0f * M_PI * float(i) 
            / float(sides);

        float x = dimension.xpos * cosf(theta);
        float y = dimension.ypos * sinf(theta);

        glVertex2f(x + center.xpos,
                y + center.ypos);
    }
    glEnd();
}

void circle::draw(const vertex& center, const rgbcolor& color) const {
    DEBUGF('d', this << "(" << center << "," << color << ")");
    return ellipse::draw(center, color);
}

void polygon::draw(const vertex& center, const rgbcolor& color) const {
    DEBUGF('d', this << "(" << center << "," << color << ")");
    glBegin(GL_LINE_LOOP);
    glColor3ubv(color.ubvec);
    for (vertex v : vertices) {
        cout << "V: " << v.xpos << "," << v.ypos << endl;
        glVertex2f(v.xpos + center.xpos,
                v.ypos + center.ypos);
    }
    glEnd();
}

void shape::show(ostream& out) const {
    out << this << "->" << demangle(*this) << ": ";
}

void text::show(ostream& out) const {
    shape::show(out);
    out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
        << ") \"" << textdata << "\"";
}

void ellipse::show(ostream& out) const {
    shape::show(out);
    out << "{" << dimension << "}";
}

void polygon::show(ostream& out) const {
    shape::show(out);
    out << "{" << vertices << "}";
}

void rectangle::show(ostream& out) const {
    shape::show(out);
    polygon::show(out);
}

ostream& operator<<(ostream& out, const shape& obj) {
    obj.show(out);
    return out;
}
