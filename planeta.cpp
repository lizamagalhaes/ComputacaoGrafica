#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "STB_image.h"
#include <iostream>


static int rotX, rotY, obsZ, year = 0, day = 0;
int posicaoluz = 0;
GLfloat Angulo, Aspecto, Larg_Janela, Alt_Janela;
GLuint TexturaEstrelas, TexturaSol, TexturaTerra, TexturaMarte, TexturaVenus, TexturaJupiter, TexturaNetuno, TexturaUrano, TexturaSaturno, TexturaMercurio;
GLuint Velocidade = 1;

/* Cria vetores para controle de luzes na cena */
GLfloat ambiente[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat difusa[] = { 0.7, 0.7, 0.7, 1.0 };
GLfloat especular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat posicao[] = { 0.0, 3.0, 2.0, 0.0 };
GLfloat lmodelo_ambiente[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat semespecular[4] = { 0.0,0.0,0.0,1.0 };

//--------------------------------------------------------------
void Timer(int value);
//void Anima_Idle(void);
void Iluminar();
//void PosicionaObservador(void);
void Inicializar(void);
void Esfera(int raio, int longitude, int latitude);
void Desenhar(void);
void UsarTeclado(unsigned char key, int x, int y);
void AjustarJanela(GLsizei w, GLsizei h);
//--------------------------------------------------------------


// Função callback chamada quando o tamanho da janela é alterado 
void AjustarJanela(GLsizei w, GLsizei h)
{
	Larg_Janela = w;
	Alt_Janela = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	Aspecto = (GLfloat)w / (GLfloat)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, Aspecto, 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1, 0.0);
}

void Timer(int value)
{
	rotY = (rotY + 1 * Velocidade) % 360;
	glutPostRedisplay();
	glutTimerFunc(10, Timer, 1);
}

/* Cria e configura a Luz para a cena */
void Iluminar()
{
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa);
	glLightfv(GL_LIGHT0, GL_POSITION, posicao);
	glLightfv(GL_LIGHT0, GL_SPECULAR, especular);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodelo_ambiente);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
}

//Lendo arquivo para aplicar texturas.
void carregaTextura(GLuint tex_id, std::string filePath)
{
	unsigned char* imagem;
	int largura, altura, canais;

	stbi_set_flip_vertically_on_load(true);
	imagem = stbi_load(filePath.c_str(), &largura, &altura, &canais, 4);
	if (imagem)
	{
		glBindTexture(GL_TEXTURE_2D, tex_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, largura, altura, 0, GL_RGBA, GL_UNSIGNED_BYTE, imagem);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		stbi_image_free(imagem);
	}
	else {
		std::cout << "ERRO! Nao foi possivel carregar a imagem " << filePath.c_str() << std::endl;
	}
}


void Inicializar(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	Iluminar();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glGenTextures(1, &TexturaEstrelas);
	glGenTextures(1, &TexturaSol);
	glGenTextures(1, &TexturaMercurio);
	glGenTextures(1, &TexturaVenus);
	glGenTextures(1, &TexturaTerra);
	glGenTextures(1, &TexturaMarte);
	glGenTextures(1, &TexturaJupiter);
	glGenTextures(1, &TexturaSaturno);
	glGenTextures(1, &TexturaUrano);
	glGenTextures(1, &TexturaNetuno);
	carregaTextura(TexturaSol, "textures/estrelas.jpg");
	carregaTextura(TexturaSol, "textures/sol.jpg");
	carregaTextura(TexturaMercurio, "textures/mercurio.jpg");
	carregaTextura(TexturaVenus, "textures/venus.jpg");
	carregaTextura(TexturaTerra, "textures/terra.jpg");
	carregaTextura(TexturaMarte, "textures/marte.jpg");
	carregaTextura(TexturaJupiter, "textures/jupiter.jpg");
	carregaTextura(TexturaSaturno, "textures/saturno.jpg");
	carregaTextura(TexturaUrano, "textures/urano.jpg");
	carregaTextura(TexturaNetuno, "textures/netuno.jpg");
	// Posicionando o observador virtual
	Angulo = 60;
	rotX = 20;
	rotY = 0;
	obsZ = 1;
}

void Espaco(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, TexturaEstrelas);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
	glEnd();

	glutSwapBuffers();
}

//Criando esfera personalizada (gluSphere)
void Esfera(int raio, int longitude, int latitude)
{
	GLUquadricObj* q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);
	gluQuadricTexture(q, GL_TRUE);
	gluSphere(q, raio, longitude, latitude);
	gluDeleteQuadric(q);
}

void Desenhar(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TexturaEstrelas);
	//Armazena o estado anterior para rotação da posição da luz 
	glPushMatrix();
	glRotated((GLdouble)posicaoluz, 1.0, 0.0, 0.0);
	glLightfv(GL_LIGHT0, GL_POSITION, posicao);
	glPopMatrix(); // Posição da Luz

	//Armazena a situação atual da pilha de matrizes 
	glRotatef(0.1, 0.0, 0.0, 1.0);
	glPushMatrix();
	glRotatef((GLfloat)rotY, 1.0, 0.0, 0.0);
	glRotatef((GLfloat)1.0, 0.0, obsZ, 0.0);
	glPopMatrix(); // Posição da Luz 
	//Define a refletância do material
	glMaterialfv(GL_FRONT, GL_SPECULAR, semespecular);
	//Define a concentração do brilho
	glMateriali(GL_FRONT, GL_SHININESS, 20);

	//Habilita a textura e cria a esfera 1
	glBindTexture(GL_TEXTURE_2D, TexturaSol);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -25.0);
	Esfera(5, 100, 100);
	glPopMatrix();

	//Habilita a textura e cria a esfera 2
	glBindTexture(GL_TEXTURE_2D, TexturaMercurio);
	glPushMatrix();
	glTranslatef(0.0, 7.0, -25.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	//Habilita a textura e cria a esfera 3
	glBindTexture(GL_TEXTURE_2D, TexturaVenus);
	glPushMatrix();
	glTranslatef(0.0, 10.0, -25.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	//Habilita a textura e cria a esfera 4
	glBindTexture(GL_TEXTURE_2D, TexturaTerra);
	glPushMatrix();
	glTranslatef(0.0, 13.0, -25.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	//Habilita a textura e cria a esfera 5
	glBindTexture(GL_TEXTURE_2D, TexturaMarte);
	glPushMatrix();
	glTranslatef(0.0, 16.0, -25.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	//Habilita a textura e cria a esfera 6
	glBindTexture(GL_TEXTURE_2D, TexturaJupiter);
	glPushMatrix();
	glTranslatef(0.0, 19.0, -25.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	//Habilita a textura e cria a esfera 7
	glBindTexture(GL_TEXTURE_2D, TexturaSaturno);
	glPushMatrix();
	glTranslatef(0.0, 22.0, -25.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	//Habilita a textura e cria a esfera 8
	glBindTexture(GL_TEXTURE_2D, TexturaUrano);
	glPushMatrix();
	glTranslatef(0.0, 25.0, -25.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	//Habilita a textura e cria a esfera 9
	glBindTexture(GL_TEXTURE_2D, TexturaNetuno);
	glPushMatrix();
	glTranslatef(0.0, 28.0, -25.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	glutSwapBuffers();
}

void UsarTeclado(unsigned char key, int x, int y)
{
	switch (key) {
	case 'P':
	case 'p':
		Velocidade = 0;
		glutPostRedisplay();
		break;
	case 'G':
	case 'g':
		Velocidade = 1;
		glutPostRedisplay();
		break;
	case VK_ESCAPE:
		exit(0);
		break;
	}
}

int main()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1200, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Sistema Solar");
	Inicializar();
	glutDisplayFunc(Espaco);
	glutDisplayFunc(Desenhar);
	glutReshapeFunc(AjustarJanela);
	glutKeyboardFunc(UsarTeclado);
	glutTimerFunc(10, Timer, 1);
	glutMainLoop();
	glDisable(GL_TEXTURE_2D);
	return 0;
}