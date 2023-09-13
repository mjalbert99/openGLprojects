class DOF {
	
private:
	int value;
	float limitMin;
	float limitMax;

public: 

	//Default constructor
	DOF() {
		this->value = 3; //Default to 3
		this->limitMin = 0;
		this->limitMax = 0;
	}

	~DOF() {}

	//Updates DOF value
	void setValue(int value) {
		this->value = value;
	}

	//Returns joint DOF value
	int getValue() {
		return this->value;
	}

	//Updates limit min & limit max
	void setMinMax(float min, float max) {
		this->limitMax = max;
		this->limitMin = min;
	}
};