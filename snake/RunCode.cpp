#include <stdio.h>
#include <graphics.h>
#include <easyx.h>
#include <conio.h>
#include <Windows.h>
#include <stdlib.h>
#include <time.h>

#define FOOD_MAX 15
#define left 1
#define right 2
#define up 3
#define down 4

//以链表记录蛇身的节点
typedef struct Unit {
	int X_axis;
	int Y_axis;
	int flag;  // flag 为区分节点用处的标志，5 为蛇身， 10 为食物， 0 为可废弃节点
	struct Unit* next;
	struct Unit* prior;
}unit; 

unit* head;
unit* current;
unit* prev;
// 蛇的三项指针
int distance = right; // 用于指示在无指令状态下蛇的前进方向,1为左，2为右，3为上，4为下

unit* food_head;
unit* food_current;
unit* food_prev;
// 食物的三项指针

//绘制边界
void broad(void)
{
	int row = 0;
	int col = 0;
	for (row = 0, col = 0; row < 640; row += 10)
	{
		fillrectangle(row - 5, col - 5, row + 5, col + 5);
	}
	for (row = 0, col = 478; row < 640; row += 10)
	{
		fillrectangle(row - 5, col - 5, row + 5, col + 5);
	}
	for (row = 0, col = 0; col < 480; col += 10)
	{
		fillrectangle(row - 5, col - 5, row + 5, col + 5);
	}
	for (row = 638, col = 0; col < 480; col += 10)
	{
		fillrectangle(row - 5, col - 5, row + 5, col + 5);
	}
}

//初始化地图
void initmap(void)
{
	initgraph(640, 480);
	broad();
}

//初始化蛇身，生成三节身体
void initunit() 
{
	
	head = (unit*)malloc(sizeof(unit));
	srand((unsigned int)time(NULL));
	head->X_axis = rand()%600;
	head->Y_axis = rand()%400;
	head->flag = 5;
	head->prior = NULL;
	prev = head;
	for (int i = 0; i < 3; i++)
	{
		current = (unit*)malloc(sizeof(unit));
		current->prior = prev;
		prev->next = current;
		current->next = NULL;
		current->X_axis = current->prior->X_axis - 10;
		current->Y_axis = current->prior->Y_axis;
		current->flag = 5;
		prev = current;
	}
} 

//初始化食物，生成FOOD_MAX个食物
void initfood()
{
	int temp;
	food_head = (unit*)malloc(sizeof(unit));
	food_head->X_axis = 19999;
	food_head->Y_axis = 19999;
	food_head->flag = 10;
	food_head->prior = NULL;
	food_head->next = NULL;
	food_prev = food_head;
	srand((unsigned int)time(NULL));
	for (int i = 0; i < FOOD_MAX; i++)
	{
		food_current = (unit*)malloc(sizeof(unit));
		food_current->prior = food_prev;
		food_prev->next = food_current;
		food_current->next = NULL;

		
		food_current->X_axis = rand() % 500 + 20;

		food_current->Y_axis = rand() % 400 + 20;

		food_current->flag = 10;
		food_prev = food_current;
	}
}

//增加蛇的身体 // 可废弃
void generate_unit(void)
{
	int tail_dis = 1; // 1 时蛇往下运动， 2 时蛇往上运动, 3 时往右运动， 4 时往左运动
	if (prev->X_axis == prev->prior->X_axis) // 上下移动中
	{
		if (prev->Y_axis < prev->prior->Y_axis)
			tail_dis = 1;
		if (prev->Y_axis > prev->prior->Y_axis)
			tail_dis = 2;
	}
	if (prev->Y_axis == prev->prior->Y_axis) // 左右移动中
	{
		if (prev->X_axis < prev->prior->X_axis)
			tail_dis = 3;
		if (prev->X_axis > prev->prior->X_axis)
			tail_dis = 4;
	}
	unit* body = (unit*)malloc(sizeof(unit));
	switch (tail_dis)
	{
	case 3: //往右
		body->prior = prev;
		body->X_axis = body->prior->X_axis - 10;
		body->Y_axis = body->prior->Y_axis;
		body->flag = 5;
		body->next = NULL;
		prev->next = body;
		distance = right;
		break;
	case 4: // 往左
		body->next = NULL;
		body->prior = prev;
		body->X_axis = body->prior->X_axis + 10;
		body->Y_axis = body->prior->Y_axis;
		body->flag = 5;
		prev->next = body;
		distance = left;
		break;
	case 1: //往下
		body->next = NULL;
		body->prior = prev;
		body->X_axis = body->prior->X_axis;
		body->Y_axis = body->prior->Y_axis - 10;
		body->flag = 5;
		prev->next = body;
		distance = down;
		break;
	case 2: //往上
		body->next = NULL;
		body->prior = prev;
		body->X_axis = body->prior->X_axis;
		body->Y_axis = body->prior->Y_axis + 10;
		body->flag = 5;
		prev->next = body;
		distance = up;
		break;
	}
	prev = body;
}

//用于修复闪烁问题的打印函数
void NO_Flash_draw_snake(unit* head, unit* tail)
{
	setfillcolor(WHITE);
	solidrectangle(head->X_axis - 5, head->Y_axis + 5, head->X_axis + 5, head->Y_axis - 5);
	clearrectangle(tail->X_axis - 5, tail->Y_axis + 5, tail->X_axis + 5, tail->Y_axis - 5);
}

//用于修复闪烁问题的打印食物函数
void NO_Flash_draw_food(unit* old_food, unit* new_food)
{
	clearrectangle(old_food->X_axis - 5, old_food->Y_axis + 5, old_food->X_axis + 5, old_food->Y_axis - 5);
	setfillcolor(WHITE);
	solidrectangle(new_food->X_axis - 5, new_food->Y_axis + 5, new_food->X_axis + 5, new_food->Y_axis - 5);
}

// 更新食物节点，删除被吃掉的节点，添加一个新的节点
void renew_food(unit* food) // food 应填入需要删除的食物节点
/*
food 父节点连接 food 子节点
food 子节点连接 food 父节点
free（food）
*/
{
	int tempx;
	int tempy;
	food_prev; //指向最后一个食物节点

	unit* new_food = (unit*)malloc(sizeof(unit));
	food_prev->next = new_food;
	new_food->prior = food_prev;
	new_food->next = NULL;
	int count = 0;

	srand((unsigned int)time(NULL));
	tempx = rand() % 480 + rand() % 100 + rand() % 10;
	tempy = rand() % 380 + rand() % 50 + rand() % 10;
	food_current = food_head;
	while (food_current != NULL)
	{
		food_current = food_current->next;
		if (food_current == NULL) break;
		if ((tempx - food_current->X_axis)* (tempx - food_current->X_axis) + (tempy - food_current->Y_axis)* (tempy - food_current->Y_axis) < 64 && count < 10)
		{
			srand((unsigned int)time(NULL));
			tempx = rand() % 340 + rand() % 100 + rand() % 10 + rand() % 20 + rand() % 20 + rand() % 10 + rand() % 20 + rand() % 20 + rand() % 10 + rand() % 20 + rand() % 20;
			srand((unsigned int)time(NULL));
			tempy = rand() % 240 + rand() % 50 + rand() % 10 + rand() % 20 + rand() % 20 + rand() % 10 + rand() % 20 + rand() % 20 + rand() % 10 + rand() % 20 + rand() % 20;
			food_current = food_head->next;
			count++;
		}
	}
	new_food->X_axis = tempx;
	new_food->Y_axis = tempy;

	new_food->flag = 10;
	food_prev = new_food;

	if (food->prior && food->next)
	{
		food->prior->next = food->next;
		food->next->prior = food->prior;
	}
	else if (food->next)
	{
		food->next->prior = NULL;
	}
	else if (food->prior)
	{
		food->prior->next = NULL;
	}
	NO_Flash_draw_food(food, new_food);
	free(food);
} 

//释放蛇和食物的内存
void free_unit()
{
	current = head;
	while (current != NULL)
	{
		head = current->next;
		free(current);
		current = head;
	}
	food_current = food_head;
	while (food_current != NULL)
	{
		food_head = food_current->next;
		free(food_current);
		food_current = food_head;
	}
}

//绘制蛇 食物 边界
void draw_place(void)
{
	unit* pointer_snake;
	unit* pointer_food;
	/*
	pointer_snake = head;
	while (pointer_snake != NULL)
	{
		solidrectangle(pointer_snake->X_axis - 5, pointer_snake->Y_axis + 5, pointer_snake->X_axis + 5, pointer_snake->Y_axis - 5);
		pointer_snake = pointer_snake->next;
	}
	*/
	
	pointer_food = food_head;
	while (pointer_food != NULL)
	{
		solidrectangle(pointer_food->X_axis - 5, pointer_food->Y_axis + 5, pointer_food->X_axis + 5, pointer_food->Y_axis - 5);
		pointer_food = pointer_food->next;
	}
	
	broad();
}

//回头测试，返回值为 1 时认为发生碰撞，返回值为 0 时认为未发生碰撞
int crash(unit* node)
{
	int x = node->X_axis;
	int y = node->Y_axis;
	while (node->next != NULL)
	{
		if (node->next->X_axis == x && node->next->Y_axis == y)
		{
			return 1;
		}
		node = node->next;
	}
	return 0;
}

//碰撞测试，返回值为 1 时认为发生碰撞，返回值为 0 时认为未发生碰撞
int hit()
{
	int x = head->X_axis;
	int y = head->Y_axis;
	current = head->next;
	while (current != NULL)
	{
		if (x < current->X_axis + 5 && 
			x > current->X_axis - 5 &&
			y < current->Y_axis + 5 &&
			y > current->Y_axis - 5)
			return 1;
		current = current->next;
	}
	return 0;
}

// 删除最后一节身体
void delete_unit()
{
	unit* tem_tail = prev; 
	while (tem_tail->next != NULL)
	{
		tem_tail = tem_tail->next;
	}
	prev = tem_tail->prior;
	tem_tail->prior->next = NULL;
	tem_tail->prior = NULL;
	free(tem_tail);
}

// 进食判定
int whether_eat(unit* node) // node 应填下一个头节点
{	
	unit* index = food_head->next;
	while (index != NULL)
	{
		if (node->X_axis <= index->X_axis + 13 &&
			node->X_axis >= index->X_axis - 13 &&
			node->Y_axis <= index->Y_axis + 13 &&
			node->Y_axis >= index->Y_axis - 13)
		{
			// 需要一个函数完成删除该食物项并添加新的食物
			renew_food(index);
			// 需要一个函数在蛇身最后添加一个新的节点
			return 1;
		}
		index = index->next;
	}
	return 0;
}

//蛇的移动，在一段时间内（比如1000ms），只读取一个指令，且在无新指令的情况下继续按原方向前进
int MOVE(void)
{
	int gene = 0;
	if (_kbhit())
	{
		char step = _getch();
		unit* tem_head = (unit*)malloc(sizeof(unit));
		step = tolower(step);
		switch (step)
		{
		case 'w':
			if (distance == down || distance == up) break;
			tem_head->next = head;
			tem_head->prior = NULL;
			tem_head->X_axis = tem_head->next->X_axis;
			tem_head->Y_axis = tem_head->next->Y_axis - 10;
			tem_head->flag = 5;
			if (!crash(tem_head))
			{
				head->prior = tem_head;
				head = tem_head;
				distance = up;
			}
			gene = whether_eat(tem_head);
			if (!gene)
				delete_unit();
			NO_Flash_draw_snake(head, prev);
			break;
		case 's':
			if (distance == up || distance == down) break;
			tem_head->next = head;
			tem_head->prior = NULL;
			tem_head->X_axis = tem_head->next->X_axis;
			tem_head->Y_axis = tem_head->next->Y_axis + 10;
			tem_head->flag = 5;
			if (!crash(tem_head))
			{
				head->prior = tem_head;
				head = tem_head;
				distance = down;
			}
			gene = whether_eat(tem_head);
			if (!gene)
			delete_unit();
			NO_Flash_draw_snake(head, prev);
			break;
		case 'a':
			if (distance == right || distance == left) break;
			tem_head->next = head;
			tem_head->prior = NULL;
			tem_head->X_axis = tem_head->next->X_axis - 10;
			tem_head->Y_axis = tem_head->next->Y_axis;
			tem_head->flag = 5;
			if (!crash(tem_head))
			{
				head->prior = tem_head;
				head = tem_head;
				distance = left;
			}
			gene = whether_eat(tem_head);
			if (!gene)
			delete_unit();
			NO_Flash_draw_snake(head, prev);
			break;
		case 'd':
			if (distance == left || distance == right) break;
			tem_head->next = head;
			tem_head->prior = NULL;
			tem_head->X_axis = tem_head->next->X_axis + 10;
			tem_head->Y_axis = tem_head->next->Y_axis;
			tem_head->flag = 5;
			if (!crash(tem_head))
			{
				head->prior = tem_head;
				head = tem_head;
				distance = right;
			}
			gene = whether_eat(tem_head);
			if (!gene)
			delete_unit();
			NO_Flash_draw_snake(head, prev);
			break;
		case 27:
			return 1;
		default:
			break;
		}
	}
	else
	{
		unit* tem_head = (unit*)malloc(sizeof(unit));
		switch (distance)
		{
		case left:
			tem_head->next = head;
			tem_head->prior = NULL;
			tem_head->X_axis = tem_head->next->X_axis - 10;
			tem_head->Y_axis = tem_head->next->Y_axis;
			tem_head->flag = 5;
			head->prior = tem_head;
			head = tem_head;
			distance = left;
			gene = whether_eat(tem_head);
			if (!gene)
			delete_unit();
			NO_Flash_draw_snake(head, prev);
			break;
		case right:
			tem_head->next = head;
			tem_head->prior = NULL;
			tem_head->X_axis = tem_head->next->X_axis + 10;
			tem_head->Y_axis = tem_head->next->Y_axis;
			tem_head->flag = 5;
			head->prior = tem_head;
			head = tem_head;
			distance = right;
			gene = whether_eat(tem_head);
			if (!gene)
			delete_unit();
			NO_Flash_draw_snake(head, prev);
			break;
		case up:
			tem_head->next = head;
			tem_head->prior = NULL;
			tem_head->X_axis = tem_head->next->X_axis;
			tem_head->Y_axis = tem_head->next->Y_axis - 10;
			tem_head->flag = 5;
			head->prior = tem_head;
			head = tem_head;
			distance = up;
			gene = whether_eat(tem_head);
			if (!gene)
			delete_unit();
			NO_Flash_draw_snake(head, prev);
			break;
		case down:
			tem_head->next = head;
			tem_head->prior = NULL;
			tem_head->X_axis = tem_head->next->X_axis;
			tem_head->Y_axis = tem_head->next->Y_axis + 10;
			tem_head->flag = 5;
			head->prior = tem_head;
			head = tem_head;
			distance = down;
			gene = whether_eat(tem_head);
			if (!gene)
			delete_unit();
			NO_Flash_draw_snake(head, prev);
			break;
		}
	}
}

//死亡判定
int death(void)
{
	if (head->X_axis <= 5 ||
		head->X_axis >= 635 ||
		head->Y_axis <= 5 ||
		head->Y_axis >= 475)
		return 1;
	int d = hit();
	if (d == 1) return 1;
	return 0;
}

//开始文字
int initword(void)
{
	char str1[] = "Gluttonous Snake";
	char str2[] = "Game Start? Y/N";
	char str3[] = "Control: w upward, s downward, a left, d right";
	int i = 0;
	while (i < 17)
	{
		settextstyle(20, 0, TEXT("楷体"));
		outtextxy(200 + i*10, 150, str1[i]);
		i++;
	}
	i = 0;
	while (i < 16)
	{
		settextstyle(20, 0, TEXT("楷体"));
		outtextxy(200+i*10, 200, str2[i]);
		i++;
	}
	i = 0;
	while (i < 47)
	{
		settextstyle(20, 0, TEXT("楷体"));
		outtextxy(100 + i * 10, 300, str3[i]);
		i++;
	}
		
	while (int i = _getch())
	{
		i = tolower(i);
		if (i == 'y')
			return 1;
		if (i == 'n')
			return 0;
		else continue;
	}
}

//结束文字
int finword(void)
{
	char str[] = "Game Over";
	int i = 0;
	while (i < 10)
	{
		settextstyle(20, 0, TEXT("楷体"));
		outtextxy(200 + i * 10, 150, str[i]);
		i++;
	}
	char str1[] = "Try again? Y/N";
	i = 0;
	while (i < 15)
	{
		settextstyle(20, 0, TEXT("楷体"));
		outtextxy(200 + i * 10, 300, str1[i]);
		i++;
	}
	while (i = _getch())
	{
		i = tolower(i);
		if (i == 'y')
			return 1;
		if (i == 'n')
			return 0;
		else continue;
	}
}

//告别文字
void byeword(void)
{
	char str1[] = "Thanks for playing";
	int i = 0;
	while (i < 19)
	{
		settextstyle(20, 0, TEXT("楷体"));
		outtextxy(200 + i * 10, 150, str1[i]);
		i++;
	}
	char str[] = "Programing by ZXC";
	i = 0;
	while (i < 18)
	{
		settextstyle(20, 0, TEXT("楷体"));
		outtextxy(200 + i * 10, 300, str[i]);
		i++;
	}
	i = _getch();
}

int main()
{
	restart:
	initmap();
	int s = 1;
	s = initword();
	initunit();
	initfood();
	int i = 0;
	int c = 0;
	int d = 0;
	int again = 0; 
	cleardevice();
	draw_place();
	while (1)
	{
		if (s == 0) break;
		i = MOVE();
		if (i == 1) break;
		c = death();
		if (c == 1) break;
		Sleep(100);
	}
	free_unit();
	cleardevice();
	again = finword();
	if (again)
		goto restart;
	byeword();
	
	return 0;
}