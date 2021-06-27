#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  scale = 1;
  rotation = 0;
  pers = true;
  psi = 0.0;
  phi=0.0;
  theta=0.0;
  xant = 0;
  yant = 0;
  patricio = glm::vec3(0,0,0);
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions(); 
  
  

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  creaBuffers();
  escenaInicial();
  
  //ACTIVA EL Z-BUFFER
  glEnable(GL_DEPTH_TEST);
}

void MyGLWidget::paintGL () 
{
// Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#endif

// En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
// useu els paràmetres que considereu (els que hi ha són els de per defecte)
//  glViewport (0, 0, ample, alt);
  
  // Esborrem el frame-buffer I EL Z-BUFFER
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Carreguem la transformació de model
  

    
  // Activem el VAO per a pintar el terra. 
  
    glBindVertexArray(VAO_Homer);
    patricio = glm::vec3(2,0,2);
    rotationP = 0;
    modelTransform ();
    glDrawArrays (GL_TRIANGLES, 0, m.faces ().size () * 3);
  
    glBindVertexArray (0);
    
    glBindVertexArray(VAO_Homer);
    rotationP = M_PI/2;
    patricio = glm::vec3(0,0,0);
    modelTransform ();
    glDrawArrays (GL_TRIANGLES, 0, m.faces ().size () * 3);
  
    glBindVertexArray (0);
    
    glBindVertexArray(VAO_Homer);
    patricio = glm::vec3(-2,0,-2);
    rotationP = -M_PI;
    modelTransform ();
    glDrawArrays (GL_TRIANGLES, 0, m.faces ().size () * 3);
  
    glBindVertexArray (0);

    glBindVertexArray(VAO_Terra);
    terraTransform ();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
     glBindVertexArray (0);
}

/********************COSES NOVES *****************************/

void MyGLWidget::maxmincapsa(){
   float minx,maxx, miny, minz,maxz,maxy;
    minx = maxx = m.vertices()[0];
    miny = maxy = m.vertices()[0+1];
    minz = maxz = m.vertices()[0+2];
    
    for(unsigned int i = 3; i<m.vertices().size();i+= 3){
        if(m.vertices()[i] > maxx) maxx = m.vertices()[i];
        if(m.vertices()[i] < minx) maxx = m.vertices()[i];
        if(m.vertices()[i+1] > maxy) maxy = m.vertices()[i+1];
        if(m.vertices()[i+1] < miny) miny = m.vertices()[i+1];
        if(m.vertices()[i+2] > maxz) maxz = m.vertices()[i+2];
        if(m.vertices()[i+2] < minz) minz = m.vertices()[i+2];
    }

    
    cbase = glm::vec3(((minx+maxx)/2.0),(miny),((minz+maxz)/2.0));
    scalep = 1.0/(maxy-miny);
    
    
}


void MyGLWidget::calculsInicials(glm::vec3 min, glm::vec3 max){
    
    //centre d'escena VDR (xmax+xmin)/2 (de les tres coordenades)
    centreEscena = max+min;
    centreEscena = centreEscena / glm::vec3(2.0);
    
    radi=distance(max,min)/2;
    dist = radi*2;
}


void MyGLWidget::escenaInicial(){
    
    // maxim i min --> glm::vec3(-2, -1, -2), glm::vec3(2, 1, 2)
    
    // alfa = arcsin
    // FOV = 2*alfa
    calculsInicials(glm::vec3(-2.5, 0, -2.5), glm::vec3(2.5, 1, 2.5));
    maxmincapsa();
    
    l = -radi; 
    r = radi;
    b = -radi;
    t = radi;
    
    fovIni = 2*(asin(radi/dist));
    fov = fovIni;
    ra = 1;
    
    //zNear = d-R zFar = d+R
    znear  = dist-radi;
    zfar   = dist+radi;
    
    OBS = centreEscena + glm::vec3(0,0,dist);
    
    projectTransform(fov,ra,znear,zfar);
    viewTransform();
    
}
void MyGLWidget::viewTransform () {
// glm::lookAt (OBS, VRP, UP)
//glm::mat4 View = glm::lookAt (OBS, centreEscena, glm::vec3(0,1,0));
    glm::mat4 View(1.0f);
    View = glm::translate(View,glm::vec3(0,0,-dist));
    View = glm::rotate(View,-phi,glm::vec3(0.0,0.0,1.0));
    View = glm::rotate(View,theta,glm::vec3(1.0,0.0,0.0));
    View = glm::rotate(View,-psi,glm::vec3(0.0,1.0,0.0));
    View = glm::translate(View,-centreEscena);
    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}


 void MyGLWidget::projectTransform(float fov, float ra,float znear, float zfar){
// glm::perspective (FOV en radians, ra window, znear, zfar)
    glm::mat4 Proj = glm::perspective (fov, ra, znear, zfar);
    if(!pers) Proj = glm::ortho(l,r,b,t,znear,zfar);
    glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}


 
/************************* ***************************************/

void MyGLWidget::modelTransform () 
{
  // Matriu de transformació de model
  glm::mat4 transform (1.0f);
  transform = glm::translate(transform, patricio);
  transform = glm::rotate(transform, rotation+rotationP, glm::vec3(0.0,1.0,0.0));
  transform = glm::scale(transform, glm::vec3(scale,scale,scale));
  transform = glm::scale(transform, glm::vec3(scalep,scalep,scalep));
  transform = glm::translate(transform, -cbase);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::terraTransform () 
{
  // Matriu de transformació de model
  glm::mat4 transform (1.0f);
  //transform = glm::translate(transform, glm::vec3(0,0,0));
  transform = glm::scale(transform, glm::vec3(scale,scale,scale));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::resizeGL (int w, int h) 
{
    //
    ra = float(w)/float(h);
    if(pers){
        if(ra < 1){fov = float(2.0*atan(tan(fovIni/2.0)/ra)); } //FOV INI !!!
    }else{
        if(ra>1){
            l = -radi*ra;
            r = radi*ra;
            b = -radi;
            t = radi;
        }else{
            l = -radi;
            r = radi;
            b = -radi/ra;
            t = radi/ra;
        }
    }
    
    projectTransform(fov,ra,znear,zfar);
    glViewport(0,0,w,h);
              
}  
            
            
            
            

            
            
            
void MyGLWidget::mouseMoveEvent(QMouseEvent *e){
    makeCurrent();
    
    float anglex = abs(e->x()-xant);
    float angley = abs(e->y()-yant);
    
    if(anglex > angley){
        if(e->x() > xant) psi -= 0.03;
        else if(e->x() < xant) psi += 0.03;
    }else{
        if(e->y() > yant) theta += 0.03;
        else if(e->y() < yant) theta -= 0.03;
    }
    xant = e->x();
    yant = e->y();
    
    
    viewTransform();
    update();
    
}
            
            
            
            
            
void MyGLWidget::keyPressEvent(QKeyEvent* event) {
    makeCurrent();
    switch( event->key()){
    
    case Qt::Key_D: { // escalar a més petit
      scale -= 0.05;
      break;
    }
    case Qt::Key_R: { // escalar a més petit
      rotation += M_PI/4;
      break;
    }
    case Qt::Key_X: { // escalar a més petit
      //zoom-out
        fov += 0.05;
        projectTransform(fov,ra,znear,zfar);
      break;
    }
    case Qt::Key_Z: { // escalar a més petit
      //zoom-in
        fov -= 0.05;
        projectTransform(fov,ra,znear,zfar);
      break;
    }
    case Qt::Key_O: { // escalar a més petit
      pers = !pers;
      projectTransform(fov,ra,znear,zfar);
      //escenaInicial();
      break;
    }
    
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::creaBuffers () 
{
  // Dades de la caseta
  // Dos VBOs, un amb posició i l'altre amb color
    
    
    glm::vec3 terraVertices[6]{
        glm::vec3(-2.5, -0, -2.5),
        glm::vec3(-2.5, -0, 2.5),
        glm::vec3(2.5, -0, -2.5),
        glm::vec3(2.5, -0, -2.5),  
        glm::vec3(-2.5, -0, 2.5),   
        glm::vec3(2.5, -0, 2.5) 
    };
    
    glm::vec3 terraColors[6]{
        glm::vec3(0,1,0),
        glm::vec3(0,1,0),
        glm::vec3(0,1,0),
        glm::vec3(0,1,0),
        glm::vec3(0,1,0),
        glm::vec3(0,1,0)
    };
    
    glGenVertexArrays(1,&VAO_Terra);
    glBindVertexArray(VAO_Terra);
    
    GLuint VBOTerra[2];
    glGenBuffers(2, VBOTerra);
    glBindBuffer(GL_ARRAY_BUFFER, VBOTerra[0]);
    
    glBufferData (GL_ARRAY_BUFFER,sizeof(terraVertices),terraVertices, GL_STATIC_DRAW); 
    
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glBindBuffer(GL_ARRAY_BUFFER, VBOTerra[1]);
    // NOOOOUUU glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
    glBufferData (GL_ARRAY_BUFFER, sizeof(terraColors),terraColors, GL_STATIC_DRAW); // color

  // Activem l'atribut colorLoc
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);
    
    
    
    
    

    m.load("/assig/idi/models/Patricio.obj");

  // Creació del Vertex Array Object per pintar
  glGenVertexArrays(1, &VAO_Homer);
  glBindVertexArray(VAO_Homer);
  

  GLuint VBO_Homer[2];
  glGenBuffers(2, VBO_Homer);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[0]);

  glBufferData (GL_ARRAY_BUFFER, sizeof(GLfloat) * m.faces().size() * 3 * 3, m.VBO_vertices(), GL_STATIC_DRAW); // posició



  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[1]);
  // NOOOOUUU glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
  glBufferData (GL_ARRAY_BUFFER, sizeof(GLfloat) * m.faces ().size () * 3 * 3, m.VBO_matdiff(), GL_STATIC_DRAW); // color

  // Activem l'atribut colorLoc
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray (0);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/basicShader.frag");
  vs.compileSourceFile("shaders/basicShader.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  // Obtenim identificador per a l'atribut “color” del vertex shader
  colorLoc = glGetAttribLocation (program->programId(), "color");
  // Uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");
  
  
  //COSES NOVES
  projLoc = glGetUniformLocation (program->programId(), "proj");

  viewLoc = glGetUniformLocation (program->programId(), "view");


}

