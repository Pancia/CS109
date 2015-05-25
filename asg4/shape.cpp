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

text::text(void* glut_bitmap_font, const string& textdata):
    glut_bitmap_font(glut_bitmap_font), textdata(textdata) {
        DEBUGF('c', this);
    }

ellipse::ellipse(GLfloat width, GLfloat height):
    dimension({width, height}) {
        DEBUGF('c', this);
    }

circle::circle(GLfloat diameter): ellipse(diameter, diameter) {
    DEBUGF('c', this);
}

polygon::polygon(const vertex_list& vertices): vertices(vertices) {
    DEBUGF('c', this);
}

rectangle::rectangle(GLfloat w, GLfloat h):
    polygon({}), width(w), height(h)  {
        DEBUGF('c', this << "(" << width << "," << height << ")");
    }

square::square(GLfloat width): rectangle(width, width) {
    DEBUGF('c', this);
}

void text::draw(const vertex& center, const rgbcolor& color) const {
    DEBUGF('d', this << "(" << center << "," << color << ")");
}

void ellipse::draw(const vertex& center, const rgbcolor& color) const {
    DEBUGF('d', this << "(" << center << "," << color << ")");
}

void polygon::draw(const vertex& center, const rgbcolor& color) const {
    DEBUGF('d', this << "(" << center << "," << color << ")");
}

void rectangle::draw(const vertex& center, const rgbcolor& color)const {
    DEBUGF('d', "CENTER: " << center << ", COLOR " << color);
    glBegin(GL_LINE);

    glColor3ubv(color.ubvec);
    GLfloat x0 = center.xpos - (width / 2);
    GLfloat y0 = center.ypos + (height / 2);
    DEBUGF('d', "x0:" << x0 << ", y0:" <<  y0);
    glVertex2f(x0, y0);

    GLfloat x1 = center.xpos + (width / 2);
    GLfloat y1 = center.ypos + (height / 2);
    DEBUGF('d', "x1:" << x1 << ", y1:" <<  y1);
    glVertex2f(x1, y1);

    GLfloat x2 = center.xpos + (width / 2);
    GLfloat y2 = center.ypos - (height / 2);
    DEBUGF('d', "x2:" << x2 << ", y2:" <<  y2);
    glVertex2f(x2, y2);

    GLfloat x3 = center.xpos - (width / 2);
    GLfloat y3 = center.ypos - (height / 2);
    DEBUGF('d', "x3:" << x3 << ", y3:" <<  y3);
    glVertex2f(x3, y3);

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
    out << "{w:" << width << ",h:" << height << "}";
}

ostream& operator<<(ostream& out, const shape& obj) {
    obj.show(out);
    return out;
}
