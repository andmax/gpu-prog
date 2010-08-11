#include <QtGui>
#ifdef NEEDGLEE
#include "GLee.h"
#endif
#include <QPainter>
#include <QMatrix4x4>
#include <QDebug>
#include <QMessageBox>

#include "glwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLWidget::GLWidget(QWidget *parent): 
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent), m_texcache(this)
{
    setAcceptDrops(true);
    m_frames = 0;
    m_timer.setInterval(0);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slotTimer()));
    m_vertshader = m_fragshader = m_program = 0;
    m_modeldrag = m_lightdrag = m_wireframe = m_tspace = false;
    m_start = m_time.time();
}

GLWidget::~GLWidget() {
    ;
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Q:
        case Qt::Key_Escape:
            emit quit();
            break;
        case Qt::Key_B:
            if (m_timer.isActive()) m_timer.stop();
            else m_timer.start();
            break;
        case Qt::Key_F:
            m_modelarcball.setRotation();
            m_lightarcball.setRotation();
            m_zoom.setScale();
            updateGL();
            break;
        case Qt::Key_D:
            if (!m_models.empty()) {
                Model *m = m_models.last();
                delete m;
                m_models.pop_back();
                m_bbox.reset();
                for (int i = 0; i < m_models.size(); i++) {
                    m_bbox.merge(m_models[i]->bbox());
                }
                updateGL();
            }
            break;
        case Qt::Key_T:
            m_tspace = !m_tspace;
            updateGL();
            break;
        case Qt::Key_W:
            m_wireframe = !m_wireframe;
            updateGL();
            break;
        default:
            return QGLWidget::keyPressEvent(event);
    }
}

void GLWidget::dragEnterEvent(QDragEnterEvent *event) {   
    event->acceptProposedAction();
}   

void GLWidget::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
}

void GLWidget::slotTimer(void) {
    updateGL();
}

static GLchar *loadFile(const char *name) {
    FILE *fp = fopen(name, "r");
    if (!fp) return NULL;
    fseek(fp, 0, SEEK_END);
    size_t n = ftell(fp);
    GLchar *b = (GLchar *) malloc(n+1);
    if (!b) return NULL;
    fseek(fp, 0, SEEK_SET);
    fread(b, n, 1, fp);
    b[n] = '\0';
    return b;
}

static bool loadShader(GLenum type, const char *filename,
    GLuint *shader, QString *log) {
    *shader = glCreateShader(type);
    const GLchar *p = loadFile(filename);
    if (!p) return false;
    glShaderSource(*shader, 1, &p, NULL);
    free((void *) p);
    glCompileShader(*shader);
    GLint len = 0;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        GLchar *m = (GLchar *) malloc(len);
        if (m) {
            GLint gotten = 0;
            glGetShaderInfoLog(*shader, len, &gotten, m);
            *log = m;
            free(m);
        }
    }
    GLint ret = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &ret);
    if (ret != GL_TRUE) {
        glDeleteShader(*shader);
        *shader = 0;
        return false;
    } else
      return true; 
}

static bool buildProgram(GLuint vertshader, GLuint fragshader, 
        GLuint *program, QString *log) {
    *program = glCreateProgram();
    glAttachShader(*program, vertshader);
    glAttachShader(*program, fragshader);
    glLinkProgram(*program);
    GLint len = 0;
    glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        GLchar *m = (GLchar *) malloc(len);
        if (m) {
            GLint gotten = 0;
            glGetProgramInfoLog(*program, len, &gotten, m);
            *log = m;
            free(m);
        }
    }
    GLint ret = 0;
    glGetProgramiv(*program, GL_LINK_STATUS, &ret);
    if (ret != GL_TRUE) {
        glDeleteProgram(*program);
        *program = 0;
        return false;
    } return true;
}

void GLWidget::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        QString filename;
        QFileInfo info;
        for (int i = 0; i < urlList.size(); ++i) {
            filename = urlList.at(i).toLocalFile();
            info.setFile(filename);
            if (info.isFile()) {
                QString ext = info.suffix();
                if (ext == "ply") {
                    Model *m = new Model(&this->m_texcache);
                    if (m->load(filename.toLatin1().data())) {
                        m_bbox.merge(m->bbox());
                        m_models.push_back(m);
                        event->acceptProposedAction();
                    } 
                } else if (ext == "vert") {
                    if (glIsProgram(m_program)) glDeleteProgram(m_program);
                    if (glIsShader(m_vertshader)) glDeleteShader(m_vertshader);
                    m_program = m_vertshader = 0;
                    QString log;
                    if (!loadShader(GL_VERTEX_SHADER, 
                        filename.toLatin1().data(), &m_vertshader, &log)) {
                        QMessageBox::information(this, 
                            "Error compiling shader",
                            info.fileName() + "\n\n" + log);
                    } else
                        event->acceptProposedAction();
                } else if (ext == "frag") {
                    if (glIsProgram(m_program)) glDeleteProgram(m_program);
                    if (glIsShader(m_fragshader)) glDeleteShader(m_fragshader);
                    m_program = m_fragshader = 0;
                    QString log;
                    if (!loadShader(GL_FRAGMENT_SHADER, 
                        filename.toLatin1().data(), &m_fragshader, &log)) {
                        QMessageBox::information(this, 
                            "Error compiling shader",
                            info.fileName() + "\n\n" + log);
                    } else
                        event->acceptProposedAction();
                } else 
                    qDebug() << "Unable to load file";
                updateGL();
            }
        }
        if (!glIsProgram(m_program) && 
             glIsShader(m_vertshader) && glIsShader(m_fragshader)) {
            QString log;
            if (!buildProgram(m_vertshader, m_fragshader, &m_program, &log)) {
                QMessageBox::information(this, "Error building program", log);
            }
            updateGL();
        }
    }
}

void GLWidget::dragLeaveEvent(QDragLeaveEvent *event) {
    event->accept();
}

QSize GLWidget::minimumSizeHint() const {
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const {
    return QSize(512, 512);
}

void GLWidget::initializeGL() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_MULTISAMPLE);

    static GLfloat lightModelAmbient[4] = { 0., 0., 0., 0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightModelAmbient);

    static GLfloat lightPosition[4] = { 0.5, 0.25, 0.83, 0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    static GLfloat lightAmbient[4] = { 0.25, 0.25, 0.25, 1 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    static GLfloat lightDiffuse[4] = { 0.85, 0.85, 0.85, 1 };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    static GLfloat lightSpecular[4] = { 0.5, 0.5, 0.5, 1 };
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    static GLfloat materialAmbient[4] = { 0.8, 0.5, 0.4, 1 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
    static GLfloat materialDiffuse[4] = { 0.8, 0.5, 0.4, 1 };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
    static GLfloat materialSpecular[4] = { 0.8, 0.8, 0.8, 1 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
    static GLfloat materialEmission[4] = { 0., 0., 0., 1 };
    glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);

    static GLfloat materialShininess = 16;
    glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glColor4d(0.8, 0.5, 0.4, 1.);

    m_font.setPointSize(10);
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    qreal z = m_zoom.scale();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (width() < height()) {
        qreal s = ((qreal) width())/height();
        glOrtho(-0.6*s, +0.6*s, -0.6, +0.6, 2.0*z, 4.0*z);
    } else {
        qreal s = ((qreal) height())/width();
        glOrtho(-0.6, +0.6, -0.6*s, +0.6*s, 2.0*z, 4.0*z);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // position light in eye space already
    QVector3D lp = m_lightarcball.rotation().rotatedVector(
            QVector3D(0.5, 0.25, 0.83)); 
    float lightposition[4] = {lp.x(), lp.y(), lp.z(), 0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightposition);

    glTranslated(0.0, 0.0, -3.0*z);

    QMatrix4x4 modelrotation;
    modelrotation.rotate(m_modelarcball.rotation().normalized());
    glMultMatrixd((GLdouble *) modelrotation.constData());

    QVector3D size = m_bbox.size();
    qreal s = z/qMax(qMax(size.x(), size.y()), size.z());
    glScaled(s, s, s);

    QVector3D center = m_bbox.center();
    glTranslatef(-center.x(), -center.y(), -center.z());
    glPointSize(3);

    glUseProgram(m_program);
    GLint diffuse = glGetUniformLocation(m_program, "diffuse_texture");
    glUniform1i(diffuse, 0);
    GLint normalmap = glGetUniformLocation(m_program, "normalmap_texture");
    glUniform1i(normalmap, 1);
    GLint time = glGetUniformLocation(m_program, "time");
    glUniform1f(time, (float) (m_time.time()-m_start));

    for (int i = 0; i < m_models.size(); i++)
        m_models[i]->draw();

    if( m_wireframe ) {

	glUseProgram( 0 );
        glPushAttrib( GL_POLYGON_BIT | GL_LINE_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT );
	glDisable( GL_LIGHTING );

	glEnable( GL_POLYGON_OFFSET_LINE | GL_LINE_SMOOTH );
        glPolygonMode( GL_FRONT, GL_LINE );
	glLineWidth( 2.f );

	glPolygonOffset( .1f, 1.f );
	glColor3f( .33f, .33f, .33f );

        for (int i = 0; i < m_models.size(); i++)
            m_models[i]->drawBare();
    
        glPopAttrib();

    }

    if( m_tspace ) {

	glUseProgram( 0 );
        glPushAttrib( GL_ENABLE_BIT | GL_CURRENT_BIT );
	glDisable( GL_LIGHTING );

        for (int i = 0; i < m_models.size(); i++)
            m_models[i]->drawTBN();

        glPopAttrib();

    }

    m_frames++;
    if (m_frames > 10) {
        double dt = m_time.elapsed();
        if (dt > 0.5) {
            m_fps = m_frames/dt;
            m_frames = 0;
            m_time.reset();
            emit fps(QString("%1 fps").arg((int)(m_fps+0.5)));
        }
    }
}

void GLWidget::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);
    qreal w = width/2.0f, h = height/2.0f;
    m_modelarcball.resize(QVector2D(w, h), qMin(w, h));
    m_lightarcball.resize(QVector2D(w, h), qMin(w, h));
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_modelarcball.mousePressEvent(event);
        m_modeldrag = true;
    } else if (event->button() == Qt::RightButton) {
        m_lightdrag = true;
        m_lightarcball.mousePressEvent(event);
    }
}

void GLWidget::wheelEvent(QWheelEvent *event) {
    m_zoom.wheelEvent(event);
    updateGL();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_modeldrag) m_modelarcball.mouseMoveEvent(event);
    if (m_lightdrag) m_lightarcball.mouseMoveEvent(event);
    updateGL();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_modelarcball.mouseReleaseEvent(event);
        m_modeldrag = false;
    } else if (event->button() == Qt::RightButton) {
        m_lightarcball.mouseReleaseEvent(event);
        m_lightdrag = false;
    }
    updateGL();
}
