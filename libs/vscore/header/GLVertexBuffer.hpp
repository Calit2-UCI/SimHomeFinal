//!
//!
//! File: GLVertexBuffer.hpp
//! Desc: Header of GLVertexBuffer.cpp that simplifies managment of OpenGL VBO
//! Author: Enrique Oriol
//! E-mail: enriqueoriol@naradarobotics.com
//!
//! Owner: Narada Robotics
//!
//!
//! Data: 13/09/2010
//!
//!



#ifndef GLVERTEXBUFFER_CORE_HPP_FILE
#define GLVERTEXBUFFER_CORE_HPP_FILE

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <cmath>
#include <iostream>

#include "VRMLRendererTraverser.h"

#ifdef WIN32
#include <gl\glaux.h>
#include <gl\glext.h>
#include <direct.h>

#ifndef GLsizeiptr
typedef int GLsizeiptr;
#endif

#elif defined(LINUX)
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <unistd.h>
#elif defined(MAC)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#include <unistd.h>
#elif defined(IPHONE)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
//#include "glues.h"
#include <unistd.h>
#endif




using namespace std;




namespace VisageSDK
{


	class GLVertexBuffer
	{

	private:
		
		float * coordPtr;//ES: half_float o algo así 
		float * normalPtr;
		float * cpvPtr;//uchar
		float * texCoordPtr;
		
		int * coordIndPtr;
		int * normalIndPtr;
		int * cpvIndPtr;
		int * texCoordIndPtr;
		
		int coordIndex;
		int normalIndex;
		int colorIndex;
		int textureIndex;
		
		int coordIndPtrLength;
		int normalIndPtrLength;
		int cpvIndPtrLength;
		int texCoordIndPtrLength;
		
		int coordPtrLength;
		int normalPtrLength;
		int cpvPtrLength;
		int texCoordPtrLength;
		
		int NPV;
		int CPV;
		bool mat;
		bool cubeMap;
		
		
		GLsizeiptr buffer_data_size;
		int vertex_data_size;
		
		
		
	public:

		
		
		//! \brief
		//!
		//! a new GLVertexBuffer object is created
		//!
		//! \param[in] *coord: pointer to vertex coordinates array
		//! \param[in] coordLength: length of the vertex coordinates array
		//! \param[in] *coordInd: pointer to coordinates index array
		//! \param[in] coordIndLength: length fo the coordinates index array
		//! \param[in] *norm: pointer to vertex normals array
		//! \param[in] normLength: length of the vertex normals array
		//! \param[in] *normInd: pointer to normals index array
		//! \param[in] normIndLength: length fo the normals index array
		//! \param[in] *tcoord: pointer to vertex texture coordinates array
		//! \param[in] tcoordLength: length of the vertex texture coordinates array
		//! \param[in] *tcoordInd: pointer to texture coordinates index array
		//! \param[in] tcoordIndLength: length fo the texture coordinates index array
		//! \param[in] *cpv: pointer to color-per-vertex coordinates array
		//! \param[in] cpvLength: length of the color-per-vertex coordinates array
		//! \param[in] *cpvInd: pointer to color-per-vertex index array
		//! \param[in] cpvIndLength: length fo the color-per-vertex index array
		//! \param[in] NPV: Normal per vertex, 1 if ON, 0 if OFF (then, normal per face)
		//! \param[in] CPV: Color per vertex, 1 if ON, 0 if OFF (then, color per face) 
		//!
		GLVertexBuffer(VRMLMaterial * material,
					   float *coord,
					   int coordLength,
					   int *coordInd,
					   int coordIndLength,
					   float *norm,
					   int normLength,
					   int *normInd,
					   int normIndLength,
					   float *tcoord,
					   int tcoordLength, 
					   int *tcoordInd, 
					   int tcoordIndLength, 
					   float *cpv,
					   int cpvLength, 
					   int *cpvInd, 
					   int cpvIndLength,
					   int NPV,
					   int CPV
					   );
		
		
		
		//! \brief VBO buffer size getter
		//!
		//! \return the total size (in bytes) needed to allocate a VBO with all data interleaved
		//!
		GLsizeiptr get_VBO_buffer_size();
		
		
		//! \brief num of vertex getter
		//!
		//! \return the total amount of vertex on the VBO
		//!
		int get_num_of_vertex();
		
		
		//! \brief Check if there're more vertex to read
		//!
		//! \return true if there are more vertex to read, false if not
		//!
		bool has_next();

		
		//! \brief Check if we are using colors
		//!
		//! \return true if using colors, false if not
		//!
		bool check_colors();

		
		//! \brief Check if we are using texture coordinates
		//!
		//! \return true if using texture coordinates, false if not
		//!
		bool check_texCoord();
		
		
		
		//! \brief coord getter
		//!
		//! \return a vector with next vertex coordinate values
		//!
		float* get_coord();


		//! \brief normal getter
		//!
		//! \return a vector with next vertex normal values
		//!
		float* get_normal();


		//! \brief texCoord getter
		//!
		//! \return a vector with next vertex texture coordinates values
		//!
		float* get_texCoord();


		//! \brief Color per Vertex getter
		//!
		//! \return a vector with next color per vertex values
		//!
		float* get_cpv();
		
		
		//! \brief coord size getter
		//!
		//! \return the size (in bytes) of a coordinates vertex
		//!
		int get_coord_size();
		
		
		//! \brief coord size getter
		//!
		//! \return the size (in bytes) of a coordinates vertex
		//!
		int get_normal_size();
		
		
		//! \brief coord size getter
		//!
		//! \return the size (in bytes) of a coordinates vertex
		//!
		int get_texCoord_size();
		
		
		//! \brief coord size getter
		//!
		//! \return the size (in bytes) of a coordinates vertex
		//!
		int get_cpv_size();
		
		
		//! \brief vertex size getter
		//!
		//! \return the size (in bytes) of a full vertex (coord, normals, texCoord and colors if any)
		//!
		int get_vertex_size();
		
		
		//! \brief coord. offset getter
		//!
		//! \return the offset of coordinates in the interleaved VBO array
		//!
		GLvoid *get_coord_offset();
		
		
		//! \brief normals offset getter
		//!
		//! \return the offset of normals in the interleaved VBO array
		//!
		GLvoid *get_normal_offset();
		
		
		//! \brief coord. offset getter
		//!
		//! \return the offset of cpv in the interleaved VBO array
		//!
		GLvoid *get_cpv_offset();
		
		
		//! \brief coord. offset getter
		//!
		//! \return the offset of texture coordinates in the interleaved VBO array
		//!
		GLvoid *get_texCoord_offset();

		
		//! \brief coordinate components getter
		//!
		//! \return the number of components in a coordinate
		//!
		unsigned char get_coord_comp();

		
		//! \brief normal components getter
		//!
		//! \return the number of components in a normal
		//!
		unsigned char get_normal_comp();
		
		
		//! \brief texture coordinate components getter
		//!
		//! \return the number of components in a texture coordinate
		//!
		unsigned char get_texCoord_comp();
		
		
		//! \brief color components getter
		//!
		//! \return the number of components in a color
		//!
		unsigned char get_cpv_comp();
		
		
		//! \brief kind of data coord getter
		//!
		//! \return the OpenGL kind of data associated with a coordinate
		//!
		int get_kindof_coord();

		
		//! \brief kind of data normal getter
		//!
		//! \return the OpenGL kind of data associated with a normal
		//!
		int get_kindof_normal();
		
		
		//! \brief kind of data texture coord getter
		//!
		//! \return the OpenGL kind of data associated with a texture coordinate
		//!
		int get_kindof_texCoord();
		
		
		//! \brief kind of data cpv getter
		//!
		//! \return the OpenGL kind of data associated with a color
		//!
		int get_kindof_cpv();
		


	private:




	};
	
}

#endif
