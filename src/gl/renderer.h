/***** BEGIN LICENSE BLOCK *****

BSD License

Copyright (c) 2005-2015, NIF File Format Library and Tools
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the NIF File Format Library and Tools project may not be
   used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***** END LICENCE BLOCK *****/

#ifndef GLSHADER_H
#define GLSHADER_H

#include <QCoreApplication>
#include <QMap>
#include <QString>


//! @file renderer.h Renderer, Renderer::ConditionSingle, Renderer::ConditionGroup, Renderer::Shader, Renderer::Program

class NifModel;
class Shape;
class PropertyList;

class QOpenGLContext;
class QOpenGLFunctions;

typedef unsigned int GLenum;
typedef unsigned int GLuint;

//! Manages rendering and shaders
class Renderer : public QObject
{
	Q_OBJECT

	friend class Program;

public:
	Renderer( QOpenGLContext * c, QOpenGLFunctions * f );
	~Renderer();

	//! Set up shaders
	bool initialize();
	//! Whether shader support is available
	bool hasShaderSupport();

	//! Updates shaders
	void updateShaders();
	//! Releases shaders
	void releaseShaders();

	//! Context
	QOpenGLContext * cx;
	//! Context Functions
	QOpenGLFunctions * fn;

	//! Set up shader program
	QString setupProgram( Shape *, const QString & hint = QString() );
	//! Stop shader program
	void stopProgram();

public slots:
	void updateSettings();

protected:
	//! Base Condition class for shader programs
	class Condition
	{
public:
		Condition() {}
		virtual ~Condition() {}

		virtual bool eval( const NifModel * nif, const QList<QModelIndex> & iBlocks ) const = 0;
	};

	//! Condition class for single conditions
	class ConditionSingle final : public Condition
	{
public:
		ConditionSingle( const QString & line, bool neg = false );

		bool eval( const NifModel * nif, const QList<QModelIndex> & iBlocks ) const override final;

protected:
		QString left, right;
		enum Type
		{
			NONE, EQ, NE, LE, GE, LT, GT, AND, NAND
		};
		Type comp;
		const static QHash<Type, QString> compStrs;

		bool invert;

		QModelIndex getIndex( const NifModel * nif, const QList<QModelIndex> & iBlock, QString name ) const;
		template <typename T> bool compare( T a, T b ) const;
	};

	//! Condition class for grouped conditions (OR or AND)
	class ConditionGroup final : public Condition
	{
public:
		ConditionGroup( bool o = false ) { _or = o; }
		~ConditionGroup() { qDeleteAll( conditions ); }

		bool eval( const NifModel * nif, const QList<QModelIndex> & iBlocks ) const override final;

		void addCondition( Condition * c );

		bool isOrGroup() const { return _or; }

protected:
		QList<Condition *> conditions;
		bool _or;
	};

	//! Parsing and loading of .frag or .vert files
	class Shader
	{
public:
		Shader( const QString & name, GLenum type, QOpenGLFunctions * fn );
		~Shader();

		bool load( const QString & filepath );

		QOpenGLFunctions * f;
		QString name;
		GLuint id;
		bool status;

protected:
		GLenum type;
	};

	//! Parsing and loading of .prog files
	class Program
	{
public:
		Program( const QString & name, QOpenGLFunctions * fn );
		~Program();

		bool load( const QString & filepath, Renderer * );

		QOpenGLFunctions * f;
		QString name;
		GLuint id;
		bool status = false;

		ConditionGroup conditions;
		QMap<int, QString> texcoords;
	};

	QMap<QString, Shader *> shaders;
	QMap<QString, Program *> programs;

	bool setupProgram( Program *, Shape *, const PropertyList &, const QList<QModelIndex> & iBlocks );
	void setupFixedFunction( Shape *, const PropertyList & );

	struct Settings
	{
		bool useShaders = true;
	} cfg;
};


// Templates

template <typename T> inline bool Renderer::ConditionSingle::compare( T a, T b ) const
{
	switch ( comp ) {
	case EQ:
		return a == b;
	case NE:
		return a != b;
	case LE:
		return a <= b;
	case GE:
		return a >= b;
	case LT:
		return a < b;
	case GT:
		return a > b;
	case AND:
		return a & b;
	case NAND:
		return !(a & b);
	default:
		return true;
	}
}

template <> inline bool Renderer::ConditionSingle::compare( float a, float b ) const
{
	switch ( comp ) {
	case EQ:
		return a == b;
	case NE:
		return a != b;
	case LE:
		return a <= b;
	case GE:
		return a >= b;
	case LT:
		return a < b;
	case GT:
		return a > b;
	default:
		return true;
	}
}

template <> inline bool Renderer::ConditionSingle::compare( QString a, QString b ) const
{
	switch ( comp ) {
	case EQ:
		return a == b;
	case NE:
		return a != b;
	default:
		return false;
	}
}

#endif
