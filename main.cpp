#include "application.h"
#include <vector>
#include <cmath>

const double EPS = 1e-6;
const double G = 6.67430e-11;

class Object
{
public:
	std::vector<double> position;			// x, y
	std::vector<double> velocity;			// vx, vy
	std::vector<double> acceleration; // ax, ay
	double mass;
	double radius;

	Object(std::vector<double> position, std::vector<double> velocity, double mass, double radius)
			: position(position), velocity(velocity), mass(mass), radius(radius)
	{
		// acceleration = {0.0, -9.81}; // Initial acceleration due to gravity
		acceleration = {0.0, 0.0};
	}

	void accelerate(const std::vector<double> &a)
	{
		acceleration[0] += a[0];
		acceleration[1] += a[1];
	};

	void move(double dt)
	{
		velocity[0] += acceleration[0] * dt;
		velocity[1] += acceleration[1] * dt;
		position[0] += velocity[0] * dt;
		position[1] += velocity[1] * dt;
		// acceleration = {0.0, -9.81}; // Reset acceleration after move
		acceleration = {0.0, 0.0};
		if (position[1] - radius < -1.0)
		{ // Ground collision
			position[1] = -1.0 + radius;
			velocity[1] = -velocity[1] * 0.6; // Simple bounce with damping
		}
		if (position[1] + radius > 1.0)
		{ // Ceiling collision
			position[1] = 1.0 - radius;
			velocity[1] = -velocity[1] * 0.6;
		}
		if (position[0] - radius < -1.0)
		{ // Left wall collision
			position[0] = -1.0 + radius;
			velocity[0] = -velocity[0] * 0.6;
		}
		if (position[0] + radius > 1.0)
		{ // Right wall collision
			position[0] = 1.0 - radius;
			velocity[0] = -velocity[0] * 0.6;
		}
	}

	void applyGravitation(const Object &other)
	{
		double dx = other.position[0] - position[0];
		double dy = other.position[1] - position[1];
		double distSq = dx * dx + dy * dy;
		if (distSq < EPS)
			return; // Avoid singularity
		double dist = sqrt(distSq);
		double force = G * mass * other.mass / distSq;
		double ax = force * dx / (dist * mass);
		double ay = force * dy / (dist * mass);
		accelerate({ax, ay});
	}
};

int handleCollisions(Object &object, Object &object2)
{
	double dx = object2.position[0] - object.position[0];
	double dy = object2.position[1] - object.position[1];
	double dist = sqrt(dx * dx + dy * dy);
	if (dist < EPS)
		return 0;																 // Avoid singularity
	if (dist < object.radius + object2.radius) // Simple collision response
	{
		double rvx = object2.velocity[0] - object.velocity[0];
		double rvy = object2.velocity[1] - object.velocity[1];
		double nx = dx / dist;
		double ny = dy / dist;
		double velAlongNormal = rvx * nx + rvy * ny;
		if (velAlongNormal > 0)
			return 0;			// They are moving apart
		double e = 0.8; // Coefficient of restitution
		double j = -(1 + e) * velAlongNormal / (1 / object.mass + 1 / object2.mass);

		object.velocity[0] -= j * nx / object.mass;
		object.velocity[1] -= j * ny / object.mass;

		object.position[0] -= nx * (object.radius + object2.radius - dist) / 2;
		object.position[1] -= ny * (object.radius + object2.radius - dist) / 2;

		object2.velocity[0] += j * nx / object2.mass;
		object2.velocity[1] += j * ny / object2.mass;

		object2.position[0] += nx * (object.radius + object2.radius - dist) / 2;
		object2.position[1] += ny * (object.radius + object2.radius - dist) / 2;
	}
	return 0;
}

int main()
{
	Application application("Space Simulation", 800, 600, 4, 1, GL_TRUE);

	while (!application.getWindowShouldClose())
	{
		application.update();
		application.render();
	}
	return 0;
}