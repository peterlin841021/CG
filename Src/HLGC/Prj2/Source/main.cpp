#include "..\include\Common.h"
#define MENU_EXIT   4

using namespace glm;
using namespace std;

#define ACTION_STATE_IDLE 0
#define ACTION_STATE_MOVE 1
#define ACTION_STATE_JUMP 2
#define ACTION_STATE_ATTACK 3
#define ACTION_STATE_DIE 4

#define HP 0
#define DAMAGE 1
#define STATE 2
#define ATTACK_COUNTER 3
#define JUMP_COUNTER 4
#define SCALE 5
#define XPOS 6
#define YPOS 7
#define ATTACK_DISTANCE 8
#define JUMP_DISTANCE 9
#define FOOTSTEP 10

GLuint sp;
const unsigned int interval = 100;
GLuint vao, vvbo;
const size_t defalut_w = 800;
const size_t defalut_h = 600;
int scene_counter = 0;
mat4 projection_matrix;
bool boxMoveUp = true;
bool eatAttackUp = false;
void Render(glm::mat4 pm, glm::mat4 mm,vector<vec2> uv,size_t texture, clock_t time, int effect,int type);
static inline float random_float();

GLuint generateTexture(const char *image);
struct Uniform
{
	GLuint mv;
	GLuint pm;
	GLuint time;
	GLuint effect;
	GLuint tex;
	GLuint type;
};
struct Character 
{
	string charactername;
	mat4 modelview;	
	vector<vector<vec2>> action;
	pair<int, int> idle;	
	pair<int, int> move;
	pair<int, int> jump;
	vector<pair<int, int>> attack;
	pair<int, int> injured;
	pair<int, int> die;
	size_t nextframe = 0;

	size_t hp = 0;
	size_t damage = 0;
	size_t state = 0;
	size_t jumpcounter = 0;
	size_t attackcounter = 0;
	size_t footsetps = 0;

	bool isinjured = false;
	bool left = false;
	bool isjump = false;
	bool isdied = false;

	GLuint textureidL;
	GLuint textureidR;
	float xpos = 0.f, ypos= 0.f;	
	float attack_distance= 0.f;
	float jump_distance= 0.f;
	float scale_ratio= 0.f;
	
	
	Character(string name, mat4 mv, float *attributes)
	{
		charactername = name;		
		reset(mv, attributes);
	}	
	void reset(mat4 mv, float *attributes)
	{		
		modelview = mv;
		hp = attributes[HP];
		damage = attributes[DAMAGE];
		state = attributes[STATE];
		attackcounter = attributes[ATTACK_COUNTER];
		jumpcounter = attributes[JUMP_COUNTER];
		scale_ratio = attributes[SCALE];
		xpos = attributes[XPOS];
		ypos = attributes[YPOS];
		attack_distance = attributes[ATTACK_DISTANCE];
		jump_distance = attributes[JUMP_DISTANCE];
		footsetps = attributes[FOOTSTEP];
		left = false;
		isinjured = false;
		isjump = false;
		isdied = false;		
		nextframe = 0;		
	}
};
Uniform *uniform;
vector<Character*> characters;
vector<vec3> particles_pos;
vector<vec3> square_pos;
vector<vec2> square_uv;
vector<vec2> origin_left;
vector<vector<vec2>> generate_ani_uv(float origin_w, float origin_h,size_t wpart, size_t hpart)
{
	vector<vector<vec2>> output;
	float xstart = 0.0,ystart = 1.0;
	float xoffset = (origin_w / wpart) / origin_w;
	float yoffset = (origin_h / hpart) / origin_h;
	for (size_t i = 0; i < hpart; i++)
	{
		float x = 0;		
		for (size_t j = 0; j < wpart; j++)
		{
			vector<vec2> v;
			xstart += xoffset;
			v.push_back(vec2(x, ystart));
			v.push_back(vec2(xstart, ystart));
			v.push_back(vec2(xstart, ystart - yoffset));
			v.push_back(vec2(xstart, ystart - yoffset));
			v.push_back(vec2(x, ystart - yoffset));
			v.push_back(vec2(x, ystart));
			x = xstart;
			output.push_back(v);
		}
		ystart -= yoffset;
		xstart = 0;	
	}
	return output;
}
void init_shader()
{
	sp = glCreateProgram();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	char** vsSource = LoadShaderSource("./vs.glsl");
	char** fsSource = LoadShaderSource("./fs.glsl");
	glShaderSource(vs, 1, vsSource, NULL);
	glShaderSource(fs, 1, fsSource, NULL);

	glCompileShader(vs);
	glCompileShader(fs);
	ShaderLog(vs);
	ShaderLog(fs);
	//Attach Shader to program
	glAttachShader(sp, vs);
	glAttachShader(sp, fs);
	glLinkProgram(sp);

	glGenBuffers(1, &vvbo);
	glGenVertexArrays(1, &vao);	
}
size_t collisiondetect(vec3 v1, vec3 v2, size_t damage,float attack_distance)
{
	float dis = sqrt(pow(v1.x - v2.x,2)+ pow(v1.y - v2.y, 2));	
	if (dis < attack_distance)
		return damage;
	else
		return 0;
}
size_t attackcal(vec3 v1, vec3 v2, size_t damage, float attack_distance)
{
	float dis = sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
	float valid_dis = 0.5f;
	if (dis < attack_distance)
	{
		return damage;
	}
	else
	{
		return 0;
	}
}
void My_Init()
{	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	init_shader();
	//Uniform variables
	uniform = new Uniform();
	uniform->pm = glGetUniformLocation(sp, "pm");
	uniform->mv = glGetUniformLocation(sp, "mm");
	uniform->effect = glGetUniformLocation(sp, "effect");
	uniform->time = glGetUniformLocation(sp, "time");
	uniform->type = glGetUniformLocation(sp, "type");
	uniform->tex = glGetUniformLocation(sp, "tex");
	const size_t count = 12;	

	mat4 pm(1.0);
	mat4 identity(1.0);
	mat4 character_mv(1.0);
	mat4 scene_mv(1.0);
	mat4 monster_mv(1.0);
	mat4 box_mv(1.0);
	mat4 attackup_mv(1.0);
	mat4 hpwater_mv(1.0);

	std::vector<float*> character_attributes;
	character_attributes.reserve(count);
	float ground = -0.5f;
	float leftboundary = -0.8f;
	float *attributes = new float[10];
	float *none = new float[10]{0};
	//*Scene*//
		attributes[HP] = 0;
		attributes[DAMAGE] = 0;
		attributes[STATE] = 0;
		attributes[ATTACK_COUNTER] = 0;
		attributes[JUMP_COUNTER] = 0;
		attributes[SCALE] = 1.f;
		attributes[XPOS] = 2500;
		attributes[YPOS] = 1.6f;//0.9-(-0.7)
		attributes[ATTACK_DISTANCE] = 0;
		attributes[JUMP_DISTANCE] = 0;
		attributes[FOOTSTEP] = 25;
	character_attributes.push_back(attributes);
	attributes = new float[10];//Reset
	//*Miku*//
		attributes[HP] = 1000;
		attributes[DAMAGE] = 0;
		attributes[STATE] = 0;
		attributes[ATTACK_COUNTER] = 0;
		attributes[JUMP_COUNTER] = 0;
		attributes[SCALE] = 0.2f;
		attributes[XPOS] = 0;
		attributes[YPOS] = 0;
		attributes[ATTACK_DISTANCE] = 0.4f;
		attributes[JUMP_DISTANCE] = 0.6f;
		attributes[FOOTSTEP] = 2;
	character_attributes.push_back(attributes);
	attributes = new float[10];//Reset
	//*Particle sys*//
		attributes[HP] = 1;
		attributes[DAMAGE] = 1;
		attributes[STATE] = 1;
		attributes[ATTACK_COUNTER] = 0;
		attributes[JUMP_COUNTER] = 0;
		attributes[SCALE] = 5.f;
		attributes[XPOS] = 0;
		attributes[YPOS] = 0;
		attributes[ATTACK_DISTANCE] = 0;
		attributes[JUMP_DISTANCE] = 0;
		attributes[FOOTSTEP] = 0;
	character_attributes.push_back(attributes);
	Character* cs[count] =
	{
		new Character("Background",identity,
			character_attributes[0]),
		new Character("Miku",identity,
			character_attributes[1]),
		new Character("CrimsonBalrog",identity,
			character_attributes[1]),
		new Character("Origin marhroom",identity,
			none),
		new Character("Pig",identity,
			none),
			new Character("Wolf",identity,
			none),
		new Character("Suu",identity,
			none),		
		new Character("Magnus",identity,
			none),
		new Character("Particle sys",identity,
		character_attributes[2]),
		new Character("box",identity,
		none),
		new Character("attackup",identity,
		none),
		new Character("hpwater",identity,
		none)
	};
	const char* texture_images[count+1] = { 
		"background.png",
		"mikuL.png",
		"mikuR.png",
		"CrimsonBalrogR.png",
		"mashroom.png",
		"pig.png",
		"suu.png",
		"wolf.png",
		"walk.png",
		"star.png",
		"box.png",
		"attackup.png",
		"hpwater.png"
	};
		
	for (size_t i = 0; i < count; i++)
	{		
		if (i == 0)//Scene
		{
			scene_mv *= translate(identity,vec3(0,0,-2));
			scene_mv *= scale(identity, vec3(cs[i]->scale_ratio, cs[i]->scale_ratio, cs[i]->scale_ratio));
			cs[i]->modelview = scene_mv;			
			cs[i]->action = generate_ani_uv(1596, 599, 2, 1);
			//*******//
			origin_left = cs[i]->action[0];
			cs[i]->idle = pair<int, int>(0, 1);
			cs[i]->textureidL = generateTexture(texture_images[i]);			
			cs[i]->left = true;
		}
		else if (i == 1)
		{			
			character_mv *= translate(identity, vec3(0, ground, -2));
			
			character_mv *= scale(identity, vec3(cs[i]->scale_ratio, cs[i]->scale_ratio, cs[i]->scale_ratio));
			cs[i]->modelview = character_mv;			
			cs[i]->action = generate_ani_uv(1100, 100, 11, 1);
			cs[i]->idle = pair<int, int>(0, 1);
			cs[i]->move = pair<int, int>(1, 4);
			cs[i]->jump = pair<int, int>(5, 1);						
			cs[i]->die = pair<int, int>(6, 1);
			cs[i]->injured = pair<int, int>(10, 1);
			vector<pair<int, int>> attacks;
			attacks.push_back(pair<int, int>(7, 3));
			cs[i]->attack = attacks;

			cs[i]->left = false;
			cs[i]->textureidL = generateTexture(texture_images[i]);
			cs[i]->textureidR = generateTexture(texture_images[i+1]);
		}
		else
		{	
			if (i == 2)//CrimsonBalrog
			{
				monster_mv *= translate(identity, vec3(-0.9f, -0.2f, -2));
				monster_mv *= scale(identity, vec3(0.5f, 0.5f, 0.5f));
				cs[i]->modelview = monster_mv;

				cs[i]->action = generate_ani_uv(1280, 231, 4, 1);
				cs[i]->move = pair<int, int>(0, 4);	

				cs[i]->left = false;
				cs[i]->textureidR = generateTexture(texture_images[i+1]);
			}
			else if (i == 3)//Origin mashroom
			{
				monster_mv *= translate(identity, vec3(3.f, -1.f, -2));
				monster_mv *= scale(identity, vec3(0.2f, 0.2f, 0.2f));
				cs[i]->modelview = monster_mv;

				cs[i]->action = generate_ani_uv(600, 100, 6, 1);
				cs[i]->move = pair<int, int>(0, 3);
				cs[i]->die = pair<int, int>(3, 3);

				cs[i]->left = true;
				cs[i]->textureidL = generateTexture(texture_images[i + 1]);
			}
			else if (i == 4)//Pig
			{
				monster_mv *= translate(identity, vec3(3.f, 0.f, -2));
				monster_mv *= scale(identity, vec3(1.f, 1.f, 1.f));
				cs[i]->modelview = monster_mv;

				cs[i]->action = generate_ani_uv(201, 50, 3, 1);
				cs[i]->move = pair<int, int>(0, 3);				

				cs[i]->left = true;
				cs[i]->textureidL = generateTexture(texture_images[i + 1]);
			}
			else if (i == 5)//Suu
			{
				monster_mv *= translate(identity, vec3(0, 0.5f, -2));
				monster_mv *= scale(identity, vec3(0.8f, 0.8f, 0.8f));
				cs[i]->modelview = monster_mv;

				cs[i]->action = generate_ani_uv(2040, 166, 12, 1);
				cs[i]->move = pair<int, int>(0, 4);
				cs[i]->die = pair<int, int>(4, 8);
				cs[i]->left = true;
				cs[i]->textureidL = generateTexture(texture_images[i + 1]);
			}
			else if (i == 6)//Wolf
			{				
				monster_mv *= translate(identity, vec3(0.f, 1.f, -2));
				monster_mv *= scale(identity, vec3(5.f, 5.f, 5.f));
				cs[i]->modelview = monster_mv;

				cs[i]->action = generate_ani_uv(3108, 255, 12, 1);
				cs[i]->move = pair<int, int>(0, 12);

				cs[i]->left = true;
				cs[i]->textureidL = generateTexture(texture_images[i + 1]);
			}
			else if (i == 7)//Magnus
			{
				monster_mv *= translate(identity, vec3(0, 0, -2));
				monster_mv *= scale(identity, vec3(1.f, 1.f, 1.f));
				cs[i]->modelview = monster_mv;

				cs[i]->action = generate_ani_uv(2912, 197, 8, 1);
				cs[i]->move = pair<int, int>(0, 8);				
				cs[i]->left = true;
				cs[i]->textureidL = generateTexture(texture_images[i + 1]);
			}
			else if (i == 8)//Particle sys
			{
				//monster_mv *= translate(identity, vec3(0, 0, 0));
				monster_mv *= scale(identity, vec3(cs[i]->scale_ratio, cs[i]->scale_ratio, cs[i]->scale_ratio));
				cs[i]->modelview = monster_mv;

				cs[i]->action = generate_ani_uv(128, 128, 1, 1);
				cs[i]->move = pair<int, int>(0, 1);
				cs[i]->left = false;
				cs[i]->textureidL = generateTexture(texture_images[i+1]);
				cs[i]->textureidR = generateTexture(texture_images[i + 1]);
			}
			else if (i == 9) 
			{ 
				box_mv *= scale(identity, vec3(0.07f, 0.07f, -2.07f));
				box_mv *= translate(identity, vec3(-0.9f, -8.2f, 1.f));
				cs[i]->modelview = box_mv;
				cs[i]->action = generate_ani_uv(558, 299, 1, 1);
				cs[i]->idle = pair<int, int>(0, 1);
				cs[i]->left = true;
				cs[i]->textureidL = generateTexture(texture_images[i + 1]);
			}
			else if (i == 10) 
			{ 
				attackup_mv *= scale(identity, vec3(0.07f, 0.07f, -2.07f));
				attackup_mv *= translate(identity, vec3(-5.9f, -8.2f, 1.f));
				cs[i]->modelview = attackup_mv;
				cs[i]->action = generate_ani_uv(558, 299, 1, 1);
				cs[i]->idle = pair<int, int>(0, 1);
				cs[i]->left = true;
				cs[i]->textureidL = generateTexture(texture_images[i + 1]);
			}
			else if (i == 11) 
			{ 
				hpwater_mv *= scale(identity, vec3(0.06f, 0.06f, -2.06f));
				hpwater_mv *= translate(identity, vec3(5.0f, -9.7f, 1.f));
				cs[i]->modelview = hpwater_mv;
				cs[i]->action = generate_ani_uv(558, 299, 1, 1);
				cs[i]->idle = pair<int, int>(0, 1);
				cs[i]->left = true;
				cs[i]->textureidL = generateTexture(texture_images[i + 1]);
			}
		}
		characters.push_back(cs[i]);
	}
	//****//
	size_t particle_num = 100;
	for (size_t i = 0; i < particle_num; i++)
	{
		particles_pos.push_back(vec3( (random_float() * 3.0f - 1.0f) * 1, (random_float() * 3.0f - 1.0f) * 1,-2));
		//particles_pos.push_back(vec3(0,0, -2));
	}
	//Position of square
	square_pos.push_back(vec3(-1, 1, 0));
	square_pos.push_back(vec3(1, 1, 0));
	square_pos.push_back(vec3(1, -1, 0));
	square_pos.push_back(vec3(1, -1, 0));
	square_pos.push_back(vec3(-1, -1, 0));
	square_pos.push_back(vec3(-1, 1, 0));
	//UV of square
	square_uv.push_back(vec2(0,0));
	square_uv.push_back(vec2(1, 0));
	square_uv.push_back(vec2(1, 1));
	square_uv.push_back(vec2(1, 1));
	square_uv.push_back(vec2(0, 1));
	square_uv.push_back(vec2(0, 0));
}
static unsigned int seed = 0x13371337;
static inline float random_float()
{
	float res;
	unsigned int tmp;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
}
GLuint generateTexture(const char *image)
{
	GLuint id = characters.size();
	TextureData *image_data = &(Load_png(image));
	if (!image_data->data)
	{
		printf("Read image fail!\n");
		system("pause");
		return 0;
	}
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, image_data->width, image_data->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	return id;
}

void Render(glm::mat4 pm, glm::mat4 mm, vector<vec2> uv, size_t texture, clock_t time,int effect,int type)
{	
	glUseProgram(sp);
	glBindVertexArray(vao);
		
	int pos_size = (type == 0)? 6 : particles_pos.size();
	//Two triangles	
	vector<float> vtx;	
	for (int i = 0; i < pos_size; i++)
	{
		int r = (rand() % 100);
		if (type == 0)
		{
			vtx.push_back(square_pos[i].x);
			vtx.push_back(square_pos[i].y);
			vtx.push_back(square_pos[i].z);
		}
		else if (type == 1)
		{
			vtx.push_back(particles_pos[r].x);
			vtx.push_back(particles_pos[r].y);
			vtx.push_back(particles_pos[r].z);
		}
		
	}
	for (int i = 0; i < uv.size(); i++)
	{
		vtx.push_back(uv[i].x);
		vtx.push_back(uv[i].y);
	}
	
	glUniformMatrix4fv(uniform->mv,1, GL_FALSE, &mm[0][0]);
	glUniformMatrix4fv(uniform->pm, 1, GL_FALSE, &pm[0][0]);
	
	glUniform1i(uniform->effect, effect);
	glUniform1f(uniform->time, time);
	glUniform1f(uniform->type, type);
	glUniform1i(uniform->tex, texture);
	glBindBuffer(GL_ARRAY_BUFFER, vvbo);
	glBufferData(GL_ARRAY_BUFFER, (vtx.size()) * sizeof(float),vtx.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,0,0);	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,0, (void*)(pos_size  * 3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	if (type == 0)
	{
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}		
	else if (type == 1)
	{
		glDrawArrays(GL_POINTS, 0, particles_pos.size());
	}
}

void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1, 1, 1, 1);
	
	glUseProgram(sp);	
	//Time
	clock_t time = clock();	
	float f_timer_cnt = glutGet(GLUT_ELAPSED_TIME);
	float currentTime = f_timer_cnt * 0.001f;

	currentTime *= 0.1f;
	currentTime -= floor(currentTime);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);

	//Injure detect
	if (!characters[1]->isinjured)
	{
		for (size_t i = 2; i < characters.size(); i++)
		{
			size_t amount = collisiondetect(vec3(characters[1]->xpos, characters[1]->ypos, 0), vec3(characters[i]->xpos, characters[i]->ypos, 0), characters[i]->damage, characters[i]->attack_distance);
			if (amount > 0 && characters[i] ->state != ACTION_STATE_DIE)
			{
				characters[1]->isinjured = true;
				if (characters[1]->hp > 0)
					characters[1]->hp -= amount;
				if (characters[1]->hp == 0)
				{
					characters[1]->isdied = true;
					characters[1]->state = ACTION_STATE_DIE;
				}					
				printf("Miku hP:%d\n", characters[1]->hp);
			}
		}
	}	
	//Attack detect
	if (characters[1]->state == ACTION_STATE_ATTACK && characters[1]->attackcounter == 3)
	{
		for (size_t i = 2; i < characters.size(); i++)
		{
			size_t amount = attackcal(vec3(characters[1]->xpos, characters[1]->ypos, 0), vec3(characters[i]->xpos, characters[i]->ypos, 1), characters[1]->damage, characters[1]->attack_distance);
			characters[i]->hp -= amount;
			if (amount > 0 && characters[i]->state != ACTION_STATE_DIE)
			{
				if (characters[i]->hp == 0)
					characters[i]->state = 4;
				printf("Monster hp:%d\n", characters[i]->hp);
			}			
		}
	}	
	for (size_t i = 0; i < characters.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0);		
		if (i == 0)
		{
			//printf("SC:%d\n", scene_counter);					
			glBindTexture(GL_TEXTURE_2D, characters[i]->textureidL);
			Render(projection_matrix, characters[i]->modelview, characters[0]->action[0],0, time, 0,0);
		}	
		else if(i == 1)//Miku
		{
			//Directions			
			if (characters[i]->left)
			{				
				glBindTexture(GL_TEXTURE_2D, characters[i]->textureidL);
			}
			else
			{				
				glBindTexture(GL_TEXTURE_2D, characters[i]->textureidR);
			}
			size_t action_index = 0;
			switch (characters[i]->state)
			{
			case ACTION_STATE_IDLE:
				action_index = characters[i]->idle.first;
				break;
			case ACTION_STATE_MOVE:
				action_index = characters[i]->move.first + characters[i]->nextframe;
				if (characters[i]->nextframe == characters[i]->move.second -1)
				{
					characters[i]->nextframe = 0;
					characters[i]->state = 0;
				}
				else
				{					
					characters[i]->nextframe++;
				}
				break;
			case ACTION_STATE_JUMP:
				action_index = characters[i]->jump.first + characters[i]->nextframe;
				if (characters[i]->nextframe == characters[i]->jump.second -1)
				{
					characters[i]->nextframe = 0;					
				}
				else
				{					
					characters[i]->nextframe++;
				}
					if (characters[i]->jumpcounter < 3)
					{
						characters[i]->ypos += characters[i]->jump_distance;
						characters[1]->modelview *= translate(mat4(1.0), vec3(0, characters[i]->jump_distance, 0));					
						characters[1]->jumpcounter++;
					}				
					else if(characters[1]->jumpcounter < 6)
					{
						characters[i]->ypos -= characters[i]->jump_distance;
						characters[1]->modelview *= translate(mat4(1.0), vec3(0, -characters[i]->jump_distance, 0));				
						characters[i]->jumpcounter ++;
					}
					else if (characters[1]->jumpcounter == 6)
					{
						characters[i]->state = 0;
						characters[1]->isjump = false;
						characters[i]->ypos =-0.7f;				
						characters[1]->jumpcounter = 0;
					}
				break;
			case ACTION_STATE_ATTACK:
				action_index = characters[i]->attack[0].first + characters[i]->nextframe;
				if (characters[i]->nextframe == characters[i]->attack[0].second-1)
				{
					characters[i]->nextframe = 0;
					characters[1]->attackcounter = 0;
					characters[i]->state = 0;					
					if (characters[1]->jumpcounter > 0)
					{
						characters[i]->state = ACTION_STATE_JUMP;
					}						
				}
				else
				{					
					characters[i]->nextframe++;
					characters[1]->attackcounter++;
				}
				break;
			case ACTION_STATE_DIE:
				action_index = characters[i]->die.first;				
				break;
			}			
			Render(projection_matrix, characters[i]->modelview, characters[i]->action[action_index], 0, time, 0, 0);
			
			if (characters[i]->isinjured && !characters[1]->isdied)
			{
				if (characters[i]->left)
					Render(projection_matrix, characters[i]->modelview, characters[i]->action[characters[i]->injured.first], characters[i]->textureidL, time, 0,0);
				else
					Render(projection_matrix, characters[i]->modelview, characters[i]->action[characters[i]->injured.first], characters[i]->textureidR, time, 0, 0);
				characters[i]->isinjured = false;
			}				
		}
		//else if()
		//{		
		//	//Monsters directions
		//	if (characters[i]->left)
		//	{
		//		glActiveTexture(characters[i]->textureidL);
		//		glBindTexture(GL_TEXTURE_2D, characters[i]->textureidL);
		//	}
		//	else
		//	{
		//		glActiveTexture(characters[i]->textureidR);
		//		glBindTexture(GL_TEXTURE_2D, characters[i]->textureidR);
		//	}			
		//	////
		//	size_t action_index = 0;
		//	switch (characters[i]->state)
		//	{
		//	case ACTION_STATE_IDLE:
		//		action_index = characters[i]->idle.first;
		//		break;
		//	case ACTION_STATE_MOVE:
		//		action_index = characters[i]->move.first + characters[i]->nextframe;
		//		if (characters[i]->nextframe == characters[i]->move.second - 1)
		//		{
		//			characters[i]->nextframe = 0;
		//			characters[i]->state = 0;
		//		}
		//		else
		//		{
		//			characters[i]->nextframe++;
		//		}
		//		break;
		//	case ACTION_STATE_ATTACK:
		//		action_index = characters[i]->attack[0].first + characters[i]->nextframe;
		//		if (characters[i]->nextframe == characters[i]->attack[0].second - 1)
		//		{
		//			characters[i]->nextframe = 0;
		//			characters[1]->attackcounter = 0;
		//			characters[i]->state = 0;
		//			if (characters[1]->jumpcounter > 0)
		//			{
		//				characters[i]->state = ACTION_STATE_JUMP;
		//			}
		//		}
		//		else
		//		{
		//			characters[i]->nextframe++;
		//			characters[1]->attackcounter++;
		//		}
		//		break;			
		//	case ACTION_STATE_DIE:
		//		action_index = characters[i]->die.first;
		//		break;
		//	}
		//	Render(projection_matrix, characters[i]->modelview, characters[i]->action[action_index], time, 0, 0);
		//}
		else if( i == 8)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, characters[i]->textureidL);
			glEnable(GL_POINT_SPRITE);
			glEnable(GL_PROGRAM_POINT_SIZE);
				Render(projection_matrix, characters[i]->modelview, characters[i]->action[0], 1, time, 0, 1);
			//glDisable(GL_POINT_SPRITE);			
		}
		else if (i == 9 || i == 10 || i == 11)
		{
			if (characters[i]->ypos == 5.0f) 
			{
				boxMoveUp = false;
			}
			else if (characters[i]->ypos == 0.0f) 
			{
				boxMoveUp = true;
			}
			if (boxMoveUp) 
			{
				characters[i]->ypos += 1.f;
				characters[i]->modelview *= translate(mat4(1.0), vec3(0, 0.08f, 0));
			}
			else 
			{
				characters[i]->ypos -= 1.f;
				characters[i]->modelview *= translate(mat4(1.0), vec3(0, -0.08f, 0));
			}
			//glActiveTexture(characters[i]->textureidL);
			glBindTexture(GL_TEXTURE_2D, characters[i]->textureidL);
			Render(projection_matrix, characters[i]->modelview,characters[i]->action[0], 0, time, 0,0);
		}
	}	
	glDisable(GL_BLEND);
	glutSwapBuffers();
}
//Call to resize the window
void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);	

	float viewportAspect = (float)width / (float)height;

	projection_matrix = perspective(deg2rad(50.0f), viewportAspect, 0.1f, 1000.0f);		
}
//Kayboard input
mat4 camrea_mv(1.0);
void keyboardevent(unsigned char key,int x,int y) 
{
	float sc = 0.2f, ground =-0.7f;
	mat4 identity(1.0), mv(1.0);
	float *attributes = new float[10]{0};
	switch (key)
	{
	case 'w':
	case 'W':
		characters[1]->attackcounter = 0;
		if (!characters[1]->isjump && !characters[1]->isdied)
		{
			characters[1]->isjump = true;
			characters[1]->nextframe = 0;
			characters[1]->state = ACTION_STATE_JUMP;
		}		
		break;
	case 'd':
	case 'D':		
		characters[1]->attackcounter = 0;
		characters[1]->left = false;
		if ((characters[1]->jumpcounter < 3 && characters[1]->isjump || !characters[1]->isjump && characters[1]->jumpcounter == 0) && !characters[1]->isdied)
		{			
			characters[1]->state = ACTION_STATE_MOVE;
			float d = (float)characters[1]->footsetps / characters[0]->footsetps;
			if (characters[0]->xpos - characters[1]->xpos > 0)
			{
				scene_counter++;
				characters[1]->xpos += characters[1]->footsetps;
				if (scene_counter > 2)
				{
					scene_counter = 0;
					for (size_t i = 0; i < 6; i++)
					{
						characters[0]->action[0][i].x += 0.01f;
						if (characters[0]->action[0][1].x > characters[0]->action[1][1].x)
						{
							characters[0]->action[0][0].x = origin_left[0].x;
							characters[0]->action[0][1].x = origin_left[1].x;
							characters[0]->action[0][2].x = origin_left[2].x;
							characters[0]->action[0][3].x = origin_left[3].x;
							characters[0]->action[0][4].x = origin_left[4].x;
							characters[0]->action[0][5].x = origin_left[5].x;
							break;
						}
					}
				}				
			}			
			if (characters[1]->isjump)
				characters[1]->state = ACTION_STATE_JUMP;
		}
		break;
	case 's':
	case 'S':
		projection_matrix *= translate(mat4(1.0), vec3(0, 0, -1));
		break;
	case 'a':
	case 'A':
		characters[1]->attackcounter = 0;
		characters[1]->left = true;
		if ((characters[1]->jumpcounter < 3 && characters[1]->isjump) ||
			(!characters[1]->isjump && characters[1]->jumpcounter == 0))
		{
			if (!characters[1]->isdied)
			{				
				characters[1]->state = ACTION_STATE_MOVE;
				float d = (float)characters[1]->footsetps / characters[0]->footsetps;
				if (characters[0]->xpos - characters[1]->xpos < 2500)
				{
					scene_counter--;
					characters[1]->xpos -= characters[1]->footsetps;					
					if (scene_counter < -2)
					{
						scene_counter = 0;
						for (size_t i = 0; i < 6; i++)
						{
							characters[0]->action[0][i].x -= 0.01f;
							if (characters[0]->action[0][0].x < origin_left[0].x)
							{
								characters[0]->action[0][0].x = characters[0]->action[1][0].x;
								characters[0]->action[0][1].x = characters[0]->action[1][1].x;
								characters[0]->action[0][2].x = characters[0]->action[1][2].x;
								characters[0]->action[0][3].x = characters[0]->action[1][3].x;
								characters[0]->action[0][4].x = characters[0]->action[1][4].x;
								characters[0]->action[0][5].x = characters[0]->action[1][5].x;
								break;
							}
						}
					}					
				}					
				if (characters[1]->isjump)
					characters[1]->state = ACTION_STATE_JUMP;
			}			
		}
		break;
	case 'z':
	case 'Z':
		if (!characters[1]->isdied && characters[1]->attackcounter == 0)
		{
			characters[1]->nextframe = 0;
			characters[1]->state = ACTION_STATE_ATTACK;			
		}	
		break;
	case '+':		
		break;
	case '-':		
		break;
	case 'R':
	case 'r':
		attributes[HP] = 1000;
		attributes[DAMAGE] = 1;
		attributes[STATE] = 0;
		attributes[ATTACK_COUNTER] = 0;
		attributes[JUMP_COUNTER] = 0;
		attributes[SCALE] = 0.2f;
		attributes[XPOS] = 0;
		attributes[YPOS] = 0;
		attributes[ATTACK_DISTANCE] = 0.4f;
		attributes[JUMP_DISTANCE] = 0.6f;
		attributes[FOOTSTEP] = 2;
		mv *= translate(identity,vec3(-0.8f, -0.5f,-2));
		mv *= scale(identity, vec3(characters[1]->scale_ratio, characters[1]->scale_ratio, characters[1]->scale_ratio));
		characters[1]->reset(mv, attributes);
		break;
	default:		
		break;
	}	
}
//Timer event
void My_Timer(int val)
{	
	glutPostRedisplay();
	glutTimerFunc(interval, My_Timer, val);
}
//Menu event
void My_Menu(int id)
{
	switch (id)
	{
	case MENU_EXIT:
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{	
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(defalut_w, defalut_h);
	glutCreateWindow("Game"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif	
	DumpInfo();
	//Call custom initialize function
	My_Init();

	//Define Menu
	int menu_main = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	//Register GLUT callback functions
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutKeyboardFunc(keyboardevent);	
	
	glutTimerFunc(interval, My_Timer, 0);
	// Enter main event loop.
	glutMainLoop();
	return 0;
}


