#include "mlx.h"
#include "utils/get_next_line.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#define Pi 3.14
#define WIDTH 640
#define HEIGHT 640
#define MAXINT 2147483647
void    cast(float ray);
// char		**ft_split(char const *s, char c);
// int         ft_atoi(char *s);
void *img;
int a,b,c;
int *data;

//Field of Vu
float fov;

//
//Mini Map
float minimap;
//columnId
int columnId;

//Number of rays and Rays_width
float   Rays_width;
unsigned int   Num_rays;
// height and Weight of the map
int height;
int width;

// mlx variables
void *mlx_ptr;
void *win_ptr;

// rays

typedef struct s_castRay{
	float horizontalx;
	float horizontaly;
	float verticalx;
	float verticaly;
	float rayAngle;
	float wallHitx;
	float wallHity;
	float distance;
	float isRayFacingDown;
}castRay;

//castRay cast;
typedef struct s_Rays{
	float *rays;
	float rayHight;
	float maxDistance;
	castRay cast;
}Rays;

Rays r;

// stock map into lines
char **lines;

float rayAngle;

typedef struct s_player{
		float x;
		float y;
		float radius;
		float turnDirection; // -1 if left, +1 if right
		float walkDirection; // -1 if back, +1 if front
		float rotationAngle; // = Math.PI / 2;
		float moveSpeed; // = 3.0;
		float rotationSpeed; // = 3 * (Math.PI / 180);
		short walk_for;
		short walk_back;
		short cam_left;
		short cam_right;
}player;

player p;

//Normalize Angle;
float	normalize(float rayAngle)
{
	rayAngle = fmod(rayAngle, 2 * Pi);
	while(rayAngle < 0)
		rayAngle += 2 * Pi;
	// else
	// 	rayAngle = rayAngle * (-1);
	return(rayAngle);
}

int keypress(int key)
{
	if (key == 13)
		p.walk_for = 1;
	if (key == 1)
		p.walk_back = 1;
	if (key == 123)
		p.cam_left = 1;
	if (key == 124)
		p.cam_right = 1;
	return (0);
}

int keyreleased(int key)
{
	if (key == 13)
		p.walk_for = -1;
	if (key == 1)
		p.walk_back = -1;
	if (key == 123)
		p.cam_left = -1;
	if (key == 124)
		p.cam_right = -1;
	return(0);
}

void    put_square(int x, int y, unsigned int color)
{
	int y_last;
	int x_last;
	int y_init;
	int savey;

	x_last = x + 32;
	y_last = y + 32;
	y_init = y;
	while(x < x_last)
	{
		y = y_init;
		while(y < y_last)
		{
				if  (color == 1)
				{
					//mlx_pixel_put(mlx_ptr, win_ptr, x, y, 0x5a5a97);
					data[x + y * WIDTH] = 0xFFFFFF;
				}
				else if (color == 0)
				{
					//mlx_pixel_put(mlx_ptr, win_ptr, x, y, 0xbbcedd);
					data[x + y * WIDTH] = 0xbbcedd;
				}
			y++;
		}
		x++;
	}
}
char    **ft_cube3d(int fd)
{
	int i;
	int j;
	int c;

	i = 0;
	j = 0;
	lines = malloc(sizeof(char ** ) * 100);
	while(get_next_line(fd, &lines[j]))
		j++;
	lines[++j] = NULL;
	j = 0;
	return(lines);
}

void    drawLine(float angle, unsigned int color, int indice)
{
	float	x;
	float	y;
	float distancex;
	float distancey;
	int i;
	float deltax;
	float deltay;
	float m;
	float m2;
	float xincrement;
	float yincrement;
	int		steps;


	// clock_t start = clock();
	x = p.x;
	y = p.y;
	distancex = x + 100 * cos(angle);
	distancey = y + 100 * sin(angle);
	deltax = distancex - x;
	deltay = distancey - y;
	if (indice == 1)
	{
		deltax = r.cast.wallHitx - x;
		deltay = r.cast.wallHity - y;
	}
	m = deltay / deltax;
	if(fabs(deltax) > fabs(deltay))
		steps = fabs(deltax);
	else
		steps = fabs(deltay);
	xincrement = deltax / steps;
	yincrement = deltay / steps;
	i = 0;
	while(i < steps)
	{
		data[(int )x + (int )y * WIDTH] = color;
		// if(floor(x) == floor(r.cast.wallHitx) && floor(y) == floor(r.cast.wallHity))
		// 	break;
		x += xincrement;
		y += yincrement;
		i++;
	}
// 	clock_t end = clock();
// 	double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
// 	printf("ray is %f\n", time_spent);
}

int 	mult_colors(unsigned int color, float val)
{	
// 	int res;
// 	unsigned char c[3];

// val = 1;
// 	c[2] = ((unsigned char)color) * val;
// 	c[1] = ((unsigned char)color << 8) * val;
//  	c[0] = ((unsigned char)color << 16) * val;

// 	 res = c[2] * 256;
// 	 res = (res + c[1]) * 256;
// 	 res = (res + c[2]);
// 	return (res);

	unsigned char	c1[3];
	int				result;

	c1[2] = (color % 256) * val;
	color /= 256;
	c1[1] = (color % 256) * val;
	color /= 256;
	c1[0] = (color % 256) * val;
	result = c1[0] * 256;
	result = (result + c1[1]) * 256;
	result = result + c1[2];
	return (result);
}

void	rectangle(int i, int L, unsigned int color)
{
	int j;
	int c;
	int i_end;
	static int save = 0;
	int e;
	float opacity;

	e = 0;
	c = 0;
	opacity = 0.95 - r.cast.distance / r.maxDistance;



	//printf("|%f|", opacity);
	// if(r.cast.distance < 50 && r.cast.distance > 32 && save == 0)
	// {
	// 	save = r.rayHight;
	// }
	// else if (r.cast.distance < 50 && r.cast.distance > 10)
	// {
	// 		r.rayHight = save;
	// }
		//printf("|%f|\n", r.cast.distance);
	// while(i < 1)
	// {
		j = HEIGHT / 2 - r.rayHight / 2;
		if (j <= 0)
			j = 0;
		while(e < j)
		{
			data[(int )i + (int )e * WIDTH] = 0x87CEEB;
			e++;
		}
		while(c <= r.rayHight && j <= HEIGHT)
		{
			//printf("0");
			if (j >= HEIGHT)
				j = HEIGHT;
			data[(int )i + (int )j * WIDTH] = mult_colors(color, opacity);
			j++;
			c++;
		}
		//printf("\n");
	// 	i++;
	// }
}

void DDA(int X0, int Y0, int X1, int Y1) 
{ 
    // calculate dx & dy 
    int dx = X1 - X0; 
    int dy = Y1 - Y0; 

    // calculate steps required for generating pixels 
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy); 
  
    // calculate increment in x & y for each steps 
    float Xinc = dx / (float) steps; 
    float Yinc = dy / (float) steps; 
  
    // Put pixel for each step 
    float X = X0; 
    float Y = Y0; 
    for (int i = 0; i <= steps; i++) 
    { 
		if(Y < 0 || Y > HEIGHT || X < 0 || X > WIDTH)
			break;
        data[(int )X + (int )Y * WIDTH] = 0xfa2c34;  // put pixel at (X,Y) 
        X += Xinc;
		Y += Yinc;           // increment in x at each step 
    }
}

void	render3d(int i)
{
	float projectDistance;

	projectDistance = (WIDTH / 2) / tan(fov / 2);
	r.cast.distance = cos(p.rotationAngle - r.rays[i]) * r.cast.distance;
	r.rayHight = (projectDistance / r.cast.distance) * 32;
	//printf("%f | %f \n", projectDistance, fov);
	rectangle(i, 2, 0xFFFFFF);
	//DDA(i, HEIGHT - HEIGHT / 2 - r.rayHight / 2, i, r.rayHight);
	//printf("%f\n", r.rayHight);
}

void    drawRays(void)
{
	// clock_t start = clock();
	int i;
	unsigned int color;

	i = 0;
	while(i <= Num_rays)
	{
		color = 0xfa2c34;
		cast(r.rays[i]);
		//drawLine(r.rays[i], color, 1);
		render3d(i);
		i++;
	}
}

void    render(int height, int width, char **lines, int indice)
{
	int a;
	int b;
	int x;
	int y;

	y = 0;
	a = 0;
	while(y < height)
	{
		x = 0;
		b = 0;
		while(x < width)
		{
			//printf("|%c|", lines[a][b]);
			// if(lines[a][b] == '1')
			// {
			// 	put_square(x, y, 1);
			// }
			// else
			// {
			// 	put_square(x, y, 0);
			// }
			if(lines[a][b] == 'N' && indice == 0)
			{
				//printf("loool");
				p.x = x;
				p.y = y;
				//mlx_pixel_put(mlx_ptr, win_ptr, p.x, p.y, 0x000000);
				//data[(int )p.x + (int )p.y * WIDTH] = 0x000000;
				//Line();
				//drawLine(p.rotationAngle, 0x000000);
				//printf("|%f|", p.rotationAngle + fov / 2);
			}
			// else if(indice > 0)
			// {
			// 	data[(int )p.x + (int )p.y * WIDTH] = 0x000000;
			// 	//Line;
			// 	drawRays();
			// 	drawLine(p.rotationAngle, 0x000000);
			// 	//drawLine();
			// }
			x+=32;
			b++;
		}
		y+=32;
		a++;
	}
	if(indice > 0)
	{
		data[(int )p.x + (int )p.y * WIDTH] = 0x000000;
		//Line;
		drawRays();
		//drawLine(p.rotationAngle, 0x000000);
	}
}

void    printmatrice(void)
{
	int a;
	int b;
	
	a = 0;
	while (lines[a])
	{
		b = 0;
		while(lines[a][b])
		{
			printf("%c|", lines[a][b]);
			b++;
		}
		printf("\n");
		a++;
	}
}
int checknextstep(void)
{
	int a;
	int b;

	if (p.walk_for == 1)
	{
		b = (int )(p.x + cos(p.rotationAngle) * p.moveSpeed) / 32;
		a = (int )(p.y + sin(p.rotationAngle) * p.moveSpeed) / 32;;
		if(lines[a][b] == '0' || lines[a][b] == 'N')
			return(1);
		else
			return (0);;
	}
	if (p.walk_back == 1)
	{
		b = (int )(p.x - cos(p.rotationAngle) * p.moveSpeed) / 32;
		a = (int )(p.y - sin(p.rotationAngle) * p.moveSpeed) / 32;
		if(lines[a][b] == '0' || lines[a][b] == 'N')
			return(1);
		else
			return (0);
	}
	return(0);
}

void    player_update(void)
{
	if (p.cam_left == 1) // left
	{
		p.rotationAngle -= p.rotationSpeed;
	}
	if (p.cam_right == 1) //right
	{
		p.rotationAngle += p.rotationSpeed;
	}
	if (p.walk_for == 1)
	{
		if (checknextstep())
		{
			p.x += cos(p.rotationAngle) * p.moveSpeed;
			p.y += sin(p.rotationAngle) * p.moveSpeed;
		}
	}
	if (p.walk_back == 1 && checknextstep())
	{
		p.x -= cos(p.rotationAngle) * p.moveSpeed;
		p.y -= sin(p.rotationAngle) * p.moveSpeed;
	}
}

int		rayFacingDown(float angle)
{
	if (angle > 0 && angle <= Pi)
		return(1);
	// if (sin(angle) < 0)
	// 	return(1);
	 return(0);
}

int		rayFacingLeft(float angle)
{
	// if (angle > (Pi / 2) && angle < (3 * Pi) / 2)
	// 	return(1);
	if (cos(angle) < 0)
		return (1);
	return(0);
}

void	horizontalintersect(float rayAngle)
{
	float ystep;
	float xstep;
	float ay;
	float ax;
	float angle;

	angle = normalize(rayAngle);
	//printf("angle is%f\n", angle);
	//angle = (5 * Pi) / 4;
	ystep = 32 * (rayFacingDown(angle) ? 1 : -1);
	//printf("|%ld|",  ystep);
	xstep = tan(angle) ? ystep / tan(angle) : 0;
	//xstep *= rayFacingLeft(angle) ? -1 : 1;
	ay = (floor(p.y / 32) * 32) + (rayFacingDown(angle) ? 32 : 0);
	//printf("|%d| |%d|\n", rayFacingLeft(angle), rayFacingDown(angle));
	//printf("%f", ay);
	ax = p.x + ((ay - p.y) / tan(angle));
	r.cast.distance = 0;
	//printf("first|%f|%f|\n", ax, ay);
	if(!rayFacingDown(angle))
		ay--;
	while(ay >= 0 && ax >= 0 && ax < width && ay < height)
	{
		// printf("|%f|%f|\n", ax, ay);
		// printf("|%c|\n", lines[(int )(ay) / 32][(int )(ax) / 32]);
		if(lines[(int )(ay) / 32][(int )(ax) / 32] == '1')
		{
			//printf("inter|%f|%f|\n", r.cast.wallHitx, r.cast.wallHity);
			r.cast.horizontalx = ax;
			r.cast.horizontaly = ay + (!rayFacingDown(angle) ? 1 : 0);
			break;
		}
		ax += xstep;
		ay += ystep;
	}
}

void 	verticalintersect(float rayAngle)
{
	float xstep;
	float ystep;
	float ay;
	float ax;
	float angle;

	angle = normalize(rayAngle);
	xstep = 32 * (rayFacingLeft(angle) ? -1 : 1);
	ystep = xstep * (tan(angle));
	ax = (floor(p.x / 32) * 32) + (!rayFacingLeft(angle) ? 32 : 0);
	ay = p.y - (tan(angle) * (p.x - ax));
	if(rayFacingLeft(angle))
		ax--;
	while(ay >= 0 && ax >= 0 && ax < width && ay < height)
	{
		if(lines[(int )(ay) / 32][(int )(ax) / 32] == '1')
		{
			//printf("inter|%f|%f|\n", r.cast.wallHitx, r.cast.wallHity);
			r.cast.verticalx = ax + (rayFacingLeft(angle) ? 1 : 0);
			r.cast.verticaly = ay;
			break;
		}
		ax += xstep;
		ay += ystep;
	}
}
void	chosePoints()
{
	float a;
	float b;

	a = sqrtf(powf(p.x - r.cast.verticalx, 2) + powf(p.y - r.cast.verticaly, 2));
	b = sqrtf(powf(p.x - r.cast.horizontalx, 2) + powf(p.y - r.cast.horizontaly, 2));
	r.maxDistance = sqrtf(powf(0 - height, 2) + powf(0 - width, 2));
	//printf("||%f||\n", r.maxDistance);
	if (a <= b)
	{
		r.cast.wallHitx = r.cast.verticalx;
		r.cast.wallHity = r.cast.verticaly;
		r.cast.distance = a;
	}
	else
	{
		r.cast.wallHitx = r.cast.horizontalx;
		r.cast.wallHity = r.cast.horizontaly;
		r.cast.distance = b;
	}
}
void	cast(float rayAngle)
{
	r.cast.wallHitx = MAXINT;
	r.cast.wallHity = MAXINT;
	r.cast.horizontalx = MAXINT;
	r.cast.horizontaly = MAXINT;
	r.cast.verticalx = MAXINT;
	r.cast.verticaly = MAXINT;
	horizontalintersect(rayAngle);
	verticalintersect(rayAngle);
	chosePoints();
}
void    castAllRays(void)
{
	int i;

	columnId = 0;
	i = 0;
	rayAngle = p.rotationAngle - (fov / 2);
	while(i < Num_rays)// && rayAngle < p.rotationAngle + (fov / 2))
	{
		r.rays[columnId] = rayAngle;
		//cast(r.rays[columnId]);
		rayAngle += fov / Num_rays;
		//("|%f|", rayAngle);
		i+=1;
		columnId++;
	}
}

void	blackscreen(void)
{
	int x;
	int y;

	x = 0;
	y = 0;
	while(x <= WIDTH)
	{
		y = 0;
		while(y <= HEIGHT)
		{
			data[(int )x + (int )y * WIDTH] = 0xa0522d;
			y++;
		}
		x++;
	}
}
int update()
{
	static char i;
	//clock_t b, e;
	mlx_hook(win_ptr , 2 , 0 ,  keypress, 0);
	mlx_hook(win_ptr, 3 , 0 ,  keyreleased,  0);
	//b = clock();
	player_update();
	//e = clock();
	//printf("time taken by player_update(): %lf\n", (double)(e - b) / CLOCKS_PER_SEC);
	//b = clock();
	castAllRays();
	//e = clock();
	//printf("time taken by castAllRays(): %lf\n", (double)(e - b) / CLOCKS_PER_SEC);
	//if (i % 1000 == 0)
	mlx_clear_window(mlx_ptr, win_ptr);
	//b = clock();
	blackscreen();
	render(height, width, lines, 1);
	//e = clock();
	//printf("time taken by render(): %lf\n", (double)(e - b) / CLOCKS_PER_SEC);
	//data[HEIGHT / 2 + WIDTH / 2 * WIDTH] = 0xFFFFFF;
	if (!i || i++ == 127)
		mlx_put_image_to_window(mlx_ptr, win_ptr, img, 0 , 0);
	//mlx_destroy_image(mlx_ptr, img);
	return (0);
}


int main()
{
	int fd;
	int a;
	int b;

	fd = open("map.txt", O_RDWR);
	lines = ft_cube3d(fd);
	a = 0;
	b = 0;
	while(lines[a])
		a++;
	height = 32 * a;
	while(lines[0][b])
		b++;
	width = 32 * b;

	mlx_ptr = mlx_init();
	win_ptr = mlx_new_window(mlx_ptr, HEIGHT, WIDTH, "mlx 42");

	//init field of View
	fov = 60 * (Pi / 180);
	//init number of rays
	Rays_width = 1;
	Num_rays = WIDTH / Rays_width;
	r.rays = malloc(sizeof(float) * Num_rays);
	//init params of player
	p.x = 0;
	p.y = 0;
	p.radius = 3;
	p.turnDirection = 0;
	p.walkDirection = 0;
	p.rotationAngle = Pi / 2;
	p.moveSpeed = 1.5;
	p.rotationSpeed = 3 * Pi / 180;
	img = mlx_new_image(mlx_ptr, WIDTH, HEIGHT);
	data = (int*)mlx_get_data_addr(img, &a, &b, &c);
	render(height, width, lines, 0);
//	mlx_hook(win_ptr , 2 , 0 ,  keypress, 0);
//	mlx_hook(win_ptr, 3 , 0 ,  keyreleased,  0);
	mlx_loop_hook(mlx_ptr, update, 0);
	mlx_loop(mlx_ptr);
	return (0);
}
