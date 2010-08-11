#include <QtGui>
#ifdef NEEDGLEE
#include "GLee.h"
#endif
#include <QDebug>
#include <QFileInfo>

#include <cstdio>
#include <cfloat>

#include <cmath>

#define RAD_TO_DEG(x)    ((x)*180.0/M_PI)
#define DEG_TO_RAD(x)    ((x)*M_PI/180.0)

#include "model.h"

Model::Model(TexCache *texcache): m_texcache(texcache) {
	;
}

static void resetError(void) {
	while (glGetError() != GL_NO_ERROR) {
		;
	}
}

Model::Model(void): m_texcache(NULL) {
	;
}

Model::~Model() {
	unload();
}

#define cb(C, M)							\
	static int C ## _ ## M ## _cb(const p_ply_argument argument) {	\
		void *pdata;						\
		ply_get_argument_user_data(argument, &pdata, NULL);	\
		QVector<C> &array = *static_cast<QVector<C> *>(pdata);	\
		long index;						\
		ply_get_argument_element(argument, NULL, &index);	\
		array[index].M(ply_get_argument_value(argument));	\
		return 1;						\
	}

cb(QVector4D,setX)

cb(QVector4D,setY)

cb(QVector4D,setZ)

cb(QVector4D,setW)

cb(QVector3D,setX)

cb(QVector3D,setY)

cb(QVector3D,setZ)

cb(QVector2D,setX)

cb(QVector2D,setY)

cb(Material,setDiffuseTexture)

cb(Material,setNormalmapTexture)

cb(Material,setFaceOffset)

cb(Material,setFaceCount)

cb(Material,setDiffuseRed)

cb(Material,setDiffuseGreen)

cb(Material,setDiffuseBlue)


static int face_cb(p_ply_argument argument) {
	void *pdata;
	ply_get_argument_user_data(argument, &pdata, NULL);
	QVector<GLuint> &array = *static_cast<QVector<GLuint> *>(pdata);
	long length, value_index;
	ply_get_argument_property(argument, NULL, &length, &value_index);
	if (length != 3) return 0;
	if (value_index >= 0) {
		long instance_index;
		ply_get_argument_element(argument, NULL, &instance_index);
		array[3*instance_index+value_index] = 
			(int) ply_get_argument_value(argument);
	}
	return 1;
}

void Model::unload() {
	glVertexPointer(3, GL_FLOAT, 0, NULL); 
	glNormalPointer(GL_FLOAT, 0, NULL);
	glColorPointer(3, GL_FLOAT, 0, NULL);
	glClientActiveTexture(GL_TEXTURE0); 
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	glClientActiveTexture(GL_TEXTURE1); 
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	glClientActiveTexture(GL_TEXTURE2); 
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	for (int i = 0; i < m_textures.size(); i++)
		m_texcache->deleteTexture(m_textures[i]);
	m_textures.resize(0);
	m_triangles.resize(0);
	m_normals.resize(0);
	m_positions.resize(0);
	m_texcoords.resize(0);
	m_colors.resize(0);
	m_tangents.resize(0);
	m_bitangents.resize(0);
}

void Model::computeNormals(void) {
	m_normals.resize(m_positions.size());
	for (int i = 0; i < m_triangles.size()/3; i++) {
		QVector3D &v1 = m_positions[m_triangles[3*i+0]];
		QVector3D &v2 = m_positions[m_triangles[3*i+1]];
		QVector3D &v3 = m_positions[m_triangles[3*i+2]];
		QVector3D n = QVector3D::crossProduct(v2-v1, v3-v1);
		m_normals[m_triangles[3*i+0]] += n;
		m_normals[m_triangles[3*i+1]] += n;
		m_normals[m_triangles[3*i+2]] += n;
	}
	for (int i = 0; i < m_positions.size(); i++) {
		m_normals[i].normalize();
	}
}

void Model::computeTangents(void) {
	m_tangents.resize(m_positions.size());
	m_bitangents.resize(m_positions.size());
	QVector<QVector3D> tangents(m_positions.size()), 
		bitangents(m_positions.size());
	for (int i = 0; i < m_triangles.size()/3; i++) {
		const GLuint i1 = m_triangles[3*i+0];
		const GLuint i2 = m_triangles[3*i+1];
		const GLuint i3 = m_triangles[3*i+2];
		const QVector3D &v1 = m_positions[i1];
		const QVector3D &v2 = m_positions[i2];
		const QVector3D &v3 = m_positions[i3];
		const QVector2D &u1 = m_texcoords[i1];
		const QVector2D &u2 = m_texcoords[i2];
		const QVector2D &u3 = m_texcoords[i3];
		const QVector3D v21 = v2-v1;
		const QVector3D v31 = v3-v1;
		const QVector2D u21 = u2-u1;
		const QVector2D u31 = u3-u1;
		float ta = u21.x()*u31.y() - u31.x()*u21.y();
		if( fabs(ta) < .00001f ) continue;
		float d = QVector3D::crossProduct(v21, v31).length() / ta;
		QVector3D t = d*(u31.y()*v21 - u21.y()*v31);
		QVector3D b = d*(u21.x()*v31 - u31.x()*v21);
		tangents[i1] += t;
		tangents[i2] += t;
		tangents[i3] += t;
		bitangents[i1] += b;
		bitangents[i2] += b;
		bitangents[i3] += b;
	}

	for (int i = 0; i < m_positions.size(); i++) {
		const QVector3D &n = m_normals[i];
		const QVector3D &t = tangents[i];
		const QVector3D &b = bitangents[i];
		QVector3D nt = (t - n*QVector3D::dotProduct(n, t)).normalized();
		qreal w = QVector3D::dotProduct(QVector3D::crossProduct(n, nt), b) < 0.f? -1.f: 1.f;
		m_tangents[i] = QVector4D(nt, w);
		m_bitangents[i] = w * QVector3D::crossProduct(n, nt);
	}
}

void Model::computeBBox(void) {
	m_bbox.reset();
	for (int i = 0; i < m_positions.size(); i++) {
		m_bbox.merge(m_positions[i]);
	}
}

int Model::loadMesh(p_ply ply, const char *filename) {
	(void) filename;
	int nvertices = ply_set_read_cb(ply, "vertex", "x", 
					QVector3D_setX_cb, &m_positions, 0);
	if (!nvertices) {
		ply_close(ply);
		return 0;
	}
	m_positions.resize(nvertices);
	ply_set_read_cb(ply, "vertex", "y", QVector3D_setY_cb, &m_positions, 0);
	ply_set_read_cb(ply, "vertex", "z", QVector3D_setZ_cb, &m_positions, 0);
	int nnormals = ply_set_read_cb(ply, "vertex", "nx", 
				       QVector3D_setX_cb, &m_normals, 0);
	if (nnormals) {
		m_normals.resize(nnormals);
		ply_set_read_cb(ply, "vertex", "ny", QVector3D_setY_cb, &m_normals, 0);
		ply_set_read_cb(ply, "vertex", "nz", QVector3D_setZ_cb, &m_normals, 0);
	}
	int ntexcoords = ply_set_read_cb(ply, "vertex", "s", 
					 QVector2D_setX_cb, &m_texcoords, 0);
	if (ntexcoords) {
		m_texcoords.resize(ntexcoords);
		ply_set_read_cb(ply, "vertex", "t", QVector2D_setY_cb, &m_texcoords, 0);
	} 

	int ntangents = ply_set_read_cb(ply, "vertex", "tx", 
					QVector4D_setX_cb, &m_tangents, 0);
	if (ntangents) {
		m_tangents.resize(ntangents);
		ply_set_read_cb(ply, "vertex", "ty", QVector4D_setY_cb, &m_tangents, 0);
		ply_set_read_cb(ply, "vertex", "tz", QVector4D_setZ_cb, &m_tangents, 0);
	}

	int nbitangents = ply_set_read_cb(ply, "vertex", "bx", 
					  QVector3D_setX_cb, &m_bitangents, 0);
	if (nbitangents) {
		m_bitangents.resize(nbitangents);
		ply_set_read_cb(ply, "vertex", "by", QVector3D_setY_cb, 
				&m_bitangents, 0);
		ply_set_read_cb(ply, "vertex", "bz", QVector3D_setZ_cb, 
				&m_bitangents, 0);
	}

	int ncolors = ply_set_read_cb(ply, "vertex", "red", 
				      QVector3D_setX_cb, &m_colors, 0);
	if (ncolors) {
		m_colors.resize(ncolors);
		ply_set_read_cb(ply, "vertex", "green", 
				QVector3D_setY_cb, &m_colors, 0);
		ply_set_read_cb(ply, "vertex", "blue", 
				QVector3D_setZ_cb, &m_colors, 0);
	} else {
		ncolors = ply_set_read_cb(ply, "vertex", "diffuse_red", 
					  QVector3D_setX_cb, &m_colors, 0);
		if (ncolors) {
			m_colors.resize(ncolors);
			ply_set_read_cb(ply, "vertex", "diffuse_green", 
					QVector3D_setY_cb, &m_colors, 0);
			ply_set_read_cb(ply, "vertex", "diffuse_blue", 
					QVector3D_setZ_cb, &m_colors, 0);
		}
	}
	int nt = ply_set_read_cb(ply, "face", "vertex_indices", face_cb, 
				 &m_triangles, 0);
	if (!nt) {
		ply_close(ply);
		unload();
		return 0;
	} else
		m_triangles.resize(3*nt);

	int nmo = ply_set_read_cb(ply, "material", "face_offset", 
				  Material_setFaceOffset_cb, &m_materials, 0);
	int nmc = ply_set_read_cb(ply, "material", "face_count", 
				  Material_setFaceCount_cb, &m_materials, 0);
	if (nmo != nmc) {
		ply_close(ply);
		unload();
		return 0;
	}
	m_materials.resize(nmo);
	ply_set_read_cb(ply, "material", "diffuse_texture", 
			Material_setDiffuseTexture_cb, &m_materials, 0);
	ply_set_read_cb(ply, "material", "normalmap_texture", 
			Material_setNormalmapTexture_cb, &m_materials, 0);
	ply_set_read_cb(ply, "material", "diffuse_red", 
			Material_setDiffuseRed_cb, &m_materials, 0);
	ply_set_read_cb(ply, "material", "diffuse_green", 
			Material_setDiffuseGreen_cb, &m_materials, 0);
	ply_set_read_cb(ply, "material", "diffuse_blue", 
			Material_setDiffuseBlue_cb, &m_materials, 0);

	if (!ply_read(ply)) {
		ply_close(ply);
		unload();
		return 0;
	}

	if (m_normals.isEmpty()) 
		computeNormals();

	if (hasColors()) {
		for (int i = 0; i < m_colors.size(); i++) 
			m_colors[i] *= (1.0/255.0);
	}

	if (hasTexCoords() && !hasTangents())
		computeTangents();

	qDebug() << "loaded" << m_positions.size() << "v" << m_triangles.size() << "t" << m_materials.size() << "m";
	return 1;
}

int Model::loadTextures(p_ply ply, const char *filename) {
	QFileInfo info;
	info.setFile(filename);
	QString path = info.canonicalPath();
	const char *obj_info = NULL;
	QRegExp rx("texture (.*)");
	while ((obj_info = ply_get_next_obj_info(ply, obj_info))) {
		QString str(obj_info);
		if (rx.indexIn(str, 0) != -1) {
			if (m_texcache) {
				QString texname = path + "/" + rx.cap(1);
				m_textures.push_back(m_texcache->bindTexture(QPixmap(texname)));
				qDebug() << "loaded" << texname << "into" << m_textures.last();
			} 
		}
	}
	return 1;
}

int Model::load(const char *filename) {
	unload();
	m_name = filename;
	p_ply ply = ply_open(filename, NULL, 0, NULL);
	if (!ply) return 0;
	if (!ply_read_header(ply)) {
		ply_close(ply);
		return 0;
	}
	if (!loadMesh(ply, filename)) return 0;
	computeBBox();
	loadTextures(ply, filename);
	createBuffers();
	return 1;
}

void Model::createBuffers(void) {
	glGenBuffers(1, &m_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	GLsizei pos = m_positions.size()*sizeof(QVector3D);
	GLsizei normal = m_normals.size()*sizeof(QVector3D);
	GLsizei color = m_colors.size()*sizeof(QVector3D);
	GLsizei tex = m_texcoords.size()*sizeof(QVector2D);
	GLsizei tan = m_tangents.size()*sizeof(QVector4D);
	GLsizei bitan = m_bitangents.size()*sizeof(QVector3D);
	resetError();
	glBufferData(GL_ARRAY_BUFFER, pos+normal+color+tex+tan+bitan, 
		     NULL, GL_STATIC_DRAW);
	GLenum error = glGetError();
	unsigned char *dest = NULL;
	if (error == GL_NO_ERROR && (dest = (unsigned char *) 
				     glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY))) {
		memcpy(dest, m_positions.constData(), pos);
		memcpy(dest+pos, m_normals.constData(), normal);
		memcpy(dest+pos+normal, m_colors.constData(), color);
		memcpy(dest+pos+normal+color, m_texcoords.constData(), tex);
		memcpy(dest+pos+normal+color+tex, m_tangents.constData(), tan);
		memcpy(dest+pos+normal+color+tex+tan, m_bitangents.constData(), bitan);
		glUnmapBuffer(GL_ARRAY_BUFFER);
	} else
		qDebug() << "Error" << error << "allocating VBO!";
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &m_elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementbuffer);
	GLsizei index = m_triangles.size()*sizeof(GLuint);
	resetError();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index, NULL, GL_STATIC_DRAW);
	if (error == GL_NO_ERROR && (dest = (unsigned char *) 
				     glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY))) {
		memcpy(dest, m_triangles.constData(), index);
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	} else
		qDebug() << "Error" << error << "allocating VBO!";

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	qDebug() << "vertex" << m_vertexbuffer << "element" << m_elementbuffer;
}

const BBox3D &Model::bbox(void) const {
	return m_bbox;
}

bool Model::hasColors(void) const {
	return m_positions.size() == m_colors.size();
}

bool Model::hasTexCoords(void) const {
	return m_positions.size() == m_texcoords.size();
}

bool Model::hasTangents(void) const {
	return m_positions.size() == m_tangents.size();
}

bool Model::hasBitangents(void) const {
	return m_positions.size() == m_bitangents.size();
}

void Model::draw(void) const {
	GLsizei pos = m_positions.size()*sizeof(QVector3D);
	GLsizei normal = m_normals.size()*sizeof(QVector3D);
	GLsizei color = m_colors.size()*sizeof(QVector3D);
	GLsizei tex = m_texcoords.size()*sizeof(QVector2D);
	GLsizei tan = m_tangents.size()*sizeof(QVector4D);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);

	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glEnableClientState(GL_VERTEX_ARRAY);
	glNormalPointer(GL_FLOAT, 0, ((unsigned char *) NULL)+pos);
	glEnableClientState(GL_NORMAL_ARRAY);

	if (hasColors()) {
		glColorPointer(3, GL_FLOAT, 0, ((unsigned char *) NULL)+pos+normal);
		glEnableClientState(GL_COLOR_ARRAY);
	}

	if (hasTexCoords()) {
		glClientActiveTexture(GL_TEXTURE0);
		glTexCoordPointer(2, GL_FLOAT, 0, 
				  ((unsigned char *) NULL)+pos+normal+color);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if (hasTangents()) {
		glClientActiveTexture(GL_TEXTURE1);
		glTexCoordPointer(4, GL_FLOAT, 0, 
				  ((unsigned char *) NULL)+pos+normal+color+tex);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if (hasBitangents()) {
		glClientActiveTexture(GL_TEXTURE2);
		glTexCoordPointer(3, GL_FLOAT, 0, 
				  ((unsigned char *) NULL)+pos+normal+color+tex+tan);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementbuffer);

	if (!m_materials.empty()) {
		for (int i = 0; i < m_materials.size(); i++) {
			const Material &m = m_materials[i];
			glColor3ub(m.diffuseRed(), m.diffuseGreen(), m.diffuseBlue());
			if (m.diffuseTexture() >= 0 && 
			    m.diffuseTexture() < m_textures.size()) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, m_textures[m.diffuseTexture()]);
			} 
			if (m.normalmapTexture() >= 0 && 
			    m.normalmapTexture() < m_textures.size()) {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, m_textures[m.normalmapTexture()]);
			} 
			if ((int) (3*(m.faceOffset() + m.faceCount())) 
			    <= m_triangles.size()) {
				glDrawElements(GL_TRIANGLES, 3*m.faceCount(),
					       GL_UNSIGNED_INT, ((char *) NULL)+3*m.faceOffset());
			}
		}
	} else {
		glDrawElements(GL_TRIANGLES, m_triangles.size(), 
			       GL_UNSIGNED_INT, NULL);
	}

	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glClientActiveTexture(GL_TEXTURE0); 
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE1); 
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE2); 
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

/// Draw model with minimum information (positions)
///   useful for wireframe drawing mode

void Model::drawBare(void) const {

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);

	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementbuffer);

	glDrawElements(GL_TRIANGLES, m_triangles.size(), 
		       GL_UNSIGNED_INT, NULL);

}

/// Draw a vector starting from p and going in d direction
/// @arg p starting position (x, y, z)
/// @arg d vector direction
/// @arg s vector scale (0.4% diagonal bounding box)

static
void glDrawVector( const QVector3D& p,
		   const QVector3D& d,
		   const float& s ) {

	QVector3D z(0.0, 0.0, 1.0); ///< z-axis direction

	float ra = RAD_TO_DEG( acos( QVector3D::dotProduct(d, z) / d.length() ) ); ///< rotation angle

	QVector3D rv = QVector3D::crossProduct(z, d); ///< rotation vector

	QVector3D l = d * s; ///< vector length
	QVector3D t = p + l; ///< vector tip

	GLUquadric *q = gluNewQuadric();

	glPushMatrix();

	glTranslatef( p.x(), p.y(), p.z() );

	glRotatef( ra, rv.x(), rv.y(), rv.z() );

	gluCylinder( q, s/16., s/16., l.length(), 8, 8 );

	glPopMatrix();

	glPushMatrix();

	glTranslatef( t.x(), t.y(), t.z() );

	glRotatef( ra, rv.x(), rv.y(), rv.z() );

	gluDisk( q, 0.0, s/8., 8, 8 );

	gluCylinder( q, s/8., 0.0, l.length() / 4., 8, 8 );

	glPopMatrix();

	gluDeleteQuadric(q);

}

/// Draw all TBN coordinate frame of all vertices
///   (Tangent Bitangent Normal)

void Model::drawTBN(void) const {

	qreal s = m_bbox.size().length() * 0.004;

	for (int i = 0; i < m_positions.size(); i++) {

		glColor4ub( 152, 32, 32, 128 );
		glDrawVector( m_positions[i], m_tangents[i].toVector3D(), s );

		glColor4ub( 32, 152, 32, 128 );
		glDrawVector( m_positions[i], m_bitangents[i], s );

		glColor4ub( 32, 32, 152, 128 );
		glDrawVector( m_positions[i], m_normals[i], s );

	}

}
