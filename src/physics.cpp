
#include <glm\gtc\matrix_transform.hpp>
#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <glm\mat4x4.hpp>
#include <glm\gtc\quaternion.hpp>
#include <iostream>
#include "glm/ext.hpp"
using namespace std;

bool show_test_window = false;
bool reset = false;
void GUI() {
	{    //FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//TODO
	}
/*	if (ImGui::Button("RESETEAR WEY")) {
		reset = true;
	}*/

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

float halfW = 0.5;
extern glm::vec3 randPos;
glm::mat4 position;
extern glm::mat4 qMat4; //Quaternion to matrix4                        
glm::mat4 externRV; //posiciones de cada punto a partir del centro de masas.
bool collisioned, collisionDown = false;


bool hasCollision(glm::vec3 preVertexPos, glm::vec3 n, float d, glm::vec3 vertexPos) {

	float getPos;
	getPos = ((glm::dot(n, preVertexPos) + d) * (glm::dot(n, vertexPos) + d));
	if (getPos <= 0) { return true; }
	else { return false; }
}


class Cub {
private:
public:

	glm::vec3 xC; //Posici�n
	glm::vec3 force; //Fuerza
	glm::vec3 pC; //Lineal Momentum
	glm::vec3 lC; //Angular Momentum
	glm::mat3 iC; //Intertia Tensor
	glm::mat3 iBodyC; //Inertia Tensor Body
	glm::vec3 vel; //Velocidad
	glm::mat3 rC; //Quaternion to matrix3 = Orientation
	float torque; //Torque
	glm::vec3 wC; //Velocidad Angular
	glm::quat qC; //Quaternions, orientation. X, Y, Z, W (W ~= Angulo de rotacion)
	float mass; //Masa
	glm::vec3 impulse;
	glm::vec3 torqueImpulse;

				//vertices del cubo.
	glm::vec3 verts[8] = {
		glm::vec3(-halfW, -halfW, -halfW), //1
		glm::vec3(-halfW, -halfW,  halfW), //2
		glm::vec3(halfW, -halfW,  halfW),  //3
		glm::vec3(halfW, -halfW, -halfW),  //4
		glm::vec3(-halfW,  halfW, -halfW), //5
		glm::vec3(-halfW,  halfW,  halfW), //6
		glm::vec3(halfW,  halfW,  halfW),  //7
		glm::vec3(halfW,  halfW, -halfW)   //8
	};
};

Cub *cub;
glm::vec3 vertexPosition[8];
glm::mat4 lastExternRV;
glm::vec3 LastVertex[8];
void PhysicsInit() {
	cub = new Cub;
	randPos = glm::vec3(0.f, 5.f, 0.f);
	cub->xC = randPos;
	cub->force = glm::vec3(0.f, -9.81f, 0.f);
	cub->pC = glm::vec3(0.f, 0.f, 0.f);
	cub->iC = glm::mat3(1.0f);
	cub->iBodyC = glm::mat3(1.0f);
	cub->vel = glm::vec3(0.f, 0.f, 0.f);
	cub->rC = glm::mat3(0.f);
	cub->torque = 1.f;
	cub->lC = cub->lC + cub->torque;
	cub->wC = glm::vec3(0.f);
	cub->qC = glm::quat(0.f, 0.f, 0.f, 0.f);
	cub->mass = 1.0f;
	qMat4 = glm::mat4(1.f);

}

void detectLastPoint(glm::vec3 vertex, int i)
{
	glm::vec4 v(vertex, 1);
	vertexPosition[i] = externRV * v;
	LastVertex[i] = lastExternRV * v;


}
bool collisionUp;
bool collisionLeft;
bool collisionRight;
bool collisionFront;
bool collisionBack;

void PhysicsCleanup() {
	//TODO
	delete cub;

}

void PhysicsUpdate(float dt) {
	
	for (int i = 0; i < 8; i++)
	{
		//glm::vec3 preVertexPosition = externRV*glm::vec4(cub.verts[i], 1);
		//guardar posicion en una array segun el vertice
		detectLastPoint(cub->verts[i], i);
		collisionDown = hasCollision(/*pre*/LastVertex[i], glm::vec3(0, 1, 0), 0, vertexPosition[i]);
		collisionUp = hasCollision(LastVertex[i], glm::vec3(0, -1, 0), 10, vertexPosition[i]);
		collisionLeft = hasCollision(LastVertex[i], glm::vec3(1, 0, 0), 5, vertexPosition[i]);
		collisionRight = hasCollision(LastVertex[i], glm::vec3(-1, 0, 0), 5, vertexPosition[i]);
		collisionFront = hasCollision(LastVertex[i], glm::vec3(0, 0, -1), 5, vertexPosition[i]);
		collisionBack = hasCollision(LastVertex[i], glm::vec3(0, 0, 1), 5, vertexPosition[i]);

		//cout << glm::to_string(vertexPosition);
	}	
	//cout << "POSITION CUBE "<<" x " << cub.xC.x << " y " << cub.xC.y << " z " << cub.xC.z << endl;
	//cout << "LASTVE POSITION " << " x " << LastVertex[1].x << " y " << LastVertex[1].y << " z " << LastVertex[1].z << endl;
	//cout << "VERTEX POSITION " << " x " << vertexPosition[1].x << " y " << vertexPosition[1].y << " z " << vertexPosition[1].z << endl <<endl;
	
	//Euler
	//Translacion
	cub->pC = cub->pC + dt*cub->force;
	cub->vel = cub->pC / cub->mass;
	cub->xC = cub->xC + dt * cub->vel;
	
	//Rotacion
	//cub.lC = cub.lC + cub.torque * dt;
	cub->iBodyC = glm::mat3((1.f / 12.f) * cub->mass*(1 + 1));
	cub->iBodyC = glm::inverse(cub->iBodyC);
	cub->rC = glm::mat3_cast(cub->qC);
	cub->iC = cub->rC * cub->iBodyC * glm::transpose(cub->rC);
	cub->wC = cub->iC * cub->lC;
	cub->qC = cub->qC + dt *1.f / 2.f * glm::quat(0.f, cub->wC)  * cub->qC;
	cub->qC = normalize(cub->qC);
	qMat4 = mat4_cast(cub->qC);
	randPos = glm::vec3(cub->xC.x, cub->xC.y, cub->xC.z);

	//posiciones de los vertices en nuestro mundo.
	for (int i = 0; i < 8; i++)
	{
		if (collisionDown) {
			cub->impulse = cub->force;
			cub->pC = abs(cub->impulse);
			cub->vel = cub->impulse / cub->mass;
			for (int i = 0; i<8; i++)
				cub->torqueImpulse = cross((vertexPosition[i] - cub->xC), cub->impulse);
			cub->lC = cub->torqueImpulse;
			cub->wC = glm::inverse(cub->iC)*cub->torqueImpulse;

			//printf("inCollision");
			//funcion rebote 
		}
		if (collisionUp) {}
		if (collisionLeft){}
		if (collisionRight){}
		if (collisionFront){}
		if (collisionBack){}
		//cout << glm::to_string(vertexPosition);
	}
	
	//down



}

