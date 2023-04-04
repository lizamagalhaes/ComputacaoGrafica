#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "STB_image.h"
#include <iostream>



float rotX, rotYTerra, rotYMercurio, rotYJupiter, rotYSol, rotYSaturno, rotYUrano, rotYNetuno, rotYVenus, rotYMarte, obsZ, year = 0, day = 0;
int posicaoluz = 0;
GLfloat Angulo, Aspecto, Larg_Janela, Alt_Janela;
GLuint TexturaEstrelas, TexturaSol, TexturaTerra, TexturaMarte, TexturaVenus, TexturaJupiter, TexturaNetuno, TexturaUrano, TexturaSaturno, TexturaMercurio;
float VelocidadeSol = 0.5;
float VelocidadeMercurio = 0.45;
float VelocidadeVenus = 0.30;
float VelocidadeTerra = 0.35;
float VelocidadeMarte = 0.3;
float VelocidadeJupiter = 0.25;
float VelocidadeSaturno = 0.2;
float VelocidadeUrano = 0.15;
float VelocidadeNetuno = 0.10;

/* Cria vetores para controle de luzes na cena */
GLfloat ambiente[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat difusa[] = { 0.7, 0.7, 0.7, 1.0 };
GLfloat especular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat posicao[] = { 0.0, 3.0, 2.0, 0.0 };
GLfloat lmodelo_ambiente[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat semespecular[4] = { 0.0,0.0,0.0,1.0 };
bool executarLoop = true;
bool pausado = false;

float cameraZ = 70.0f;


// Fun��o callback chamada quando o tamanho da janela � alterado 
void AjustarJanela(GLsizei w, GLsizei h)
{
	Larg_Janela = w;
	Alt_Janela = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	Aspecto = (GLfloat)w / (GLfloat)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, Aspecto, 0.1, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, cameraZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
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
	carregaTextura(TexturaEstrelas, "textures/estrelas.jpg");
	carregaTextura(TexturaSol, "textures/sol.jpg");
	carregaTextura(TexturaMercurio, "textures/mercurio.jpg");
	carregaTextura(TexturaVenus, "textures/venus.jpg");
	carregaTextura(TexturaTerra, "textures/terra.jpg");
	carregaTextura(TexturaMarte, "textures/marte.jpg");
	carregaTextura(TexturaJupiter, "textures/jupiter.jpg");
	carregaTextura(TexturaSaturno, "textures/saturno.jpg");
	carregaTextura(TexturaUrano, "textures/urano.jpg");
	carregaTextura(TexturaNetuno, "textures/netuno.jpg");
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

void Desenhar()
{
	AjustarJanela(1200, 800);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glEnable(GL_TEXTURE_2D);
	// Desenhar as estrelas
	glBindTexture(GL_TEXTURE_2D, TexturaEstrelas);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-500.0f, -200.0f, -200.0f); // coordenada inferior esquerda
	glTexCoord2f(1.0f, 0.0f); glVertex3f(500.0f, -200.0f, -200.0f); // coordenada inferior direita
	glTexCoord2f(1.0f, 1.0f); glVertex3f(500.0f, 200.0f, -200.0f); // coordenada superior direita
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-500.0f, 200.0f, -200.0f); // coordenada superior esquerda
	glEnd();

	glPushMatrix();
	glRotated((GLdouble)posicaoluz, 1.0, 0.0, 0.0);
	glLightfv(GL_LIGHT0, GL_POSITION, posicao);
	glPopMatrix(); // Posi��o da Luz

	//Define a reflet�ncia do material
	glMaterialfv(GL_FRONT, GL_SPECULAR, semespecular);
	//Define a concentra��o do brilho
	glMateriali(GL_FRONT, GL_SHININESS, 20);

	//Habilita a textura e cria a esfera 1
	glBindTexture(GL_TEXTURE_2D, TexturaSol);
	glPushMatrix();
	glRotatef(0.0, 0.0, 0.0, 0.0); //define a rotacao da esfera
	glTranslatef(0.0, 0.0, -10.0); // posiciona a esfera nos eixos x, y e z
	Esfera(7, 150, 150); // determina o tamanho o raio, a lontitude e a lagitude
	glPopMatrix();

	//Habilita a textura e cria a esfera 2
	glBindTexture(GL_TEXTURE_2D, TexturaMercurio);
	glPushMatrix();
	glRotatef(rotYMercurio, 0.0, 0.1, 0.0);
	glTranslatef(0.0, 0.0, -15.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	//Habilita a textura e cria a esfera 3
	glBindTexture(GL_TEXTURE_2D, TexturaVenus);
	glPushMatrix();
	glRotatef(rotYVenus, 0.0, 0.1, 0.0);
	glTranslatef(0.0, 0.0, -20.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	//Habilita a textura e cria a esfera 4
	glBindTexture(GL_TEXTURE_2D, TexturaTerra);
	glPushMatrix();
	glRotatef(rotYTerra, 0.0, 0.1, 0.0);
	glTranslatef(0.0, 0.0, -25.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	//Habilita a textura e cria a esfera 5
	glBindTexture(GL_TEXTURE_2D, TexturaMarte);
	glPushMatrix();
	glRotatef(rotYMarte, 0.0, 0.1, 0.0);
	glTranslatef(0.0, 0.0, -30.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	//Habilita a textura e cria a esfera 6
	glBindTexture(GL_TEXTURE_2D, TexturaJupiter);
	glPushMatrix();
	glRotatef(rotYJupiter, 0.0, 0.1, 0.0);
	glTranslatef(0.0, 0.0, -35.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	//Habilita a textura e cria a esfera 7
	glBindTexture(GL_TEXTURE_2D, TexturaSaturno);
	glPushMatrix();
	glRotatef(rotYSaturno, 0.0, 0.1, 0.0);
	glTranslatef(0.0, 0.0, -40.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	//Habilita a textura e cria a esfera 8
	glBindTexture(GL_TEXTURE_2D, TexturaUrano);
	glPushMatrix();
	glRotatef(rotYUrano, 0.0, 0.1, 0.0);
	glTranslatef(0.0, 0.0, -45.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	//Habilita a textura e cria a esfera 9
	glBindTexture(GL_TEXTURE_2D, TexturaNetuno);
	glPushMatrix();
	glRotatef(rotYNetuno, 0.0, 0.1, 0.0);
	glTranslatef(0.0, 0.0, -50.0);
	Esfera(1, 50, 50);
	glPopMatrix();

	glutSwapBuffers();

}

void Timer(int value)
{
	if (!pausado) {
		rotYMercurio = (rotYMercurio + 1 * VelocidadeMercurio);
		if (rotYMercurio >= 360) {
			rotYMercurio -= 360;
		}
		rotYSol = (rotYSol + 1 * VelocidadeSol);
		if (rotYSol >= 360) {
			rotYSol -= 360;
		}

		rotYVenus = (rotYVenus + 1 * VelocidadeVenus);
		if (rotYVenus >= 360) {
			rotYVenus -= 360;
		}


		rotYTerra = (rotYTerra + 1 * VelocidadeTerra);
		if (rotYTerra >= 360) {
			rotYTerra -= 360;
		}


		rotYMarte = (rotYMarte + 1 * VelocidadeMarte);
		if (rotYMarte >= 360) {
			rotYMarte -= 360;
		}


		rotYJupiter = (rotYJupiter + 1 * VelocidadeJupiter);
		if (rotYJupiter >= 360) {
			rotYJupiter -= 360;
		}


		rotYSaturno = (rotYSaturno + 1 * VelocidadeSaturno);
		if (rotYSaturno >= 360) {
			rotYSaturno -= 360;
		}


		rotYUrano = (rotYUrano + 1 * VelocidadeUrano);
		if (rotYUrano >= 360) {
			rotYUrano -= 360;
		}

		rotYNetuno = (rotYNetuno + 1 * VelocidadeNetuno);
		if (rotYNetuno >= 360) {
			rotYNetuno -= 360;
		}
	}
	if (!executarLoop) {
		return;
	}
	glutDisplayFunc(Desenhar);
	glutTimerFunc(10, Timer, 1);
	glutPostRedisplay();
}


void imagem(const char* nomeArquivo) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int width, height, channels;
	unsigned char* image = stbi_load(nomeArquivo, &width, &height, &channels, 0);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);


	glutSwapBuffers();
}
void exibirImagem1() {
	//primeiro
	imagem("inicio.jpg"); // ou o n�mero do argumento que voc� deseja passar
}

void exibirImagem2(int value) {
	//segunda
	imagem("ultima.jpg"); // ou o n�mero do argumento que voc� deseja passar
}
void exibirImagemUltima(int value) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//ultima
	imagem("inicio.jpg");
	glutTimerFunc(5000, exit, 0);
}
void EncerrarPrograma(int valor) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// aqui a penultima 
	imagem("ultima.jpg");
	glutTimerFunc(5000, exibirImagemUltima, 1);
}

void Teclado(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC
		executarLoop = false;
		EncerrarPrograma(0);
		break;
	case 'r':
		pausado = false;
		break;
	case 'p':
		pausado = true;
		break;
	case '-':
		cameraZ -= 1.0;
		if (cameraZ < 20.0) {
			cameraZ = 20.0;
		}

		break;
	case '+':
		cameraZ += 1.0;
		if (cameraZ > 200.0) {
			cameraZ = 200.0;
		}
		break;
	}
}
int main()
{

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1200, 800); //determina o tamanho da tela
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Sistema Solar");
	Inicializar();
	glutKeyboardFunc(Teclado);
	glutDisplayFunc(exibirImagem1);
	glutTimerFunc(5000, exibirImagem2, 1);
	glutTimerFunc(10000, Timer, 1);
	glutMainLoop();
	return 0;
}