#include <iostream>
#include <string>
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
