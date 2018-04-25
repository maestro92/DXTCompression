#ifndef UTILITY_H_
#define UTILITY_H_

#include <cstdlib>

#include <iostream>

#include <string>
#include <algorithm>

#include <cstdio>
#include <iomanip>
#include <queue>


//#define NO_SDL_GLEXT
//#include <GL/glew.h>




#include <cstdlib>
#include <iostream>
#include <stdio.h>



#include <vector>

//#include "RakNet/StringCompressor.h"



#include <cassert>
#include <fstream>
#include <mutex>




using namespace std;





#define DEBUG_FLAG 1





#define RENDER_TO_SCREEN 0

#define BLACK       glm::vec3(0.0,0.0,0.0)
#define DARK_BLUE   glm::vec3(0.0,0.0,0.75)
#define COLOR_BLUE        glm::vec3(0.0,0.0,0.5)
#define COLOR_LIGHT_BLUE        glm::vec3(240/255.0f, 248/255.0f, 1)
#define GREEN       glm::vec3(0.0,1.0,0.0)
#define COLOR_TEAL        glm::vec3(0.0,1.0,1.0)
#define COLOR_RED         glm::vec3(1.0,0.0,0.0)
#define COLOR_PURPLE      glm::vec3(1.0,0.0,1.0)
#define COLOR_YELLOW      glm::vec3(1.0,1.0,0.0)
#define COLOR_RED      glm::vec3(1.0,0.0,0.0)


#define DARK_GRAY   glm::vec3(0.25,0.25,0.25)
#define COLOR_GRAY        glm::vec3(0.5,0.5,0.5)
#define LIGHT_GRAY  glm::vec3(0.75, 0.75, 0.75)
#define COLOR_WHITE       glm::vec3(1.0,1.0,1.0)

typedef long long UniqueId;




#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <ctime>
#endif

/* Remove if already defined */
typedef long long int64; typedef unsigned long long uint64;










template<class T>
struct LinkedListNode
{
	T data;
	LinkedListNode<T>* next;

	LinkedListNode(const T& d) : data(d), next() 
	{}

	
};


template<class T>
class LinkedList
{
	LinkedListNode* head;


};






namespace utl
{
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
	const int SCREEN_WIDTH_MIDDLE = SCREEN_WIDTH / 2;
	const int SCREEN_HEIGHT_MIDDLE = SCREEN_HEIGHT / 2; 

	const float MATH_EPISON = 0.000001f;

	const float DEGREE_TO_RADIAN = 0.0174;    /// pi/180
	const float RADIAN_TO_DEGREE = 57.32;     /// 180/pi

	template<typename T>
	vector<T> reserveVector(int size);

	long long getCurrentTime_ms();
	
	// http://stackoverflow.com/questions/1861294/how-to-calculate-execution-time-of-a-code-snippet-in-c

	void checkGLError();






	// utl_time.cpp
	long long getCurrentTimeMillis();





};




template<typename T>
vector<T> utl::reserveVector(int size)
{
	vector<T> v;
	v.reserve(size);
	return v;
}





#endif
