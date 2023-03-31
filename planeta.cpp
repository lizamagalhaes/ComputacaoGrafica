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
GLuint TexturaEspaco;
GLuint TexturaTerra;
GLuint TexturaLua;
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
	glGenTextures(1, &TexturaEspaco);
	carregaTextura(TexturaEspaco, "textures/estrelas.jpg");
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	Iluminar();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glGenTextures(1, &TexturaTerra);
	glGenTextures(1, &TexturaLua);
	carregaTextura(TexturaTerra, "textures/terra.jpg");
	carregaTextura(TexturaLua, "textures/lua.jpg");
	// Posicionando o observador virtual
	Angulo = 60;
	rotX = 20;
	rotY = 0;
	obsZ = 1;
}

void Espaco(void)
{
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-Larg_Janela, -Alt_Janela);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(0, Alt_Janela);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(Larg_Janela, Alt_Janela);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(Larg_Janela, 0);
	glEnd();
	glPopMatrix();
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
	glBindTexture(GL_TEXTURE_2D, TexturaEspaco);
	//Armazena o estado anterior para rotação da posição da luz 
	glPushMatrix();
	glRotated((GLdouble)posicaoluz, 1.0, 0.0, 0.0);
	glLightfv(GL_LIGHT0, GL_POSITION, posicao);
	glPopMatrix(); // Posição da Luz 9

	//Armazena a situação atual da pilha de matrizes 
	glPushMatrix();
	glTranslatef(0.0, 0.0, 2.0);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -3.0);
	glPushMatrix();
	glRotatef(9, 0.0, 0.0, 1.0);
	glPushMatrix();
	glRotatef((GLfloat)rotY, 1.0, 0.0, 0.0);
	glRotatef((GLfloat)1.0, 0.0, obsZ, 0.0);
	//Define a refletância do material
	glMaterialfv(GL_FRONT, GL_SPECULAR, semespecular);
	//Define a concentração do brilho
	glMateriali(GL_FRONT, GL_SHININESS, 20);
	//Habilita a textura e cria a esfera 
	glBindTexture(GL_TEXTURE_2D, TexturaTerra);
	Esfera(3.0, 50, 50);
	glBindTexture(GL_TEXTURE_2D, TexturaLua);
	Esfera(0.3, 20, 20);
	glBindTexture(GL_TEXTURE_2D, 0);
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
	glutInitWindowSize(600, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Planeta Terra com lua");
	Inicializar();
	glutDisplayFunc(Desenhar);
	glutReshapeFunc(AjustarJanela);
	glutKeyboardFunc(UsarTeclado);
	glutTimerFunc(10, Timer, 1);
	glutMainLoop();
	glDisable(GL_TEXTURE_2D);
	glDeleteTextures(1, &TexturaTerra);
	glDeleteTextures(1, &TexturaLua);
	return 0;
}