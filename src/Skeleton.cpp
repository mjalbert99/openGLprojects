#include "Joint.cpp"
#include <map>

class Skeleton {

public:
	Joint* root;
	std::map<std::string, Joint*> nameJoints; //Access joints by name value
	std::vector<Joint*> indexJoints;	// Access joints by order of creation
	int number = 0;
	glm::mat4 world = glm::mat4(1.0);

public:

	Skeleton() {
		this->root = nullptr;
	}

	~Skeleton() {
		delete this->root;
	}

	bool load(const char* file) {
		Tokenizer t;
		t.Open(file);
		t.FindToken("balljoint");

		this->root = new Joint;
		this->root->num = number++;
		this->root->name = "root";
		indexJoints.push_back(this->root);
		nameJoints.emplace("root", this->root);

		loadJoints(t, this->root);
		t.Close();
		return true;
	}

	bool loadJoints(Tokenizer& t, Joint* joint) {
		t.FindToken("{");

		while (1) {
			char temp[256];
			t.GetToken(temp);
			if (strcmp(temp, "offset") == 0) {
				joint->offset.x = t.GetFloat();
				joint->offset.y = t.GetFloat();
				joint->offset.z = t.GetFloat();
			}
			else if (strcmp(temp, "boxmin") == 0) {
				joint->boxMin.x = t.GetFloat();
				joint->boxMin.y = t.GetFloat();
				joint->boxMin.z = t.GetFloat();
			}
			else if (strcmp(temp, "boxmax") == 0) {
				joint->boxMax.x = t.GetFloat();
				joint->boxMax.y = t.GetFloat();
				joint->boxMax.z = t.GetFloat();
			}
			else if (strcmp(temp, "rotxlimit") == 0) {
				joint->rotxlimit.x = t.GetFloat();
				joint->rotxlimit.y = t.GetFloat();
			}
			else if (strcmp(temp, "rotylimit") == 0) {
				joint->rotylimit.x = t.GetFloat();
				joint->rotylimit.y = t.GetFloat();
			}
			else if (strcmp(temp, "rotzlimit") == 0) {
				joint->rotzlimit.x = t.GetFloat();
				joint->rotzlimit.y = t.GetFloat();
			}
			else if (strcmp(temp, "pose") == 0) {
				joint->pose.x = t.GetFloat();
				joint->pose.y = t.GetFloat();
				joint->pose.z = t.GetFloat();
			}
			else if (strcmp(temp, "balljoint") == 0) {
				char temp2[256];
				t.GetToken(temp2);
				Joint* j = new Joint;
				j->name = temp2; //set joint name
				j->num = number++; // increment each joint
				indexJoints.push_back(j);
				nameJoints.emplace(j->name, j); // placing in map
				j->setParent(joint);
				joint->addChild(j);
				loadJoints(t,j);
			}
			else if (strcmp(temp, "}") == 0) {
				joint->box = new Cube(joint->boxMin, joint->boxMax);
				return true;
			}
			else t.SkipLine();
		}
	}

	void update(){
		root->update(world);		//Inital parent matrix is the identity
	}

	void draw(glm::mat4 view, GLuint shader){
		root->draw(view, shader);
	}

	std::map<std::string, Joint*> getNameJoints() {
		return nameJoints;
	}

	std::vector<Joint*> getIndexJoints() {
		return indexJoints;
	}
};