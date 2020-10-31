#pragma once

class IMazeSolver {
public:
	virtual ~IMazeSolver() {};

	virtual void Update(float dt) = 0;
};