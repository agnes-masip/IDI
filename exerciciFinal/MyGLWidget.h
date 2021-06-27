#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "model.h"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *e);

  private:
    void creaBuffers ();
    void carregaShaders ();
    void modelTransform ();
    void terraTransform();
    void projectTransform(float fov, float ra,float znear, float zfar);
    void escenaInicial();
    void viewTransform();
    void calculsInicials(glm::vec3 min,glm::vec3 max);
    void maxmincapsa();

    // attribute locations
    GLuint vertexLoc, colorLoc, projLoc, viewLoc;
    // uniform locations
    GLuint transLoc;
    // VAO i VBO names
    GLuint VAO_Homer;
    GLuint VAO_Terra;
    // Program
    QOpenGLShaderProgram *program;
    // Viewport
    GLint ample, alt;
    // Internal vars
    
    glm::vec3 pos;
    
    float scale, rotation, scalep;
    float fov, fovIni, ra, znear, zfar, radi, dist;
    glm::vec3 OBS,centreEscena, cbase;
    bool pers;
    Model m; 
    glm::vec3 obs = glm::vec3(1,0,0);
    glm::vec3 patricio;
    float l,r,b,t;
    float phi,psi,theta;
    float xant,yant;
    float rotationP;
};

