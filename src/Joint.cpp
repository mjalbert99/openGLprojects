#include "core.h"
#include "DOF.cpp"
#include "Tokenizer.h"
#include "Cube.h"
#include "Shader.h"
#include <vector>
#include <string>

class Joint {
public:
	std::string name;
	glm::mat4 localMat;  // local Matrix
	glm::mat4 worldMat;  // World Matrix
	std::vector<DOF> dofVec; // Array of DOFs
	Joint* parent;		// Parent Joint
	std::vector<Joint*> children; // Array of child Joints

	//Joint specific data
	glm::vec3 offset;
	glm::vec3 boxMin;
	glm::vec3 boxMax;

	glm::vec2 rotxlimit;
	glm::vec2 rotylimit;
	glm::vec2 rotzlimit;

	glm::vec3 pose;
	Cube* box;

	int num;

public:
	//Default Constructor
	Joint() {
		this->dofVec = std::vector<DOF>();
		this->parent = nullptr;
		this->children = std::vector<Joint*>();
		this->offset = glm::vec3(0.0);
		this->boxMin = glm::vec3(-0.1f);
		this->boxMax = glm::vec3(0.1f);
		this->rotxlimit = glm::vec2(-10000,10000);
		this->rotylimit = glm::vec2(-10000,10000);
		this->rotzlimit = glm::vec2(-10000,10000);
		this->pose = glm::vec3(0.0);
		this->box = nullptr;
		this->num = 0;
	}

	//Defualt Destructor
	~Joint() {
		delete this->parent;
		this->children.clear();
	}

	//Set Parent Joint
	void setParent(Joint* parent) {
		this->parent = parent;
	}

	//Add new child to current joint
	void addChild(Joint* newChild) {
		this->children.push_back(newChild);
	}

	//Calculate Local Matrix
	void calculateLocal() {
		float x = (rotxlimit.x == -10000) ? pose.x :
			(rotxlimit.x > pose.x) ? rotxlimit.x :
			(rotxlimit.y < pose.x) ? rotxlimit.y :
			pose.x;

		float y = (rotylimit.x == -10000) ? pose.y :
			(rotylimit.x > pose.y) ? rotylimit.x :
			(rotylimit.y < pose.y) ? rotylimit.y :
			pose.y;

		float z = (rotzlimit.x == -10000) ? pose.z :
			(rotzlimit.x > pose.z) ? rotzlimit.x :
			(rotzlimit.y < pose.z) ? rotzlimit.y :
			pose.z;

		glm::mat4 rotX = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,						//x-axis rotation
									0.0f, glm::cos(x), glm::sin(x), 0.0f,
									0, -glm::sin(x), glm::cos(x), 0.0f,
									0.0f, 0.0f, 0.0f, 1.0f);

		glm::mat4 rotY = glm::mat4(glm::cos(y), 0.0f, -glm::sin(y), 0.0f,		//y-axis rotation
									0.0f, 1.0f, 0.0f, 0.0f, 
									glm::sin(y), 0.0f, glm::cos(y), 0.0f,
									0.0f, 0.0f, 0.0f, 1.0f);

		glm::mat4 rotZ = glm::mat4(glm::cos(z), glm::sin(z), 0.0f, 0.0f,		//z-axis rotation
									-glm::sin(z), glm::cos(z), 0.0f, 0.0f, 
									0.0f, 0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 0.0f, 1.0f);

		glm::mat4 temp = rotZ * rotY * rotX;			//rotation matrix
		temp[3] = glm::vec4(this->offset, 1.0f);		//roataion + transform

		this->localMat = temp;
	}

	//Updates Matrices and children
	void update(glm::mat4 &matrix) {
		calculateLocal();								// computes local matrix
	
		this->worldMat = matrix * this->localMat;		// computes world matrix

		//Updates children
		for (int i = 0; i < children.size(); i++) {		//updates all children matrices
			children.at(i)->update(this->worldMat);
		}
	}

	//Draws the joint and children
	void draw(glm::mat4 view, GLuint shader) {

		box->draw(view*this->worldMat, shader);					//draws box
		
		for (int i = 0; i < children.size(); i++) {		//draws for all children of joint
			children.at(i)->draw(view,shader);
		}
	}

};