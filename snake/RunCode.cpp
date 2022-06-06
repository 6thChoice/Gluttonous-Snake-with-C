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

//�������¼����Ľڵ�
typedef struct Unit {
	int X_axis;
	int Y_axis;
	int flag;  // flag Ϊ���ֽڵ��ô��ı�־��5 Ϊ���� 10 Ϊʳ� 0 Ϊ�ɷ����ڵ�
	struct Unit* next;
	struct Unit* prior;
}unit; 

unit* head;
unit* current;
unit* prev;
// �ߵ�����ָ��
int distance = right; // ����ָʾ����ָ��״̬���ߵ�ǰ������,1Ϊ��2Ϊ�ң�3Ϊ�ϣ�4Ϊ��

unit* food_head;
unit* food_current;
unit* food_prev;
// ʳ�������ָ��

//���Ʊ߽�
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

//��ʼ����ͼ
void initmap(void)
{
	initgraph(640, 480);
	broad();
}

//��ʼ������������������
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

//��ʼ��ʳ�����FOOD_MAX��ʳ��
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

//�����ߵ����� // �ɷ���
void generate_unit(void)
{
	int tail_dis = 1; // 1 ʱ�������˶��� 2 ʱ�������˶�, 3 ʱ�����˶��� 4 ʱ�����˶�
	if (prev->X_axis == prev->prior->X_axis) // �����ƶ���
	{
		if (prev->Y_axis < prev->prior->Y_axis)
			tail_dis = 1;
		if (prev->Y_axis > prev->prior->Y_axis)
			tail_dis = 2;
	}
	if (prev->Y_axis == prev->prior->Y_axis) // �����ƶ���
	{
		if (prev->X_axis < prev->prior->X_axis)
			tail_dis = 3;
		if (prev->X_axis > prev->prior->X_axis)
			tail_dis = 4;
	}
	unit* body = (unit*)malloc(sizeof(unit));
	switch (tail_dis)
	{
	case 3: //����
		body->prior = prev;
		body->X_axis = body->prior->X_axis - 10;
		body->Y_axis = body->prior->Y_axis;
		body->flag = 5;
		body->next = NULL;
		prev->next = body;
		distance = right;
		break;
	case 4: // ����
		body->next = NULL;
		body->prior = prev;
		body->X_axis = body->prior->X_axis + 10;
		body->Y_axis = body->prior->Y_axis;
		body->flag = 5;
		prev->next = body;
		distance = left;
		break;
	case 1: //����
		body->next = NULL;
		body->prior = prev;
		body->X_axis = body->prior->X_axis;
		body->Y_axis = body->prior->Y_axis - 10;
		body->flag = 5;
		prev->next = body;
		distance = down;
		break;
	case 2: //����
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

//�����޸���˸����Ĵ�ӡ����
void NO_Flash_draw_snake(unit* head, unit* tail)
{
	setfillcolor(WHITE);
	solidrectangle(head->X_axis - 5, head->Y_axis + 5, head->X_axis + 5, head->Y_axis - 5);
	clearrectangle(tail->X_axis - 5, tail->Y_axis + 5, tail->X_axis + 5, tail->Y_axis - 5);
}

//�����޸���˸����Ĵ�ӡʳ�ﺯ��
void NO_Flash_draw_food(unit* old_food, unit* new_food)
{
	clearrectangle(old_food->X_axis - 5, old_food->Y_axis + 5, old_food->X_axis + 5, old_food->Y_axis - 5);
	setfillcolor(WHITE);
	solidrectangle(new_food->X_axis - 5, new_food->Y_axis + 5, new_food->X_axis + 5, new_food->Y_axis - 5);
}

// ����ʳ��ڵ㣬ɾ�����Ե��Ľڵ㣬���һ���µĽڵ�
void renew_food(unit* food) // food Ӧ������Ҫɾ����ʳ��ڵ�
/*
food ���ڵ����� food �ӽڵ�
food �ӽڵ����� food ���ڵ�
free��food��
*/
{
	int tempx;
	int tempy;
	food_prev; //ָ�����һ��ʳ��ڵ�

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

//�ͷ��ߺ�ʳ����ڴ�
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

//������ ʳ�� �߽�
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

//��ͷ���ԣ�����ֵΪ 1 ʱ��Ϊ������ײ������ֵΪ 0 ʱ��Ϊδ������ײ
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

//��ײ���ԣ�����ֵΪ 1 ʱ��Ϊ������ײ������ֵΪ 0 ʱ��Ϊδ������ײ
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

// ɾ�����һ������
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

// ��ʳ�ж�
int whether_eat(unit* node) // node Ӧ����һ��ͷ�ڵ�
{	
	unit* index = food_head->next;
	while (index != NULL)
	{
		if (node->X_axis <= index->X_axis + 13 &&
			node->X_axis >= index->X_axis - 13 &&
			node->Y_axis <= index->Y_axis + 13 &&
			node->Y_axis >= index->Y_axis - 13)
		{
			// ��Ҫһ���������ɾ����ʳ�������µ�ʳ��
			renew_food(index);
			// ��Ҫһ������������������һ���µĽڵ�
			return 1;
		}
		index = index->next;
	}
	return 0;
}

//�ߵ��ƶ�����һ��ʱ���ڣ�����1000ms����ֻ��ȡһ��ָ���������ָ�������¼�����ԭ����ǰ��
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

//�����ж�
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

//��ʼ����
int initword(void)
{
	char str1[] = "Gluttonous Snake";
	char str2[] = "Game Start? Y/N";
	char str3[] = "Control: w upward, s downward, a left, d right";
	int i = 0;
	while (i < 17)
	{
		settextstyle(20, 0, TEXT("����"));
		outtextxy(200 + i*10, 150, str1[i]);
		i++;
	}
	i = 0;
	while (i < 16)
	{
		settextstyle(20, 0, TEXT("����"));
		outtextxy(200+i*10, 200, str2[i]);
		i++;
	}
	i = 0;
	while (i < 47)
	{
		settextstyle(20, 0, TEXT("����"));
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

//��������
int finword(void)
{
	char str[] = "Game Over";
	int i = 0;
	while (i < 10)
	{
		settextstyle(20, 0, TEXT("����"));
		outtextxy(200 + i * 10, 150, str[i]);
		i++;
	}
	char str1[] = "Try again? Y/N";
	i = 0;
	while (i < 15)
	{
		settextstyle(20, 0, TEXT("����"));
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

//�������
void byeword(void)
{
	char str1[] = "Thanks for playing";
	int i = 0;
	while (i < 19)
	{
		settextstyle(20, 0, TEXT("����"));
		outtextxy(200 + i * 10, 150, str1[i]);
		i++;
	}
	char str[] = "Programing by ZXC";
	i = 0;
	while (i < 18)
	{
		settextstyle(20, 0, TEXT("����"));
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