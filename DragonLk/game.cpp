

#include "Framework.h"
#include <iostream>
#include <vector>
/* Test Framework realization */

#define PAD_WIDTH 8
#define PAD_HEIGHT 12

#define BALL_WIDTH 40
#define BALL_HEIGHT 32

#define BLOCK_WIDTH 15
#define BLOCK_HEIGHT 15

#define ABILITY_WIDTH 15
#define ABILITY_HEIGHT 15



class MyFramework : public Framework {
	struct Mesh {
		Sprite* mesh{ NULL };
		double mesh_x{ 0 }, mesh_y{ 0 };
		double mesh_width{ 0 }, mesh_height{ 0 };
	}pad, ball;

	struct AbilityMesh {
		Sprite* mesh{ NULL };
		double mesh_x{ 0 }, mesh_y{ 0 };
		double mesh_width{ 0 }, mesh_height{ 0 };
		int code{0};
	};

	struct Point {
		double x{ 0 };
		double y{ 0 };
	}current_mouse_position;

	struct Abilities {
		double miliseconds{ 0 };
		int code{ 0 };
	};

	struct TwoPoints {
		void initialise(double xA, double yA, double xB, double yB) {
			this->xA = xA;
			this->yA = yA;
			this->xB = xB;
			this->yB = yB;
		}
		double xA{ 0 };
		double yA{ 0 };
		double xB{ 0 };
		double yB{ 0 };
	}ball_trajectory;

	double ball_speed{ 0.1 };
	double ball_slope{ 0 };
	int block_number{22 };
	bool game_start{ false };
	bool victory{ false };
	bool loss{ false };
	double flag{ 0};
	double last_cos_value{ 0 };
	double ball_velocity{ 1 };
	double ball_max_velocity{ ball_velocity * 2 };
	double ball_min_velocity{ ball_velocity / 2 };

	std::vector<Mesh> block{ block_number };
	std::vector<AbilityMesh> ability {};
	std::vector<Abilities> abilities{};

	int pad_movement_speed{ 30 };
	int window_width{ 0 }, window_height{ 0 };

	void CalculateTwoPointsBallTrajectory() {
		double diff_x = current_mouse_position.x - ball.mesh_x;
		double diff_y = current_mouse_position.y - ball.mesh_y;
		double theta = atan2(diff_y, diff_x);


		ball_speed = cos(theta);
		ball_slope = sin(theta);	
	}

	void CaculateBallCoordinates() {
		ball.mesh_x += ball_speed*ball_velocity;
		ball.mesh_y += ball_slope * ball_velocity;
		
		drawSprite(ball.mesh, ball.mesh_x, ball.mesh_y);
	}

	bool IsIntersecting(TwoPoints first, TwoPoints second) {
		return !(first.xA >= second.xB || first.xB <= second.xA || first.yA >= second.yB || first.yB <= second.yA);
	}

	void TestCollision() {

		//top-border
		if (ball.mesh_y <= 0) {
			ball_slope *= -1;
		}
		//bottom-border
		if (ball.mesh_y + ball.mesh_height >= window_height) {
			ball_slope *= -1;
			loss = true;
		}
		//left-border
		if (ball.mesh_x <= 0) {
			ball_speed *= -1;
		}
		//right-border
		if (ball.mesh_x + ball.mesh_width >= window_width) {
			ball_speed *= -1;
		}

		int index{ -1 };
		TwoPoints first, second, third;
		first.initialise(ball.mesh_x, ball.mesh_y, ball.mesh_x + ball.mesh_width, ball.mesh_y + ball.mesh_height);
		third.initialise(pad.mesh_x, pad.mesh_y, pad.mesh_x + pad.mesh_width, pad.mesh_y + pad.mesh_height);

		for (auto const& it : block) {
			
			index++;
			second.initialise(it.mesh_x, it.mesh_y, it.mesh_x + it.mesh_width, it.mesh_y + it.mesh_height);
			if (IsIntersecting(second, third) || it.mesh_y + it.mesh_height >= window_height) {
				loss = true;
				return;
			}
			bool is_intersecting{ IsIntersecting(first,second) };

			if (!is_intersecting) {
				continue;
			}
			double overlapLeft{ ball.mesh_x + ball.mesh_width - it.mesh_x };
			double overlapRight{ it.mesh_x + it.mesh_width - ball.mesh_x };
			double overlapTop{ ball.mesh_y + ball.mesh_height - it.mesh_y };
			double overlapBottom{ it.mesh_y + it.mesh_height - ball.mesh_y };

			bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));
			bool ballFromTop(abs(overlapTop) < abs(overlapBottom));

			double minOverlapX{ ballFromLeft ? overlapLeft : overlapRight };
			double minOverlapY{ ballFromTop ? overlapTop : overlapBottom };


			

			if (rand() % 4 == 2) {
				AbilityMesh abil;
				if (rand() % 2 == 1) {
					abil.mesh = createSprite("data/42-Breakout-Tiles.png");
					abil.code = 1;
				}
				else {
					abil.mesh = createSprite("data/41-Breakout-Tiles.png");
					abil.code = 2;
				}
				abil.mesh_width = window_width / ABILITY_WIDTH;
				abil.mesh_height = window_height / ABILITY_HEIGHT;
				abil.mesh_x = it.mesh_x;
				abil.mesh_y = it.mesh_y;
				setSpriteSize(abil.mesh, abil.mesh_width, abil.mesh_height);
				ability.push_back(abil);
			}
			block.erase(block.begin() + index);
			if (abs(minOverlapX) < abs(minOverlapY)) {
				if (ballFromLeft) {
					ball_speed *= -1;
					return;
				}
				else {
					ball_speed *= -1;
					return;
				}
			}
			else {
				if (ballFromTop) {
					ball_slope *= -1;
					return;
				}
				else {
					ball_slope *= -1;
					return;
				}
			}

		}
		index = -1;
		for (auto &it : ability) {
			index++;
			second.initialise(it.mesh_x, it.mesh_y, it.mesh_x + it.mesh_width, it.mesh_y + it.mesh_height);
			if(IsIntersecting(third, second)) {
					Abilities abil;
					abil.code = it.code;
					abil.miliseconds = getTickCount();
					abilities.push_back(abil);
					switch (it.code) {
					case 1: if (ball_velocity + ball_velocity * 40 / 100 < ball_max_velocity)
						ball_velocity += ball_velocity * 40 / 100;
						  else
						ball_velocity = ball_max_velocity;
						break;

					case 2: if (ball_velocity- ball_velocity * 40 / 100 > ball_min_velocity)
						ball_velocity -= ball_velocity * 40 / 100;
						  else
						ball_velocity = ball_min_velocity;
						break;
					}
					ability.erase(ability.begin() + index);
			}	
		}

		if (IsIntersecting(third, first)) {
			if (block.size() == 0) {
				victory = true;
			}
			double overlapLeft{ ball.mesh_x + ball.mesh_width - pad.mesh_x };
			double overlapRight{ pad.mesh_x + pad.mesh_width - ball.mesh_x };
			double overlapTop{ ball.mesh_y + ball.mesh_height - pad.mesh_y };
			double overlapBottom{ pad.mesh_y + pad.mesh_height - ball.mesh_y };

			bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));
			bool ballFromTop(abs(overlapTop) < abs(overlapBottom));

			double minOverlapX{ ballFromLeft ? overlapLeft : overlapRight };
			double minOverlapY{ ballFromTop ? overlapTop : overlapBottom };

			if (abs(minOverlapX) < abs(minOverlapY)) {
				if (ballFromLeft) {
					ball_speed *= -1;
					ball_slope *= -1;
					return;
				}
				else {
					ball_speed *= -1;
					ball_slope *= -1;
					return;
				}
			}
			else {
				if (ballFromTop) {
					ball_slope *= -1;
					return;
				}
			}
		}
	}

	void restart() {
		pad_movement_speed = 30;
		block.clear();
		block.insert(block.begin(), block_number, Mesh());
		ball_speed= 0 ;
		ball_slope= 0 ;
		game_start = false;
		victory = false;
		loss = false;
		ball_velocity = 1;
		abilities.clear();
		ability.clear();
	}


public:

	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		
		
		width = 900;
		height = 600;
		fullscreen = false;
	}

	virtual bool Init() {
		getScreenSize(window_width, window_height);
	    pad.mesh = createSprite("data/50-Breakout-Tiles.png");
		ball.mesh = createSprite("data/62-Breakout-Tiles.png");
		int next_row{ 0 };
		int next_to{ 0 };

		for(auto &it: block){
			it.mesh = createSprite("data/01-Breakout-Tiles.png");
			it.mesh_width = window_width / BLOCK_WIDTH;
			it.mesh_height = window_height / BLOCK_HEIGHT;
			setSpriteSize(it.mesh, it.mesh_width, it.mesh_height);
			if ((next_to+1) * (it.mesh_width+ window_width / (it.mesh_width+1)) >= window_width) {
				next_row++;
				next_to = 0;
			}

			it.mesh_x = window_width / (it.mesh_width+1) + next_to * (it.mesh_width+window_width/(it.mesh_width+1));
			it.mesh_y = next_row * (it.mesh_height+5);
			next_to++;
		}

		pad.mesh_width = window_width / PAD_WIDTH;
		pad.mesh_height = window_height / PAD_HEIGHT;
		pad.mesh_x = 3.5 * window_width / PAD_WIDTH;
		pad.mesh_y = 11 * window_height / PAD_HEIGHT;

		ball.mesh_width = window_width / BALL_WIDTH;
		ball.mesh_height = window_height / BALL_HEIGHT;
		setSpriteSize(ball.mesh, ball.mesh_width, ball.mesh_height);

		setSpriteSize(pad.mesh, pad.mesh_width, pad.mesh_height);
		return true;
	}

	virtual void Close() {

	}

	virtual bool Tick() {

		int index{ -1 };
		for (auto& it : abilities) {
			index++;
			if (getTickCount() - it.miliseconds >= 20000) {
				switch (it.code) {
				case 1: if (ball_velocity - ball_velocity * 40 / 100 > ball_min_velocity)
					ball_velocity -= ball_velocity * 40 / 100;
					  else
					ball_velocity = ball_min_velocity;
					break;

				case 2: if (ball_velocity + ball_velocity * 40 / 100 < ball_max_velocity)
					ball_velocity += ball_velocity * 40 / 100;
					  else
					ball_velocity = ball_max_velocity;
					break;
				}
				abilities.erase(abilities.begin() + index);
			}
		}

		TwoPoints first, second;
		bool isIntersecting{false};
		for (auto& it : ability) {
			isIntersecting = false;
			first.initialise(it.mesh_x, it.mesh_y, it.mesh_x+it.mesh_width, it.mesh_y+it.mesh_height);
			for(const auto& it_block: block){
				second.initialise(it_block.mesh_x, it_block.mesh_y, it_block.mesh_x + it.mesh_width, it_block.mesh_y + it.mesh_height);
				if (IsIntersecting(first, second))
					isIntersecting = true;
			}
			drawSprite(it.mesh, it.mesh_x, it.mesh_y);
			if(!isIntersecting)
				it.mesh_y += 0.1;
		}
		
		
		drawSprite(pad.mesh,pad.mesh_x,pad.mesh_y);

		if (game_start && !victory) {
			
			CaculateBallCoordinates();
			TestCollision();
			
			
		}
		if (victory || loss) {
			restart();
			Init();
		}

		

		flag += 0.01;
		for(auto &it : block){
			if(game_start && !victory){
				it.mesh_y += 0.01;
				it.mesh_x += 12* sin(flag) - last_cos_value;
				
			}
			drawSprite(it.mesh, it.mesh_x, it.mesh_y);
		}
		last_cos_value = 12 * sin(flag);
		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) {
		current_mouse_position.x = x;
		current_mouse_position.y = y;
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) {
		if (isReleased && button == FRMouseButton::LEFT && game_start == false) {
			if(current_mouse_position.y < pad.mesh_y - ball.mesh_height + 1){
				ball.mesh_x = pad.mesh_x + pad.mesh_width / 2 - ball.mesh_width/2;
				ball.mesh_y = pad.mesh_y - ball.mesh_height - 10;

				ball_trajectory.xA = ball.mesh_x;
				ball_trajectory.yA = ball.mesh_y;
				ball_trajectory.xB = current_mouse_position.x-ball.mesh_width/2;
				ball_trajectory.yB = current_mouse_position.y-ball.mesh_height/2;
				
				CalculateTwoPointsBallTrajectory();
				game_start = true;
			}
		}
	}

	virtual void onKeyPressed(FRKey k) {
		if (k == FRKey::LEFT)
			if (pad.mesh_x - pad_movement_speed >= 0)
				pad.mesh_x -= pad_movement_speed;
			else
				pad.mesh_x = 0;

		if (k == FRKey::RIGHT)
			if (pad.mesh_x + pad.mesh_width + pad_movement_speed <= window_width)
				pad.mesh_x += pad_movement_speed;
			else
				pad.mesh_x = window_width - pad.mesh_width;
	}

	virtual void onKeyReleased(FRKey k) {
	}
	
	virtual const char* GetTitle() override
	{
		return "Arcanoid";
	}
};

int main(int argc, char *argv[])
{
	return run(new MyFramework);
}
