﻿#include <time.h>
#include"TrainView.h"
#include <fstream>

#define VT_WATER false
#define SLEEPER false


int uv_size = 0;
int pos_size = 0;
int normal_size = 0;

GLuint highmap_textureid = 0;
GLuint normalmap_textureid = 0;
GLuint terrain_textureid = 0;
//FBO
QOpenGLFramebufferObject *fbo;
QOpenGLTexture *fbo_texture;
GLfloat *lightview;
GLfloat lightprojection[16]{ 0 };
void TrainView::changeEffect(size_t effectNum)
{
	this->effectNum = effectNum;	
}
struct Model
{
private:
	QVector<GLfloat> values;
	GLuint textureid;
	std::vector<int> bufferoffset;	
public:
	Model()
	{
	
	}
	void setTextureid(GLuint id)
	{
		textureid = id;
	}
	GLuint getTextureid()
	{
		return textureid;
	}
	void setValues(QVector<GLfloat> v)
	{
		values = v;
	}
	QVector<GLfloat> getValues()
	{
		return values;
	}
	void setBufferOffset(std::vector<int> b)
	{
		bufferoffset = b;
	}
	std::vector<int> getBufferOffset()
	{
		return bufferoffset;
	}
};
std::vector<Model*> models;
void loadmodel(string modelname, string texturename, Model *model, QVector<QOpenGLTexture*> *textures);
char *stringToChar(string str);
void generateTextureCube(std::vector<QImage> images, QVector<QOpenGLTexture*> *textures)
{	
	QOpenGLTexture *t = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);	
	t->create();
	t->setSize(images.at(0).width(), images.at(0).height());
	t->setFormat(QOpenGLTexture::RGBAFormat);
	t->allocateStorage();
	t->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
	t->setData(0, 0, QOpenGLTexture::CubeMapNegativeZ, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (void*)images.at(0).bits());
	t->setData(0, 0, QOpenGLTexture::CubeMapPositiveZ, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (void*)images.at(1).bits());
	t->setData(0, 0, QOpenGLTexture::CubeMapNegativeX, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (void*)images.at(2).bits());
	t->setData(0, 0, QOpenGLTexture::CubeMapPositiveX, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (void*)images.at(3).bits());
	t->setData(0, 0, QOpenGLTexture::CubeMapPositiveY, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (void*)images.at(4).bits());
	t->setData(0, 0, QOpenGLTexture::CubeMapNegativeY, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, (void*)images.at(5).bits());
	t->setWrapMode(QOpenGLTexture::ClampToEdge);
	textures->push_back(t);
}
void TrainView::myTimer()
{
	clock_t currentTime = clock();
	float effect_interval = 100.f;
	float model_interval = 100.f;
	if (isrun && (currentTime - train_clock) > train_interval)
	{
		train_clock = currentTime;
		path_index+=train_speed;
		if (path_index >= path.size())
			path_index = 0;
	}
	if ((currentTime - effect_clock) > effect_interval)
	{
		effect_clock = currentTime;
	}
	if ((currentTime - model_clock) > model_interval)
	{
		model_clock = currentTime;
		wholeRotateAngle = (wholeRotateAngle < 360.f) ? wholeRotateAngle+=changeAngle:0.f;
		if (rightArmAngle < -100.f)
			angleTemp = changeAngle;
		else if (rightArmAngle > 0.f)
			angleTemp = changeAngleOpposite;

		rightArmAngle += angleTemp;		
		shadowShake = (shadowShake < 10.f) ? shadowShake += 1.f : (shadowShake > -10.f) ? shadowShake -= 1.f : shadowShake += 1.f;
	}
}
void TrainView::initializeGL()
{		
	trackobj = new Obj();
	trackobj->Init(2);
	trainobj = new Obj();
	trainobj->Init(2);
	miku = new Obj();
	miku->Init(2);
	land = new Obj();
	land->Init(4);
	mountain = new Obj();
	mountain ->Init(4);
	water = new Obj();
	if(VT_WATER)
		water->Init(2);
	else
		water->Init(4);
	skybox = new Obj();
	skybox->Init(2);
	nendoroid = new Obj();
	nendoroid->Init(2);		
	miku3d = new Obj();
	miku3d->Init(2);
	initializeTexture();
	Model *mikuhair, *mikuface,
		*mikubody,*mikuskirt,
		*mikuLFM,*mikuLHM,*mikuLH,
		*mikuRFM,*mikuRHM,*mikuRH,
		*mikuLT,*mikuLC,*mikuRT,*mikuRC,
		*scallion,
		*tunnel;
	{//Model init
		mikuhair = new Model();
		mikuface = new Model();
		mikubody = new Model();
		mikuskirt = new Model();
		mikuLFM = new Model();
		mikuLHM = new Model();
		mikuLH = new Model();
		mikuRFM = new Model();
		mikuRHM = new Model();
		mikuRH = new Model();
		mikuLT = new Model();
		mikuLC = new Model();
		mikuRT = new Model();
		mikuRC = new Model();
		scallion = new Model();
		//
		tunnel = new Model();

	}
	{//Load models
		loadmodel("./src/BSGC/prj3/3dmodel/mikuhair.obj", "./src/BSGC/prj3/3dmodel/mikuhair.png", mikuhair,&Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/mikuface.obj", "./src/BSGC/prj3/3dmodel/mikuface.png", mikuface, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/mikubody.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikubody, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/mikuskirt.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuskirt, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuLeftForeArm.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuLFM, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuLeftHindArm.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuLHM, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuLeftHand.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuLH, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuRightForeArm.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuRFM, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuRightHindArm.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuRHM, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuRightHand.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuRH, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuLeftThigh.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuLT, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuLeftCalf.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuLC, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuRightThigh.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuRT, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/MikuRightCalf.obj", "./src/BSGC/prj3/3dmodel/mikubody.png", mikuRC, &Textures);
		loadmodel("./src/BSGC/prj3/3dmodel/scallion.obj", "./src/BSGC/prj3/3dmodel/scallion.png", scallion, &Textures);
		//
		loadmodel("./src/BSGC/prj3/3dmodel/tunnel.obj", "./src/BSGC/prj3/3dmodel/tunnel_diffusemap.jpg", tunnel, &Textures);
	}
	{//Model vector
		models.push_back(mikuhair);
		models.push_back(mikuface);
		models.push_back(mikubody);
		models.push_back(mikuskirt);
		models.push_back(mikuLFM);
		models.push_back(mikuLHM);
		models.push_back(mikuLH);
		models.push_back(mikuRFM);
		models.push_back(mikuRHM);
		models.push_back(mikuRH);
		models.push_back(mikuLT);
		models.push_back(mikuLC);
		models.push_back(mikuRT);
		models.push_back(mikuRC);
		models.push_back(scallion);
		models.push_back(tunnel);		
	}	
	fbo = new QOpenGLFramebufferObject(width(), height());
	fbo->setAttachment(QOpenGLFramebufferObject::Depth);
	fbo->bind();
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	fbo->bindDefault();
}
char *stringToChar(string str)
{
	char *c = new char[str.length() + 1];
	return strcpy(c, str.c_str());
}
void loadmodel(string modelname,string texturename, Model *model, QVector<QOpenGLTexture*> *textures)
{
	ifstream in;
	QVector<QVector3D> v,vn,f;
	QVector<QVector2D> vt;
	QVector<GLfloat> model_vts;
	in.open(modelname,ios::in || ios::binary);
	if (!in)
	{
		printf("Can't read file.");
		return;
	}
	char buffer[4096];
	while (in.peek() != EOF)
	{
		in.getline(buffer, 4096);
		char *t = strtok(buffer, " "),*t1,*t2,*t3;
		if (t && strncmp(t, "v", 1) == 0 && strncmp(t, "vt", 2) != 0 && strncmp(t, "vn", 2) != 0)
		{
			float f1 = 0,f2 = 0,f3 = 0;
			t = strtok(NULL, " ");
			f1 = atof(t);
			t = strtok(NULL, " ");
			f2 = atof(t);
			t = strtok(NULL, " ");
			f3 = atof(t);
			v << QVector3D(f1,f2,f3);
		}
		else if (t && strncmp(t, "v", 1) == 0 && strncmp(t, "vt", 2) == 0 && strncmp(t, "vn", 2) != 0)
		{
			float f1 = 0, f2 = 0;
			t = strtok(NULL, " ");
			f1 = atof(t);
			t = strtok(NULL, " ");
			f2 = atof(t);			
			vt << QVector2D(f1, f2);
		}
		else if (t && strncmp(t, "v", 1) == 0 && strncmp(t, "vt", 2) != 0 && strncmp(t, "vn", 2) == 0)
		{
			float f1 = 0, f2 = 0, f3 = 0;
			t = strtok(NULL, " ");
			f1 = atof(t);
			t = strtok(NULL, " ");
			f2 = atof(t);
			t = strtok(NULL, " ");
			f3 = atof(t);
			vn << QVector3D(f1, f2, f3);
		}
		else if (t && strncmp(t, "f", 1) == 0)
		{	
			int i1= 0 , i2 = 0, i3 = 0;
			t1 = strtok(NULL, " ");			
			t2 = strtok(NULL, " ");
			t3 = strtok(NULL, " ");

			t = strtok(t1, "/");
			i1 = atoi(t);
			t = strtok(NULL, "/");
			i2 = atoi(t);
			t = strtok(NULL, "/");
			i3 = atoi(t);
			f << QVector3D(i1,i2,i3);

			t = strtok(t2, "/");
			i1 = atoi(t);
			t = strtok(NULL, "/");
			i2 = atoi(t);
			t = strtok(NULL, "/");
			i3 = atoi(t);
			f << QVector3D(i1, i2, i3);
			
			t = strtok(t3, "/");
			i1 = atoi(t);
			t = strtok(NULL, "/");
			i2 = atoi(t);
			t = strtok(NULL, "/");
			i3 = atoi(t);
			f << QVector3D(i1, i2, i3);
		}
	}
	in.close();
	//Model processing
	for (size_t i = 0; i < f.size(); i++)
	{
		model_vts << v[f[i].x()-1].x() << v[f[i].x()-1].y() << v[f[i].x()-1].z();
	}
	pos_size = model_vts.size();
	for (size_t i = 0; i < f.size(); i++)
	{
		model_vts << vt[f[i].y()-1].x() << vt[f[i].y()-1].y();
	}
	uv_size = (model_vts.size() - pos_size);
	for (size_t i = 0; i < f.size(); i++)
	{
		model_vts << vn[f[i].z() - 1].x() << vn[f[i].z() - 1].y() << vn[f[i].z() - 1].z();;
	}
	normal_size = (model_vts.size() - pos_size - uv_size);
	model->setValues(model_vts);
	QOpenGLTexture *tex = new QOpenGLTexture(QImage(stringToChar(texturename)));
	tex->setMinificationFilter(QOpenGLTexture::Linear);
	tex->setMagnificationFilter(QOpenGLTexture::Linear);
	tex->setWrapMode(QOpenGLTexture::ClampToEdge);
	model->setTextureid(textures->size());
	textures->push_back(tex);
	std::vector<int> bufferoffset;
	bufferoffset.push_back(pos_size);
	bufferoffset.push_back(uv_size);
	bufferoffset.push_back(normal_size);
	model->setBufferOffset(bufferoffset);	
}
void TrainView::initializeTexture()
{	
	miku->textureId = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/miku_transparent.png")));
	land->textureId = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/underwater.jpg")));
	water->textureId = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/water.png")));	
	std::vector<QImage> skybox_imgs;
	skybox_imgs.push_back(QImage("./src/BSGC/prj3/Textures/hangingstone_front.jpg"));
	skybox_imgs.push_back(QImage("./src/BSGC/prj3/Textures/hangingstone_back.jpg"));
	skybox_imgs.push_back(QImage("./src/BSGC/prj3/Textures/hangingstone_left.jpg"));
	skybox_imgs.push_back(QImage("./src/BSGC/prj3/Textures/hangingstone_right.jpg"));
	skybox_imgs.push_back(QImage("./src/BSGC/prj3/Textures/hangingstone_top.jpg"));
	skybox_imgs.push_back(QImage("./src/BSGC/prj3/Textures/hangingstone_bottom.jpg"));
	generateTextureCube(skybox_imgs,&Textures);
	skybox->textureId = Textures.size()-1;	
	nendoroid->textureId = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/nendoroid_front.png")));
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/nendoroid_back.png")));
	//Highmap
	highmap_textureid = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/heightmap.png")));
	normalmap_textureid = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/water_normal.jpg")));
	terrain_textureid = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/terrain.jpg")));
	mountain->textureId = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/mountain.jpg")));
	trackobj->textureId = Textures.size();
	Textures.push_back(new QOpenGLTexture(QImage("./src/BSGC/prj3/Textures/train.png")));
}
TrainView::TrainView(QWidget *parent) :  
QGLWidget(parent)  
{  
	resetArcball();	
}  
TrainView::~TrainView()  
{}  
void TrainView:: resetArcball()
	//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}
void TrainView::changeSpeed(int speed) 
{
	//train_interval = 1000.f - 999.f * (float)(speed/100.f);
	train_speed = 10 * (float)(speed / 100.f);
	if (speed > 70 && speed < 85)
		train_effect = 5;
	else if(speed > 85)
		train_effect = 4;
	else
		train_effect = 0;
}
void TrainView::paint()
{
	//Update time
	myTimer();
	glEnable(GL_DEPTH);
	if (useFBO)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, 1000, 1000);
		fbo->bind();		
		glMatrixMode(GL_PROJECTION);
		glOrtho(-15.f,15.f,-15.f,15.f,0.1f,50.f);		
		gluLookAt(30.f, 50.f, 560.f,0,0,0,0,1,0);
		//glGetFloatv(GL_MODELVIEW_MATRIX, lightview);
		QMatrix4x4 m;
		m.lookAt(QVector3D(30.f, 50.f, 560.f), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
		lightview = m.data();
		glGetFloatv(GL_PROJECTION_MATRIX, lightprojection);
	}
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	if (this->camera == 1)
	{
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}
	else 
	{
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}
	GLfloat lightPosition1[] = { 0,1,1,0 }; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[] = { 1, 0, 0, 0 };
	GLfloat lightPosition3[] = { 0, -1, 0, 0 };
	GLfloat yellowLight[] = { 0.5f, 0.5f, .1f, 1.0 };
	GLfloat whiteLight[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	GLfloat blueLight[] = { .1f,.1f,.3f,1.0 };
	GLfloat grayLight[] = { .3f, .3f, .3f, 1.0 };
	std::vector<int> buffer_size;
	float boxsize = 1500;

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);
	glDisable(GL_LIGHTING);

	glEnable(GL_LIGHTING);
	setupObjects();
		
	//All texture	
	for (size_t i = 0; i < Textures.size(); i++)
	{
		Textures[i]->bind(i);
	}
	//Draw skybox
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glTranslatef(0, 700, 0);
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);	
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	float skyboxVertices[] =
	{
		/*//////positions//////*/
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	for (size_t i = 0; i < sizeof(skyboxVertices) / sizeof(float); i++)
	{
		skyboxVertices[i] *= boxsize;
	}
	int vt_size = sizeof(skyboxVertices) / sizeof(float);
	QVector<GLfloat> skybox_vts;
	skybox->Begin();

	skybox->shaderProgram->setUniformValue("texcube", skybox->textureId);

	for (size_t i = 0; i < vt_size; i++)
	{
		skybox_vts << skyboxVertices[i];
	}

	buffer_size.push_back(vt_size);	
	skybox->Render(GL_TRIANGLES,false,1,ProjectionMatrex, ModelViewMatrex, skybox_vts, buffer_size, 1.f, effect_clock,(effectNum == 0?2: effectNum));
	skybox_vts.clear();
	buffer_size.clear();
	skybox->End();
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);

	//Draw track and train
	glPushMatrix();
	drawStuff(false);
	glPopMatrix();

	//Draw land
	float horizon = -250.f;
	glPushMatrix();
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	land->Begin();
	land->shaderProgram->setUniformValue("tex", land->textureId);
	land->shaderProgram->setUniformValue("heightmap", terrain_textureid);
	land->shaderProgram->setUniformValue("camerapos", QVector3D(arcball.posx, arcball.posy, arcball.posz));
	QVector<GLfloat> land_vts;
	land_vts
		<< -boxsize << horizon << -boxsize
		<< boxsize << horizon << -boxsize
		<< boxsize << horizon << boxsize
		<< -boxsize << horizon << boxsize;
	buffer_size.clear();
	buffer_size.push_back(12);	
	land->Render(GL_PATCHES,true,0,ProjectionMatrex, ModelViewMatrex, land_vts, buffer_size, 1.f, effect_clock, effectNum);
	land->End();
	glPopMatrix();

	//Draw water
	glPushMatrix();
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		float wave_t = 0;
		int water_size = 70;
		float min = -boxsize;
		float amplitude = 2.5f;
		float speed = 0.0001f;
		float step = boxsize / water_size * 2;
		float old_wave_height = 0;
		float wy = 0.f;
		float wave_height = 0.f;

		setupFloor();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		water->Begin();
		water->shaderProgram->setUniformValue("tex", water->textureId);
		water->shaderProgram->setUniformValue("heightmap", highmap_textureid);	
		water->shaderProgram->setUniformValue("texcube", skybox->textureId);
		water->shaderProgram->setUniformValue("camerapos", QVector3D(arcball.posx, arcball.posy, arcball.posz));

		float ratio = 25;
		float xfrom = 0;
		float zfrom = 0;
		float xto = 0;
		float zto = 0;
		float padding = 0;

		QVector<GLfloat> water_vertices;
		water_vertices.clear();
		buffer_size.clear();
		water_vertices
			<< min << wy << min
			<< -min << wy << min
			<< -min << wy << -min
			<< min << wy << -min;
		buffer_size.push_back(12);
		buffer_size.push_back(0);
		water->Render(GL_PATCHES,true,2,ProjectionMatrex, ModelViewMatrex, water_vertices, buffer_size, 0.7, effect_clock*0.001f, effectNum);
		water->End();
		water_vertices.clear();
		buffer_size.clear();
	glPopMatrix();
	glDisable(GL_BLEND);
	//Draw shadows
	glPushMatrix();
		if (this->camera != 1)
		{
			glTranslatef(0, shadowShake, 0);
			setupShadows();
			drawStuff(true);
			unsetupShadows();		
		}
	glPopMatrix();
	glPopMatrix();
		//Draw mountains
		float mountain_h = -125.f;
		glPushMatrix();
		QMatrix4x4 m;
		m.setToIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		mountain->Begin();
		mountain->shaderProgram->setUniformValue("tex", mountain->textureId);
		mountain->shaderProgram->setUniformValue("heightmap", terrain_textureid);
		mountain->shaderProgram->setUniformValue("camerapos", QVector3D(arcball.posx, arcball.posy, arcball.posz));
		QVector<GLfloat> mountain_vts;
		mountain_vts
			<< -200.f << mountain_h << -200.f
			<< 200.f << mountain_h << -200.f
			<< 200.f << mountain_h << 200.f
			<< -200.f << mountain_h << 200.f;
		buffer_size.clear();
		buffer_size.push_back(12);
		buffer_size.push_back(0);
		mountain->Render(GL_PATCHES,true,0,ProjectionMatrex,ModelViewMatrex, mountain_vts, buffer_size, 1.f, effect_clock, effectNum);
		mountain->End();
	glPopMatrix();
	
	/*fbo->bindDefault();
	
	if (useFBO)
	{
		fbo->release();
		fbo_texture = new QOpenGLTexture(fbo->toImage());

	}*/	
}
void TrainView::paintGL()
{	
	glClearColor(0, 0, 0, 0);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	
	//useFBO = true;
	//paint();
	glViewport(0, 0, width(), height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();
	//Draw shadows
	/*QVector<GLfloat> fbo_vts;
	std::vector<int>buffer_size;
	float frame_size = 200.f;
	fbo_texture->bind(0);
	glPushMatrix();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		miku->Begin();
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		fbo_vts.clear();
		fbo_vts
			<< -frame_size << -frame_size << 300
			<< frame_size << -frame_size << 300
			<< frame_size << frame_size << 300
			<< frame_size << frame_size << 300
			<< -frame_size << frame_size << 300
			<< -frame_size << -frame_size << 300;
		buffer_size.clear();
		buffer_size.push_back(18);
		fbo_vts
			<< 0 << 1
			<< 1 << 1
			<< 1 << 0
			<< 1 << 0
			<< 0 << 0
			<< 0 << 1;
		buffer_size.push_back(12);
		miku->shaderProgram->setUniformValue("tex", 0);
		miku->Render(GL_TRIANGLES, false, 0, lightprojection, lightview, fbo_vts, buffer_size, 1, effect_clock, effectNum);
		
		miku->End();
	glPopMatrix();*/
	useFBO = false;
	paint();
}
//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(width()) / static_cast<float>(height());
	
	// Check whether we use the world camp
	if (this->camera == 0)
	{						
		glMatrixMode(GL_MODELVIEW);		
		glLoadIdentity();
		arcball.setProjection(false);
		
		update();
	// Or we use the top cam
	}else if (this->camera == 1) 
	{
		float wi, he;
		if (aspect >= 1) 
		{
			wi = 110;
			he = wi / aspect;
		} 
		else {
			he = 110;
			wi = he * aspect;
		}
		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		float view_ratio = 4.5f;
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi * view_ratio, wi * view_ratio, -he * view_ratio, he * view_ratio, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90,1,0,0);
		update();
	}	
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else if (this->camera == 2) 
	{		
		if (path.size() > 0) 
		{		
		glPushMatrix();
			glMatrixMode(GL_PROJECTION);				
			gluPerspective(120, 1, 1, 200);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();		
			Pnt3f p1 = path[path_index].points;
			Pnt3f p2 = path[(path_index + 1) % path.size()].points;
			Pnt3f dec = trainEnd - trainStart;
			dec.normalize();
			
			float angle = -radiansToDegrees(atan2(path[(path_index)].orients.z, path[(path_index)].orients.x));			
			if (angle > 0)
				angle = -radiansToDegrees(acos(path[(path_index)].orients.y));
			else
				angle = radiansToDegrees(acos(path[(path_index)].orients.y));
			glRotatef(-angle, 0, 0, dec.z);
			gluLookAt
			(
				p1.x, p1.y + 25, p1.z ,//camera coordinates
				p2.x, p2.y + 25, p2.z,//look for
				-path[(path_index)].orients.x, path[(path_index)].orients.y, -path[(path_index)].orients.z
			);		
		glPopMatrix();
		update();
		}		
		
#ifdef EXAMPLE_SOLUTION
		trainCamView(this,aspect);
#endif		
		
	}
}
void TrainView::drawTrack(bool doingShadows) 
{
	float alpha = 0.3f;
	QVector<GLfloat> tracks,sleepers;
	std::vector<int> buffer_size;
	spline_t spline_type = (spline_t)curve;
	Pnt3f qt0, qt, orient_t, or0, or1;		
	track_path.clear();
	track_orient_cross.clear();
	track_orient.clear();
	Pnt3f track_start_inside, track_start_outside, track_end_inside, track_end_outside;
	Pnt3f temp;
	std::vector<Pnt3f> connect_head,connect_tail;
	for (size_t i = 0; i < m_pTrack->points.size(); i++) 
	{
		Pnt3f points1 = m_pTrack->points[i].pos;
		Pnt3f points2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].pos;
		Pnt3f orient1 = m_pTrack->points[i].orient;
		Pnt3f orient2 = m_pTrack->points[(i + 1) % m_pTrack->points.size()].orient;
		
		float distance = sqrt(pow(points1.x - points2.x, 2) + pow(points1.y - points2.y, 2) + pow(points1.z - points2.z, 2));
		unsigned int divide = distance / 8.f;
		if (divide < 4)
		{
			divide = 4;
		}
		ControlPoint q_matrix[4];		
		
		if (i == 0)
		{
			q_matrix[0] = m_pTrack->points[m_pTrack->points.size() - 1];
			q_matrix[1] = m_pTrack->points[0];
		}
		else if (i == 1)
		{
			q_matrix[0] = m_pTrack->points[0];
			q_matrix[1] = m_pTrack->points[i];
		}
		else
		{
			q_matrix[0] = m_pTrack->points[i - 1];
			q_matrix[1] = m_pTrack->points[i];
		}		
		q_matrix[2] = m_pTrack->points[(i + 1) % m_pTrack->points.size()];
		q_matrix[3] = m_pTrack->points[(i + 2) % m_pTrack->points.size()];
		
		float percent = 1.f / divide;
		float t = 0.f - percent;
		if (spline_type == spline_CardinalCubic) 
		{			
			qt = Pnt3f(q_matrix[0].pos * (-1 / 2.f) + q_matrix[1].pos * (3 / 2.f) + q_matrix[2].pos * (-3 / 2.f) + q_matrix[3].pos * (1 / 2.f)) * pow(t, 3)
				+ (q_matrix[0].pos * (2 / 2.f) + q_matrix[1].pos * (-5 / 2.f) + q_matrix[2].pos * (4 / 2.f) + q_matrix[3].pos * (-1 / 2.f)) * pow(t, 2)
				+ (q_matrix[0].pos * (-1 / 2.f) + q_matrix[1].pos * 0 + q_matrix[2].pos * (1 / 2.f) + q_matrix[3].pos * 0) * pow(t, 1)
				+ (q_matrix[0].pos * 0 + q_matrix[1].pos * (2 / 2.f) + q_matrix[2].pos * 0 + q_matrix[3].pos * 0) * 1;
			orient_t = Pnt3f(q_matrix[0].orient * (-1 / 2.f) + q_matrix[1].orient * (3 / 2.f) + q_matrix[2].orient * (-3 / 2.f) + q_matrix[3].orient * (1 / 2.f)) * pow(t, 3)
				+ (q_matrix[0].orient * (2 / 2.f) + q_matrix[1].orient * (-5 / 2.f) + q_matrix[2].orient * (4 / 2.f) + q_matrix[3].orient * (-1 / 2.f)) * pow(t, 2)
				+ (q_matrix[0].orient * (-1 / 2.f) + q_matrix[1].orient * 0 + q_matrix[2].orient * (1 / 2.f) + q_matrix[3].orient * 0) * pow(t, 1)
				+ (q_matrix[0].orient * 0 + q_matrix[1].orient * (2 / 2.f) + q_matrix[2].orient * 0 + q_matrix[3].orient * 0) * 1;
		}
		else if (spline_type == spline_CubicB_Spline) 
		{			
			qt = (q_matrix[0].pos * (-1 / 6.f) + q_matrix[1].pos * (3 / 6.f) + q_matrix[2].pos * (-3 / 6.f) + q_matrix[3].pos * (1 / 6.f)) * pow(t, 3)
				+ (q_matrix[0].pos * (3 / 6.f) + q_matrix[1].pos * (-6 / 6.f) + q_matrix[2].pos * (3 / 6.f) + q_matrix[3].pos * 0) * pow(t, 2)
				+ (q_matrix[0].pos * (-3 / 6.f) + q_matrix[1].pos * 0 + q_matrix[2].pos * (3 / 6.f) + q_matrix[3].pos * 0) * pow(t, 1)
				+ (q_matrix[0].pos * (1 / 6.f) + q_matrix[1].pos * (4 / 6.f) + q_matrix[2].pos * (1 / 6.f) + q_matrix[3].pos * 0) * 1;
			orient_t = Pnt3f(q_matrix[0].orient * (-1 / 6.f) + q_matrix[1].orient * (3 / 6.f) + q_matrix[2].orient * (-3 / 6.f) + q_matrix[3].orient * (1 / 6.f)) * pow(t, 3)
				+ (q_matrix[0].orient * (3 / 6.f) + q_matrix[1].orient * (-6 / 6.f) + q_matrix[2].orient * (3 / 6.f) + q_matrix[3].orient * 0) * pow(t, 2)
				+ (q_matrix[0].orient * (-3 / 6.f) + q_matrix[1].orient * 0 + q_matrix[2].orient * (3 / 6.f) + q_matrix[3].orient * 0) * pow(t, 1)
				+ (q_matrix[0].orient * (1 / 6.f) + q_matrix[1].orient * (4 / 6.f) + q_matrix[2].orient * (1 / 6.f) + q_matrix[3].orient * 0) * 1;
		}
		else if (spline_type == spline_Linear) 
		{
			qt0 = points1;
			qt = points2;
			if (t < 0)
				t = 0;
			orient_t = (1.f - t)*orient1 + t * orient2;
		}

		Pnt3f connect_outside, connect_inside;
		for (size_t j = 0; j <= divide; j++)
		{			
			if (j == 0)
			{				
				qt0 = qt0;
			}				
			else
			{
				qt0 = qt;
			}
			or0 = orient_t;
			trainStart = qt0;
			switch (spline_type) 
			{
			case spline_Linear:
				orient_t = (1.f - t)*orient1 + t * orient2;
				break;
			case spline_CardinalCubic:
				orient_t = Pnt3f(q_matrix[0].orient * (-1 / 2.f) + q_matrix[1].orient * (3 / 2.f) + q_matrix[2].orient * (-3 / 2.f) + q_matrix[3].orient * (1 / 2.f)) * pow(t, 3)
					+ (q_matrix[0].orient * (2 / 2.f) + q_matrix[1].orient * (-5 / 2.f) + q_matrix[2].orient * (4 / 2.f) + q_matrix[3].orient * (-1 / 2.f)) * pow(t, 2)
					+ (q_matrix[0].orient * (-1 / 2.f) + q_matrix[1].orient * 0 + q_matrix[2].orient * (1 / 2.f) + q_matrix[3].orient * 0) * pow(t, 1)
					+ (q_matrix[0].orient * 0 + q_matrix[1].orient * (2 / 2.f) + q_matrix[2].orient * 0 + q_matrix[3].orient * 0) * 1;
				break;
			case spline_CubicB_Spline:
				orient_t = Pnt3f(q_matrix[0].orient * (-1 / 6.f) + q_matrix[1].orient * (3 / 6.f) + q_matrix[2].orient * (-3 / 6.f) + q_matrix[3].orient * (1 / 6.f)) * pow(t, 3)
					+ (q_matrix[0].orient * (3 / 6.f) + q_matrix[1].orient * (-6 / 6.f) + q_matrix[2].orient * (3 / 6.f) + q_matrix[3].orient * 0) * pow(t, 2)
					+ (q_matrix[0].orient * (-3 / 6.f) + q_matrix[1].orient * 0 + q_matrix[2].orient * (3 / 6.f) + q_matrix[3].orient * 0) * pow(t, 1)
					+ (q_matrix[0].orient * (1 / 6.f) + q_matrix[1].orient * (4 / 6.f) + q_matrix[2].orient * (1 / 6.f) + q_matrix[3].orient * 0) * 1;
				break;
			}
			t += percent;
			or1 = orient_t;
			switch (spline_type) 
			{
			case spline_Linear:
				qt = (1.f - t)*points1 + t * points2;
				break;
			case spline_CardinalCubic:
				qt = Pnt3f(q_matrix[0].pos * (-1 / 2.f) + q_matrix[1].pos * (3 / 2.f) + q_matrix[2].pos * (-3 / 2.f) + q_matrix[3].pos * (1 / 2.f)) * pow(t, 3)
					+ (q_matrix[0].pos * (2 / 2.f) + q_matrix[1].pos * (-5 / 2.f) + q_matrix[2].pos * (4 / 2.f) + q_matrix[3].pos * (-1 / 2.f)) * pow(t, 2)
					+ (q_matrix[0].pos * (-1 / 2.f) + q_matrix[1].pos * 0 + q_matrix[2].pos * (1 / 2.f) + q_matrix[3].pos * 0) * pow(t, 1)
					+ (q_matrix[0].pos * 0 + q_matrix[1].pos * (2 / 2.f) + q_matrix[2].pos * 0 + q_matrix[3].pos * 0) * 1;
				break;
			case spline_CubicB_Spline:
				qt = (q_matrix[0].pos * (-1 / 6.f) + q_matrix[1].pos * (3 / 6.f) + q_matrix[2].pos * (-3 / 6.f) + q_matrix[3].pos * (1 / 6.f)) * pow(t, 3)
					+ (q_matrix[0].pos * (3 / 6.f) + q_matrix[1].pos * (-6 / 6.f) + q_matrix[2].pos * (3 / 6.f) + q_matrix[3].pos * 0) * pow(t, 2)
					+ (q_matrix[0].pos * (-3 / 6.f) + q_matrix[1].pos * 0 + q_matrix[2].pos * (3 / 6.f) + q_matrix[3].pos * 0) * pow(t, 1)
					+ (q_matrix[0].pos * (1 / 6.f) + q_matrix[1].pos * (4 / 6.f) + q_matrix[2].pos * (1 / 6.f) + q_matrix[3].pos * 0) * 1;
				break;
			}
			trainEnd = qt;
			
			Pnt3f cross_t0, cross_t1;
			float track_width = 5.f;
			cross_t0 = Pnt3f(qt.x - qt0.x, qt.y - qt0.y, qt.z - qt0.z) * or0;
			cross_t1 = Pnt3f(qt.x - qt0.x, qt.y - qt0.y, qt.z - qt0.z) * or1;
			cross_t0.normalize();
			cross_t1.normalize();
			cross_t0 = cross_t0 * track_width;//Track width
			
			if (track == 1) 
			{				
				glLineWidth(2);								
				orient_t.normalize();
				float tan_v = atan2f(cross_t0.z, cross_t0.x);
				if (!doingShadows) 
				{
					glColor4f(0, 0, 0, 1);
				}
				else 
				{
					glColor4f(0, 0, 0, alpha);
				}
				if (j > 1 && spline_type != spline_Linear)
				{
					tracks
						<< qt.x - cross_t0.x << qt.y - cross_t0.y << qt.z - cross_t0.z
						<< connect_inside.x << connect_inside.y << connect_inside.z
						<< qt.x + cross_t1.x << qt.y + cross_t1.y << qt.z + cross_t1.z
						<< connect_outside.x << connect_outside.y << connect_outside.z;					
				}
				else if(j == 1 && spline_type == spline_Linear)
				{					
					tracks
						<< qt.x - cross_t0.x << qt.y - cross_t0.y << qt.z - cross_t0.z
						<< connect_inside.x << connect_inside.y << connect_inside.z
						<< qt.x + cross_t1.x << qt.y + cross_t1.y << qt.z + cross_t1.z
						<< connect_outside.x << connect_outside.y << connect_outside.z;					
				}
				else if ( (j != divide && j > 1) && spline_type == spline_Linear)
				{					
					tracks
						<< qt.x - cross_t0.x << qt.y - cross_t0.y << qt.z - cross_t0.z
						<< connect_inside.x << connect_inside.y << connect_inside.z
						<< qt.x + cross_t1.x << qt.y + cross_t1.y << qt.z + cross_t1.z
						<< connect_outside.x << connect_outside.y << connect_outside.z;					
				}							
				if (j == 1)
				{					
					if (spline_type != spline_Linear)
					{
						connect_head.push_back(Pnt3f(qt.x - cross_t0.x, qt.y - cross_t0.y, qt.z - cross_t0.z));
						connect_head.push_back(Pnt3f(qt.x + cross_t1.x, qt.y + cross_t1.y, qt.z + cross_t1.z));
					}
					else
					{
						connect_head.push_back(connect_inside);
						connect_head.push_back(connect_outside);
					}										
				}
				if (j == divide)
				{
					if (spline_type != spline_Linear)
					{
						connect_tail.push_back(Pnt3f(qt.x - cross_t0.x, qt.y - cross_t0.y, qt.z - cross_t0.z));
						connect_tail.push_back(Pnt3f(qt.x + cross_t1.x, qt.y + cross_t1.y, qt.z + cross_t1.z));
					}
					else
					{
						connect_tail.push_back(connect_inside);
						connect_tail.push_back(connect_outside);
					}					
				}
				
				//Sleepers
				if (j == divide && spline_type == spline_Linear)
				{

				}
				else
				{					
					sleepers
						<< qt.x - cross_t0.x << qt.y - cross_t0.y << qt.z - cross_t0.z					
						<< qt.x + cross_t1.x << qt.y + cross_t1.y << qt.z + cross_t1.z;
				}				
				connect_inside = Pnt3f(qt.x - cross_t0.x, qt.y - cross_t0.y, qt.z - cross_t0.z);
				connect_outside = Pnt3f(qt.x + cross_t1.x, qt.y + cross_t1.y, qt.z + cross_t1.z);
			}
			else if (track == 0)
			{
				glLineWidth(5);
				glBegin(GL_LINES);
				glVertex3f(qt0.x, qt0.y, qt0.z);
				glVertex3f(qt.x, qt.y, qt.z);
				glEnd();
			}
			if (j > 0) 
			{
				track_path.push_back(Pnt3f(qt.x, qt.y, qt.z));
				track_orient_cross.push_back(cross_t0);
				track_orient.push_back(orient_t);
			}			
		}//Divide			
	}//Controlpoints
	
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	
	buffer_size.push_back(tracks.size());
	int color_counts = tracks.size();
	for (size_t i = 0; i < color_counts; i++)
	{
		tracks << 0;
	}
	buffer_size.push_back(color_counts);//Colors
	trackobj->Begin();
	if (!doingShadows)
		trackobj->Render(GL_TRIANGLES,false,0,ProjectionMatrex, ModelViewMatrex, tracks, buffer_size,1.f,effect_clock,(effectNum == 0?3: effectNum));
	else
		trackobj->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, tracks, buffer_size, 0.4f, effect_clock, effectNum);
	trackobj->End();
	buffer_size.clear();
	tracks.clear();
	//Draw connects
	if (track != 0)
	{
		for (size_t i = 0; i < connect_tail.size() - 1; i += 2)
		{
			tracks
				<< connect_tail[i].x << connect_tail[i].y << connect_tail[i].z
				<< connect_head[(i + 2) % connect_head.size()].x << connect_head[(i + 2) % connect_head.size()].y << connect_head[(i + 2) % connect_head.size()].z
				<< connect_tail[i + 1].x << connect_tail[i + 1].y << connect_tail[i + 1].z
				<< connect_head[(i + 3) % connect_head.size()].x << connect_head[(i + 3) % connect_head.size()].y << connect_head[(i + 3) % connect_head.size()].z;
		}
		buffer_size.push_back(tracks.size());
		color_counts = tracks.size();
		for (size_t i = 0; i < color_counts; i++)
		{
			tracks << 0;
		}
		buffer_size.push_back(color_counts);
		trackobj->Begin();
		if (!doingShadows)
			trackobj->Render(GL_LINES,false,0,ProjectionMatrex, ModelViewMatrex, tracks, buffer_size, 1.f, effect_clock,(effectNum == 0?3: effectNum));
		else
			trackobj->Render(GL_LINES, false, 0,ProjectionMatrex, ModelViewMatrex, tracks, buffer_size, 0.4f, effect_clock, effectNum);
		trackobj->End();

		//Draw sleepers
		buffer_size.clear();
		color_counts = sleepers.size();
		buffer_size.push_back(sleepers.size());
		for (size_t i = 0; i < color_counts; i++)
		{
			sleepers << 1.f;
		}
		buffer_size.push_back(color_counts);
		trackobj->Begin();
		if (SLEEPER)
		{
			if (!doingShadows)
				trackobj->Render(GL_LINES, false, 0,ProjectionMatrex, ModelViewMatrex, sleepers, buffer_size, 1.f, effect_clock, effectNum);
			else
				trackobj->Render(GL_LINES, false, 0,ProjectionMatrex, ModelViewMatrex, sleepers, buffer_size, 0.4f, effect_clock, effectNum);
			trackobj->End();
		}
	}
	
	//Generate path	
	if (trackUpdate)
	{
		path.clear();
		size_t divide = 10;
		for (size_t i = 0; i < track_path.size(); i++)
		{
			TrackTrail t1 = TrackTrail(track_path[i], track_orient[i], track_orient_cross[i]);
			TrackTrail t2 = TrackTrail(track_path[(i + 1) % track_path.size()], track_orient[(i + 1) % track_orient.size()], track_orient_cross[(i + 1) % track_orient_cross.size()]);
			Pnt3f pos_divided = t2.points - t1.points;
				pos_divided.x /= (float)divide;
				pos_divided.y /= (float)divide;
				pos_divided.z /= (float)divide;
			Pnt3f orient_divided = t2.orients - t1.orients;
				orient_divided.x /= (float)divide;
				orient_divided.y /= (float)divide;
				orient_divided.z /= (float)divide;
			Pnt3f cross_divided = t2.orients_cross - t1.orients_cross;
				cross_divided.x /= (float)divide;
				cross_divided.y /= (float)divide;
				cross_divided.z /= (float)divide;
			for (size_t j = 1; j < divide; j++)
			{
				TrackTrail t3 = TrackTrail(t1.points+ pos_divided*j, t1.orients+ orient_divided * j,t1.orients_cross+ cross_divided * j);
				path.push_back(t3);
			}
			//path.push_back(t1);
		}
		trackUpdate = false;
	}
}
void TrainView::drawStuff(bool doingShadows)
{
	draw3DObj(doingShadows);
	//glPushMatrix();		
		glTranslatef(0,10,0);
		if (this->camera != 2) 
		{
			for (size_t i = 0; i < this->m_pTrack->points.size(); ++i) 
			{
				if (!doingShadows) 
			{
					if (((int)i) != selectedCube)
						glColor3ub(240, 60, 60);
					else{
						glColor3ub(240, 240, 30);					
					}
				}
				this->m_pTrack->points[i].draw();
			}
			update();
		}
		drawTrack(doingShadows);
#ifdef EXAMPLE_SOLUTION
	//drawTrack(this, doingShadows);
	
#endif		
		if (path.size() > 0) 
		{
			drawTrain(path[path_index].points, path[path_index].orients_cross, path[path_index].orients,doingShadows);
		}
	//glPopMatrix();	
#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
#endif	
}
void TrainView::draw3DObj(bool doingShadows)
{
	//**********************OTHERS***************************//	
	float right_position[15][3]
	{
		{0, 2.3f, 0},
		{0, 2.5f, 0},

		{0, 0, 0},
		{0, -0.1f, 0},

		{0.5f, 1.8f, 0},
		{1.4f, -0.7f, 0},
		{1.4f, -0.7f, 0},

		{-0.5f, 1.8f, 0},
		{-1.4f, -0.7f, 0},
		{-1.4f, -0.7f, 0},

		{0.5f, -0.3f, 0},
		{-0.05f, -2.2f, 0},

		{-0.5f, -0.3f, 0},
		{0.05f, -2.2f, 0},

		{0, 0, 0}
	};
	float sr = 20;	
	for (size_t i = 0; i < models.size() - 1; i++)
	{
		right_position[i][0] *= sr;
		right_position[i][1] *= sr;
		right_position[i][2] *= sr;
	}
	//Draw 3d models reflect environment map
	{
		glPushMatrix();
		QMatrix4x4 m;
		m.setToIdentity();
		glTranslatef(0, 130, 0);
		glRotatef(wholeRotateAngle, 0, 1, 0);
		if(doingShadows)
			glScalef(1, 1, -1);
		else
			glScalef(1, 1, 1);
		
		m.translate(QVector3D(0, 130, 0));
		m.rotate(wholeRotateAngle, QVector3D(0, 1, 0));
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		miku3d->Begin();
		for (size_t i = 0; i < models.size() - 1; i++)
		{
			glPushMatrix();
			switch (i)
			{
			case 0://hair
				glTranslatef(right_position[0][0], right_position[0][1], right_position[0][2]);
				m.translate(QVector3D(right_position[0][0], right_position[0][1], right_position[0][2]));
				break;
			case 1:
				glTranslatef(right_position[1][0], right_position[1][1], right_position[1][2]);
				m.translate(QVector3D(right_position[1][0], right_position[1][1], right_position[1][2]));
			case 3://Skirt
				glTranslatef(right_position[3][0], right_position[3][1], right_position[3][2]);
				m.translate(QVector3D(right_position[3][0], right_position[3][1], right_position[3][2]));
				break;
			case 4:
				glTranslatef(right_position[4][0], right_position[4][1], right_position[4][2]);
				m.translate(QVector3D(right_position[4][0], right_position[4][1], right_position[4][2]));
				break;
			case 5:
				glTranslatef(right_position[4][0], right_position[4][1], right_position[4][2]);
				glTranslatef(right_position[5][0], right_position[5][1], right_position[5][2]);
				m.translate(QVector3D(right_position[4][0], right_position[4][1], right_position[4][2]));
				m.translate(QVector3D(right_position[5][0], right_position[5][1], right_position[5][2]));
				break;
			case 6:
				glTranslatef(right_position[4][0], right_position[4][1], right_position[4][2]);
				glTranslatef(right_position[5][0], right_position[5][1], right_position[5][2]);
				glTranslatef(right_position[6][0], right_position[6][1], right_position[6][2]);
				m.translate(QVector3D(right_position[4][0], right_position[4][1], right_position[4][2]));
				m.translate(QVector3D(right_position[5][0], right_position[5][1], right_position[5][2]));
				m.translate(QVector3D(right_position[6][0], right_position[6][1], right_position[6][2]));
				break;
			case 7:
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				m.translate(QVector3D(right_position[7][0], right_position[7][1], right_position[7][2]));
				break;
			case 8:
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				glTranslatef(right_position[8][0], right_position[8][1], right_position[8][2]);
				m.translate(QVector3D(right_position[7][0], right_position[7][1], right_position[7][2]));
				m.translate(QVector3D(right_position[8][0], right_position[8][1], right_position[8][2]));
				break;
			case 9:
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				glTranslatef(right_position[8][0], right_position[8][1], right_position[8][2]);
				glTranslatef(right_position[9][0], right_position[9][1], right_position[9][2]);
				m.translate(QVector3D(right_position[7][0], right_position[7][1], right_position[7][2]));
				m.translate(QVector3D(right_position[8][0], right_position[8][1], right_position[8][2]));
				m.translate(QVector3D(right_position[9][0], right_position[9][1], right_position[9][2]));
				break;
			case 10://Thigh
				glTranslatef(right_position[10][0], right_position[10][1], right_position[10][2]);
				m.translate(QVector3D(right_position[10][0], right_position[10][1], right_position[10][2]));
				break;
			case 11:
				glTranslatef(right_position[10][0], right_position[10][1], right_position[10][2]);
				glTranslatef(right_position[11][0], right_position[11][1], right_position[11][2]);
				m.translate(QVector3D(right_position[10][0], right_position[10][1], right_position[10][2]));
				m.translate(QVector3D(right_position[11][0], right_position[11][1], right_position[11][2]));
				break;
			case 12:
				glTranslatef(right_position[12][0], right_position[12][1], right_position[12][2]);
				m.translate(QVector3D(right_position[12][0], right_position[12][1], right_position[12][2]));
				break;
			case 13:
				glTranslatef(right_position[12][0], right_position[12][1], right_position[12][2]);
				glTranslatef(right_position[13][0], right_position[13][1], right_position[13][2]);
				m.translate(QVector3D(right_position[12][0], right_position[12][1], right_position[12][2]));
				m.translate(QVector3D(right_position[13][0], right_position[13][1], right_position[13][2]));
				break;
			case 14:
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				glTranslatef(right_position[8][0], right_position[8][1], right_position[8][2]);
				glTranslatef(right_position[9][0], right_position[9][1], right_position[9][2]);
				m.translate(QVector3D(right_position[7][0], right_position[7][1], right_position[7][2]));
				m.translate(QVector3D(right_position[8][0], right_position[8][1], right_position[8][2]));
				m.translate(QVector3D(right_position[9][0], right_position[9][1], right_position[9][2]));
				break;
			}
			glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
			miku3d->shaderProgram->setUniformValue("tex", models[i]->getTextureid());
			miku3d->shaderProgram->setUniformValue("texcube", skybox->textureId);
			if (i != 14)
			{
				if (doingShadows)
					miku3d->Render(GL_TRIANGLES, false, 3, ProjectionMatrex, ModelViewMatrex, models[i]->getValues(), models[i]->getBufferOffset(), 0.6f, effect_clock, effectNum);
				else
					miku3d->Render(GL_TRIANGLES, false, 3, ProjectionMatrex, ModelViewMatrex, models[i]->getValues(), models[i]->getBufferOffset(), 0.3f, effect_clock, effectNum);
			}				
			glPopMatrix();
		}
		miku3d->End();
		glPopMatrix();
	}
	//Draw 3d models
	{
		glPushMatrix();
		glTranslatef(-140, 130, 0);
				
		glRotatef(90.f, 0, 1, 0);
		if(doingShadows)
			glScalef(1, -1, 1);
		else
			glScalef(1, 1, 1);
			
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		miku3d->Begin();
		for (size_t i = 0; i < models.size() - 1; i++)
		{
			glPushMatrix();
			switch (i)
			{
			case 0://hair
				glTranslatef(right_position[0][0], right_position[0][1], right_position[0][2]);
				break;
			case 1:
				glTranslatef(right_position[1][0], right_position[1][1], right_position[1][2]);
			case 3://Skirt
				glTranslatef(right_position[3][0], right_position[3][1], right_position[3][2]);
				break;
			case 4://LFA
				glTranslatef(right_position[4][0], right_position[4][1], right_position[4][2]);
				glRotatef(-30.f, 0, 0, 1);
				break;
			case 5:
				glTranslatef(right_position[4][0], right_position[4][1], right_position[4][2]);
				glRotatef(-30.f, 0, 0, 1);
				glTranslatef(right_position[5][0], right_position[5][1], right_position[5][2]);
				break;
			case 6:
				glTranslatef(right_position[4][0], right_position[4][1], right_position[4][2]);
				glRotatef(-30.f, 0, 0, 1);
				glTranslatef(right_position[5][0], right_position[5][1], right_position[5][2]);
				glTranslatef(right_position[6][0], right_position[6][1], right_position[6][2]);
				break;
			case 7://RFA
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				glRotatef(30.f, 0, 0, 1);
				glRotatef(rightArmAngle, 1, 0, 0);
				break;
			case 8:
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				glRotatef(30.f, 0, 0, 1);
				glRotatef(rightArmAngle, 1, 0, 0);
				glTranslatef(right_position[8][0], right_position[8][1], right_position[8][2]);
				break;
			case 9:
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				glRotatef(30.f, 0, 0, 1);
				glRotatef(rightArmAngle, 1, 0, 0);
				glTranslatef(right_position[8][0], right_position[8][1], right_position[8][2]);
				glTranslatef(right_position[9][0], right_position[9][1], right_position[9][2]);
				break;
			case 10://Thigh
				glTranslatef(right_position[10][0], right_position[10][1], right_position[10][2]);
				break;
			case 11:
				glTranslatef(right_position[10][0], right_position[10][1], right_position[10][2]);
				glTranslatef(right_position[11][0], right_position[11][1], right_position[11][2]);
				break;
			case 12:
				glTranslatef(right_position[12][0], right_position[12][1], right_position[12][2]);
				break;
			case 13:
				glTranslatef(right_position[12][0], right_position[12][1], right_position[12][2]);
				glTranslatef(right_position[13][0], right_position[13][1], right_position[13][2]);
				break;
			case 14:
				glTranslatef(right_position[7][0], right_position[7][1], right_position[7][2]);
				glRotatef(30.f, 0, 0, 1);
				glRotatef(rightArmAngle, 1, 0, 0);
				glTranslatef(right_position[8][0], right_position[8][1], right_position[8][2]);
				glTranslatef(right_position[9][0], right_position[9][1], right_position[9][2]);
				break;
			}
			glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
			miku3d->shaderProgram->setUniformValue("tex", models[i]->getTextureid());
			if (!doingShadows)
				miku3d->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, models[i]->getValues(), models[i]->getBufferOffset(), 1.f, effect_clock, effectNum);
			else
				miku3d->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, models[i]->getValues(), models[i]->getBufferOffset(), 0.3f, effect_clock, effectNum);
			glPopMatrix();
		}
		miku3d->End();
		glPopMatrix();
	}
	//3D tunnel
	{
		glPushMatrix();
		glTranslatef(95, 95, 0);
		if (doingShadows)
			glScalef(0.2f, 0.2f, -0.2f);
		else
			glScalef(0.2f, 0.2f, 0.2f);		
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		miku3d->Begin();
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		miku3d->shaderProgram->setUniformValue("tex", models[models.size() - 1]->getTextureid());
		if (!doingShadows)
			miku3d->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, models[models.size() - 1]->getValues(), models[models.size() - 1]->getBufferOffset(), 1.f, effect_clock, effectNum);
		else
			miku3d->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, models[models.size() - 1]->getValues(), models[models.size() - 1]->getBufferOffset(), 0.3f, effect_clock, effectNum);
		miku3d->End();
		glPopMatrix();
	}	
	//**********************OTHERS***************************//
}
void TrainView::doPick(int mx, int my)
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	makeCurrent();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

	gluPickMatrix((double)mx, (double)(viewport[3]-my), 
		5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	glTranslatef(0, 10, 0);
	// draw the cubes, loading the names as we go
	for(size_t i=0; i<m_pTrack->points.size(); ++i) 
	{
		glLoadName((GLuint) (i+1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;
}
void TrainView::drawTrain(Pnt3f pos, Pnt3f orient_cross,Pnt3f orient,bool doingShadows)
{
	int height = 2;
	int width = 5;
	int length = 3;
	float angle = 0.f;
	std::vector<int> buffersize;
	QVector<float> train_vts;
	train_vts.clear();
		
	pos.y += 6;
	glTranslatef(pos.x, pos.y, pos.z);
	angle = -radiansToDegrees(atan2(orient_cross.z, orient_cross.x));
	glRotatef(angle, 0, 1, 0);
	if(angle > 0)
		angle = -radiansToDegrees(acos(orient.y));
	else
		angle = radiansToDegrees(acos(orient.y));
	glRotatef(-angle, 0, 0, orient.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	float w = 10 / 2, h = 10 / 2;
	if (doingShadows)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);
	}	
	//Train body
	{
		train_vts
			<< -width + 1 << height << -width + 1 - length
			<< width - 1 << height << -width + 1 - length
			<< width - 1 << height + 3 << width - 3
			<< width - 1 << height + 3 << width - 3
			<< -width + 1 << height + 3 << width - 3
			<< -width + 1 << height << -width + 1 - length;
		train_vts
			<< -width + 1 << height + 3 << width - 3
			<< -width + 1 << height << -width + 1 - length
			<< -width + 1 << height << width - 3
			<< -width + 1 << height << width - 3
			<< -width + 1 << height + 3 << width - 3
			<< -width + 1 << height + 3 << width - 3;
		train_vts
			<< width - 1 << height + 3 << width - 3
			<< width - 1 << height << -width + 1 - length
			<< width - 1 << height << width - 3
			<< width - 1 << height << width - 3
			<< width - 1 << height + 3 << width - 3
			<< width - 1 << height + 3 << width - 3;
		//Bottom
		train_vts
			<< -width << -height << -width - length
			<< width << -height << -width - length
			<< width << -height << width + length
			<< width << -height << width + length
			<< -width << -height << width + length
			<< -width << -height << -width - length;
		//Top
		train_vts
			<< -width << height << -width - length
			<< width << height << -width - length
			<< width << height << width + length
			<< width << height << width + length
			<< -width << height << width + length
			<< -width << height << -width - length;
		//Right
		train_vts
			<< -width << height << -width - length
			<< -width << -height << -width - length
			<< -width << -height << width + length
			<< -width << -height << width + length
			<< -width << height << width + length
			<< -width << height << -width - length;
		//Left
		train_vts
			<< width << height << -width - length
			<< width << -height << -width - length
			<< width << -height << width + length
			<< width << -height << width + length
			<< width << height << width + length
			<< width << height << -width - length;
		//Front
		train_vts
			<< width << height << width
			<< -width << height << width
			<< -width << -height << width
			<< -width << -height << width
			<< width << -height << width
			<< width << height << width;
		//Rear
		train_vts
			<< width << height << -width
			<< -width << height << -width
			<< -width << -height << -width
			<< -width << -height << -width
			<< width << -height << -width
			<< width << height << -width;
	}
	int p = train_vts.size();
	buffersize.push_back(p);
	float blackUv[] = {0,1,1,1,1,0.5,1,0.5,0,0.5,0,1};
	float grayUv[] =  {0,0.5,1,0.5,1,0,1,0,0,0,0,0.5};
	float normal[] = {0,1,0};
	for (size_t i = 0; i < 18*2; i++)
	{
		train_vts << blackUv[i%12];
	}
	for (size_t i = 0; i < 36*2; i++)
	{
		train_vts << grayUv[i%12];
	}
	for (size_t i = 0; i < p; i++)
	{
		train_vts << normal[i%3];
	}
	buffersize.push_back(108);
	buffersize.push_back(p);
	trackobj->Begin();
	trackobj->shaderProgram->setUniformValue("tex", trackobj->textureId);
	if (!doingShadows)
		trackobj->Render(GL_TRIANGLES,false,0,ProjectionMatrex, ModelViewMatrex, train_vts, buffersize, 1.f,effect_clock, (effectNum==0)? train_effect: effectNum);
	else
		trackobj->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, train_vts, buffersize, 0.3f, effect_clock, (effectNum == 0) ? train_effect : effectNum);
	trackobj->End();

	if (doingShadows)
	{
		glDisable(GL_CULL_FACE);
	}
	glTranslatef(0,7.f,0);
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	//Draw nenoroid
	h = 10.f;
	w = 9.f;	
	QVector<GLfloat> nendoroid_vts;
	nendoroid_vts
		<< -w << h << 2
		<< w << h << 2
		<< w << -h << 2
		<< w << -h << 2
		<< -w << -h << 2
		<< -w << h << 2;
	nendoroid_vts
		<< 0.f << 0.f
		<< 1.f << 0.f
		<< 1.f << 1.f
		<< 1.f << 1.f
		<< 0.f << 1.f
		<< 0.f << 0.f;	
	for (size_t i = 0; i < 18; i++)
	{
		nendoroid_vts << normal[i % 3];
	}
	buffersize.clear();
	buffersize.push_back(18);
	buffersize.push_back(12);
	buffersize.push_back(18);

	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);		
	nendoroid->Begin();
	nendoroid->shaderProgram->setUniformValue("tex", nendoroid->textureId);
	nendoroid->Render(GL_TRIANGLES, false, 0,ProjectionMatrex, ModelViewMatrex, nendoroid_vts, buffersize,1.f,effect_clock, effectNum);
	nendoroid->End();
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);		
	nendoroid->Begin();
	nendoroid->shaderProgram->setUniformValue("tex", nendoroid->textureId + 1);
	nendoroid->Render(GL_TRIANGLES, false, 0, ProjectionMatrex, ModelViewMatrex, nendoroid_vts, buffersize, 1.f, effect_clock, effectNum);
	nendoroid->End();	
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);	
}

