#include "pch.h"
#include "MyJoint.h"

MyJoint::MyJoint(void)
{
    parent = nullptr;
    sibling = nullptr;
    child = nullptr;
    activate = 0;
    localPos = Vector3f(0, 0, 0);
    globalPos = Vector3f(0, 0, 0);
    globalRot = Quat(0, 0, 0, 1);
    localRot = Quat(0, 0, 0, 1);
    color = Vector3f(1, 1, 0);
    name.clear();
}
MyJoint::~MyJoint(void) {

}
void MyJoint::init(int number, Vector3f offset) {
    this->number = number;
    this->localPos = offset;
}
MyJoint* MyJoint::getLastChild()
{
    MyJoint* p = this;

    if (p->child == nullptr)
        return child;
    else
    {
        p = p->child;
        while (p->sibling != nullptr)
        {
            p = p->sibling;
        }
        return p;
    }
}
void MyJoint::addChild(MyJoint* c)
{
    if (getLastChild() == nullptr) {
        child = c;
    }
    else {
        getLastChild()->sibling = c;
    }
}

void MyJoint::draw(Vector3f _c, int lineWidth)
{
    glPushMatrix();
    
    if (lineWidth < 1)
        glLineWidth(5.0f);
    else
        glLineWidth(lineWidth);

    glBegin(GL_LINES);

    Vector3f c = color;
    if (_c.x >= 0.0f)
        c = _c;

    if (this->parent != nullptr) {
        //c = parent->color;
        glColor3f(c.x, c.y, c.z);
        glVertex3f(0, 0, 0);
        glVertex3f(localPos.x, localPos.y, localPos.z);
    }
   
    glEnd();
 
    glTranslatef(localPos.x, localPos.y, localPos.z);

    Vector3f rot = ln(localRot);
    rot = rot / 3.141592f * 180.0f * 2.0f;
    float leng = sqrt(rot % rot);
    if (leng > 0.0001f)
    {
        rot = rot / leng;
        glRotatef(leng, rot.x, rot.y, rot.z);
    }

    glPushMatrix();
/*
    GLUquadricObj* obj;
    obj = gluNewQuadric();
    gluQuadricDrawStyle(obj, GLU_FILL);
    
    glColor3f(c.x, c.y, c.z);
    //gluSphere(obj, 0.3f, 20, 20);
    gluDeleteQuadric(obj);
*/
    /*glBegin(GL_LINES);

    if (name=="RightFoot" || name=="LeftFoot" || name =="Hips") {
        drawFeature();
    }
    glEnd();*/

    glPopMatrix();

    if (child != nullptr) {
        child->draw(_c, lineWidth);
    }

    glPopMatrix();

    if (sibling != nullptr) { 
        sibling->draw(_c, lineWidth);
    }
}

void MyJoint::drawShadow(Vector3f _c, int lineWidth)
{
    glPushMatrix();

    if (lineWidth < 1)
        glLineWidth(5.0f);
    else
        glLineWidth(lineWidth);

    if (this->parent != nullptr)
    {
        glBegin(GL_LINES);
        glColor3f(_c.x, _c.y, _c.z);
        glVertex3f(shadowPos.x, shadowPos.y, shadowPos.z);
        glVertex3f(parent->shadowPos.x, parent->shadowPos.y, parent->shadowPos.z);
        glEnd();
    }
    glPopMatrix();
    if (child != nullptr)   child->drawShadow(_c, lineWidth);
    if (sibling != nullptr) sibling->drawShadow(_c, lineWidth);
  

}

MyJoint* MyJoint::getByName(std::string in) {
    if (in == name) {
        return this;
    }
    MyJoint* out = nullptr;;

    if (child != nullptr)
        out = child->getByName(in);
    if (out != nullptr) return out;

    if (sibling != nullptr)
        out = sibling->getByName(in);
    if (out != nullptr) return out;

    return nullptr;
}
Vector3f LPos = Vector3f(-100, 100, -100);
void MyJoint::setGlobalTransform(Vector3f parentoffset, Quat parentRot)
{
    globalPos = parentoffset + rotate(parentRot, localPos);
    globalRot = parentRot * localRot;

    if (child != nullptr)
        child->setGlobalTransform(globalPos, globalRot);
    if (sibling != nullptr)
        sibling->setGlobalTransform(parentoffset, parentRot);

    shadowPos.x = globalPos.x - (globalPos.y / LPos.y) * LPos.x;
    shadowPos.y = 0.01f;
    shadowPos.z = globalPos.z - (globalPos.y / LPos.y) * LPos.z;
}



//void MyJoint::drawFeature()
//{
//    GLUquadricObj* q = gluNewQuadric();
//    gluQuadricDrawStyle(q, GLU_FILL);
//    gluQuadricNormals(q, GLU_SMOOTH);
//
//    glPushMatrix();
//
//    glDisable(GL_LIGHTING);
//    glBegin(GL_LINES);
//
//    /*glRotatef(ang[2], 0, 0, 1);
//    glRotatef(ang[1], 0, 1, 0);
//    glRotatef(ang[0], 1, 0, 0);*/
//    
//    glVertex3f(0.0f, 0.0f, 0.0f);
//    glVertex3f(0.0f, 0.0f, 1.0f * 2);
//    //glVertex3f(0.0f, 0.0f, 1.0f * m_velocity);
//
//    glEnd();
//    glEnable(GL_LIGHTING);
//
//    glPushMatrix();
//    glTranslatef(0.0f, 0.0f, 1.0f * 2);
//    gluCylinder(q, 0.1 * 2, 0.0, 0.2 * 2, 8, 1);
//    //glTranslatef(0.0f, 0.0f, 1.0f * m_velocity);
//    //gluCylinder(q, 0.1 * m_velocity, 0.0, 0.2 * m_velocity, 8, 1);
//    glPopMatrix();
//
//    gluDeleteQuadric(q);
//}



void MyJoint::printGlobalPos()
{
    std::cout << name << ": " << globalPos << std::endl;

    if (child != nullptr)
        child->printGlobalPos();
    if (sibling != nullptr)
        sibling->printGlobalPos();
}
